#include "page.h"

page_directory* get_page_directory(CR3 cr3)
{
    return (page_directory*)(void*)
        (((u32)cr3.page_directory_4k_aligned) & ~0xfff);
}

static void swap(void** l, void** r)
{
    void* tmp = l;
    *l = *r;
    *r = tmp;
}

page_indirection* page_alloc(page_allocator *const allocator)
{
    u32 free_length = allocator->free_stack.length;
    if(free_length)
    {
        swap((void**)&allocator->free_stack.ptr[0],
             (void**)&allocator->free_stack.ptr[free_length-1]);
        allocator->free_stack.length--;
        //Use old length to get the element that just fell out of the freestack
        return allocator->free_stack.ptr[free_length-1];
    }
    else if((void*)(allocator->head) 
        < (void*)(allocator->underlying_buffer.ptr 
                + allocator->underlying_buffer.length))
    {
        return (page_indirection*)allocator->head++;
    }
    else
    {
        klog_printf("page_alloc: Out of memory!\n");
    }
    return 0;
}

page_indirection* page_free(page_allocator *const allocator,
     page_indirection * const page_ind)
{
    u32 free_length = allocator->free_stack.length;
    if(free_length && 
        (void*)(allocator->free_stack.ptr + allocator->free_stack.length)
            < (void*)(allocator->underlying_buffer.ptr 
                    + allocator->underlying_buffer.length))
    {
        allocator->free_stack.ptr[free_length] = page_ind;
        return 0;
    }
    else if((void*)(allocator->head) 
        < (void*)(allocator->underlying_buffer.ptr
                + allocator->underlying_buffer.length))
    {
        allocator->free_stack.ptr = (page_indirection**) allocator->head;
        allocator->free_stack.length = 1;
        allocator->free_stack.ptr[0] = page_ind;
        return 0;
    }
    else
    {
        klog_printf("page_free: Out of memory!\n");
    }
    return page_ind;
}

//TODO: Check this
static void* next_page = (void*)0x100000;
static u32 alloc_0_buffer[0x400];
static page_allocator kp_allocator = 
{
    .head = (page_indirection*)&alloc_0_buffer,
    .free_stack = 
    {
        .ptr=0,
        .length=0
    },
    .underlying_buffer =
    {
        .ptr = (page_indirection*)&alloc_0_buffer,
        .length = 0x400
    }
};

static allocator_page_allocator kp_allocator_allocator = 
{
    .head = 0,
    .free_stack = 
    {
        .ptr=0,
        .length=0
    },
    .underlying_buffer =
    {
        .ptr = 0,
        .length = 0
    }
};

page_indirection* kpalloc()
{
    page_indirection* next_page_ref = page_alloc(&kp_allocator);
    if(!next_page_ref)
    {
        page_allocator *next_allocator = 
            allocator_page_allocator_alloc(&kp_allocator_allocator);
        next_allocator->head = next_page;
        next_allocator->free_stack.ptr = 0;
        next_allocator->free_stack.length = 0;
        next_allocator->underlying_buffer.ptr = next_page;
        next_allocator->underlying_buffer.length = 
            0x1000 / sizeof(page_allocator);

        kp_allocator = *next_allocator;

        next_page += PAGE_SIZE;
        
        return kpalloc();
    }
    else
    {
        next_page_ref->page = next_page;
        next_page += PAGE_SIZE;
        klog_printf("Allocated page at %8x, indirecting to %8x\n",
            next_page_ref, next_page_ref->page);
    }

    return next_page_ref;
}

page_indirection* kpfree(page_indirection *const page_ind)
{
    return page_free(&kp_allocator, page_ind);
}

int page_allocator_is_full(page_allocator const allocator)
{
    return allocator.head >= 
        (allocator.underlying_buffer.ptr + allocator.underlying_buffer.length)
        || (allocator.free_stack.length 
          &&(void*)(allocator.free_stack.ptr +
                    allocator.free_stack.length) >=
            (void*)(allocator.underlying_buffer.ptr + 
                    allocator.underlying_buffer.length));
}

int page_allocator_can_alloc(page_allocator const allocator)
{
    return allocator.free_stack.length || 
        (void*)(allocator.head) 
        < (void*)(allocator.underlying_buffer.ptr 
                + allocator.underlying_buffer.length);
}

int page_allocator_can_free(page_allocator const allocator)
{
    return allocator.free_stack.length && 
          (void*)(allocator.free_stack.ptr
                + allocator.free_stack.length) 
        < (void*)(allocator.underlying_buffer.ptr 
                + allocator.underlying_buffer.length);
}

///////////////////////////////////////////////////////////////////////////////

mk_allocator(page_allocator);

int page_init(void)
{
    page_indirection *alloc_alloc_page = kpalloc();
    kp_allocator_allocator.head = (page_allocator*) alloc_alloc_page->page;

    kp_allocator_allocator.underlying_buffer.ptr = 
        (page_allocator*) alloc_alloc_page->page;
    kp_allocator_allocator.underlying_buffer.length = 
        0x1000 / sizeof(page_allocator);

    
    klog_printf("page: initialised\n");

    //Example
    //for(int i = 0; i < 0x402; i++) kpalloc();
    return 1;
}
