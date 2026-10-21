
#include <exec/types.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/devicetree.h>

#include <libraries/configregs.h>
#include <libraries/configvars.h>

#include <common/compiler.h>

#include "cma.h"

extern UBYTE rom_end;
extern const char deviceName[];
extern const char deviceIdString[];

/*
    Some properties, like e.g. #size-cells, are not always available in a key, but in that case the properties
    should be searched for in the parent. The process repeats recursively until either root key is found
    or the property is found, whichever occurs first
*/
CONST_APTR GetPropValueRecursive(APTR key, CONST_STRPTR property, APTR DeviceTreeBase)
{
    do {
        /* Find the property first */
        APTR prop = DT_FindProperty(key, property);

        if (prop)
        {
            /* If property is found, get its value and exit */
            return DT_GetPropValue(prop);
        }
        
        /* Property was not found, go to the parent and repeat */
        key = DT_GetParent(key);
    } while (key);

    return NULL;
}

APTR Init(REGARG(struct ExecBase *SysBase, "a6"))
{
    struct CMABase *CMABase = NULL;
    struct ExpansionBase *ExpansionBase = NULL;
    struct CurrentBinding binding;
    BOOL success = FALSE;

    APTR base_pointer = NULL;

    binding.cb_ConfigDev = NULL;

    ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library", 0);

    if (ExpansionBase != NULL)
    {
        GetCurrentBinding(&binding, sizeof(binding));
    }

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
        CMABase->cma_Node.lib_Node.ln_Pri = 120;
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
                ULONG base = 0;
                ULONG size = 0;

                /* reg is <base size>, two plain 32 bit words as the user writes them: not read with the cells of the
                   parent (the root of the tree has #address-cells = 2, which would take the size for the base) */
                if (reg != NULL)
                {
                    base = reg[0];
                    size = reg[1];
                }

                DT_CloseKey(key);

                /* DeviceTree entry is there, see if the memory is available */
                if (base != 0 && size != 0) {
                    tlsf_add_memory(&CMABase->cma_TLSF, (APTR)base, size);

                    SumLibrary((struct Library*)CMABase);
                    AddResource(CMABase);

                    if (binding.cb_ConfigDev != NULL)
                    {
                        binding.cb_ConfigDev->cd_Flags &= ~CDF_CONFIGME;
                        binding.cb_ConfigDev->cd_Driver = CMABase;
                    }

                    success = TRUE;
                }
            }
        }
    }

    /* If anything failed, clean up now */
    if (!success) {
        if (base_pointer)
            FreeMem(base_pointer, BASE_NEG_SIZE + BASE_POS_SIZE);
        CMABase = NULL;
    }

    if (ExpansionBase != NULL)
        CloseLibrary((struct Library*)ExpansionBase);

    return CMABase;
}
