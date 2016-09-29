/*
 *      Panasonic MN102 (PanaXSeries) processor module for IDA.
 *      Copyright (c) 2000-2006 Konstantin Norvatoff, <konnor@bk.ru>
 *      Freeware.
 */

#ifndef _PAN_HPP
#define _PAN_HPP

#include <ida.hpp>
#include <idp.hpp>

#include "../idaidp.hpp"
#define near
#define far
#include "ins.hpp"

//-----------------------------------------------
// Вспомогательные биты
#define URB_ADDR        0x1     // Непоср. аргумент - адрес

//------------------------------------------------------------------------
#ifdef _MSC_VER
#define ENUM8BIT : uint8
#else
#define ENUM8BIT
#endif
// список регистров процессора
enum mn102_registers ENUM8BIT { rNULLReg,
        rD0, rD1, rD2, rD3,
        rA0, rA1, rA2, rA3,
        rMDR,rPSW, rPC,
        rVcs, rVds};

enum mn101_registers {
    OP_REG_NONE = 0,

    OP_REG_D,
    OP_REG_D0 = OP_REG_D,
    OP_REG_D1,
    OP_REG_D2,
    OP_REG_D3,

    OP_REG_A,
    OP_REG_A0 = OP_REG_A,
    OP_REG_A1,

    OP_REG_DW,
    OP_REG_DW0 = OP_REG_DW,
    OP_REG_DW1,

    OP_REG_SP,
    OP_REG_PSW,
    OP_REG_HA,

    OP_REG_LAST,
};

#if IDP_INTERFACE_VERSION > 37
extern char deviceparams[];
extern char device[];
#endif

extern netnode helper;
#define NODETAG_HALFBYTE 'h'

//------------------------------------------------------------------------
void    idaapi mn102_header(void);
void    idaapi mn102_footer(void);

void    idaapi mn102_segstart(ea_t ea);

int     idaapi mn101_ana(void);
int     idaapi mn102_emu(void);
void    idaapi mn101_out(void);
bool    idaapi mn101_outop(op_t &op);

void    idaapi mn102_data(ea_t ea);

#endif
