/*
 *      Panasonic MN102 (PanaXSeries) processor module for IDA.
 *      Copyright (c) 2000-2006 Konstantin Norvatoff, <konnor@bk.ru>
 *      Freeware.
 */

#include "pan.hpp"

instruc_t Instructions[] = {
#include "instructions.gen.c"
};

CASSERT(qnumber(Instructions) == INS_LAST);
