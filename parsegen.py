import argparse, re

def mnem2enum(m):
    return 'INS_' + m

def outMnem(fd, mnem, flaglist):
    cflags = '|'.join(flaglist)
    if not cflags:
        cflags = '0'
    name = '"%s",' % mnem
    fd.write('{ %-10s %-32s },\n' % (name, cflags))

def outMnemEnum(fd, mnem):
    fd.write('    %s,\n' % mnem2enum(mnem))

def main():
    parser = argparse.ArgumentParser(description='Instruction set parsing table generator.')
    parser.add_argument('infile',  type=argparse.FileType('r'))

    args = parser.parse_args()
    mnems = set()

    parseinfo = ([],[],[])
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
        code = int(''.join(bits[0]), 2)
        mask = int(''.join(bits[1]), 2)

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
            used = False
            cf_call = False
            cf_jump = False
            posflag = 0
            #Extract flags
            for c in opcode:
                if c.isalnum():
                    break
                if c == '!':
                    cflags.append('CF_STOP')
                elif c == '%':
                    cf_jump = True
                elif c == '^':
                    cf_call = True
                elif c == '>':
                    changed = True
                elif c == '+':
                    displ = True
                elif c == '*':
                    load = True
            #Extract position, if any
            if len(opcode) > 2 and opcode[-2] == '@':
                posflag = int(opcode[-1])
                opcode = opcode[:-2]
            opcode = opcode.lstrip('!>^*+%')

            if opcode and not displ:
                used = True
            if opcode.startswith('BRANCH'):
                cf_jump = True
            if opcode.startswith('CALL'):
                cf_call = True
            if opcode.startswith('BITPOS'):
                used = False

            if posflag:
                opindex = posflag - 1
            else:
                opindex = opcounter

            opval = []
            if opcode:
                opval.append('OPG_' + opcode)
            if displ:
                opval.append('OPGF_RELATIVE')
            if load:
                opval.append('OPGF_LOAD')
            if posflag:
                opval.append('OPGF_SHOWAT_%d' % opcounter)
            if cf_call:
                cflags.append('CF_CALL')
            if cf_jump:
                cflags.append('CF_JUMP')
            ops[opindex] = ' | '.join(opval)
            opflags.append((used, changed))
            opcounter += 1

        # Update display order dependent CF flags
        opindex = 0
        for used, changed in opflags:
            if used:
                opindex += 1
                cflags.append('CF_USE%d' % opindex)
            if changed:
                cflags.append('CF_CHG%d' % opindex)

        # Split codes by extension
        extension = code >> 8
        if extension not in (0,2,3):
            print 'ERROR: invalid code detected for %s: %s' % (mnem, bin(code))
            continue
        if extension > 0:
            extension -= 1

        code &= 0xFF
        mask &= 0xFF
        values = (hex(code), hex(mask), mnem2enum(mnem)) + tuple(ops)
        parseinfo[extension].append(values)
        mnems.add((mnem, tuple(cflags)))

    # Output parsetables
    for extension, pinfo in enumerate(parseinfo):
        with open('parsetable%d.gen.c' % extension, 'w') as fd:
            for values in pinfo:
                fd.write('{ %s },\n' % ', '.join(values))

    mnems = list(mnems)
    mnems.sort()

    # Output instruc_t table
    with open('instructions.gen.c', 'w') as fd:
        outMnem(fd, '', [])
        for mnem in mnems:
            outMnem(fd, *mnem)

    # Output instructions enum
    with open('instructions.gen.h', 'w') as fd:
        fd.write('enum ins_enum_t {\n')
        outMnemEnum(fd, 'NULL = 0')
        for mnem in mnems:
            outMnemEnum(fd, mnem[0])
        outMnemEnum(fd, 'LAST')
        fd.write('};\n\n')

if __name__ == '__main__':
    main()
