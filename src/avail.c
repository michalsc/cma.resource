#include <proto/exec.h>
#include "cma.h"
#include "tlsf.h"

ULONG L_Avail(REGARG(struct CMABase *CMABase, "a6"))
{
    ULONG avail;
    struct ExecBase *SysBase = *(struct ExecBase**)4UL;
    ObtainSemaphore(&CMABase->cma_Lock);
    avail = tlsf_get_free_size(&CMABase->cma_TLSF);
    ReleaseSemaphore(&CMABase->cma_Lock);
    return avail;
}
