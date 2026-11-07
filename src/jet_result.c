#include <jet_result.h>
#include <jet_logger.h>
#include <string.h>

static jet_result jet_result_base(jet_result_kind kind);

static jet_result jet_result_base(jet_result_kind kind)
{
    jet_result result = { .kind = kind };
    return result;
}


jet_result jet_result_nid(node_id nid)
{
    jet_result result = jet_result_base(JET_RESULT_NID);
    result.as.nid = nid;
    return result;
}

jet_result jet_result_error(jet_error* error)
{
    JET_ASSERT(error != NULL);
    jet_result result = jet_result_base(JET_RESULT_ERROR);
    result.as.error = *error;
    return result;
}
