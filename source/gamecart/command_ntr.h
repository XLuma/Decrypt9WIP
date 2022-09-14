// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.
//
// modifyed by osilloscopion (2 Jul 2016)
//

#pragma once

#include "common.h"

void NTR_CmdReset(void);
u32 NTR_CmdGetCartId(void);
void NTR_CmdEnter16ByteMode(void);
void NTR_CmdReadHeader (u8* buffer);
void NTR_CmdReadCopts (u8* buffer);
void NTR_CmdReadData (u32 offset, void* buffer);

bool NTR_Secure_Init (u8* buffer, u32 CartID, int iCardDevice);

void NTR_CmdAF(void); //activate debug mode, dont know which cartridges are affected
void NTR_Cmd6D(void *buff); //cartridge info, copts and bad block list
void NTR_Cmd94(void *buff); //nand id
void NTR_CmdA0(void); //id2, returns FFFF when write mode
void NTR_Cmd3C(void); //puts cartridges in a "ignore all commands" state
void NTR_Cmd71(void); //does something on cartridges, listed by 3dbrew
void NTR_Cmd9E7D(void); //sets twl and ctr 1-st gen in write mode
void NTR_Cmd9E(void); //ntr write mode
void NTR_Cmd9A(void); //unknown, apparently a 4 byte read. rom id ?
void NTR_Cmd98(void); //unknown
void NTR_Cmd99(void); //unknown
void NTR_Cmd9D(u32 blkNum); //erase nand block
u8* NTR_Cmd6F(void); //poll for write/erase status