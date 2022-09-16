// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.
//
// modifyed by osilloscopion (2 Jul 2016)
//

#include "command_ntr.h"
#include "protocol_ntr.h"
#include "card_ntr.h"
#include "delay.h"


u32 ReadDataFlags = 0;

void NTR_CmdReset(void)
{
    cardReset ();
    ioDelay2(0xF000);
}

u32 NTR_CmdGetCartId(void)
{
    return cardReadID (0);
}

void NTR_CmdEnter16ByteMode(void)
{
    static const u32 enter16bytemode_cmd[2] = { 0x3E000000, 0x00000000 };
    NTR_SendCommand(enter16bytemode_cmd, 0x0, 0, NULL);
}

void NTR_CmdReadHeader (u8* buffer)
{
	REG_NTRCARDROMCNT=0;
	REG_NTRCARDMCNT=0;
	ioDelay2(167550);
	REG_NTRCARDMCNT=NTRCARD_CR1_ENABLE|NTRCARD_CR1_IRQ;
	REG_NTRCARDROMCNT=NTRCARD_nRESET|NTRCARD_SEC_SEED;
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	cardReset();
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	u32 iCardId=cardReadID(NTRCARD_CLK_SLOW);
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	
	u32 iCheapCard=iCardId&0x80000000;
	
    if(iCheapCard)
    {
      //this is magic of wood goblins
      for(size_t ii=0;ii<8;++ii)
        cardParamCommand(NTRCARD_CMD_HEADER_READ,ii*0x200,NTRCARD_ACTIVATE|NTRCARD_nRESET|NTRCARD_CLK_SLOW|NTRCARD_BLK_SIZE(1)|NTRCARD_DELAY1(0x1FFF)|NTRCARD_DELAY2(0x3F),(u32*)(void*)(buffer+ii*0x200),0x200/sizeof(u32));
    }
    else
    {
      //0xac3f1fff
      cardParamCommand(NTRCARD_CMD_HEADER_READ,0,NTRCARD_ACTIVATE|NTRCARD_nRESET|NTRCARD_CLK_SLOW|NTRCARD_BLK_SIZE(4)|NTRCARD_DELAY1(0x1FFF)|NTRCARD_DELAY2(0x3F),(u32*)(void*)buffer,0x1000/sizeof(u32));
    }
    //cardReadHeader (buffer);
}

void NTR_CmdReadData (u32 offset, void* buffer)
{
    cardParamCommand (NTRCARD_CMD_DATA_READ, offset, ReadDataFlags | NTRCARD_ACTIVATE | NTRCARD_nRESET | NTRCARD_BLK_SIZE(1), (u32*)buffer, 0x200 / 4);
}

void NTR_CmdReadCopts (u8* buffer)
{
	REG_NTRCARDROMCNT=0;
	REG_NTRCARDMCNT=0;
	ioDelay2(167550);
	REG_NTRCARDMCNT=NTRCARD_CR1_ENABLE|NTRCARD_CR1_IRQ;
	REG_NTRCARDROMCNT=NTRCARD_nRESET|NTRCARD_SEC_SEED;
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	cardReset();
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	u32 iCardId=cardReadID(NTRCARD_CLK_SLOW);
	while(REG_NTRCARDROMCNT&NTRCARD_BUSY) ;
	
	u32 iCheapCard=iCardId&0x80000000;
	NTR_Cmd9E7D();
    u32 cmd[2] = {0x6D000000, 0x00000000};
    NTR_SendCommand(cmd, 0, 0, NULL);
    ioDelay2(0x60000);
    //if(iCheapCard)
    //{
      //this is magic of wood goblins
      for(size_t ii=0;ii<1;++ii)
        cardParamCommand(0x9Fu,ii*0x200,NTRCARD_ACTIVATE|NTRCARD_nRESET|NTRCARD_CLK_SLOW|NTRCARD_BLK_SIZE(1)|NTRCARD_DELAY1(0x1FFF)|NTRCARD_DELAY2(0x3F),(u32*)(void*)(buffer+ii*0x200),0x200/sizeof(u32));
    //}
    //else
    //{
      //0xac3f1fff
    //  cardParamCommand(NTRCARD_CMD_HEADER_READ,0,NTRCARD_ACTIVATE|NTRCARD_nRESET|NTRCARD_CLK_SLOW|NTRCARD_BLK_SIZE(4)|NTRCARD_DELAY1(0x1FFF)|NTRCARD_DELAY2(0x3F),(u32*)(void*)buffer,0x1000/sizeof(u32));
    //}
    //cardReadHeader (buffer);
}

