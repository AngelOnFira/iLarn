#include "palm.h"
#include <CharAttr.h> /* for IsDigit */
#include "iLarnRsc.h"
#include "Globals.h"
/***********************************************************************
 * This form is used when player enters the LRS office.
 * If you do not owe taxes, this form is quite boring.
 * Otherwise,
 * There is a field that lets you enter a numeric amount
 * to pay off some or all of your taxes.
 ***********************************************************************/

Boolean Lrs_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  FieldPtr fld;
  Word chr;
  VoidHand h;
  VoidPtr p;
  CharPtr textp;
  Short len, objid;
  ControlPtr btn;
  Long gold;
  //  Boolean ok;
    
    // CALLBACK_PROLOGUE

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    // allocate memory for field, I hope.
    h = MemHandleNew(sizeof(Char) * 12);
    p = MemHandleLock(h);
    ((Char *)p)[0] = '\0';
    MemPtrUnlock(p);
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_lrs_amt));
    FldSetTextHandle(fld, (Handle) h);
    FrmDrawForm(frm);
    //    bank_lrs_init(frm, (in_store == STORE_BANK), env);
    lrs_init_or_refresh(frm);
    handled = true;
    break;

  case keyDownEvent:
    chr = e->data.keyDown.chr;
    handled = true;
    // first, some keyboard support.
    objid = -1;
    if (chr == 'P')       objid = btn_lrs_dp;
    else if (chr == 'R')  objid = btn_lrs_wd;
    else if (chr == 'X')  objid = btn_lrs_cancel;
    // Else objid remains -1.
    if (objid != -1) {
      frm = FrmGetActiveForm();
      btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objid));
      if (btn->attr.usable) 
	CtlHitControl(btn);
      return handled;
    }
    // filter out all non-handled, i.e. non-numeric, characters.
    if ( IsDigit(GetCharAttr(), chr) ||
	 (chr == backspaceChr)       ||
	 (chr == leftArrowChr)       ||
	 (chr == rightArrowChr) )
      handled = false;
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_lrs_amt));
    switch(e->data.ctlSelect.controlID) {
    case btn_lrs_wd:
      // bank transfer
      lrs_req_trans(frm);
      handled = true;
      break;
    case btn_lrs_dp:
      // cash payment
      len = FldGetTextLength(fld);
      if (len > 0) {
	textp = FldGetTextPtr(fld);
	gold = StrAToI(textp);
	if (gold > 0)
	  lrs_pay_cash(frm, gold);
	FldDelete(fld, 0, len); // clear the field
      }
      // I probably want to do this
      FldReleaseFocus(fld);
      FldSetSelection(fld, 0, 0);
      handled = true;
      break;
    case btn_lrs_cancel:
      // hope these do the right thing.
      FldReleaseFocus(fld);
      FldSetSelection(fld, 0, 0);
      FldFreeMemory(fld);
      LeaveForm(); // LeaveForm(true);
      //      i_used_too_much_heap();
      //      message_clear(); // do I need this?
      print_stats(); // in case you spent money
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

    // CALLBACK_EPILOGUE

  return handled;
}
