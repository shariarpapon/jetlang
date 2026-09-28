#include <jet_result.h>
#include <string.h>

bool jet_result_init(jet_result* result, bool success)
{
    if(!result) return false;
    memset((void*)result, 0, sizeof(*result));
    result->success = success;
    return true;
}

void jet_result_dispose(jet_result* result)
{
    if(!result) return;
    memset(result, 0, sizeof(*result));
}
