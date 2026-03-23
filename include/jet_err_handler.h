#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

void jet_err_handler_start(const char* filename);
void jet_err_handler_end();
void jet_err_handler_reset();

bool jet_err_handler_is_started(); 
const char* jet_err_handler_get_filename();
size_t jet_err_handler_get_count();

void jet_err_handler_push(const jet_span* span, const char* msg);
void jet_err_handler_pushf(const jet_span* span, const char* fmt, ...);
