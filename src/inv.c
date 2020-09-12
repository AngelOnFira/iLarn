/*********************************************************************
 * iLarn - Larn adapted for the PalmPilot.                           *
 * Copyright (C) 2000 Bridget Spitznagel                             *
 * iLarn is derived from Ularn by Phil Cordier                       *
 * which in turn was based on Larn by Noah Morgan                    *
 *                                                                   *
 * This program is free software; you can redistribute it and/or     *
 * modify it under the terms of the GNU General Public License       *
 * as published by the Free Software Foundation; either version 2    *
 * of the License, or (at your option) any later version.            *
 *                                                                   *
 * This program is distributed in the hope that it will be useful,   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 * GNU General Public License for more details.                      *
 *                                                                   *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to                         *
 * The Free Software Foundation, Inc.,                               *
 * 59 Temple Place - Suite 330,                                      *
 * Boston, MA  02111-1307, USA.                                      *
 *********************************************************************/
#include "palm.h"
#include "Globals.h"
#include "iLarnRsc.h"


static void you_dont_have(Char x) SEC_2;
static void make_it_fit(CharPtr buf, Short avail_space, Short maxbuflen) SEC_2;

Char ** inventory_select_items;
Short inventory_select_numItems;


/*
 * just to print a message that you don't have something
 * (which probably should never happen anyway)
 */
static void you_dont_have(Char x)
{
  Char buf[30];
  StrPrintF(buf, "You don't have item %c!", x);
  message(buf);
}

/*
 *  Drop object k, or print a message if you can't
 */
void drop_object(Short k)
{
  Short itm;
  Short playerx = env->playerx;
  Short playery = env->playery;

  if ((k<0) || (k>=NINVT)) return; // success or failure? you decide
  itm = env->iven[k];
  if (!itm) { 
    you_dont_have(k+'a');
    return;
  }
  if (env->item[playerx][playery]) { 
    message("There's something here already"); 
    return;
  }
  if (playery==MAXY-1 && playerx==33) 
    // can't drop items on the entrance
    return;
  
  env->item[playerx][playery] = itm;
  env->iarg[playerx][playery] = env->ivenarg[k];
  
  message("You drop:"); 
  show3(k); // print the item
  env->know[playerx][playery] = 0;  
  env->iven[k]=0;
  if (env->cdude[WIELD]==k)     env->cdude[WIELD] = -1;
  if (env->cdude[WEAR]==k)      env->cdude[WEAR] = -1;
  if (env->cdude[SHIELD]==k)    env->cdude[SHIELD] = -1;
  adjustcvalues(itm, env->ivenarg[k], true);
  recalc();
  /* say dropped an item so wont ask to pick it up right away */
  env->dropflag = true;
  return;
}

/*
 * Take off armor.  This is done via commandline/menu, not inventory-screen
 */
Boolean takeoff()
{
  if (env->cdude[SHIELD] != -1) { 
    env->cdude[SHIELD] = -1; 
    message("Your shield is off");
  } else if (env->cdude[WEAR] != -1) { 
    env->cdude[WEAR] = -1; 
    message("Your armor is off"); 
  } else {
    message("You aren't wearing any armor");
    return false;
  }
  recalc();
  //  print_stats();
  return true;
}


/*
 *  This curtails a string making it small enough to fit an available space
 */
static void make_it_fit(CharPtr buf, Short avail_space, Short maxbuflen)
{
  Boolean fits;
  Int length_msg, width_msg;
  length_msg = StrLen(buf);
  width_msg = avail_space;
  FntCharsInWidth(buf, &width_msg, &length_msg, &fits);
  if (!fits) {
    width_msg -= 14; /* make room for ducklings, er, "..." */
    FntCharsInWidth(buf, &width_msg, &length_msg, &fits);
    if (length_msg+5 > maxbuflen)
      length_msg -= 5; /* to be sure.  actually this will never happen? */
    buf[length_msg+1] = '.';
    buf[length_msg+2] = '.';
    buf[length_msg+3] = '.';
    buf[length_msg+4] = '\0';
  }
}

