#include "sbi-path.h"
#include "event.h"

int svf_sbi_open(void)
{
    ogs_sbi_nf_instance_t *nf_instance = NULL;
    ogs_sbi_nf_service_t *service = NULL;

    /* Initialize SELF NF instance */
    nf_instance = ogs_sbi_self()->nf_instance;
    ogs_assert(nf_instance);
    ogs_sbi_nf_fsm_init(nf_instance);

    /* Build NF instance information. It will be transmitted to NRF. */
    ogs_sbi_nf_instance_build_default(nf_instance);

    /* Initialize NRF NF Instance */
    nf_instance = ogs_sbi_self()->nrf_instance;
    if (nf_instance)
        ogs_sbi_nf_fsm_init(nf_instance);

    if (ogs_sbi_server_start_all(ogs_sbi_server_handler) != OGS_OK)
        return OGS_ERROR;

    return OGS_OK;
}

void svf_sbi_close(void)
{
    ogs_sbi_client_stop_all();
    ogs_sbi_server_stop_all();
}

bool svf_sbi_send_request(
        ogs_sbi_nf_instance_t *nf_instance, ogs_sbi_xact_t *xact)
{
    ogs_assert(nf_instance);
    ogs_assert(xact);
    return ogs_sbi_send_request_to_nf_instance(nf_instance, xact);
}

ogs_sbi_request_t* svf_build_nudr_query_auth(svf_sess_t* sess, void* data) {
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;
    char* imsi = data;

    ogs_assert(imsi);
    ogs_assert(sess);
    
    // ogs_assert(amf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_GET;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDR_DR;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = "subscription-data";
    message.h.resource.component[1] = imsi;
    message.h.resource.component[2] = "authentication-data";
    message.h.resource.component[3] = "authentication-subscription";

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}

void svf_sbi_discover_and_send(
        ogs_sbi_service_type_e service_type,
        ogs_sbi_discovery_option_t *discovery_option,
        ogs_sbi_request_t *(*build)(svf_sess_t *sess, void *data),
        svf_sess_t *sess, void *data)
{
    ogs_sbi_xact_t *xact = NULL;
    int r;

    ogs_assert(service_type);
    ogs_assert(sess);
    ogs_assert(build);

    xact = ogs_sbi_xact_add(
            sess->id, &sess->sbi, service_type, discovery_option,
            (ogs_sbi_build_f)build, sess, data);
    if (!xact) {
        ogs_error("svf_sbi_discover_and_send() failed");
        return;
    }

    r = ogs_sbi_discover_and_send(xact);
    if (r != OGS_OK) {
        ogs_error("svf_sbi_discover_and_send() failed");
        return;
    }
}
