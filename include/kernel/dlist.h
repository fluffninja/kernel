#ifndef _INC_KERNEL_DLIST
#define _INC_KERNEL_DLIST 1

#include <kernel/kernel.h>
#include <kernel/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

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

// Casts a type's member to the containing type
// ITEM_PTR:    - pointer to a member
// CONT_TYPE:   - type of the containing type
// MEMBER:      - name of the member within the type
#ifndef CONTAINER_OF
#define CONTAINER_OF(ITEM_PTR, CONT_TYPE, MEMBER) \
    (CONT_TYPE *) ((char *) (ITEM_PTR) - offsetof(CONT_TYPE, MEMBER))
#endif

#define DLIST_CONTAINER_OF_NEXT(NODE, CONT_TYPE, MEMBER) \
    CONTAINER_OF(NODE->MEMBER.next, CONT_TYPE, MEMBER)

#define DLIST_CONTAINER_OF_PREV(NODE, CONT_TYPE, MEMBER) \
    CONTAINER_OF(NODE->MEMBER.prev, CONT_TYPE, MEMBER)

// Iterates forwards through a given list.
// CONT_TYPE:   The type of the given container.
// ITER:        The name of the iterator variable within the loop body.
// CONT:        The first container.
// MEMBER:      Name of the list node as a member of container CONT.
//
// Example Usage:
// struct foo { int val; struct dlist_node list; } f1, f2, f3, ...;
// ...
// DLIST_FOR_EACH(struct foo, f, &f1, list) {
//     printf("Val: %d\n", f->val);
// }
#define DLIST_FOR_EACH(CONT_TYPE, ITER, CONT, MEMBER)               \
    for (                                                           \
        CONT_TYPE *ITER = CONT,                                     \
        *__first = NULL;                                            \
        ITER != __first;                                            \
        __first = !__first ? ITER : __first,                        \
        ITER = DLIST_CONTAINER_OF_NEXT(ITER, CONT_TYPE, MEMBER)     \
    )

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_DLIST */