/*
 *  Free some space that we used in the inventory-selection form
 */
#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
void free_inventory_select(FormPtr frm)
{
  ListPtr lst;
  Short i;
  VoidHand h;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
  LstSetListChoices(lst, NULL, 0);
  for (i = 0 ; i < inventory_select_numItems ; i++) {
    free_me(inventory_select_items[i]);
  }
  if (inventory_select_numItems > 0) {
    free_me(inventory_select_items);
  }
}
#undef free_me
/*
 *  Initialize the inventory-selection form
 *  which is also used by the trading post, incidentally
 */
void init_inventory_select(FormPtr frm, Boolean t)
{
  ListPtr lst;
  Short numItems, i, k;
  ControlPtr btn;
  Char my_buf[80]; // whatever show3_cp needs plus maybe 10

  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_i_cancel));
  if (t) {
    // we're not just an inventory list, we're a trading post.
    CtlSetLabel(btn, "Exit");
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    CtlSetLabel(btn, "Sell");
  } else {
    CtlSetLabel(btn, "Cancel");
  }

  // allocate list
  for (i = 0, numItems = 0 ; i < NINVT ; i++)
    if (env->iven[i])
      numItems++;
  if (!numItems && !t)
    LeaveForm(); //LeaveForm(true); // aiee!  bug out! (this should not happen)
  inventory_select_items = (Char **) md_malloc(sizeof(Char *) * numItems);
  inventory_select_numItems = numItems; // for freeing

  // put stuff in list
  for (i = 0, k = 0 ; i < NINVT && k < numItems ; i++) {
    if (env->iven[i]) {
      // copy a description to my_buf
      show3_cp(i, my_buf);
      make_it_fit(my_buf, 160, 80); // add "...\0" at end of fitting-ness.
      inventory_select_items[k] = (Char *) md_malloc(sizeof(Char) *
						     (StrLen(my_buf)+1) );
      StrCopy(inventory_select_items[k], my_buf); // watch that index.
      k++;
    }
  }
  // Yay.
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
  LstSetListChoices(lst, inventory_select_items, numItems);
  LstSetSelection(lst, -1);

  FrmDrawForm(frm);
  if (!t) {
    StrPrintF(my_buf, "Time left: %ld mobuls", get_mobuls());
    WinDrawChars(my_buf, StrLen(my_buf), 2, 141);
  }
}

/*
 *  Remove an item from the inventory-selection form
 *  (in its character as a trading post)
 */
void remove1_inventory_select(FormPtr frm, Short k)
{
  // leave 0 to k-1 intact...
  // free k
  // move k+1 to k, etc.
  // decrement length
  Short i;
  VoidHand h;
  ListPtr lst;
  if (k < 0 || k >= inventory_select_numItems) return; // OOB
  h = MemPtrRecoverHandle(inventory_select_items[k]);
  if (h) 
    MemHandleFree(h);
  for (i = k ; i < inventory_select_numItems-1 ; i++)
    inventory_select_items[i] = inventory_select_items[i+1];
  inventory_select_numItems--;
  // redraw!
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_i));
  LstSetListChoices(lst, inventory_select_items, inventory_select_numItems);
  LstSetSelection(lst, -1);
  LstDrawList(lst);
  specialize_labels(frm, -1, true); // clear sell-button and value
}

/*
 *  Differentiate between the inventory-selection form's original use
 *  and its trading post alter ego.
 *  Also, if the former, what item you have selected determins what
 *  'verbs' are available to you on the buttons.
 */
