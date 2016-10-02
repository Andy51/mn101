#include "mn101.hpp"

instruc_t Instructions[] = {
#include "instructions.gen.c"
};

CASSERT(qnumber(Instructions) == INS_LAST);
