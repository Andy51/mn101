#ifndef __INSTRS_HPP
#define __INSTRS_HPP

// List of instructions
extern instruc_t Instructions[];

enum ins_enum_t {
#include "instructions.gen.h"
};

#endif
