#ifndef _CMA_H
#define _CMA_H

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <common/compiler.h>

#include "compiler.h"
#include "tlsf.h"

#define CMA_PRIORITY     118
#define CMA_VERSION      1
#define CMA_REVISION     0

struct CMABase {
    struct Library          cma_Node;
    struct ExecBase *       cma_ExecBase;
    struct SignalSemaphore  cma_Lock;
    tlsf_t                  cma_TLSF;
};

#define NUMBER_OF_FUNCTIONS 5
#define BASE_NEG_SIZE (((NUMBER_OF_FUNCTIONS * 6) + 3) & ~3)
#define BASE_POS_SIZE ((sizeof(struct CMABase)))

APTR  L_AllocMem(REGARG(ULONG byte_size, "d0"), REGARG(struct CMABase *CMABase, "a6"));
APTR  L_AllocAligned(REGARG(ULONG byte_size, "d0"), REGARG(ULONG alignment, "d1"), REGARG(struct CMABase *CMABase, "a6"));
APTR  L_ReallocMem(REGARG(APTR buffer, "a0"), REGARG(ULONG new_size, "d0"), REGARG(struct CMABase *CMABase, "a6"));
VOID  L_FreeMem(REGARG(APTR buffer, "a0"), REGARG(struct CMABase *CMABase, "a6"));
ULONG L_Avail(REGARG(struct CMABase *CMABase, "a6"));

#endif /* _CMA_H */
