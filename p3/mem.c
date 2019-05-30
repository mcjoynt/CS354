////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      n/a
// Semester:         CS 354 Spring 2019
//
// Author:           Matthew McJoynt
// Email:            mmcjoynt@wisc.edu
// CS Login:         mcjoynt
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          n/a
//
// Online sources:   n/a
//
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct block_header {
        int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    *
    * End Mark:
    *  The end of the available memory is indicated using a size_status of 1.
    *
    * Examples:
    *
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    *
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} block_header;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

block_header *start_block = NULL;

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use BEST-FIT PLACEMENT POLICY to find the block closest to the required block size
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic.
 */
void* Alloc_Mem(int size) {

	size += sizeof(block_header);

//	int totalmem = start_block->size_status - (start_block->size_status & 3) + 10;
	int totalmem = 4096;//need to generalize

	if(size <= 0 || size > totalmem - 1){
		return NULL;
	}

	//make a multiple of 8
	while(size % 8 != 0){
		size++;
	}

	block_header *cur = start_block;
	block_header *best = NULL;
	int sum = 0;

	while(sum < totalmem) {

		int curSize = (cur->size_status) - (cur->size_status & 3);

		if((cur->size_status & 1) == 0) { //free block
			if(curSize >= size){
				totalmem = curSize;
				best = cur;
			}
		}
		cur = (block_header*)((char*) cur + sizeof(block_header));

		sum += curSize;
	}

	if(best == NULL) {
		return NULL;
	}

	best->size_status += 1;

	if(totalmem >= size + 16) {
		int stat = totalmem - size + 2;
		block_header *header = (block_header*)((char*) best + size);
		header->size_status = stat;
		block_header *footer = (block_header*)((char*) best + totalmem - 4);
		footer->size_status = stat - 2;
		block_header *next = best;
		next->size_status = size + 3;
	} else {
		block_header *next = best;
		next->size_status = totalmem + 3;
		block_header *nextHead = NULL;
		nextHead = (block_header*)((char*) next + totalmem);
		nextHead->size_status += 2;
	}

	return (void*)best + 4;
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */
int Free_Mem(void *ptr) {
	block_header *free = (block_header*)((char*)ptr - 4);
	block_header *lastFoot = NULL;
	block_header *lastBlock = NULL;

	free->size_status = free->size_status - 1;

	if((free->size_status & 2) == 0) {
		lastFoot = (block_header*)((char*)free - 4);
		lastBlock = (block_header*)((char*)lastFoot - lastFoot->size_status + sizeof(block_header));
		int coalesce = lastBlock->size_status + free->size_status - 2;
		free = lastBlock;
		free->size_status = coalesce + 2;
	}

	block_header *next = (block_header*)((char*)free + free->size_status - 2);
	next->size_status = next->size_status -  2;

	if((next->size_status & 1) == 0)
		free->size_status += next->size_status;

	next = (block_header*)((char*)free + free->size_status - 2);
	block_header* freeFooter = (block_header*)((char*)free + free->size_status - 6);
	freeFooter->size_status = free->size_status - 2;

	return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int Init_Mem(int sizeOfRegion) {

    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    block_header* end_mark;
    static int allocated_once = 0;

    if (0 != allocated_once) {
        fprintf(stderr,
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that start block meets
    // double word alignement requirement
    start_block = (block_header*) space_ptr + 1;
    end_mark = (block_header*)((void*)start_block + alloc_size);

    // Setting up the header
    start_block->size_status = alloc_size;

    // Marking the previous block as used
    start_block->size_status += 2;

    // Setting up the end mark and marking it as used
    end_mark->size_status = 1;

    // Setting up the footer
    block_header *footer = (block_header*) ((char*)start_block + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
}

/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void Dump_Mem() {
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    block_header *current = start_block;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status,
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (block_header*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
