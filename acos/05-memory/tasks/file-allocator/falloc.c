#include "falloc.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h> // ffsll find first least significant bit

#define error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

bool GetBit(int64_t x, int index) {
    return (x >> index) & 1;
}

void falloc_init(file_allocator_t* allocator, const char* filepath, uint64_t allowed_page_count) {
    int result; // TODO: error checking
    int fd = open(filepath, O_CREAT | O_RDWR, 0777);
    if (fd == -1) {
        error("open");
    }
    allocator->fd =  fd;
    allocator->allowed_page_count = allowed_page_count;
    allocator->curr_page_count = 0;
    result = ftruncate(fd, PAGE_SIZE * (allowed_page_count + 1)); // One more page for PAGE_MASK_SIZE array
    if (result < 0) {
        error("ftruncate");
    }
    allocator->page_mask = (uint64_t*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (allocator->page_mask == MAP_FAILED) {
        error("mmap");
    }
    allocator->base_addr = mmap(NULL, PAGE_SIZE * allowed_page_count, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PAGE_SIZE);
    if (allocator->base_addr == MAP_FAILED) {
        error("mmap");
    }
    for (int i = 0; i < (allowed_page_count + 63) / 64; ++i) {
        for (int j = 0; j < 64; ++j) {
            if (GetBit(allocator->page_mask[i], j)) {
                ++allocator->curr_page_count;
            }
        }
    }
}

void falloc_destroy(file_allocator_t* allocator) {
    int result;
    result = munmap(allocator->page_mask, PAGE_SIZE);
    if (result < 0) {
        error("munmap");
    }
    result = munmap(allocator->base_addr, PAGE_SIZE);
    if (result < 0) {
        error("munmap");
    }

    result = close(allocator->fd);
    if (result < 0) {
        error("close");
    }

    // Set to default
    allocator->base_addr = NULL;
    allocator->page_mask = NULL;
    allocator->curr_page_count = 0;
    allocator->allowed_page_count = 0;
}

void* falloc_acquire_page(file_allocator_t* allocator) {
    if (allocator->curr_page_count == allocator->allowed_page_count) {
        return NULL;
    }

    for (int64_t i = 0; i < (allocator->allowed_page_count + 63 / 64); ++i) {
        int64_t pos = ffsll(~(allocator->page_mask[i])) - 1; // inverting zeroes to one
        if (pos != -1) {
            ++allocator->curr_page_count;
            allocator->page_mask[i] |= (1ull << pos); // setting found bit to 1
            void* addr = mmap(allocator->base_addr + (64 * i + pos) * PAGE_SIZE, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED |
                                MAP_FIXED, allocator->fd, (64 * i + pos + 1) * PAGE_SIZE);
            if (addr == MAP_FAILED) {
                error("mmap");
            }
            return addr;
        }
    }
    return NULL;
}

void falloc_release_page(file_allocator_t* allocator, void** addr) {
    int result;
    result = munmap(*addr, PAGE_SIZE);
    if (result < 0) {
        error("munmap");
    }
    int64_t diff = (*addr - allocator->base_addr) / PAGE_SIZE; // in count of pages
    allocator->page_mask[diff / 64] ^= (1 << (diff % 64));
    --allocator->curr_page_count;
    *addr = NULL;
}
