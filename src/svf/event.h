#ifndef SVF_EVENT_H
#define SVF_EVENT_H

#include "ogs-proto.h"
#include "ogs-sbi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct svf_sess_s {
    ogs_sbi_object_t sbi;
    ogs_pool_id_t id;
    
    char* response;
    ogs_thread_mutex_t* mutex;
    ogs_thread_cond_t* cond;
} svf_sess_t;

typedef enum {
    AF_EVENT_BASE = OGS_MAX_NUM_OF_PROTO_EVENT,

    QUERY_AUTH_DATA_REQUEST,

    MAX_NUM_OF_AF_EVENT,

} svf_event_e;

typedef struct svf_event_s {
    ogs_event_t h;

    union {
        struct { 
            const char* imsi;
            char* response;
            ogs_thread_mutex_t* mutex;
            ogs_thread_cond_t* cond;
        } query_auth_data_req;
    } data;

    ogs_pkbuf_t *pkbuf;

    svf_sess_t *sess;
} svf_event_t;

OGS_STATIC_ASSERT(OGS_EVENT_SIZE >= sizeof(svf_event_t));

const char *svf_event_get_name(svf_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* AF_EVENT_H */
