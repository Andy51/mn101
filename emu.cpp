/*
 *      Panasonic MN102 (PanaXSeries) processor module for IDA.
 *      Copyright (c) 2000-2006 Konstantin Norvatoff, <konnor@bk.ru>
 *      Freeware.
 */

#include "pan.hpp"

static bool flow;        // флажок стопа
//----------------------------------------------------------------------
// поставим использование/изменение операндов
static void near TouchArg(op_t &x,int isAlt,int isload)
{

    ea_t ea = toEA(codeSeg(x.addr / 2,x.n), x.addr / 2);
  switch ( x.type ) {
  // эта часть не используется !
  case o_void:  break;
  // тут тоже нечего делать
  case o_reg:           break;

  // попробуем рассматривать как смещение
  case o_displ: // если не форсирован и помечен смещением
                                if ( !isAlt && isOff(uFlag,x.n) ){
                        // если изменяется - поставим переменную
                                                if ( ! isload ) doVar(ea);
                                                // добавим ссылку на память
                                                ua_add_dref(x.offb,ea,isload ? dr_R : dr_W);
                                }
                                break;

  // непосредственный операнд
  case o_imm:   // непосредственный не может меняться
                if ( ! isload ) goto badTouch;
                // поставим флажок непосредственного операнда
                doImmd(cmd.ea);
                // если не форсирован
                if( !isAlt){
                        if ( isOff(uFlag,x.n) ||
                             ( (x.specflag1&URB_ADDR) &&
                               (!isDefArg(uFlag,x.n))
                             )
                        ){
                                if ( !isOff(uFlag,x.n) )
                                        set_op_type(cmd.ea,offflag(),x.n);
                                // это смещение !
                                ua_add_dref(x.offb,ea,dr_O);
                                                }
                }
                break;

  // переход или вызов
    case o_near:  // это вызов ? (или переход)
        if (InstrIsSet(cmd.itype, CF_CALL)) {
            ua_add_cref(x.offb, ea, fl_CN);

            // это функция без возврата ?
            flow = func_does_return(ea);
        }
        else
        {
            ua_add_cref(x.offb, ea, fl_JN);
        }

        // Mark the jump address if it has halfbyte offset
        if (x.addr & 1)
        {
            helper.charset(ea, 1, NODETAG_HALFBYTE);
        }
        break;

  // ссылка на память
  case o_mem:   // сделаем данные по указанному адресу
                ua_dodata2(x.offb, ea, x.dtyp);
                // если изменяется - поставим переменную
                if ( ! isload ) doVar(ea);
                // добавим ссылку на память
                ua_add_dref(x.offb,ea,isload ? dr_R : dr_W);
                break;

  // прочее - сообщим ошибку
  default:
badTouch:
#if IDP_INTERFACE_VERSION > 37
                warning("%a %s,%d: bad optype %d",
                                cmd.ea, cmd.get_canon_mnem(),
#else
                warning("%08lX %s,%d: bad optype (%x)",
                                cmd.ea,(char far *)Instructions[cmd.itype].name,
#endif
                                x.n, x.type);
    break;
  }
}

//----------------------------------------------------------------------
// емулятер
int idaapi mn102_emu(void)
{
#if IDP_INTERFACE_VERSION > 37
uint32 Feature = cmd.get_canon_feature();
#else
uint32 Feature = Instructions[cmd.itype].feature;
uFlag = getFlags(cmd.ea);
#endif
  // получим типы операндов
  int flag1 = is_forced_operand(cmd.ea, 0);
  int flag2 = is_forced_operand(cmd.ea, 1);
  int flag3 = is_forced_operand(cmd.ea, 2);

  flow = ((Feature & CF_STOP) == 0);

  // пометим ссылки двух операндов
  if ( Feature & CF_USE1) TouchArg(cmd.Op1, flag1, 1 );
  if ( Feature & CF_USE2) TouchArg(cmd.Op2, flag2, 1 );
  if ( Feature & CF_USE3) TouchArg(cmd.Op3, flag3, 1 );
  // поставим переход в очередь
  if ( Feature & CF_JUMP) QueueSet(Q_jumps,cmd.ea );

  // поставим изменения
  if ( Feature & CF_CHG1) TouchArg(cmd.Op1, flag1, 0 );
  if ( Feature & CF_CHG2) TouchArg(cmd.Op2, flag2, 0 );
  if ( Feature & CF_CHG3) TouchArg(cmd.Op3, flag3, 0 );
  // если не стоп - продолжим на след. инструкции
  if ( flow) ua_add_cref(0,cmd.ea+cmd.size,fl_F );

  return(1);
}
