#include "simplestring.h"
#include "assert.h"

static void ensure_cstr(lcb_string *str);

int lcb_string_init(lcb_string *str)
{
    str->base = NULL;
    str->nalloc = 0;
    str->nused = 0;
    return 0;
}

void lcb_string_release(lcb_string *str)
{
    if (str->base == NULL) {
        return;
    }
    free(str->base);
    memset(str, 0, sizeof(*str));
}

void lcb_string_clear(lcb_string *str)
{
    str->nused = 0;
    if (str->nalloc) {
        ensure_cstr(str);
    }
}

void lcb_string_added(lcb_string *str, lcb_size_t nused)
{
    str->nused += nused;
    lcb_assert(str->nused <= str->nalloc);
    ensure_cstr(str);
}

int lcb_string_reserve(lcb_string *str, lcb_size_t size)
{
    lcb_size_t newalloc;
    char *newbuf;

    /** Set size to one greater, for the terminating NUL */
    size++;
    if (!size) {
        return -1;
    }

    if (str->nalloc - str->nused >= size) {
        return 0;
    }

    newalloc = str->nalloc;
    if (!newalloc) {
        newalloc = 1;
    }

    while (newalloc - str->nused < size) {
        if (newalloc * 2 < newalloc) {
            return -1;
        }

        newalloc *= 2;
    }

    newbuf = realloc(str->base, newalloc);
    if (newbuf == NULL) {
        return -1;
    }

    str->base = newbuf;
    str->nalloc = newalloc;
    return 0;
}

static void ensure_cstr(lcb_string *str)
{
    str->base[str->nused] = '\0';
}

int lcb_string_append(lcb_string *str, const void *data, lcb_size_t size)
{
    if (lcb_string_reserve(str, size)) {
        return -1;
    }

    memcpy(str->base + str->nused, data, size);
    str->nused += size;
    ensure_cstr(str);
    return 0;
}

int lcb_string_appendz(lcb_string *str, const char *s)
{
    return lcb_string_append(str, s, strlen(s));
}

int lcb_string_rbappend(lcb_string *str, ringbuffer_t *rb, int rbadvance)
{
    lcb_size_t expected, nw;
    expected = rb->nbytes;
    if (!expected) {
        return 0;
    }

    if (lcb_string_reserve(str, expected)) {
        return -1;
    }

    nw = ringbuffer_peek(rb, lcb_string_tail(str), expected);
    if (nw != expected) {
        return -1;
    }

    if (rbadvance) {
        ringbuffer_consumed(rb, nw);
    }

    lcb_string_added(str, nw);
    return 0;
}

void lcb_string_erase_end(lcb_string *str, lcb_size_t to_remove)
{
    lcb_assert(to_remove <= str->nused);
    str->nused -= to_remove;
    ensure_cstr(str);
}

void lcb_string_erase_beginning(lcb_string *str, lcb_size_t to_remove)
{
    lcb_assert(to_remove <= str->nused);
    if (!to_remove) {
        str->nused = 0;
        return;
    }

    memmove(str->base, str->base + to_remove, str->nused - to_remove);
    str->nused -= to_remove;
    ensure_cstr(str);
}

void lcb_string_transfer(lcb_string *from, lcb_string *to)
{
    lcb_assert(to->base == NULL);
    *to = *from;
    memset(from, 0, sizeof(*from));
}
