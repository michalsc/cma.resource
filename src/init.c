
#include <exec/types.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/devicetree.h>

#include <libraries/configregs.h>
#include <libraries/configvars.h>

#include "compiler.h"
#include "cma.h"

extern UBYTE rom_end;
extern const char deviceName[];
extern const char deviceIdString[];

APTR Init(REGARG(struct ExecBase *SysBase, "a6"))
{
    struct CMABase *CMABase = NULL;
    struct ExpansionBase *ExpansionBase = NULL;
    struct CurrentBinding binding;
    BOOL success = FALSE;

    APTR base_pointer = NULL;
    
    ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library", 0);
    GetCurrentBinding(&binding, sizeof(binding));

    base_pointer = AllocMem(BASE_NEG_SIZE + BASE_POS_SIZE, MEMF_PUBLIC | MEMF_CLEAR);

    if (base_pointer)
    {
        ULONG relFuncTable[NUMBER_OF_FUNCTIONS + 1];

        relFuncTable[0] = (ULONG)&L_AllocMem;
        relFuncTable[1] = (ULONG)&L_AllocAligned;
        relFuncTable[2] = (ULONG)&L_ReallocMem;
        relFuncTable[3] = (ULONG)&L_FreeMem;
        relFuncTable[4] = (ULONG)&L_Avail;

        relFuncTable[NUMBER_OF_FUNCTIONS] = (ULONG)-1;

        CMABase = (struct CMABase *)((UBYTE *)base_pointer + BASE_NEG_SIZE);
        MakeFunctions(CMABase, relFuncTable, 0);
        
        CMABase->cma_Node.lib_Node.ln_Type = NT_RESOURCE;
        CMABase->cma_Node.lib_Node.ln_Pri = CMA_PRIORITY;
        CMABase->cma_Node.lib_Node.ln_Name = (STRPTR)deviceName;

        CMABase->cma_Node.lib_NegSize = BASE_NEG_SIZE;
        CMABase->cma_Node.lib_PosSize = BASE_POS_SIZE;
        CMABase->cma_Node.lib_Version = CMA_VERSION;
        CMABase->cma_Node.lib_Revision = CMA_REVISION;
        CMABase->cma_Node.lib_IdString = (STRPTR)deviceIdString;

        CMABase->cma_ExecBase = SysBase;

        InitSemaphore(&CMABase->cma_Lock);

        APTR DeviceTreeBase = OpenResource("devicetree.resource");

        if (DeviceTreeBase != NULL)
        {
            /* Find out if user provided some uncached memory for use of this resource */
            APTR key = DT_OpenKey("/emu68/cma@0");

            if (key)
            {
                const ULONG *reg = DT_GetPropValue(DT_FindProperty(key, "reg"));

                DT_CloseKey(key);
               
                /* DeviceTree entry is there, see if the memory is available */
                if (reg != NULL && reg[0] != 0 && reg[1] != 0) {
                    tlsf_add_memory(&CMABase->cma_TLSF, (APTR)reg[0], reg[1]);

                    SumLibrary((struct Library*)CMABase);
                    AddResource(CMABase);

                    binding.cb_ConfigDev->cd_Flags &= ~CDF_CONFIGME;
                    binding.cb_ConfigDev->cd_Driver = CMABase;

                    success = TRUE;
                }
            }
        }
    }

    /* If anything failed, clean up now */
    if (!success) {
        FreeMem(base_pointer, BASE_NEG_SIZE + BASE_POS_SIZE);
        CMABase = NULL;
    }

    CloseLibrary((struct Library*)ExpansionBase);

    return CMABase;
}
