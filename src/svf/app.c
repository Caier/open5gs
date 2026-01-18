#include "ogs-app.h"
#include "sbi-path.h"
#include "svf-sm.h"

static int __svf_log_domain;
static ogs_thread_t *evl_thr;
static void svf_event_loop(void *data);
static int initialized = 0;

OGS_POOL(svf_sess_pool, svf_sess_t);

int app_initialize(const char *const argv[]) {
    int rv = OGS_OK;
    
    ogs_pool_init(&svf_sess_pool, 64);

    #define APP_NAME "svf"
    rv = ogs_app_parse_local_conf(APP_NAME);
    if (rv != OGS_OK) goto init_fail;

    ogs_sbi_context_init(OpenAPI_nf_type_UDM); // maybe should create custom type?
    ogs_log_install_domain(&__svf_log_domain, "svf", ogs_core()->log.level);

    rv = ogs_log_config_domain(
            ogs_app()->logger.domain, ogs_app()->logger.level);
    if (rv != OGS_OK) goto init_fail;

    rv = ogs_sbi_context_parse_config(APP_NAME, "nrf", "scp");
    if (rv != OGS_OK) goto init_fail;

    // rv = udm_context_parse_config();
    // if (rv != OGS_OK) return rv;

    rv = svf_sbi_open();
    if (rv != OGS_OK) goto init_fail;

    evl_thr = ogs_thread_create(svf_event_loop, NULL);
    if (!evl_thr) {
        rv = OGS_ERROR;
        goto init_fail;
    }

    initialized = 1;

    ogs_info("SVF initialize...done");
    return OGS_OK;

    init_fail:
    ogs_warn("Failed to initialize SVF!");
    return rv;
}

void app_terminate(void) {
    if (!initialized) return;

    ogs_sbi_nf_instance_t *nf_instance = NULL;

    /* Sending NF Instance De-registration to NRF */
    ogs_list_for_each(&ogs_sbi_self()->nf_instance_list, nf_instance)
        ogs_sbi_nf_fsm_fini(nf_instance);

    ogs_timer_t* t_termination = ogs_timer_add(ogs_app()->timer_mgr, NULL, NULL);
    ogs_assert(t_termination);
    ogs_timer_start(t_termination, ogs_time_from_msec(300));
    ogs_queue_term(ogs_app()->queue);
    ogs_pollset_notify(ogs_app()->pollset);

    /* Gracefully shutdown the server by sending GOAWAY to each session. */
    ogs_sbi_server_graceful_shutdown_all();

    ogs_thread_destroy(evl_thr);
    ogs_timer_delete(t_termination);
    svf_sbi_close();
    ogs_pool_final(&svf_sess_pool);
    ogs_sbi_context_final();

    ogs_info("SVF terminated!");
}

void svf_event_loop(void* data) {
    ogs_warn("SVF loop init");

    ogs_fsm_t svf_fsm;
    
    ogs_fsm_init(&svf_fsm, svf_state_initial, svf_state_final, NULL);

        for ( ;; ) {
        ogs_pollset_poll(ogs_app()->pollset,
                ogs_timer_mgr_next(ogs_app()->timer_mgr));

        ogs_timer_mgr_expire(ogs_app()->timer_mgr);

        for ( ;; ) {
            void *e = NULL;

            int rv = ogs_queue_trypop(ogs_app()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&svf_fsm, e);
            ogs_event_free(e);
        }
    }
done:

    ogs_fsm_fini(&svf_fsm, 0);
}