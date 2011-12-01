/**********************************************************************
 * Copyright ( C ) 2002-2005 XIMETA, Inc.
 * All rights reserved.
 **********************************************************************/
#include <sysmem.h>
#include <intrman.h>
#include "sal/types.h"
#include "sal/libc.h"
#include "sal/mem.h"
#include "sal/debug.h"


//#define MEM_DEBUG 1

#ifdef MEM_DEBUG
#define MAX_NUM_MEM_BLOCKS 2048
#define MAX_NUM_MEM_SIZES  512

typedef struct
{
    void * addr;
    unsigned int size;
} MEM_BLOCK_ENTRY;

typedef struct
{
    int      count;
    int     accum_count;
    unsigned int  size;
} MEM_BLOCK_COUNTER;

static MEM_BLOCK_ENTRY v_mem_blocks[MAX_NUM_MEM_BLOCKS];
static MEM_BLOCK_COUNTER v_mem_counters[MAX_NUM_MEM_SIZES];
static long v_mem_total_allocated = 0;
static unsigned long v_total_mallocs = 0;
static unsigned long v_total_frees = 0;

static void _SalMemAdjustSizeCounter(unsigned int size, int adj)
{
    int i;
    for(i=0;i<MAX_NUM_MEM_SIZES;i++) {
        if (v_mem_counters[i].size==0) { /* new entry */
            v_mem_counters[i].size = size; 
        } 
        if (v_mem_counters[i].size == size) {
            v_mem_counters[i].count += adj; 
            if (adj>0)
                v_mem_counters[i].accum_count +=adj;
            break;
        }
    }
    sal_assert(i<MAX_NUM_MEM_SIZES);
}
#endif


void *sal_malloc(unsigned int size)
{
#ifdef MEM_DEBUG
    int i;
#endif
    void* ptr;
    int intrStat;
    
    CpuDisableIntr(&intrStat);
    ptr = AllocSysMemory(ALLOC_FIRST, size, NULL);
    CpuEnableIntr(intrStat);
    
    if (ptr==NULL) {
        sal_debug_print("memory exhausted, request was for %d\n", size);
        sal_mem_display(0);
        return ptr;
    }
#ifdef MEM_DEBUG        
    for (i = 0; i < MAX_NUM_MEM_BLOCKS; i++)
    {
        if (v_mem_blocks[i].addr == 0)
        {
            // found no_pending_dpc entry; use it 
            v_mem_blocks[i].addr = ptr;
            v_mem_blocks[i].size = size;
            _SalMemAdjustSizeCounter(size, 1);
            break;
        }
    }
    sal_assert(i < MAX_NUM_MEM_BLOCKS);
    v_mem_total_allocated += size;    
    v_total_mallocs++;
#endif    
    return ptr;
}

void sal_free(void *ptr)
{
#ifdef MEM_DEBUG    
    int i;
    for (i = 0; i < MAX_NUM_MEM_BLOCKS; i++)
    {
        if (v_mem_blocks[i].addr == ptr)
        {
            // found block being released 
            _SalMemAdjustSizeCounter(v_mem_blocks[i].size, -1);
            v_mem_total_allocated -= v_mem_blocks[i].size;
            v_mem_blocks[i].addr = 0;
            v_mem_blocks[i].size = 0;
            break;
        }
    }
    sal_assert(i < MAX_NUM_MEM_BLOCKS);
    v_total_frees++;
#endif    
    FreeSysMemory(ptr);
}

void sal_mem_display(int verbose)
{
#ifdef MEM_DEBUG    
    int i;
    xuint32 freemem;
    sal_debug_print("\nSize\tFreq.\tAccum.");
    for (i = 0; i < MAX_NUM_MEM_SIZES; i++)
    {
        if (v_mem_counters[i].size == 0) 
            break;
        sal_debug_print("%-10d  %-10d  %-10d", 
            v_mem_counters[i].size, v_mem_counters[i].count, v_mem_counters[i].accum_count);
    }
    if (verbose>=1) {
        sal_debug_print("\nAddress\t\tSize");
        for (i = 0; i < MAX_NUM_MEM_BLOCKS; i++) {
            if (v_mem_blocks[i].addr != 0) {
                sal_debug_print("%p%-10d",v_mem_blocks[i].addr, v_mem_blocks[i].size);
            }
        }
    }
    sal_debug_print("\nTotal memory allocated: %ld\n", v_mem_total_allocated);
    freemem = QueryTotalFreeMemSize();
    sal_debug_print("Total free memory: %u\n", freemem);
    sal_debug_print("Total mallocs: %lu, total frees: %lu\n", v_total_mallocs, v_total_frees);
#else
    sal_debug_print("MEM_DEBUG flags is not turned on\n");
#endif        
}
/*
 * copy the user memory space data into kernel memory space.
 * @param kernel the kernel space memory to be copied into
 * @param user the user space memory to be copied from
 * @param size the size of data
 * @return the pointer to be used as a result after copying the data
 *     NULL if error.
 */
void* sal_copy_from_user(void* kernel,void* user,int size)
{
    memcpy(kernel,user,size);
    return kernel;
}
/*
 * copy the kernel memory space data into user memory space.
 * @param kernel the kernel space memory to be copied into
 * @param user the user space memory to be copied from
 * @param size the size of data
 * @return the pointer to be used as a result after copying the data
 *     NULL if error.
 */
void* sal_copy_to_user(void* user, void* kernel, int size)
{
    memcpy(user,kernel,size);
    return user;
}
