import argparse, re

def mnem2enum(m):
    return 'INS_' + m

def main():
    parser = argparse.ArgumentParser(description='Instruction set parsing table generator.')
    parser.add_argument('infile',  type=argparse.FileType('r'))

    args = parser.parse_args()
    mnems = set()
    with open('parsetable.gen.c', 'w') as fd:
        fd.write('static parseinfo_t parseTable[] = {\n')
        for line in args.infile.readlines():
            # Trim comment
            pos = line.find('#')
            if pos >= 0:
                line = line[:pos]

            # Remove whitespaces
            line = line.translate(None, ' \r\n')

            values = line.split(',')
            if len(values) < 2:
                continue

            # Encoding: Translate bit description to code + mask
            bits = values.pop()
            bits = [ (b, '1') if b == '1' or b == '0' else ('0','0') for b in bits ]
            bits = zip(*bits)
            code = hex(int(''.join(bits[0]), 2))
            mask = hex(int(''.join(bits[1]), 2))

            # Mnemonics
            mnem = values.pop(0)

            # Ops
            ops = [None] * len(values)
            cflags = []
            opflags = []
            opindex = 0
            opcounter = 0
            for opcode in values:
                displ = False
                changed = False
                load = False
                posflag = 0
                #Extract flags
                for c in opcode:
                    if c.isalnum():
                        break
                    if c == '!':
                        cflags.append('CF_STOP')
                    elif c == '%':
                        cflags.append('CF_JUMP')
                    elif c == '^':
                        cflags.append('CF_CALL')
                    elif c == '>':
                        changed = True
                    elif c == '+':
                        displ = True
                    elif c == '*':
                        load = True
                #Extract position, if any
                if opcode[-2] == '@':
                    posflag = int(opcode[-1])
                    opcode = opcode[:-2]
                opcode = opcode.lstrip('!>^*+%')

                if posflag:
                    opindex = posflag - 1
                else:
                    opindex = opcounter

                opval = ['OPG_' + opcode]
                if displ:
                    opval.append('OPGF_RELATIVE')
                if load:
                    opval.append('OPFG_LOAD')
                if posflag:
                    opval.append('OPFG_SHOWAT_%d' % opcounter)
                    pass
                ops[opindex] = ' | '.join(opval)
                opflags.append((displ, changed))
                opcounter += 1

            # Update display order dependent CF flags
            opindex = 0
            print opflags
            for displ, changed in opflags:
                if not displ:
                    opindex += 1
                    cflags.append('CF_USE%d' % opindex)
                if changed:
                    cflags.append('CF_CHG%d' % opindex)

            print cflags
            mnems.add((mnem, tuple(cflags)))

            values = [code, mask, mnem2enum(mnem)] + ops

            print ', '.join(values)

            # Output parseinfo
            fd.write('{ %s },\n' % ', '.join(values))
        fd.write('};\n\n')

        fd.write('instruc_t Instructions[] = {\n')
        for mnem in mnems:
            flags = mnem[1]
            name = '"%s",' % mnem[0]
            fd.write('{ %-10s %-32s },\n' % (name, '|'.join(flags)))
        fd.write('};\n\n')

    with open('parsetable.gen.h', 'w') as fd:
        fd.write('enum ins_enum_t {\n')
        fd.write('INS_NULL = 0,\n')
        for mnem in mnems:
            fd.write('%s,\n' % mnem2enum(mnem[0]))
        fd.write('};\n\n')

if __name__ == '__main__':
    main()
