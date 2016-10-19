#include "mn101.hpp"
#include <srarea.hpp>

static bool flow;

static void handle_operand(op_t &x, int isread)
{
    ea_t ea = toEA(cmd.cs, x.addr);

    switch (x.type)
    {
    case o_void:
    case o_reg:
    case o_bitpos:
    case o_phrase:
        break;

    case o_displ:
        doImmd(cmd.ea);
        if (op_adds_xrefs(uFlag, x.n))
            ua_add_off_drefs2(x, dr_O, OOF_ADDR);
        break;

    case o_imm:
        doImmd(cmd.ea);
        if (op_adds_xrefs(uFlag, x.n))
            ua_add_off_drefs(x, dr_O);
        break;

    case o_mem:
        ua_dodata2(x.offb, ea, x.dtyp);
        if (!isread)
            doVar(ea);
        ua_add_dref(x.offb, ea, isread ? dr_R : dr_W);
        break;

    case o_far: // Used for JSRV
        ua_add_dref(x.offb, x.specval, isread ? dr_R : dr_W);
        ua_dodata2(x.offb, x.specval, dt_dword);
        //fallthrough
    case o_near:
        if (InstrIsSet(cmd.itype, CF_CALL))
        {
            ua_add_cref(x.offb, ea, fl_CN);
            flow = func_does_return(ea);
        }
        else
        {
            ua_add_cref(x.offb, ea, fl_JN);
        }

        // Mark the jump target byte address if it has halfbyte offset
        // But only if it is not already inside a processed function,
        // otherwise we could destroy it by forcing segreg change
        if (get_func(ea) == NULL)
        {
            split_srarea(ea, rVh, x.value & 1, SR_auto);
        }
        break;

    default:
        warning("%a %s,%d: bad optype %d",
            cmd.ea, cmd.get_canon_mnem(), x.n, x.type);
        break;
    }
}


int idaapi mn101_emu(void)
{
    uint32 Feature = cmd.get_canon_feature();

    flow = ((Feature & CF_STOP) == 0);

    if (Feature & CF_USE1) handle_operand(cmd.Op1, 1);
    if (Feature & CF_USE2) handle_operand(cmd.Op2, 1);
    if (Feature & CF_USE3) handle_operand(cmd.Op3, 1);
    if (Feature & CF_CHG1) handle_operand(cmd.Op1, 0);
    if (Feature & CF_CHG2) handle_operand(cmd.Op2, 0);
    if (Feature & CF_CHG3) handle_operand(cmd.Op3, 0);
    if (Feature & CF_JUMP) QueueSet(Q_jumps, cmd.ea);
    if (flow) ua_add_cref(0, cmd.ea + cmd.size, fl_F);

    // Mark the next command's start halfbyte
    // Note it should be done even if flow==0 to prevent errors on following instructions autoanalysis
    // But be careful not to mess other functions
    ea_t next = cmd.ea + cmd.size;
    if (get_func(next) == NULL)
    {
        split_srarea(next, rVh, cmd.segpref, SR_auto);
    }

    return(1);
}
