#include <jet_err.h>
#include <jet_logger.h>

typedef struct 
{
    jet_err_source src;
    const char* filename;
    const jet_span* span;
} jet_err_entry; 

void jet_err_emit(jet_err_source err_src, const char* filename, const jet_span* span);
