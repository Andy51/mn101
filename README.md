# Panasonic MN101 processor module for IDA

## Build
Place it into idasdk/modules folder.
It needs python installed to build for parsing table code generation script.
There is only MN101AF68G defined now.
The project includes an MSVC solution that just invokes mingw32-make to build the module, so it expects that you have IDASDK environment set up alread, which could be quite tricky as well, but I wont describe the full process here.

## Half-byte instruction set
The main problem with MN101 is that instructions have variable length with 4-bit increments and IDA cannot decode half-byte instructions.
However, since the minimum size one instruction can take is 2 nibbles (1 byte) we can display an instruction as if it starts aligned to byte, at the same time maintaining the H flag for every code location indicating whether the instruction start have a half-byte offset or not. You can (and often have to) change it manually with Alt+G shortcut (it is represented as a pseudo segment register).

## Install
Place the mn101.w32 into \<IDAINSTALL\>/procs/ and mn101e.cfg into \<IDAINSTALL\>/cfg/
