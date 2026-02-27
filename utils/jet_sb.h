#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct jet_sb jet_sb;

jet_sb* jet_sb_create(size_t cap);
void jet_sb_dispose(jet_sb* sb);
void jet_sb_clear(jet_sb* sb);
size_t jet_sb_len(const jet_sb* sb);
char* jet_sb_dup(const jet_sb* sb);
const char* jet_sb_view(const jet_sb* sb);

void jet_sb_appendf(jet_sb* sb, const char* fmt, ...);
void jet_sb_append_char(jet_sb* sb, char c);
void jet_sb_append_cstr(jet_sb* sb, const char* s);
void jet_sb_append_u64(jet_sb* sb, uint64_t v);
void jet_sb_append_sizet(jet_sb* sb, size_t v);
void jet_sb_append_int(jet_sb* sb, int i);

#ifdef JET_SB_IMPL

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define JET_SB_GROWTH_FAC 2
#define JET_SB_MIN_CAP 2

struct jet_sb 
{
     char* buf;
     size_t cap;
     size_t len;
};

static void jet_sb_ensure_extra_cap(jet_sb* sb, size_t n);
static void jet_sb_grow(jet_sb* sb);

jet_sb* jet_sb_create(size_t cap)
{
    jet_sb* sb = malloc(sizeof(jet_sb));
    if(!sb)
    {
        fprintf(stderr, "err: couldn't allocate sb mem.\n");
        return NULL;
    }
    cap++;
    if(cap < JET_SB_MIN_CAP)
        cap = JET_SB_MIN_CAP;
    sb->buf = malloc(cap);
    if(!sb->buf)
    {
        free(sb);
        fprintf(stderr, "err: couldn't allocate sb buffer mem.\n");
        return NULL;
    } 
    sb->cap = cap;
    sb->len = 0;
    sb->buf[sb->len] = '\0';
    return sb;
}

void jet_sb_dispose(jet_sb* sb)
{
    if(sb == NULL) 
        return;
    if(sb->buf) 
        free((void*)sb->buf);
    free((void*)sb);
}

void jet_sb_clear(jet_sb* sb)
{
    assert(sb != NULL);
    sb->len = 0;
    sb->buf[0] = '\0';
}

size_t jet_sb_len(const jet_sb* sb)
{
    assert(sb != NULL);
    return sb->len;
}

char* jet_sb_dup(const jet_sb* sb)
{
    char* dup = malloc(sb->len + 1);
    if(!dup)
    {
        fprintf(stderr, "err: could not allocate memory for dup.\n");
        return NULL;
    }
    memcpy((void*)dup, (const void*)sb->buf, sb->len + 1);
    return dup;
}

const char* jet_sb_view(const jet_sb* sb)
{
    assert(sb != NULL);
    return (const char*)sb->buf;
}

void jet_sb_appendf(jet_sb* sb, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    
    int req = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if(req < 0)
    {
        va_end(args_copy);
        return;
    }
    
    jet_sb_ensure_extra_cap(sb, (size_t)req + 1);
    vsnprintf(
            sb->buf + sb->len, 
            sb->cap - sb->len, 
            fmt, 
            args_copy);

    va_end(args_copy);
    sb->len += (size_t)req;
}

void jet_sb_append_char(jet_sb* sb, char c)
{
    assert(sb != NULL); 
    jet_sb_ensure_extra_cap(sb, 1);
    sb->buf[sb->len] = c;
    sb->len++;
    sb->buf[sb->len] = '\0';
}

void jet_sb_append_cstr(jet_sb* sb, const char* s)
{
    assert(sb != NULL && s != NULL);
    size_t n = strlen(s);
    jet_sb_ensure_extra_cap(sb, n);

    memcpy(sb->buf + sb->len, s, n);
    sb->len += n;
    sb->buf[sb->len] = '\0';
}

void jet_sb_append_u64(jet_sb* sb, uint64_t v)
{
    char temp_buf[32];
    size_t i = 0;
    if(v == 0)
    {
        jet_sb_append_char(sb, '0');
        return;
    }
    while(v > 0)
    {
        temp_buf[i] = (char)('0' + (v % 10));
        i++;
        v /= 10;
    }
    for(size_t r = 0; r < i; r++)
        jet_sb_append_char(sb, temp_buf[i - 1 - r]);
}

void jet_sb_append_int(jet_sb* sb, int i)
{
    assert(sb != NULL);
    if(i < 0)
    {
        jet_sb_append_char(sb, '-');
        uint64_t magnitude = (uint64_t)(-(int64_t)i);
        jet_sb_append_u64(sb, magnitude);
    }
    else
    {
        jet_sb_append_u64(sb, (uint64_t)i);
    }
}

void jet_sb_append_sizet(jet_sb* sb, size_t v)
{
    assert(sb != NULL);
    jet_sb_append_u64(sb, (uint64_t)v);
}

static void jet_sb_ensure_extra_cap(jet_sb* sb, size_t n)
{
    assert(sb != NULL);
    while(sb->len + n >= sb->cap)
    {
        jet_sb_grow(sb);
    }
}

static void jet_sb_grow(jet_sb* sb)
{
    assert(sb != NULL);
    size_t new_cap = sb->cap * JET_SB_GROWTH_FAC;
    if(new_cap <= sb->cap)
        new_cap = sb->cap + 1;
    char* new_buf = realloc(sb->buf, new_cap);
    if(!new_buf)
    {
        fprintf(stderr, "err: couldn't grow sb buf, out of mem.\n");
        abort();
    }
    sb->cap *= JET_SB_GROWTH_FAC;
    sb->buf = new_buf;
}

#endif


