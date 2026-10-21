#include <proto/exec.h>
#include "cma.h"
#include "tlsf.h"

APTR  L_AllocMem(REGARG(ULONG byte_size, "d0"), REGARG(struct CMABase *CMABase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;
    APTR result = NULL;

    ObtainSemaphore(&CMABase->cma_Lock);
    result = tlsf_malloc(&CMABase->cma_TLSF, byte_size);
    ReleaseSemaphore(&CMABase->cma_Lock);

    return result;
}

APTR  L_AllocAligned(REGARG(ULONG byte_size, "d0"), REGARG(ULONG alignment, "d1"), REGARG(struct CMABase *CMABase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;
    APTR result = NULL;

    ObtainSemaphore(&CMABase->cma_Lock);
    result = tlsf_malloc_aligned(&CMABase->cma_TLSF, byte_size, alignment);
    ReleaseSemaphore(&CMABase->cma_Lock);

    return result;
}

APTR  L_ReallocMem(REGARG(APTR buffer, "a0"), REGARG(ULONG new_size, "d0"), REGARG(struct CMABase *CMABase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;
    APTR result = NULL;

    ObtainSemaphore(&CMABase->cma_Lock);
    result = tlsf_realloc(&CMABase->cma_TLSF, buffer, new_size);
    ReleaseSemaphore(&CMABase->cma_Lock);

    return result;
}

APTR  L_ReallocAligned(REGARG(APTR buffer, "a0"), REGARG(ULONG new_size, "d0"), REGARG(ULONG align, "d1"), REGARG(struct CMABase *CMABase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;
    APTR result = NULL;

    ObtainSemaphore(&CMABase->cma_Lock);
    result = tlsf_realloc_aligned(&CMABase->cma_TLSF, buffer, new_size, align);
    ReleaseSemaphore(&CMABase->cma_Lock);

    return result;
}

VOID  L_FreeMem(REGARG(APTR buffer, "a0"), REGARG(struct CMABase *CMABase, "a6"))
{
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;

    ObtainSemaphore(&CMABase->cma_Lock);
    tlsf_free(&CMABase->cma_TLSF, buffer);
    ReleaseSemaphore(&CMABase->cma_Lock);
}
