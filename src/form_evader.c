#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/****************************************************************
 * This form is used in a couple of cases.
 * 1. You are a tax evader and
 *    a) enter DND store (Trading Post will always do business with you), or
 *    b) enter the bank (and maybe pay your taxes via wire)
 *
 * 2. You enter your home.
 *
 * Basically (1) this form just prints a bunch of text, and lets you
 * exit once you have read it.  (If you are in bank and paid taxes,
 * you exit to the 'real' bank.)
 * (2) if you have game-terminating condition, makes you win or lose
 * otherwise just prints text and lets you exit as in (1).
 ****************************************************************/

extern Short in_store;
Boolean Evader_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    FrmDrawForm(frm);
    evader_init(frm, in_store);
    handled = true;
    break;

  case keyDownEvent:
    if (e->data.keyDown.chr == 'X') {
      ControlPtr btn;
      frm = FrmGetActiveForm();
      btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_eva_exit));
      if (btn->attr.usable) 
	CtlHitControl(btn);
      handled = true;
    }
    break;
    

  case ctlSelectEvent:
    switch(e->data.ctlSelect.controlID) {
    case btn_eva_exit:
      // where you go depends on what you owe...
      LeaveForm(); // LeaveForm(true);
      if (in_store == STORE_HOME) {
	// also, figure out how to kill them if time is up
	if (get_mobuls() < 0)
	  died(269, true);
	else if (has_potion_cured())
	  died(263, true);
      }
      if ((in_store == STORE_BANK) && (!OWES_TAXES)) {
	FrmPopupForm(StoreForm); // go to the real bank form!!!  yay!
      } else
	in_store = STORE_NONE;
      handled = true;
      break;
    }
    break;

  default:
    break;
  }
  return handled;
}
