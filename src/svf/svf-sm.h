#ifndef SVF_SM_H
#define SVF_SM_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

void svf_state_initial(ogs_fsm_t *s, svf_event_t *e);
void svf_state_final(ogs_fsm_t *s, svf_event_t *e);
void svf_state_operational(ogs_fsm_t *s, svf_event_t *e);
void svf_state_exception(ogs_fsm_t *s, svf_event_t *e);

#define svf_sm_debug(__pe) \
    ogs_debug("%s(): %s", __func__, svf_event_get_name(__pe))

#ifdef __cplusplus
}
#endif

#endif /* svf_SM_H */
