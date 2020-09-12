#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"
/***********************************************************************
 * This is the form for Dealer McDope's pad, a store where you can buy
 * four kinds of drugs.  Hey I don't make this stuff up, I just port it.
 ***********************************************************************/


Boolean Pad_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  ListPtr lst;
  //  CharPtr label;
  Short store_item;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    FrmDrawForm(frm);
    pad_gold();
    handled = true;
    break;

  case lstSelectEvent:
    handled = true;
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
    case btn_pad_buy:
      lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_pad));
      store_item = LstGetSelection(lst);
      if (store_item >= 0 && store_item <= 4)
	pad_buy(frm, store_item);
      handled = true;
      break;
    case btn_pad_split:
      LeaveForm(); // LeaveForm(true);
      //      message_clear(); // do I need this?
      print_stats(); // in case you spent money
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}