void specialize_labels(FormPtr frm, Short k, Boolean t)
{
  Short itm;
  ControlPtr btn;
  Short k_worn;
  //  Char buf[40];
  if (!t) {
    RectangleType r;
    RctSetRectangle(&r, 0, 141, 100, 12);
    WinEraseRectangle(&r, 0);
  }
  if ((k<0) || (k>=NINVT)) {
    // disable everything (feature)
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    CtlHideControl(btn);
    tpost_print_value(-1); // clear 'value'
    return;
  }
  if (t) {
    // we're not just an inventory list, we're a trading post.
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    //    CtlSetLabel(btn, "Sell"); // already set
    CtlShowControl(btn);
    // also, print the value of  k  at: 40, 141
    tpost_print_value(k);
    return;
  }
  itm = env->iven[k];
  // Ok, the middle button is ALWAYS drop. (turn off if no item selected.)
  // The third button, IF it is turned on, is ALWAYS wield.
  //  or unwield.....
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
  if (env->cdude[WIELD] != k)  CtlSetLabel(btn, "wield");
  else                         CtlSetLabel(btn, "put up");

  switch(itm) {
  case OLEATHER:  
  case OCHAIN:  
  case OPLATE:	
  case OSTUDLEATHER:
  case ORING:		
  case OSPLINT:	
  case OPLATEARMOR:	
  case OELVENCHAIN:
  case OSSPLATE:
  case OSHIELD:
    // Note that I'm NOT being clever about 'wear' vs 'takeoff' applicability.
    // hey, neither is Ularn.
    k_worn = (itm == OSHIELD) ? env->cdude[SHIELD] : env->cdude[WEAR];
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    if (k_worn == k) {
      CtlSetLabel(btn, "doff");
      CtlShowControl(btn); // WEAR
    } else if (k_worn == -1 &&
	       !((itm == OSHIELD) &&
		 (env->cdude[WIELD] != -1) &&
		 (env->iven[env->cdude[WIELD]] == O2SWORD))) {
      CtlSetLabel(btn, "wear");
      CtlShowControl(btn); // WEAR
    } else {
      CtlHideControl(btn);
    }
    //    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    //    CtlShowControl(btn); // drop
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    CtlShowControl(btn); // wield
    break;
  case OBOOK:
  case OSCROLL:
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    CtlSetLabel(btn, "read");
    CtlShowControl(btn); // READ
    //    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    //    CtlShowControl(btn); // drop
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    if (itm==OBOOK)  CtlShowControl(btn); // wield
    else             CtlHideControl(btn); //
    break;
  case OPOTION:
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    CtlSetLabel(btn, "quaff");
    CtlShowControl(btn); // QUAFF
    //    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    //    CtlShowControl(btn); // drop
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    CtlHideControl(btn); //
    break;
  case OCOOKIE: case OSPEED: case OSHROOMS: case OACID: case OHASH: case OCOKE:
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    CtlSetLabel(btn, "eat");
    CtlShowControl(btn); // EAT
    //    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    //    CtlShowControl(btn); // drop
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    CtlShowControl(btn); // wield
    break;
  default:
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_frob));
    if (env->cdude[WIELD] != k)
      CtlSetLabel(btn, "wield");
    else
      CtlSetLabel(btn, "put up");
    if (!((itm == O2SWORD) && (env->cdude[SHIELD] != -1)))
      CtlShowControl(btn); // WIELD (surprise)
    else
      CtlHideControl(btn);
    //    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
    //    CtlShowControl(btn); // drop
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_wield));
    CtlHideControl(btn); //
    break;
  }
  // You can drop any kind of item - unless the ground is occupied.
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_if_drop));
  if (env->item[env->playerx][env->playery])
    CtlHideControl(btn); // drop
  else
    CtlShowControl(btn); // drop

}

/* 
 *  Above, we specialized the button labels according to what item
 *  in the inventory was selected.  When the user hits a button,
 *  then, we have to do the "appropriate thing" to the item
 *  which thus depends on what the item is.
 *
 * Anything can be dropped.
 * Anything can be wielded except: scroll, potion
 * Certain objects can be worn: <list>
 * Two kinds can be read: book,    scroll
 * One kind can be quaffed:                potion
 * One kind can be eaten:                          cookie
 *
 * deduce-task returns true if the action took a turn.
 */
