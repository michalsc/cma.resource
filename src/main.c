/*
    Copyright © 2025 Michal Schulz <michal.schulz@gmx.de>
    https://github.com/michalsc

    This Source Code Form is subject to the terms of the
    Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
    with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <common/compiler.h>

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/errors.h>
#include <exec/libraries.h>
#include <exec/execbase.h>

#include <proto/exec.h>
#include <stdint.h>

#include "cma.h"

int dont_execute() { return -1; }

extern UBYTE rom_end;
extern const char deviceName[];
extern const char deviceIdString[];
void Init();

const struct Resident RomTag __attribute__((used)) = {
    RTC_MATCHWORD,
    (struct Resident *)&RomTag,
    (APTR)&rom_end,
    RTF_COLDSTART,
    CMA_VERSION,
    NT_RESOURCE,
    CMA_PRIORITY,
    (char *)((intptr_t)&deviceName),
    (char *)((intptr_t)&deviceIdString),
    Init,
};

const char deviceName[] = "cma.resource";
const char deviceIdString[] = VERSION_STRING;

