/*
    ? heap_validate_table:
    * takes in a pointer to a heap table, a pointer to the end of heap and a heap_table struct
    * checks if the total number of blocks in the table is equal to the size of memory space divided by the block size
    * returns 0 if the table is valid, -EINVARG otherwise

    ? heap_validate_alignment:
    * takes in a pointer to a heap
    * checks if the pointer is aligned to the block size
    * returns true if aligned, false otherwise

    ? heap_create:
    * takes in a pointer to a heap struct, a pointer to the start of the heap,
    * a pointer to the end of the heap, and a pointer to a heap table struct
    * if the table is valid,
    * it initializes the table by setting all entries to HEAP_BLOCK_TABLE_ENTRY_FREE

    ? heap_align_value_to_upper:
    * takes in an uint32_t value
    * aligns the value to the block size by
    * adding block size to the value if its not already aligned

    ? heap_get_entry_type:
    * takes in a HEAP_BLOCK_TABLE_ENTRY
    * returns the type of the entry by masking the lower 4 bits

    ? heap_get_start_block:
    * takes in a pointer to a heap struct and a uint32_t total number of blocks
    * traverses the heap table and finds the first continuous free blocks that are
    * equal to the total number of blocks requested
    * returns the starting block of the free memory space, -ENOMEM if not enough memory


    ? heap_block_to_address:
    * takes in a pointer to a heap struct and an int block number
    * returns the starting address of the block

    ? heap_mark_blocks_taken:

    * takes in a pointer to a heap struct, int starting block number and int total number of blocks
    * marks the blocks in the heap table as taken by setting the entries to HEAP_BLOCK_TABLE_ENTRY_TAKEN
    * sets the first block as HEAP_BLOCK_IS_FIRST, and sets the HAS_NEXT bit if there are more blocks\
*/

#include "heap.h"
#include "kernel.h"
#include <stdbool.h>
#include "memory/memory.h"
#include "status.h"

static int heap_validate_table(void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / viros_HEAP_BLOCK_SIZE;

    if (table->total != total_blocks)
    {
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}

static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % viros_HEAP_BLOCK_SIZE == 0);
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);
    if (res < 0)
    {
        goto out;
    }

    // Initialize the table
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val)
{
    if (val % viros_HEAP_BLOCK_SIZE == 0)
    {
        return val;
    }

    val = (val - (val % viros_HEAP_BLOCK_SIZE)); // Align to lower
    val += viros_HEAP_BLOCK_SIZE;                // Align to upper

    return val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}

int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
    struct heap_table *table = heap->table;
    int bc = 0;  // Block count
    int bs = -1; // Block start

    for (size_t i = 0; i < table->total; i++)
    {
        // If the entry is not free, reset the block count and start
        if (heap_get_entry_type(table->entries[i] != HEAP_BLOCK_TABLE_ENTRY_FREE))
        {
            bc = 0;
            bs = -1;
            continue;
        }

        // If this is the first block, set the start
        if (bs == -1)
        {
            bs = i;
        }
        bc++;
        if (bc == total_blocks)
        {
            break;
        }
    }

    if (bs == -1)
    {
        return -ENOMEM;
    }

    return bs;
}

void *heap_block_to_address(struct heap *heap, int block)
{
    return heap->saddr + (block * viros_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap *heap, int start_block, int total_blocks)
{
    int end_block = (start_block + total_blocks) - 1;

    // Mark the first block
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT; // Set the next bit
    }

    // Mark the blocks as taken
    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
    void *address = 0;

    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0)
    {
        goto out;
    }

    address = heap_block_to_address(heap, start_block);

    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
    return address;
}

void heap_mark_blocks_free(struct heap *heap, int starting_block)
{
    struct heap_table *table = heap->table;

    for (int i = starting_block; i < (int)table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

int heap_address_to_block(struct heap *heap, void *address)
{
    return ((int)(address - heap->saddr)) / viros_HEAP_BLOCK_SIZE;
}

void *heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / viros_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr)
{
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}