#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct jet_arena jet_arena;

bool jet_arena_init(jet_arena* arena, size_t capacity);
void jet_arena_dispose(jet_arena* arena);
void jet_arena_reset(jet_arena* arena);
void* jet_arena_alloc(jet_arena* arena, size_t bytes);


#ifdef JET_ARENA_IMPL
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define JET_ARENA_CAPACITY_GROWTH_FAC (2)

static jet_arena* jet_arena_create(size_t cap);
static jet_arena* jet_arena_get_next_available(jet_arena* arena, size_t bytes);

struct jet_arena
{
    char* block;
    size_t cap;
    size_t offset;
    jet_arena* next;
};

bool jet_arena_init(jet_arena* arena, size_t capacity)
{
    if(!arena)
    {
        fprintf(stderr, "err: cannot init, param arena is null.\n");
        return false;
    }

    if(arena->block)
    {
        fprintf(stderr, "err: calling init on already initialized arena.\n");
        return false;
    }

    if(capacity == 0)
    {
        fprintf(stderr, "err: cannot init, arena capacity must be > 0.\n");
        return false;
    }
    
    arena->cap = capacity;
    arena->offset = 0;
    arena->next = NULL;
    arena->block = malloc(capacity);

    if(!arena->block)
    {
        fprintf(stderr, "err: cannot init, failed to allocate block memory.\n");
        return false;
    }
    return true;
}

void jet_arena_dispose(jet_arena* arena)
{
    if(!arena)
        return;

    jet_arena* cur = arena->next;
    while(cur != NULL)
    {
        free((void*)cur->block);
        jet_arena* temp = cur->next;
        free((void*)cur);
        cur = temp;
    }
    free((void*)arena->block);
    arena->offset = 0;
    arena->block = NULL;
    arena->next = NULL;
}

void jet_arena_reset(jet_arena* arena)
{
    if(!arena)
        return;
    while(arena != NULL)
    {
        memset(arena->block, 0, arena->cap);
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

static jet_arena* jet_arena_create(size_t cap)
{
    jet_arena* arena = malloc(sizeof(jet_arena));
    if(!arena)
    {
        fprintf(stderr, "err: failed to create new arena.\n");
        return NULL;
    }
    if(!jet_arena_init(arena, cap))
    {
        free(arena);
        fprintf(stderr, "err: unable to create new arena, failed init.\n");
        return NULL;
    }
    return arena;
}

static jet_arena* jet_arena_get_next_available(jet_arena* arena, size_t bytes)
{
    while(bytes > arena->cap - arena->offset)
    {
        if(!arena->next) 
        { 
            arena->next = jet_arena_create(arena->cap * JET_ARENA_CAP_GROWTH_FAC);
            if(!arena->next)
            {
                fprintf(stderr, "err: unable to create new arena.\n"); 
                return NULL;
            }
        }
        arena = arena->next;
    }
    return arena;
}


#endif