void NTR_CmdA0(void) //id2
{
    u32 cmd[2] = {0xA0000000, 0x00000000};
    NTR_SendCommand(cmd, 0,0, NULL);
}

void NTR_Cmd9E7D(void) //CTR S1 and TWL, no NTR and CTR S2
{
    u32 cmd[2] = {0x9E7DF92A, 0x11ADA9FA};
    NTR_SendCommand(cmd, 0, 0, NULL);
}

void NTR_Cmd94(void *buff) //nand id, same command as ds/twl nand cartridges
{
    u32 cmd[2] = {0x94000000, 0x00000000}; 
    NTR_SendCommand(cmd, 512, 0x38270, buff); //firt 5 bytes is nand id, rest is bogus
}

void NTR_Cmd6D(void *buff) //cartridge info, like copts and bad blocks list
{
    u32 cmd[2] = {0x6D000000, 0x00000000};
    u32 curr; //offset by increments of 0x200
    u8 ret[512];
    u8 i = 0;
    NTR_SendCommand(cmd, 0, 0x55730, NULL); //4 512 byte reads into a 2048 byte buffer
    cmd[0] = 0x9F000000; //dummy same as nds
    cmd[1] = 0x00000000;
    while (i < 4)
    {
        NTR_SendCommand(cmd, 512, 0x100, &ret);
        memcpy(buff, &ret, sizeof(ret));
        buff = (void*)((char*)buff + 0x200); //should increment pointer by 512 bytes
        i++;
    }
}

void NTR_CmdAF(void)//some sort of debug mode command ? listed as dev spec, not mass production for some type of cartridge...
{
    u32 buff [512];
    u32 cmd[2] = {0xAF000000, 0x00000000};
    NTR_SendCommand(cmd, 0, 0, &buff);
}

void NTR_Cmd3C(void)//activates key1
{
    u32 buff [512];
    u32 cmd[2] = {0x3C000000, 0x00000000};
    NTR_SendCommand(cmd, 0, 0, &buff);
}

void NTR_Cmd3F(void)//activates 16 bit
{
    u32 buff [512];
    u32 cmd[2] = {0x3F000000, 0x00000000};
    NTR_SendCommand(cmd, 0, 0, &buff);
}

void NTR_Cmd71(void) //some command included in all ctr coms
{
    u32 buff[512];
    u32 cmd[2] = {0x71C93FE9, 0xBB0A3B18};
    NTR_SendCommand(cmd, 0, 0, &buff);
}

void NTR_Cmd9E(void)//activates ntr write mode
{
    u32 buff [512];
    u32 cmd[2] = {0x9E000000, 0x00000000};
    NTR_SendCommand(cmd, 0, 0, &buff);
}

void NTR_Cmd9A(void) //unknown in geri's shot it has a 40h before
{
    u8 buff [4];
    u32 cmd[2] = {0x409A0000, 0x00000000};
    NTR_SendCommand(cmd, 4, 0, &buff); //apparently a 4 byte read
}

void NTR_Cmd98(void) //unknown
{
    u8 buff [4];
    u32 cmd[2] = {0xC2980000, 0x00000000};
    NTR_SendCommand(cmd, 4, 0, &buff); //apparently a 4 byte read
}

void NTR_Cmd99(void) //unknown in geri's shot it has a 40h before
{
    u8 buff [4];
    u32 cmd[2] = {0x4F990000, 0x00000000};
    NTR_SendCommand(cmd, 4, 0, &buff); //apparently a 4 byte read
}

void NTR_Cmd95(void *buff)
{
    u32 cmd[2] = {0x95000000, 0x00000000};
    NTR_SendCommand(cmd, 4096, 0x100, &buff);
}

void NTR_Cmd93(u8 *address, void *buff)
{
    return;
}

void NTR_Cmd9D(u32 blkNum) //erase nand block
{
    u32 cmd[2] = {0x9D000000, 0x00000000};
    cmd[0] = cmd[0] + blkNum;
    NTR_SendCommand(cmd, 0, 0, NULL);
}

void NTR_Cmd6F(u8 *buff)//poll for write/erase status
{
    u32 cmd[2] = {0x6F000000, 0x00000000};
    NTR_SendCommand(cmd, 4, 0x100, buff);
}