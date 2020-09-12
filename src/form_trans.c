#include "palm.h"
#include <CharAttr.h> /* for IsDigit */
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form is used for entering a numeric amount of money.
 * It is used in the bank to deposit or withdraw funds.
 * It is also used when you donate money on an altar (in the dungeon).
 * The form has some text to show you the state of your pockets/account,
 * a field in which you can enter a number (and one or two [=] buttons
 * that copy a number from displayed text into the field), and ok/cancel.
 ***********************************************************************/

extern Short in_store;
Boolean Trans_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  FieldPtr fld;
  Word chr;
  VoidHand h;
  VoidPtr p;
  CharPtr textp;
  Char buf[20];
  Long gold;
  Short objid;
  ControlPtr btn;
  Boolean ok = true;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    // allocate memory for field, I hope.
    h = MemHandleNew(sizeof(Char) * 12);
    p = MemHandleLock(h);
    ((Char *)p)[0] = '\0';
    MemPtrUnlock(p);
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_trans_amt));
    FldSetTextHandle(fld, (Handle) h);
    // also set title
    if (in_store != STORE_BANK) FrmCopyTitle(frm, "Drop how much?");
    FrmDrawForm(frm);
    bank_trans_init(frm, (in_store == STORE_BANK));
    handled = true;
    break;

  case keyDownEvent:
    chr = e->data.keyDown.chr;
    handled = true;
    // first, some keyboard support.
    objid = -1;
    if (chr == 'Y' || chr == '=')  objid = btn_trans_alld;
    else if (chr == 'B')  objid = btn_trans_allw;
    else if (chr == 'W')  objid = btn_trans_wd;
    else if (chr == 'D')  objid = btn_trans_dp;
    else if (chr == 'X' || chr == 'N' || chr == 'C')  objid = btn_trans_cancel;
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
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_trans_amt));
    switch(e->data.ctlSelect.controlID) {
    case btn_trans_allw:
      // "withdraw" all
      handled = true;
      if (in_store != STORE_BANK)
	break;
      StrPrintF(buf, "%ld", env->cdude[BANKACCOUNT]);
    case btn_trans_alld:
      // "deposit/drop" all
      if (!handled)
	StrPrintF(buf, "%ld", env->cdude[GOLD]);
      h = (VoidHand) FldGetTextHandle(fld);
      if (!h)
	h = MemHandleNew(12 * sizeof(Char));
      FldSetTextHandle(fld, (Handle) 0);
      p = MemHandleLock(h);
      StrNCopy(p, buf, 12);
      MemHandleUnlock(h);
      FldSetTextHandle(fld, (Handle) h);
      // I think that will do the right thing.
      //      FrmDrawForm(frm);
      FldDrawField(fld);
      handled = true;
      break;
    case btn_trans_wd:
    case btn_trans_dp:
      textp = FldGetTextPtr(fld); // if (textP) ...
      // Well, the only problem with StrAToI is that I want StrAToL !!
      // If the string has length greater than 4, I may have to get funky.
      gold = StrAToI(textp);
      if (in_store == STORE_BANK)
	ok = bank_transact(gold, (e->data.ctlSelect.controlID==btn_trans_dp)); 
      else if (in_store == STORE_NONE) // drop-gold and donate-at-altar
	ok = (gold >= 0 && gold <= env->cdude[GOLD]);
      if (ok) {
	// hope these do the right thing.
	FldReleaseFocus(fld);
	FldSetSelection(fld, 0, 0);
	FldFreeMemory(fld);
	LeaveForm(); // LeaveForm(in_store!=STORE_BANK);
	if (in_store == STORE_BANK)
	  bank_mumble(); // reprint what gold you have
	else if (in_store == STORE_NONE) {
	  if (gold) {
	    drop_dough(gold);
	    // Note: also need to take up a turn.
	    start_of_Play_Level_loop();
	  }
	} else if (in_store == STORE_ALTAR) {
	  in_store = STORE_NONE;
	  if (!omoney(gold))
	    // Note: also need to take up a turn.
	    start_of_Play_Level_loop();
	}
      }
      // else, you tried to move more money than you had...
      handled = true;
      break;
    case btn_trans_cancel:
      // hope these do the right thing.
      FldReleaseFocus(fld);
      FldSetSelection(fld, 0, 0);
      FldFreeMemory(fld);
      LeaveForm(); // LeaveForm(in_store!=STORE_BANK);
      if (in_store == STORE_ALTAR) {
	in_store = STORE_NONE;
	omoney(0);
	// always creates a monster, do not take up a turn.
      }
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}
