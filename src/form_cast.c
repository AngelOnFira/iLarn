#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/********************************************************************
 *  This form is reused for several things.
 *  1. (when player decides to (c)ast a spell)
 *     Selecting which spell the player would like to cast.
 *  2. (when player is granted a new spell by a genie)
 *     Selecting which spell the player would like to learn.
 *  3. (when player has selected the genocide spell)
 *     Selecting which kind of monster to eliminate from game.
 *  4. (when player asks to (d)isplay known items)
 *     Listing the potions and scrolls the player has tried.
 *
 * The form has a list.  This list is an ordered subset of all things
 * of a certain kind (spells; monsters; potions + scrolls).
 * You can scroll the list with pgup/pgdown (the list itself takes care
 * of putting up/down arrows in the screen and handling them).
 *
 * There are two buttons - one to perform the desired effect (if any)
 * and the other to do nothing (i.e. cancel).  The effects are
 * 1. cast 2. learn 3. genocide 4. noop
 * Their object is the selected item.. if any..
 ************************************************************ */

extern Short cast_form_use;
Short cast_item; // (a to z) - 'a' .. index into env->iven, etc.
extern Short cast_qty;
Boolean Cast_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  ListPtr lst;
  FormPtr frm;
  CharPtr label;
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* ...init form... */
    if (cast_form_use == CAST_SPELL || cast_form_use == CAST_GENIE)
      init_cast_select(frm); // select a spell
    else if (cast_form_use == CAST_DISPLAY)
      init_known_items(frm); // view potions/scrolls
    else if (cast_form_use == CAST_GENOCIDE)
      init_doomed_monsters(frm); // select a monster
    cast_item = -1;
    FrmDrawForm(frm);
    /* this must be AFTER draw form */
    handled = true;
    break;

  case lstSelectEvent:
    if (cast_form_use == CAST_SPELL || cast_form_use == CAST_GENIE) {
      frm = FrmGetActiveForm();
      lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
      label = LstGetSelectionText(lst, e->data.lstSelect.selection);
      if (label) {
	if (label[0] >= 'a')   cast_item = label[0] - 'a';
	else                  cast_item = 26 + label[0] - 'A';
      } else cast_item = -1; // bug if this happens
    } else if (cast_form_use == CAST_GENOCIDE) {
      // hm, how do I figure out what monster to nuke?
      // list them all in order with blank spots for missing ones
      // then just go by the index of the selection.
      cast_item = 1 + e->data.lstSelect.selection; // will this DTRT?
    }
    handled = true;
    break;

  case keyDownEvent:
    // hardware button -- or else graffiti.
    frm = FrmGetActiveForm();
    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
    switch(e->data.keyDown.chr) {
    case pageUpChr:
      if (LstScrollList(lst, winUp, 11))//there are 11 spells visible at a time
	LstSetSelection(lst, -1);
      handled = true;
      break;
    case pageDownChr:
      if (LstScrollList(lst, winDown, 11))
	LstSetSelection(lst, -1);
      handled = true;
      break;
    default:
      // this is all new extra keyboard support for version 0.24:
      if (cast_form_use != CAST_SPELL)
	break; // I'm not going to try this for CAST_GENIE or CAST_GENOCIDE
      {
	Short s;
	Char c;
	c = e->data.keyDown.chr;
	if (c >= 'a' && c <= 'z') {
	  s = e->data.keyDown.chr - 'a';
	} else if (c >= 'A' && c <= 'Z') {
	  s = (e->data.keyDown.chr - 'A') + 26;
	} else break;
	cast_item = -1;
	if (s > cast_qty-1) s = cast_qty-1;
	do {
	  label = LstGetSelectionText(lst, s);
	  if (label && (label[0] == c)) {
	    LstSetSelection(lst, s); // index in list,
	    if (c >= 'a')   cast_item = c - 'a';
	    else           cast_item = 26 + c - 'A';
	    break;
	  }
	} while (--s >= 0);
	if (cast_item == -1) LstSetSelection(lst, -1);
	// ok, done, I think.
      }
      break;
    }
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
    case btn_ca_ok:
      if (cast_form_use == CAST_DISPLAY || cast_item != -1) {
	free_cast_select(frm);
	LeaveForm(); // LeaveForm(true);
	//      i_used_too_much_heap();
	/* It is important to do 'stuff' AFTER LeaveForm for msg to display */
	if (cast_form_use == CAST_SPELL) {
	  cast_form_use = CAST_NONE;
	  cast(cast_item); // cast the selected spell
	  // cast calls start of play level loop
	} else if (cast_form_use == CAST_GENIE) {
	  obrasslamp_learn(cast_item); // learn selected new spell
	  // does not call start of play level loop (should it? hm.)
	} else if (cast_form_use == CAST_GENOCIDE) {
	  //	  env->cdude[GOLD] = cast_item;// TESTING
	  genmonst(cast_item); // obliterate selected monster type
	  // genmonst calls start of play level loop
	}
      }
      handled = true;
      break;
    case btn_ca_cancel:
      free_cast_select(frm);
      LeaveForm(); // LeaveForm(true);
      cast_form_use = CAST_NONE;
      handled = true;
      break;
    }

  default:
    break;
  }
  return handled;
}
