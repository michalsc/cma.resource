#ifndef _TLSF_H
#define _TLSF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TLSF_MULTITHREADING 1


/*
 * IMPORTANT! This TLSF allocator is now fine-tuned for maintained memory area
 * smaller than 1GB. Use with caution if re-designing it back for large memory
 * blocks!!!
 */

/*
 * Minimal alignment as required by AROS. In contrary to the default
 * TLSF implementation, we do not allow smaller blocks here.
 * Size needs to be aligned to at least 8, see THIS_FREE_MASK comment.
 */
#define SIZE_ALIGN      16

/*
 * Maximum alignment allowed by tlsf_malloc_aligned function.
 */
#define MAX_ALIGNMENT   2*1024*1024

/*
 * Settings for TLSF allocator:
 * MAX_LOG2_SLI - amount of bits used for the second level list
 * MAX_FLI      - maximal allowable allocation size - 2^32 should be enough on 32bit systems
 *                64bit systems use 512GB limit.
 */
#define MAX_LOG2_SLI    (5)
#define MAX_SLI         (1 << MAX_LOG2_SLI)
#define MAX_FLI         (30)
#define FLI_OFFSET      (6)
#define SMALL_BLOCK     (2 << FLI_OFFSET)

#define REAL_FLI        (MAX_FLI - FLI_OFFSET)

#define ROUNDUP(x)      (((x) + SIZE_ALIGN - 1) & ~(SIZE_ALIGN - 1))
#define ROUNDDOWN(x)    ((x) & ~(SIZE_ALIGN - 1))

/* Fields used in the block header length field to identify busy/free blocks */
#define THIS_FREE_MASK (uintptr_t)1
#define THIS_FREE   (uintptr_t)1
#define THIS_BUSY   (uintptr_t)0

#define PREV_FREE_MASK (uintptr_t)2
#define PREV_FREE   (uintptr_t)2
#define PREV_BUSY   (uintptr_t)0

#define SIZE_MASK   (~(THIS_FREE_MASK | PREV_FREE_MASK))

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Size of additional memory needed to manage new block */
#define HEADERS_SIZE (((3 * ROUNDUP(sizeof(hdr_t))) + ROUNDUP(sizeof(tlsf_area_t))))

/* free node links together all free blocks of similar size */
typedef struct free_node_s {
    struct bhdr_s *    prev;
    struct bhdr_s *    next;
} free_node_t;

/* block header in front of each block - both free and busy */
typedef struct hdr_s {
    struct bhdr_s * prev;
    uintptr_t       length;
} hdr_t;

/*
 * Each block is defined by bhdr_t structure. Free blocks contain only
 * the header which allows us to go through all memory blocks in the system.
 * The free blocks contain additionally the node which chains them in one
 * of the free block lists
 */
typedef struct bhdr_s {
    union {
        hdr_t       header;
        uint8_t     __min_align[SIZE_ALIGN];
    };
    union {
        uint8_t         mem[1];
        free_node_t     free_node;
    };
} bhdr_t;

/* Memory area within the TLSF pool */
typedef struct tlsf_area_s {
    struct tlsf_area_s *    next;       // Next memory area
    bhdr_t*                 end;        // Pointer to "end-of-area" block header
} tlsf_area_t;

typedef struct {
    tlsf_area_t*        memory_area;

    uintptr_t           total_size;
    uintptr_t           free_size;

    uint32_t            flags;
    uint32_t            flbitmap;
    uint32_t            slbitmap[REAL_FLI];

    bhdr_t*             matrix[REAL_FLI][MAX_SLI];
} tlsf_t;

#include "cma.h"

void* tlsf_init();
void tlsf_set_flags(void *handle, uint32_t flags);
void tlsf_add_memory(void* t, void* memory, uintptr_t size);

void* tlsf_malloc(void* handle, uintptr_t size);
void* tlsf_malloc_aligned(void* handle, uintptr_t size, uintptr_t align);
void tlsf_free(void* handle, void* ptr);
void* tlsf_realloc(void* handle, void* ptr, uintptr_t new_size);
void* tlsf_init_with_memory(void* memory, uintptr_t size);
void *tlsf_realloc_aligned(void *t, void *ptr, uintptr_t new_size, uintptr_t align);

uintptr_t tlsf_get_total_size(void* memory);
uintptr_t tlsf_get_free_size(void* memory);

#ifdef __cplusplus
}

namespace Emu68 {

class TLSF {
public:
    TLSF(void* memory, uintptr_t size) { if (memory != NULL) { handle = tlsf_init_with_memory(memory, size); } }
    TLSF(void* handle) : handle(handle) {}

    void* malloc(uintptr_t size) { return tlsf_malloc(handle, size); }
    void* malloc_aligned(uintptr_t size, uintptr_t align) { return tlsf_malloc_aligned(handle, size, align); }
    void* realloc(void* ptr, uintptr_t new_size) { return tlsf_realloc(handle, ptr, new_size); }
    void free(void* ptr) { tlsf_free(handle, ptr); }
    uintptr_t total_size() { return tlsf_get_total_size(handle); }
    uintptr_t free_size() { return tlsf_get_free_size(handle); }

private:
    void *handle;
};

} // namespace Emu68

#endif

#endif /* _TLSF_H */
