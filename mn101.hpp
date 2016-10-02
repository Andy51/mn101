#ifndef _MN101_HPP
#define _MN101_HPP

#include "../idaidp.hpp"
#include <ida.hpp>
#include <idp.hpp>

#include "ins.hpp"

extern netnode helper;
#define NODETAG_HALFBYTE 'h'

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

    rVcs,
    rVds,

    OP_REG_LAST,
};


void    idaapi mn101_header(void);
void    idaapi mn101_footer(void);

void    idaapi mn101_segstart(ea_t ea);

int     idaapi mn101_ana(void);
int     idaapi mn101_emu(void);
void    idaapi mn101_out(void);
bool    idaapi mn101_outop(op_t &op);


#endif