Boolean deduce_task(Short k, Short btn012)
{
  Short itm;
  if ((k<0) || (k>=NINVT) || (btn012 < 0) || (btn012 > 2)) {
    message("doing nothing");
    return false; // invalid inventory item or action, do nothing
  }
  itm = env->iven[k];
  // Ok, the middle button is ALWAYS drop. (turn off if no item selected.)
  // The third button, IF it is turned on, is ALWAYS wield.  or unwield.
  if (btn012 == 1) {
    if (env->cdude[TIMESTOP])
      return false; // can't drop stuff while time stopped
    drop_object(k);
    // (has been recalc'd already)
    return true;
  }
  if ((btn012 == 2) && (itm != OSCROLL) && (itm != OPOTION)) {
  wieldmebaby:
    if (env->cdude[WIELD] != k) {
      if ( (env->cdude[SHIELD] != -1) && (itm == O2SWORD) ){
	message("But one arm is busy with your shield!");
	return false;
      }
      env->cdude[WIELD] = k;
      env->cdude[LANCEDEATH] = (itm == OLANCE);  
      message("wielded");
    } else {
      env->cdude[WIELD] = -1;
      env->cdude[LANCEDEATH] = false;
      message("un-wielded");
    }
    recalc();
    //    print_stats();
    return true;
  }
  switch(itm) {
  case OLEATHER:  
  case OCHAIN:  
  case OPLATE:	
  case OSTUDLEATHER:
  case ORING:		
  case OSPLINT:	
  case OPLATEARMOR:	
  case OELVENCHAIN:
  case OSSPLATE:
    if (env->cdude[WEAR] == k) { 
      env->cdude[WEAR] = -1; 
      message("Your armor is off"); 
    } else if (env->cdude[WEAR] != -1) { 
      message("You're already wearing some armor");
      return false;
    } else {
      env->cdude[WEAR] = k;
      message("worn");
    }
    recalc();
    //    print_stats();
    return true;
    break;
  case OSHIELD:
    if (env->cdude[SHIELD] == k) { 
      env->cdude[SHIELD] = -1; 
      message("Your shield is off");
    } else if (env->cdude[SHIELD] != -1) { 
      message("You're already wearing a shield");
      return false;
    } else if ((env->cdude[WIELD] != -1) &&
	       (env->iven[env->cdude[WIELD]] == O2SWORD)) { 
      message("You're already using a 2handed sword");
      return false;
    } else {
      env->cdude[SHIELD] = k;
      message("worn");
    }
    recalc();
    //    print_stats();
    return true;
    break;
  case OBOOK:
    if (!env->cdude[TIMESTOP]) {
      if (readbook(env->ivenarg[k])) {
	env->iven[k] = 0;
	return true;
      }
    }
    break;
  case OSCROLL:
    if (!env->cdude[TIMESTOP]) {
      Boolean take_time = (env->ivenarg[k] != R_PULVR);
      if (read_scroll(env->ivenarg[k])) {
	env->iven[k] = 0;
	return take_time;// are you !(the 1 scroll type that needs a direction)
      }
    }
    break;
  case OPOTION:
    if (!env->cdude[TIMESTOP]) {
      quaffpotion(env->ivenarg[k]);
      env->iven[k] = 0;
      return true;
    }
    break;
  case OCOOKIE:
    if (!env->cdude[TIMESTOP]) {
      ocookie();
      env->iven[k] = 0;
      return true;
    }
    break;
  case OSPEED:
    if (!env->cdude[TIMESTOP]) {
      drug_speed();
      env->iven[k] = 0;
      return true;
    }
    break;
  case OSHROOMS:
    if (!env->cdude[TIMESTOP]) {
      drug_shrooms();
      env->iven[k] = 0;
      return true;
    }
    break;
  case OACID:
    if (!env->cdude[TIMESTOP]) {
      drug_acid();
      env->iven[k] = 0;
      return true;
    }
    break;
  case OHASH:
    if (!env->cdude[TIMESTOP]) {
      drug_hash();
      env->iven[k] = 0;
      return true;
    }
    break;
  case OCOKE:    
    if (!env->cdude[TIMESTOP]) {
      drug_coke();
      env->iven[k] = 0;
      return true;
    }
    break;
  default:
    goto wieldmebaby;
    break;
  }
  return false;
}
