PROC=mn101
ADDITIONAL_GOALS=config

include ../module.mak

config: $(C)mn102l00.cfg
$(C)mn102l00.cfg: mn102l00.cfg
	$(CP) $? $@

#http://stackoverflow.com/questions/2973445/gnu-makefile-rule-generating-a-few-targets-from-a-single-source-file
instructions.%.h instructions.%.c parsetable0.%.c parsetable1.%.c parsetable2.%.c: parsegen.py mn101e.cmdset
	python $^

# MAKEDEP dependency list ------------------
$(F)ana$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bitrange.hpp $(I)bytes.hpp   \
	          $(I)fpro.h $(I)funcs.hpp $(I)ida.hpp $(I)idp.hpp          \
	          $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp               \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ana.cpp ins.hpp     \
	          mn101.hpp instructions.gen.h parsetable0.gen.c            \
	          parsetable1.gen.c parsetable2.gen.c
$(F)emu$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bitrange.hpp $(I)bytes.hpp   \
	          $(I)fpro.h $(I)funcs.hpp $(I)ida.hpp $(I)idp.hpp          \
	          $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp               \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp emu.cpp ins.hpp     \
	          mn101.hpp instructions.gen.h
$(F)ins$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bitrange.hpp $(I)bytes.hpp   \
	          $(I)fpro.h $(I)funcs.hpp $(I)ida.hpp $(I)idp.hpp          \
	          $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp               \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ins.cpp ins.hpp     \
	          mn101.hpp instructions.gen.h instructions.gen.c
$(F)out$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bitrange.hpp $(I)bytes.hpp   \
	          $(I)fpro.h $(I)funcs.hpp $(I)ida.hpp $(I)idp.hpp          \
	          $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp               \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ins.hpp out.cpp     \
	          mn101.hpp instructions.gen.h
$(F)reg$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bitrange.hpp $(I)bytes.hpp   \
	          $(I)diskio.hpp $(I)entry.hpp $(I)fpro.h $(I)funcs.hpp     \
	          $(I)ida.hpp $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp     \
	          $(I)llong.hpp $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp    \
	          $(I)netnode.hpp $(I)offset.hpp $(I)pro.h $(I)queue.hpp    \
	          $(I)segment.hpp $(I)srarea.hpp $(I)ua.hpp $(I)xref.hpp    \
	          ../idaidp.hpp ../iocommon.cpp ins.hpp mn101.hpp reg.cpp    \
	          instructions.gen.h
