#include "event.h"


const char *svf_event_get_name(svf_event_t *e)
{
    if (e == NULL) {
        return OGS_FSM_NAME_INIT_SIG;
    }

    switch (e->h.id) {
    case OGS_FSM_ENTRY_SIG: 
        return OGS_FSM_NAME_ENTRY_SIG;
    case OGS_FSM_EXIT_SIG: 
        return OGS_FSM_NAME_EXIT_SIG;

    case OGS_EVENT_SBI_SERVER:
        return OGS_EVENT_NAME_SBI_SERVER;
    case OGS_EVENT_SBI_CLIENT:
        return OGS_EVENT_NAME_SBI_CLIENT;
    case OGS_EVENT_SBI_TIMER:
        return OGS_EVENT_NAME_SBI_TIMER;

    case QUERY_AUTH_DATA_REQUEST:
        return "QUERY_AUTH_DATA_REQUEST";

    default: 
       break;
    }

    ogs_error("Unknown Event[%d]", e->h.id);
    return "UNKNOWN_EVENT";
}
