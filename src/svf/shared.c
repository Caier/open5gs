#ifdef BUILT_AS_DYLIB

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ogs-app.h"
#include "version.h"
#include "shared.h"
#include "event.h"

static void terminate(void) {
    app_terminate();
    ogs_app_terminate();
}

int init_open5gs(const char* conf_path) {
    const char * const argv_out[] = { 
        "hhhhh", "-c", conf_path,
        "-e", "warn", 0
    }; 
    
    // ogs_signal_init();
    // ogs_setup_signal_thread();

    int rv = ogs_app_initialize(OPEN5GS_VERSION, DEFAULT_CONFIG_FILENAME, argv_out);
    if (rv != OGS_OK) {
        if (rv == OGS_RETRY)
            return EXIT_SUCCESS;

        ogs_fatal("Open5GS initialization failed. Aborted");
        return OGS_ERROR;
    }

    rv = app_initialize(argv_out);
    if (rv != OGS_OK) {
        if (rv == OGS_RETRY)
            return EXIT_SUCCESS;

        ogs_fatal("Open5GS initialization failed. Aborted");
        return OGS_ERROR;
    }

    atexit(terminate);
    // ogs_signal_thread(check_signal);

    ogs_info("Open5GS daemon terminating...");

    return OGS_OK;
}

int svf_get_enc_k(const char* imsi, char* key) {
    int ret = OGS_OK;

    ogs_thread_mutex_t mutex = {}; 
    ogs_thread_mutex_init(&mutex);
    ogs_thread_cond_t cond = {};
    ogs_thread_cond_init(&cond);

    svf_event_t* e = ogs_event_size(QUERY_AUTH_DATA_REQUEST, sizeof(svf_event_t));
    ogs_assert(e);
    e->data.query_auth_data_req.imsi = imsi;
    e->data.query_auth_data_req.mutex = &mutex;
    e->data.query_auth_data_req.response = key;
    e->data.query_auth_data_req.cond = &cond;

    int rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("failed to push event!!");
        ogs_event_free(e);
        ret = OGS_ERROR;
    } else {
        ogs_pollset_notify(ogs_app()->pollset);

        ogs_thread_mutex_lock(&mutex);
        while(key[0] == 0) {
            int r = ogs_thread_cond_timedwait(&cond, &mutex, ogs_time_from_sec(5));
            if (r != OGS_OK) {
                ret = OGS_ERROR;
                goto cleanup;
            }
        }   
    }

    cleanup:
    ogs_thread_mutex_unlock(&mutex);
    ogs_thread_cond_destroy(&cond);
    ogs_thread_mutex_destroy(&mutex);
    return ret;
}

#endif
