#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct jet_arena 
{
    char* block;
    size_t cap;
    size_t offset;
    struct jet_arena* next;
} jet_arena;

bool jet_arena_init(jet_arena* arena, size_t cap);
void jet_arena_dispose(jet_arena* arena);
void jet_arena_reset(jet_arena* arena);
void jet_arena_zero_reset(jet_arena* arena);
void* jet_arena_alloc(jet_arena* arena, size_t bytes);

#ifdef JET_ARENA_IMPL
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define JET_ARENA_CAP_GROWTH_FAC (2)

static jet_arena* jet_arena_get_next_available(jet_arena* arena, size_t bytes);

// must init fresh arenas only, otehrwise memory leak will occure
bool jet_arena_init(jet_arena* arena, size_t cap)
{
    if(!arena)
    {
        fprintf(stderr, "err: cannot init, param arena is null.\n");
        return false;
    }
    memset(arena, 0, sizeof(*arena));

    if(cap == 0)
    {
        fprintf(stderr, "err: cannot init, arena cap must be > 0.\n");
        return false;
    }
    
    arena->offset = 0;
    arena->next = NULL;
    arena->block = malloc(cap);

    if(!arena->block)
    {
        fprintf(stderr, "err: cannot init, failed to allocate block memory.\n");
        return false;
    }
    arena->cap = cap;
    return true;
}

void jet_arena_dispose(jet_arena* arena)
{
    if(!arena)
        return;
    jet_arena* cur = arena->next;
    jet_arena* temp = NULL;
    while(cur != NULL)
    {
        free(cur->block);
        temp = cur->next;
        free(cur);
        cur = temp;
    }
    free(arena->block);
    memset(arena, 0, sizeof(*arena));
}

void jet_arena_zero_reset(jet_arena* arena)
{
    if(!arena)
        return;
    while(arena != NULL)
    {
        if(arena->block)
            memset(arena->block, 0, arena->cap);
        arena->offset = 0;
        arena = arena->next;
    }
}

void jet_arena_reset(jet_arena* arena)
{
    if(!arena)
        return;
    while(arena != NULL)
    {
        arena->offset = 0;
        arena = arena->next;
    }
}

void* jet_arena_alloc(jet_arena* arena, size_t bytes)
{
    if(!arena)
    {
        fprintf(stderr, "err: failed to alloc, param arena is null.\n");
        return NULL;
    }
    if(!arena->block)
    {
        fprintf(stderr, "err: failed to alloc, arena not initialized.\n");
        return NULL;
    }

    if(bytes == 0)
    {
        fprintf(stderr, "err: failed to alloc, allocation size must be > 0.\n");
        return NULL;
    }

    jet_arena* a = jet_arena_get_next_available(arena, bytes);
    if(!a)
    {
        fprintf(stderr, "err: unable to alloc, failed to aquire available arena.\n");
        return NULL;
    }

    void* base = (void*)(a->block + a->offset);
    a->offset += bytes;
    return base;
}


static jet_arena* jet_arena_get_next_available(jet_arena* arena, size_t bytes)
{
    while(bytes > arena->cap - arena->offset)
    {
        if(!arena->next) 
        {
            size_t next_cap = arena->cap; 
            while(next_cap < bytes)
            {
                if(next_cap > SIZE_MAX / JET_ARENA_CAP_GROWTH_FAC)
                {
                    fprintf(stderr, "err: failed to create new arena, capacity overflow.\n");
                    return NULL;
                }
                next_cap *= JET_ARENA_CAP_GROWTH_FAC;
            }

            arena->next = malloc(sizeof(jet_arena));
            if(!arena->next)
            {
                fprintf(stderr, "err: failed to alloc new arena.\n");
                return NULL;
            }
            arena->next->block = NULL;
            if(!jet_arena_init(arena->next, next_cap))
            {
                fprintf(stderr, "err: failed to init arena.\n");
                free(arena->next);
                arena->next = NULL;
                return NULL;
            }
        }
        arena = arena->next;
    }
    return arena;
}


#endif
