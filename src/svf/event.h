#ifndef SVF_EVENT_H
#define SVF_EVENT_H

#include "ogs-proto.h"
#include "ogs-sbi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct svf_sess_s {
    ogs_sbi_object_t sbi;
} svf_sess_t;

typedef enum {
    AF_EVENT_BASE = OGS_MAX_NUM_OF_PROTO_EVENT,

    WHAT_ARE_EVENTS_IDONT_KNOW,

    MAX_NUM_OF_AF_EVENT,

} svf_event_e;

typedef struct svf_event_s {
    ogs_event_t h;
    int local_id;

    ogs_pkbuf_t *pkbuf;

    svf_sess_t *sess;
} svf_event_t;

OGS_STATIC_ASSERT(OGS_EVENT_SIZE >= sizeof(svf_event_t));

svf_event_t *svf_event_new(int id);

const char *svf_event_get_name(svf_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* AF_EVENT_H */
