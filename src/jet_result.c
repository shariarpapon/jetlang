#include <jet_result.h>
#include <string.h>

static jet_result jet_result_base(bool success);

static jet_result jet_result_base(bool success)
{
    jet_result result;
    memset(&result, 0, sizeof(result));
    result->success = success;
    return result;
}

jet_result jet_result_nid(node_id nid)
{
    jet_result result = jet_result_base(true);
    result.as.nid = nid;
    return result;
}

jet_result jet_result_error(jet_error* error)
{
    ASSERT(error != NULL);
    jet_result result = jet_result_base(false);
    result.as.error = &error;
    return result;
}
