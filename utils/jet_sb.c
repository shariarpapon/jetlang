#include <jet_sb.h>
#include <assert.h>
#define JET_SB_GROWTH_FAC 2
#define JET_SB_MIN_CAP 2

struct jet_sb {
     char* buf;
     size_t cap;
     size_t len;
};

static void jet_sb_ensure_cap(jet_sb* sb, size_t n);
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

size_t jet_sb_len(jet_sb* sb)
{
    assert(sb != NULL);
    return sb->len;
}

void jet_sb_append_char(jet_sb* sb, char c)
{
    assert(sb != NULL); 
    jet_sb_ensure_cap(sb, 1);
    sb->buf[sb->len] = c;
    sb->len++;
    sb->buf[sb->len] = '\0';
}

static void jet_sb_ensure_cap(jet_sb* sb, size_t n)
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



