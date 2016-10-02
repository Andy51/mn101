#include <idp.hpp>
#include <diskio.hpp>
#include <srarea.hpp>

#include "mn101.hpp"

//--------------------------------------------------------------------------
static const char *const mn101_registerNames[] =
{
    "",
    "D0","D1","D2","D3",
    "A0","A1",
    "DW0","DW1",
    "SP", "PSW", "HA",

    "cs","ds"
};

//----------------------------------------------------------------------
//       Prepare global variables & defines for ../iocommon.cpp
//----------------------------------------------------------------------
netnode helper;
char device[MAXSTR] = "";
static size_t numports = 0;
static ioport_t *ports = NULL;

#include "../iocommon.cpp"

//----------------------------------------------------------------------
static int idaapi notify(processor_t::idp_notify msgid, ...)
{
    va_list va;
    va_start(va, msgid);

    int code = invoke_callbacks(HT_IDP, msgid, va);
    if (code) return code;

    switch (msgid)
    {
    case processor_t::init:
        helper.create("$ MN101");
        break;

    case processor_t::term:
        free_ioports(ports, numports);
        break;

    case processor_t::newfile:
        break;

    default:
        break;
    }
    va_end(va);
    return 1;
}

static const asm_t mn101asm = {
    AS_COLON | AS_UDATA | ASH_HEXF3 | ASD_DECF0 | ASO_OCTF0 | ASB_BINF0,
    0,                          // User defined flags
    "MN101 assembler",          // Assembler name
    0,                          // Help screen number
    NULL,                       // array of automatically generated header lines
    NULL,                       // array of unsupported instructions
    "org",                      // org directive
    "end",                      // end directive
    ";",                        // comment string
    '"',                        // ASCII string delimiter
    '\'',                       // ASCII char constant delimiter
    "\\\"'",                    // ASCII special chars
    "DB",                       // ASCII string directive
    "DB",                       // byte directive
    "DW",                       // word directive
    "DA",                       // dword  (4 bytes)
    NULL,                       // qword  (8 bytes)
    NULL,                       // oword  (16 bytes)
    NULL,                       // float  (4 bytes)
    NULL,                       // double (8 bytes)
    NULL,                       // long double  (10/12 bytes)
    NULL,                       // packed decimal real
    "#d dup(#v)",               // array keyword
    "DB ?",                     // uninitialized data directive
    "EQU",                      // 'equ' Used if AS_UNEQU is set
    NULL,                       // 'seg ' prefix
    NULL,                       // Pointer to checkarg_dispatch function
    NULL,                       // Unused
    NULL,                       // Unused
    NULL,                       // Translation to use in character and string constants
    NULL,                       // current IP (instruction pointer) symbol in assembler
    NULL,                       // Generate function header line
    NULL,                       // Generate function footer lines
    NULL,                       // "public" name keyword
    NULL,                       // "weak"   name keyword
    NULL,                       // "extern" name keyword
    NULL,                       // "comm" (communal variable)
    NULL,                       // Get name of type of item at ea or id
    "ALIGN",                    // "align" keyword
    '(', ')',                   // lbrace, rbrace
    NULL,                       // %  mod     assembler time operation
    NULL,                       // &  bit and assembler time operation
    NULL,                       // |  bit or  assembler time operation
    NULL,                       // ^  bit xor assembler time operation
    NULL,                       // ~  bit not assembler time operation
    NULL,                       // << shift left assembler time operation
    NULL,                       // >> shift right assembler time operation
    NULL,                       // size of type (format string)
};


static const asm_t *const asms[] = { &mn101asm, NULL };
#define FAMILY "Panasonic MN101:"
static const char *const shnames[] = { "MN101E", NULL };
static const char *const lnames[] = { FAMILY"Panasonic MN101E", NULL };

// Opcodes of return instructions
static const uchar retcode_1[] = { 0x01 };    // RTS
static const uchar retcode_2[] = { 0x03 };    // RTI
static bytes_t retcodes[] = {
    { sizeof(retcode_1), retcode_1 },
    { sizeof(retcode_2), retcode_2 },
    { 0, NULL }
};

//-----------------------------------------------------------------------
//      Processor Definition
//-----------------------------------------------------------------------

#define PLFM_MN101 0x8002

processor_t LPH = {
    IDP_INTERFACE_VERSION,
    PLFM_MN101,
    PRN_HEX | PR_SEGS | PR_SGROTHER,
    8,                          // 8 bits in a byte for code segments
    8,                          // 8 bits in a byte for other segments

    shnames,                    // array of short processor names
    lnames,                     // array of long processor names

    asms,                       // array of target assemblers

    notify,                     // the kernel event notification callback

    mn101_header,               // generate the disassembly header
    mn101_footer,               // generate the disassembly footer

    mn101_segstart,             // generate a segment declaration (start of segment)
    std_gen_segm_footer,        // generate a segment footer (end of segment)

    NULL,                       // generate 'assume' directives

    mn101_ana,                  // analyze an instruction and fill the 'cmd' structure
    mn101_emu,                  // emulate an instruction

    mn101_out,                  // generate a text representation of an instruction
    mn101_outop,                // generate a text representation of an operand
    intel_data,                 // generate a text representation of a data item
    NULL,                       // compare operands
    NULL,                       // can an operand have a type?

    qnumber(mn101_registerNames),            // Number of registers
    mn101_registerNames,                     // Regsiter names
    NULL,                       // get abstract register

    0,                          // Number of register files
    NULL,                       // Register file names
    NULL,                       // Register descriptions
    NULL,                       // Pointer to CPU registers

    rVcs,rVds,                  // Number of first/last segment register
    2,                          // size of a segment register

    rVcs,rVds,                  // Number of CS/DS register

    NULL,                       // Array of typical code start sequences
    retcodes,                   // Array of 'return' instruction opcodes.


    0, INS_LAST,                // icode of the first/last instruction
    Instructions,               // Array of instructions
    NULL,                       // is indirect far jump or call instruction?
    NULL,                       // Translation function for offsets
    0,                          // Size of long double (tbyte) for this processor
    NULL,                       // Floating point -> IEEE conversion function
    { 0, },                     // Number of digits in floating numbers after the decimal point
    NULL,                       // Find 'switch' idiom
    NULL,                       // Generate map file
    NULL,                       // Extract address from a string
    NULL,                       // Check whether the operand is relative to stack pointer or frame pointer
    NULL,                       // Create a function frame for a newly created function
    NULL,                       // Get size of function return address in bytes
    NULL,                       // Generate stack variable definition line
    NULL,                       // Generate text representation of an item in a special segment
    INS_RTS,                    // Icode of return instruction
    NULL,                       // Set IDP-specific option
    NULL,                       // Is the instruction created only for alignment purposes?
    NULL,                       // Reserved
    0                           // The number of bits in the fixup HIGH part
};
