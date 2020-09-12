#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form is the select-thing-from-inventory form.
 * It is used when you list your inventory, and in the trading post.
 *
 * When player selects an item, buttons are enabled or disabled (and may
 * also be re-labeled) to let him/her do the actions appropriate to that
 * type of item and to his/her current location.
 * There is also a cancel button.
 * If the player hits a variably-labeled button, we call a routine that
 * figures out (based on item type) what the action is supposed to be.
 **********************************************************************/

extern Short in_store;
Short inventory_item; // (a to z) - 'a' .. index into env->iven, etc.
extern Short inventory_select_numItems; // to help w/ keyboard support

Boolean InvSelect_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  ListPtr lst;
  FormPtr frm;
  CharPtr label;
  Char c;
  Short selected_btn, k, s;
  Boolean took_time;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    init_inventory_select(frm, (in_store==STORE_TPOST));
    // incorporates FrmDrawForm!
    inventory_item = -1;
    handled = true;
    break;

  case lstSelectEvent:
    frm = FrmGetActiveForm();
    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
    label = LstGetSelectionText(lst, e->data.lstSelect.selection);
    if (label) inventory_item = label[0] - 'a';
    else inventory_item = -1; // bug if this happens
    //    inventory_item = e->data.lstSelect.selection;
    specialize_labels(frm, inventory_item, (in_store==STORE_TPOST));
    handled = true;
    break;

  case keyDownEvent:
    // hardware button -- or else graffiti.
    frm = FrmGetActiveForm();
    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
    switch(e->data.keyDown.chr) {
    case pageUpChr:
      if (LstScrollList(lst, winUp, 11)) {//there're 11 items visible at a time
	LstSetSelection(lst, -1);
	inventory_item = -1;
      }
      handled = true;
      break;
    case pageDownChr:
      if (LstScrollList(lst, winDown, 11)) {
	LstSetSelection(lst, -1);
	inventory_item = -1;
      }
      handled = true;
      break;
    default:
      // Add some keyboard support: write a letter to select the item.
      c = e->data.keyDown.chr;
      if (c >= 'a' && c <= 'z') {
	// Find the line starting with this letter. Lines in order with gaps.
	s = inventory_select_numItems;
	inventory_item = -1;
	while (s > -1) {
	  label = LstGetSelectionText(lst, s);
	  if (label && (label[0] == c)) {
	    LstSetSelection(lst, s);
	    inventory_item = label[0] - 'a';
	    break;
	  }
	  s--;
	}
	if (inventory_item == -1) LstSetSelection(lst, -1);
	specialize_labels(frm, inventory_item, (in_store==STORE_TPOST));
	handled = true;
      } else {
	// Keyboard support for buttons.  Woo hoo.
	ControlPtr btn;
	Word objid = -1;
	switch(c) {
	case '\n': case '1': case 'S': objid = btn_if_frob;   break;
	case 'D':  case '2': objid = btn_if_drop;   break;
	case 'W':  case '3': objid = btn_if_wield;  break;
	case 'C':  case 'X': objid = btn_i_cancel;  break;
	}
	if (objid != -1) {
	  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objid));
	  if (btn->attr.usable) 
	    CtlHitControl(btn); // kind of silly, I could just dispatch it..
	  handled = true;
	}
      }
      break;
    }
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
    case btn_if_frob:
      if (in_store == STORE_TPOST && inventory_item != -1) {
	// pop up something to sell it..
	// then, we have to redo the darned inventory, probably
	if (tpost_sell_item(inventory_item)) {
	  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
	  k = LstGetSelection(lst);
	  remove1_inventory_select(frm, k);
	}
	break;
      }
      // if we're not in the TPOST then we just fall through.
    case btn_if_drop:
    case btn_if_wield:
      selected_btn = e->data.ctlSelect.controlID - btn_if_frob;
      free_inventory_select(frm);
      LeaveForm(); // LeaveForm(true);
      /* It is important to do 'stuff' AFTER LeaveForm for msg to display */
      message_clear();
      if (inventory_item != -1) {
	// Do The Right Thing
	took_time = deduce_task(inventory_item, selected_btn);
	if (took_time)
	  start_of_Play_Level_loop();
	//else if (env->pending_splash)
	//	  show_messages();  // FrmPopupForm(SplashForm);
      }
      handled = true;
      break;
    case btn_i_cancel:
      free_inventory_select(frm);
      LeaveForm();
      if (in_store == STORE_TPOST)
	print_stats(); // in case you spent money
      handled = true;
      break;
    }

  default:
    break;
  }

  return handled;
}
