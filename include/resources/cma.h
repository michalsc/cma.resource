#ifndef RESOURCES_CMA_H
#define RESOURCES_CMA_H
/*
**	$VER: cma.h 1.0
**
**	Public definitions of "cma.resource", the zone of uncached contiguous memory of the Raspberry Pi
**	(PiStorm / Emu68). The vector offsets are in the FD and the sfd. See Autodocs/cma.doc.
**
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#define CMANAME "cma.resource"

/* Where the zone is announced in the device tree: reg = <base size>, two plain 32 bit words */
#define CMA_DT_NODE "/emu68/cma@0"

/* CMA_AllocAligned(): a larger alignment is taken as this one */
#define CMA_MAX_ALIGNMENT       (0x200000UL)

#endif /* RESOURCES_CMA_H */
