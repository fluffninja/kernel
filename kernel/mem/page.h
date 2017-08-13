#ifndef _INC_PAGE
#define _INC_PAGE 1

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/klog.h>

#ifndef Z0TH_PAGE_ADDR
#define Z0TH_PAGE_ADDR 0x10000
#endif

#ifndef FRAME_TO_PTR
#define FRAME_TO_PTR(frno) (frno << 12)
#endif

typedef struct page_table_entry
{
    u32 is_present:1;
    u32 is_writeable:1;
    u32 is_user:1;
    u32 is_write_through:1;
    u32 is_cache_disabled:1;
    u32 has_been_accessed:1;
    u32 is_dirty:1;
    u32 reserved_1:1;
    u32 is_global:1;
    u32 available:3;
    u32 page_address_4k_aligned:20;
} page_table_entry;

GUARANTEE_SIZE(page_table_entry, 0x4);

typedef struct page_table
{
    page_table_entry pte[0x400];
} page_table;

GUARANTEE_SIZE(page_table, 0x1000);

typedef struct page_directory_entry
{
    u32 is_present:1;
    u32 is_writeable:1;
    u32 is_user:1;
    u32 is_write_through:1;
    u32 is_cache_disabled:1;
    u32 has_been_accessed:1;
    u32 reserved_1:1;
    u32 is_4MB_size:1;
    u32 ignored_1:1;
    u32 available:3;
    u32 page_table_address_4k_aligned:20;
} page_directory_entry;

GUARANTEE_SIZE(page_directory_entry, 0x4);

typedef struct page_directory
{
    page_directory_entry pde[0x400];
} page_directory;

GUARANTEE_SIZE(page_directory, 0x1000);

typedef struct CR3
{
    u32 unused:12;
    u32 page_directory_4k_aligned:20;
} CR3;

GUARANTEE_SIZE(CR3, 0x4);

typedef union virtual_pointer
{
    u32 ptr;
    struct
    {
        u32 page_offset:12;
        u32 page_table_idx:10;
        u32 page_directory_idx:10;
    };
} virtual_pointer;

GUARANTEE_SIZE(virtual_pointer, 0x4);

typedef struct page_ring_buffer
{
    u32 page_buffer[0x400];
} page_ring_buffer;

GUARANTEE_SIZE(page_ring_buffer, 0x1000);

typedef struct page_indirection
{
    u32 (*page)[0x400];
} page_indirection;

typedef slice_of(page_indirection) page_indirection_slice;
typedef slice_of(page_indirection*) page_indirection_ptr_slice;

typedef struct page_allocator
{
    slice_of(page_indirection) underlying_buffer;
    page_indirection *head;
    slice_of(page_indirection*) free_stack;

    //void* fallthrough_allocator;
    //void (*fallthrough)(const void*);
    u32 allocs;
    u64 padding;
} page_allocator;

GUARANTEE_SIZE(page_allocator, 0x20);

typedef struct generic_allocator
{
    slice_of(void) underlying_buffer;
    void *head;
    slice_of(void*) free_stack;
    u32 allocations;
    u8 elem_size_log2;
    u8 pad1;
    u16 pad2;
    u32 pad3;
} generic_allocator;

GUARANTEE_SIZE(generic_allocator, 0x20);
/*
    Reserve a new page and get a pointer to its indirection 
    (effectively a page**)
*/
page_indirection* page_alloc(page_allocator *const allocator);

/*
    Free a page indirection
    Returns nullptr if successful, else returns page_ind
*/
page_indirection* page_free(page_allocator *const allocator, 
    page_indirection *const page_ind);

page_indirection* kpalloc();
page_indirection* kpfree(page_indirection *const page_ind);

int page_allocator_is_full(page_allocator const allocator);
int page_allocator_can_alloc(page_allocator const allocator);
int page_allocator_can_free(page_allocator const allocator);

#define decl_allocator(T)\
    typedef struct allocator_##T{\
        slice_of(T) underlying_buffer;\
        T *head;\
        slice_of(T*) free_stack;\
        u32 allocations;\
        u64 pad3;\
    } allocator_##T;\
    \
    T* allocator_##T##_alloc(allocator_##T*const allocator);\
    T* allocator_##T##_free(allocator_##T*const allocator, T *const ptr);\
    int allocator_##T##_is_full(allocator_##T const allocator);\
    int allocator_##T##_can_alloc(allocator_##T const allocator);\
    int allocator_##T##_can_free(allocator_##T const allocator);

#define mk_allocator(T)\
    T* allocator_##T##_alloc(allocator_##T *const allocator)\
    {\
        u32 free_length = allocator->free_stack.length;\
        if(free_length)\
        {\
            swap((void**)&allocator->free_stack.ptr[0],\
                 (void**)&allocator->free_stack.ptr[free_length-1]);\
            allocator->free_stack.length--;\
            return allocator->free_stack.ptr[free_length-1];\
        }\
        else if((void*)(allocator->head) \
            < (void*)(allocator->underlying_buffer.ptr \
                    + allocator->underlying_buffer.length))\
        {\
            return (T*)allocator->head++;\
        }\
        else\
        {\
            klog_printf("page_alloc: Out of memory!");\
        }\
        return 0;\
    }\
    \
    T* allocator_##T##_free(allocator_##T *const allocator, T * const ptr)\
    {\
        u32 free_length = allocator->free_stack.length;\
        if(free_length && \
              (void*)(allocator->free_stack.ptr\
                    + allocator->free_stack.length)\
            < (void*)(allocator->underlying_buffer.ptr\
                    + allocator->underlying_buffer.length))\
        {\
            allocator->free_stack.ptr[free_length] = ptr;\
            return 0;\
        }\
        else if(\
                (void*)(allocator->head) \
              < (void*)(allocator->underlying_buffer.ptr\
                      + allocator->underlying_buffer.length))\
        {\
            allocator->free_stack.ptr = (T**) allocator->head;\
            allocator->free_stack.length = 1;\
            allocator->free_stack.ptr[0] = ptr;\
            return 0;\
        }\
        else\
        {\
            klog_printf("page_free: Out of memory!");\
        }\
        return ptr;\
    }\
    \
    int allocator_##T##_is_full(allocator_##T const allocator)\
    {\
        return allocator.head >= \
            ( allocator.underlying_buffer.ptr \
            + allocator.underlying_buffer.length) ||\
            ( allocator.free_stack.length \
                &&  (void*)(allocator.free_stack.ptr\
                          + allocator.free_stack.length) >=\
                    (void*)(allocator.underlying_buffer.ptr\
                          + allocator.underlying_buffer.length));\
    }\
    \
    int allocator_##T##_can_alloc(allocator_##T const allocator)\
    {\
        return allocator.free_stack.length || \
                (void*)(allocator.head)\
              < (void*)(allocator.underlying_buffer.ptr\
                     + allocator.underlying_buffer.length);\
    }\
    \
    int allocator_##T##_can_free(allocator_##T const allocator)\
    {\
        return allocator.free_stack.length && \
              (void*)(allocator.free_stack.ptr \
                    + allocator.free_stack.length)\
            < (void*)(allocator.underlying_buffer.ptr\
                    + allocator.underlying_buffer.length);\
    }

decl_allocator(page_allocator);

CR3 get_cr3();
void set_cr3(u32 val);

page_directory* get_page_directory(CR3 cr3);

int page_init(void);

#endif