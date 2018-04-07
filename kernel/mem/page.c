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

page_indirection* kpalloc(void)
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
        //klog_printf("Allocated page at %8x, indirecting to %8x\n",
        //    next_page_ref, next_page_ref->page);
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

static void setup_paging(void)
{
    //Set up root directory
    page_directory* pd = (page_directory*) (kpalloc())->page;
    for(int i = 0; i < 0x1; i++)
    {
        page_directory_entry pde;
        page_table* pt = (page_table*) (kpalloc()->page);

        for(int j = 0; j < 0x3ff; j++)
        {
            page_table_entry page;
            page.page_address_4k_aligned = ((u32)i*0x400000+j*0x1000)>>12;
            page.has_been_accessed = 0;
            page.available = 0;
            page.is_present = 1;
            page.is_cache_disabled = 0;
            page.is_dirty = 0;
            page.is_user = 0;
            page.is_write_through = 0;
            page.is_writeable = 1;

            pt->pte[j] = page;
        }
        {
            page_table_entry page;
            page.page_address_4k_aligned = ((u32)pd)>>12;
            page.has_been_accessed = 0;
            page.available = 0;
            page.is_present = 1;
            page.is_cache_disabled = 0;
            page.is_dirty = 0;
            page.is_user = 0;
            page.is_write_through = 0;
            page.is_writeable = 1;

            pt->pte[0x400] = page;
        }

        pde.page_table_address_4k_aligned = (((u32)pt) >> 12);
        pde.available = 0;
        pde.has_been_accessed = 0;
        pde.ignored_1 = 0;
        pde.is_4MB_size = 0;
        pde.is_cache_disabled = 0;
        pde.is_present = 1;
        pde.is_user = 0;
        pde.is_write_through = 0;
        pde.is_writeable = 1;
        pde.reserved_1 = 0;
        
        pd->pde[i] = pde;
    }
    pd->pde[0x200] = pd->pde[0];
    pd->pde[0x201] = pd->pde[0];
    /*kprintf("Has assignment worked? %p -> %p\n", pd->pde[0], pd->pde[0x200]);
    kprintf("Virtual memory is now overmapped:\n"
            "\tpd[0x000] -----> pt[0] --+--> p[0x000]\n"
            "\tpd[ ... ] --x     ^      |--> p[0x001]\n"
            "\tpd[0x200] --------+      |--> p[ ... ]\n");*/


    CR3 cr3;
    cr3.page_directory_4k_aligned = ((u32)pd) >> 12;
    set_cr3(cr3);

    CR0 cr0 = get_cr0();
    cr0.enable_paging = 1;
    cr0.protected_mode = 1;
    cr0.ring_0_write_protection = 1;
    set_cr0(cr0);

    enter_high_half();
}

int page_init(void)
{
    
    page_indirection *alloc_alloc_page = kpalloc();
    kp_allocator_allocator.head = (page_allocator*) alloc_alloc_page->page;

    kp_allocator_allocator.underlying_buffer.ptr = 
        (page_allocator*) alloc_alloc_page->page;
    kp_allocator_allocator.underlying_buffer.length = 
        0x1000 / sizeof(page_allocator);

    klog_printf("page: allocator initialised\n");

    //irq_enter_high_half();
    //setup_paging();

    klog_printf("page: virtual paging established\n");

    //Example
    //for(int i = 0; i < 0x402; i++) kpalloc();
    return 1;
}
