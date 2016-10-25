#ifndef _INC_DLIST
#define _INC_DLIST 1

#include <cccompat.h>
#include <stddef.h>

struct dlist_node
{
    struct dlist_node *next;
    struct dlist_node *prev;
};

static INLINE void dlist_node_create(struct dlist_node *node)
{
    node->next = node;
    node->prev = node;
}

static INLINE int dlist_is_empty(const struct dlist_node *node)
{
    return (node == node->next);
}

static INLINE void __dlist_insert(struct dlist_node *new,
    struct dlist_node *prev, struct dlist_node *next)
{
    new->next = next;
    new->prev = prev;
    prev->next = new;
    next->prev = new;
}

static INLINE void dlist_insert_before(struct dlist_node *new,
    struct dlist_node *node)
{
    __dlist_insert(new, node->prev, node);
}

static INLINE void dlist_insert_after(struct dlist_node *new,
    struct dlist_node *node)
{
    __dlist_insert(new, node, node->next);
}

static INLINE void __dlist_remove(struct dlist_node *next,
    struct dlist_node *prev)
{
    next->prev = prev;
    prev->next = next;
}

static INLINE void dlist_remove(struct dlist_node *node)
{
    __dlist_remove(node->next, node->prev);
    node->next = NULL;
    node->prev = NULL;
}

static INLINE void __dlist_split(struct dlist_node *former,
    struct dlist_node *latter)
{
    former->next = latter;
    latter->prev = former;
}

static INLINE void dlist_split_before(struct dlist_node *new,
    struct dlist_node *node)
{
    __dlist_split(new, node);
}

static INLINE void dlist_split_after(struct dlist_node *new,
    struct dlist_node *node)
{
    __dlist_split(node, new);
}

#endif /* _INC_DLIST */
