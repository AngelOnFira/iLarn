#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form is used for DnD store, college, and bank.
 * It has a list of items which you can select. (item, course, or gem).
 * To the right of this list, there is a column of text (cost of
 * item, duration of course, or value of gem).
 * There are various buttons at the bottom, and perhaps some text
 * above them.  The buttons are enabled/disabled/relabeled according
 * to the store that you are in.
 * In dnd store: you can buy item, page through inventory, or exit
 * In college: you can enroll in a course or exit
 * In bank: you can sell a gem, go to the Transaction form, or exit
 ***********************************************************************/

extern Short in_store;  // can be one of _DND, _COLLEGE, or _BANK.
extern Short store_itmlst_len; // added for keyboard support

Boolean Store_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  ListPtr lst;
  CharPtr label;
  Char c;
  ControlPtr btn;
  Short objid, store_item;
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    if (in_store == STORE_DND) {
      dndstore_init(frm);
      FrmDrawForm(frm);
      /* this must be AFTER draw form */
      dndstore_list_tens(frm, 0);
      dndstore_btns(frm);
    } else if (in_store == STORE_COLLEGE) {
      college_init(frm);
      FrmDrawForm(frm);
      college_mumble(env->cdude[GOLD]);
    } else if (in_store == STORE_BANK) {
      bank_init(frm);
      FrmDrawForm(frm);
      bank_init_buttons(frm);
      bank_mumble();
    } else {
      FrmDrawForm(frm); // error if we get to here.
    }
    handled = true;
    break;

    //  case lstSelectEvent:
    //    //    frm = FrmGetActiveForm();
    //    //    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
    //    //    store_item = LstGetSelection(lst);
    //    // (0-9, -1 if none selected.)
    //    handled = true;
    //    break;

  case keyDownEvent:
    // hardware button -- or else graffiti.
    frm = FrmGetActiveForm();
    c = e->data.keyDown.chr;
    // Rearranged for keyboard support.
    //
    objid = -1;
    if (c == 'X')
      objid = btn_st_exit;
    else if (((c == 'S') && (in_store == STORE_BANK) &&
	      (store_itmlst_len > 0)) ||
	     ((c == 'B') && (in_store == STORE_DND)) ||
	     ((c == 'E') && (in_store == STORE_COLLEGE)))
      objid = btn_st_sell;
    else if (c == 'T' && (in_store == STORE_BANK))
      objid = btn_st_trans;
    // Else objid remains -1.
    if (objid != -1) {
      btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objid));
      if (btn->attr.usable) 
	CtlHitControl(btn);
      handled = true;
      return handled;
    }
    // Ok, didn't match those conditions, try this then,
    switch(c) {
    case ' ':
      // Select next item in list.
      lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
      store_item = LstGetSelection(lst) + 1;
      if ((store_item < 0) || (store_item >= store_itmlst_len))
	store_item = 0;
      LstSetSelection(lst, store_item);
      // (Will that work?)
      break;
    case pageUpChr: case '<':
      if (in_store == STORE_DND) {
	dndstore_list_tens(frm, -1);
	handled = true;
      }
      break;
    case pageDownChr: case '>':
      if (in_store == STORE_DND) {
	dndstore_list_tens(frm, 1);
	handled = true;
      }
      break;
    default:
      if (in_store == STORE_BANK && 'a' <= c && c <= 'z') {
	// see if it matches a list item. 
	// ......
      }
      break;
    }
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
    case btn_st_sell:
      lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
      store_item = LstGetSelection(lst);
      if (in_store == STORE_DND) {
	if (store_item >= 0 && store_item <= 9)
	  dndstore_buy(frm, store_item);
      } else if (in_store == STORE_COLLEGE) {
	if (store_item >= 0 && store_item <= 7)
	  college_enroll(frm, store_item);
      } else if (in_store == STORE_BANK) {
	label = LstGetSelectionText(lst, store_item);
	// check store_item and label[0] are within bounds?  maybe?
	bank_sell(frm, label[0], store_item);
      }
      handled = true;
      break;
    case btn_st_prev:
      if (in_store == STORE_DND) {
	dndstore_list_tens(frm, -1);
      }
      handled = true;
      break;
    case btn_st_next:
      if (in_store == STORE_DND) {
	dndstore_list_tens(frm, 1);
      }
      handled = true;
      break;
    case btn_st_trans:
      FrmPopupForm(TransForm);
      handled = true;
      break;
    case btn_st_exit:
      if (in_store == STORE_DND ||
	  in_store == STORE_COLLEGE ||
	  in_store == STORE_BANK) {
	store_cleanup(frm);
      }
      LeaveForm(); // LeaveForm(true);
      in_store = STORE_NONE;
      print_stats();
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}
