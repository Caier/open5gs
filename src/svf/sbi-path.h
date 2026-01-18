#ifndef SVF_SBI_PATH_H
#define SVF_SBI_PATH_H

#include "ogs-sbi.h"
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

int svf_sbi_open(void);
void svf_sbi_close(void);

bool svf_sbi_send_request(
        ogs_sbi_nf_instance_t *nf_instance, ogs_sbi_xact_t *xact);

ogs_sbi_request_t* svf_build_nudr_query_auth(svf_sess_t* sess, void* data);


void svf_sbi_discover_and_send(
        ogs_sbi_service_type_e service_type,
        ogs_sbi_discovery_option_t *discovery_option,
        ogs_sbi_request_t *(*build)(svf_sess_t *udm_ue, void *data),
        svf_sess_t* sess, void *data);

#ifdef __cplusplus
}
#endif

#endif /* UDM_SBI_PATH_H */
