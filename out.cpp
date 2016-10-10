#include "mn101.hpp"

extern char deviceparams[];
extern char device[];

static void OutVarName(op_t &x)
{
    ea_t addr = x.addr;
    bool H = 0;
    // For branch ops x.value holds halfbyte address and x.addr holds byte address
    if (x.addr != x.value)
        H = x.value & 1;

    ea_t target = toEA(codeSeg(addr, x.n), addr);
    if (out_name_expr(x, target, addr))
        return;
    OutValue(x, OOF_ADDR | OOF_NUMBER | OOFS_NOSIGN | OOFW_32);
    if (H) out_symbol('_');
    QueueSet(Q_noName, cmd.ea);
}

bool idaapi mn101_outop(op_t &x)
{
    switch (x.type)
    {
    case o_phrase:
    case o_displ:
        out_symbol('(');
        // Do not add imm-offset when its zero
        if (x.addr != 0)
        {
            OutValue(x, OOF_ADDR);
            out_symbol(',');
        }
        out_register(ph.regNames[x.reg]);
        out_symbol(')');
        break;

    case o_reg:
        out_register(ph.regNames[x.reg]);
        break;

    case o_imm:
        OutValue(x, OOF_SIGNED | OOFW_IMM);
        break;

    case o_near:
        OutVarName(x);
        break;

    case o_mem:
        out_symbol('(');
        OutVarName(x);
        out_symbol(')');
        break;

    case o_void:
        return 0;

    default:
        warning("out: %a: bad optype %d", cmd.ea, x.type);
        break;
    }
    return 1;
}

void idaapi mn101_out(void)
{
    char buf[MAXSTR];
    init_output_buffer(buf, sizeof(buf)); // setup the output pointer

    OutMnem();

    //First operand
    if (cmd.Op1.type != o_void)
        out_one_operand(0);

    //Second operand
    if (cmd.Op2.type != o_void)
    {
        out_symbol(',');
        OutChar(' ');
        out_one_operand(1);
    }

    // Third operand
    if (cmd.Op3.type != o_void)
    {
        out_symbol(',');
        OutChar(' ');
        out_one_operand(2);
    }

    if (isVoid(cmd.ea, uFlag, 0)) OutImmChar(cmd.Op1);
    if (isVoid(cmd.ea, uFlag, 1)) OutImmChar(cmd.Op2);
    if (isVoid(cmd.ea, uFlag, 2)) OutImmChar(cmd.Op3);

    term_output_buffer();
    gl_comm = 1;
    MakeLine(buf);
}

//--------------------------------------------------------------------------
// Listing header
void idaapi mn101_header(void)
{
    gen_header(GH_PRINT_ALL_BUT_BYTESEX, device[0] ? device : NULL, deviceparams);
}

//--------------------------------------------------------------------------
// Segment start
void idaapi mn101_segstart(ea_t ea)
{
    segment_t *Sarea = getseg(ea);
    if (is_spec_segm(Sarea->type)) return;

    char sname[MAXNAMELEN];
    get_true_segm_name(Sarea, sname, sizeof(sname));

    gen_cmt_line("section %s", sname);
}


//--------------------------------------------------------------------------
// Listing footer
void idaapi mn101_footer(void)
{
    char buf[MAXSTR];
    char *const end = buf + sizeof(buf);
    if (ash.end != NULL)
    {
        MakeNull();
        char *ptr = tag_addstr(buf, end, COLOR_ASMDIR, ash.end);
        qstring name;
        if (get_colored_name(&name, inf.beginEA) > 0)
        {
            register size_t i = strlen(ash.end);
            do
                APPCHAR(ptr, end, ' ');
            while (++i < 8);
            APPEND(ptr, end, name.begin());
        }
        MakeLine(buf, inf.indent);
    }
    else
    {
        gen_cmt_line("end of file");
    }
}
