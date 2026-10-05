#include <jet_result.h>
#include <string.h>

static bool jet_result_init(jet_result* result, bool success)
{
    if(!result) return false;
    memset((void*)result, 0, sizeof(*result));
    result->success = success;
    return true;
}

jet_result jet_result_base(bool success)
{
    jet_result result;
    ASSERT(jet_result_init(&result, success));
    return result;
}

jet_result jet_result_error(jet_error* error)
{
    jet_result result = jet_result_base(false);
    result.as.error = &error;
}
