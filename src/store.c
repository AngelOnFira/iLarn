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
//#include "proto2.h"
#include "iLarnRsc.h"

static void college_remark(Char *buf) SEC_1;
static void pay_tax(Long amt) SEC_1;
//Long outstanding_taxes = 0;
//Long damage_taxes = 0; // what subset of o_t is due to nuking LRS
Boolean has_tax_form = false;
Short dndstore_current_tens = 0;
Char **store_select_items = 0;
Short store_itmlst_len = 0;
Long gemvalue[8] = {0,0,0,0,0,0,0,0};
// there are only 8 choices here
const Char college_courses[8][25] = {
  "Fighters Training I",
  "Fighters Training II",
  "Introduction to Wizardry",
  "Applied Wizardry",
  "Behavioral Psychology",
  "Faith for Today",
  "Contemporary Dance",
  "History of Ularn"
};
const Short units[8] = {10,15,10,20,10,10,10,5};
//static Short taken[8] = {1,2, 4,8, 16,32, 64,128};
// replace taken[i] with 1<<i
#define TAKEN 1<<
Long last_item_value;
Short last_item_qty_index;
Short last_item_qty;



Short packweight()
{
  Short i,k;
  Long m = env->cdude[GOLD] / 1000; // env->cdude[GOLD] / 1024;
  k = (Short) m;
  for (i = 0; i < NINVT; i++)
    switch(env->iven[i]) {
    case OSSPLATE:   
    case OPLATEARMOR: 
      k += 40;
      break;
    case OPLATE:
      k += 35;	
      break;
    case OHAMMER:	
      k += 30;	
      break;
    case OSPLINT:	
      k += 26;	
      break;
      
    case OCHAIN:
    case OBATTLEAXE:   		
    case O2SWORD:		
      k += 23;	
      break;
      
    case OLONGSWORD:
    case OPSTAFF:
    case OSWORD:
    case ORING:
    case OFLAIL:		
      k += 20;	
      break;
      
    case OELVENCHAIN: 	
    case OSWORDofSLASHING:	
    case OLANCE:
    case OSLAYER:
    case OSTUDLEATHER:	
      k += 15;	
      break;
      
      
    case OLEATHER:
    case OSPEAR:		
      k += 8;		
      break;
      
    case OORBOFDRAGON:
    case OORB:
    case OBELT:		
      k += 4;		
      break;
      
    case OSHIELD:		
      k += 7;		
      break;
    case OCHEST:		
      k += 30 + env->ivenarg[i];	
      break;
    default:		
      k++;
    };
  return(k);
}

/*
  routine to tell if player can carry one more thing
  returns true if pockets are full (i.e. player can't)
  */
// this same 'i<limit' kind of check is also incorporated into 'take'
Boolean pocketfull()
{
  Short i,limit; 
  limit = 15 + ( env->cdude[LEVEL]>>1 );
  if (limit > NINVT)  limit = NINVT;
  for (i=0; i<limit; i++) if (env->iven[i]==0) return false;
  return true;
}


/******************************************************************
                      LRS
 ******************************************************************/

// It's the caller's duty to make sure amt>0 and to decrement player's funds
static void pay_tax(Long amt)
{
  env->outstanding_taxes -= amt;
  if (env->outstanding_taxes < 0) env->outstanding_taxes = 0;
  env->damage_taxes -= amt;
  if (env->damage_taxes < 0) env->damage_taxes = 0;
  set_taxes(env->outstanding_taxes, env->damage_taxes); // in dead.c
}
// used if you blow up the LRS
void add_tax(Long amt, Boolean damage)
{
  env->outstanding_taxes += amt;
  if (env->outstanding_taxes < 0) env->outstanding_taxes = 0;
  if (damage) {
    env->damage_taxes += amt;
    if (env->damage_taxes < 0) env->damage_taxes = 0;
  }
}
// in case you ask
void print_tax()
{
  Char buf[40];
  if (env->outstanding_taxes)
    StrPrintF(buf, "You owe %ld", env->outstanding_taxes);
  else
    StrPrintF(buf, "You do not owe any taxes.");
  message(buf);
}


#define BLAH 35
void lrs_init_or_refresh(FormPtr frm)
{
  Char buf[80];
  RectangleType r;
  Short y = BLAH;
  ControlPtr btn;
  FieldPtr fld;
  RctSetRectangle(&r, 2, y, 156, 33);
  WinEraseRectangle(&r, 0);
  if (env->outstanding_taxes > 0) {
    // enable cash
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_lrs_dp));
    CtlShowControl(btn);
    StrPrintF(buf, "Amount:");
    WinDrawChars(buf, StrLen(buf), 55, 109); // next to field
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_lrs_amt));
    FldSetUsable(fld, true);
    // Hey, it would be nice to set fld to min(outstanding, GOLD);
    // I'm not sure how to do that properly though.
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_lrs_wd));
    if (has_tax_form)      CtlHideControl(btn);
    else                   CtlShowControl(btn);
    StrPrintF(buf, "You presently owe %ld gp.", env->outstanding_taxes);
    if (env->damage_taxes > 0) {
      WinDrawChars(buf, StrLen(buf), 10, y);
      y += 11;
      StrPrintF(buf, "(%ld to repair damages)", env->damage_taxes);
    }
  } else {
    StrPrintF(buf, "You do not owe us any taxes.");
    // disable cash, transfer
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_lrs_dp));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_lrs_wd));
    CtlHideControl(btn);
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fld_lrs_amt));
    FldSetUsable(fld, false); // HEY - why doesn't this make it disappear???!
    RctSetRectangle(&r, 2, 90, 156, 50);  // grumble...
    WinEraseRectangle(&r, 0);
  }
  WinDrawChars(buf, StrLen(buf), 10, y);
  y += 11;
  if (env->cdude[GOLD] > 0)
    StrPrintF(buf, "You have %ld gp.", env->cdude[GOLD]);
  else
    StrPrintF(buf, "You have no gold pieces.");
  WinDrawChars(buf, StrLen(buf), 10, y);
}

void lrs_pay_cash(FormPtr frm, Long amt)
{
  Char buf[80];
  Short y = BLAH+35;
  if (amt <= 0) return;
  amt = min(amt, env->outstanding_taxes);
  if (amt <= env->cdude[GOLD]) {
    env->cdude[GOLD] -= amt;
    pay_tax(amt);
    lrs_init_or_refresh(frm);
  } else {
    StrPrintF(buf, "You don't have %ld gp!", amt);
    WinDrawChars(buf, StrLen(buf), 5, y);
  }
}

void lrs_req_trans(FormPtr frm)
{
  Char buf[80];
  Long amt;
  Short y = BLAH+35;
  ControlPtr btn;
  RectangleType r;
  RctSetRectangle(&r, 2, y, 156, 22);
  //  RctSetRectangle(&r, 2, BLAH, 156, 22);
  //  WinEraseRectangle(&r, 0);
  StrPrintF(buf, "Just a second while I phone them...");
  WinDrawChars(buf, StrLen(buf), 10, y);
  nap(1000);
  WinEraseRectangle(&r, 0);
  if (rund(86) < 45) {
    StrPrintF(buf, "Sorry, but their line's engaged");
    WinDrawChars(buf, StrLen(buf), 10, y);
  } else {
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_lrs_wd));
    if (rund(20) < 11) {
      StrPrintF(buf, "They want a signed authorization.");
      WinDrawChars(buf, StrLen(buf), 5, y);
      StrPrintF(buf, "Here's the form, take it to the bank.");
      WinDrawChars(buf, StrLen(buf), 5, y+11);
      CtlHideControl(btn);
      has_tax_form = true;
    } else if (rund(20) < 10) {
      // beep
      StrPrintF(buf, "The $%& network is down again!");
      WinDrawChars(buf, StrLen(buf), 10, y);
    } else {
      amt = min(env->cdude[BANKACCOUNT], env->outstanding_taxes);
      if (amt > 0) {
	StrPrintF(buf, "They transferred %ldgp.", amt);
	WinDrawChars(buf, StrLen(buf), 10, y);
	env->cdude[BANKACCOUNT] -= amt;
	pay_tax(amt);
	lrs_init_or_refresh(frm);
      } else {
	StrPrintF(buf, "They say that you're out of funds.");
	WinDrawChars(buf, StrLen(buf), 5, y);
	// might as well disable the transfer button eh?
	CtlHideControl(btn);
      }
    }
  }
}



/******************************************************************
                      HOME
 ******************************************************************/

// do you have the cure-dianthroritis potion
Boolean has_potion_cured()
{
  Short i;
  for (i = 0 ; i < NINVT ; i++)
    if ( env->iven[i] == OPOTION )
      if ( env->ivenarg[i] == P_CURED )
        return true;
  return false;
}

/******************************************************************
                      MULTIPURPOSE
 ******************************************************************/

// create gold item and remove from player's cash.
void drop_dough(Long amt)
{
  Short k, x = env->playerx, y = env->playery;
  Char buf[40];
  if (amt <= 0 || amt > env->cdude[GOLD]) return; // caller is insane
  if (amt <= 32767) { 
    env->item[x][y] = OGOLDPILE;
    k = amt;
  } else if (amt <= 32767L * 10) {
    // 8's were 10's
    env->item[x][y] = ODGOLD;
    k = amt / 10;
    amt = 10L * k;
  } else if (amt <= 32767L * 100) { 
    // 128's were 100's
    env->item[x][y] = OMAXGOLD;
    k = amt / 100;
    amt = 100L * k;
  } else if (amt <= 32767L * 1000) { 
    // 1024's were 1000's
    env->item[x][y] = OKGOLD;
    k = amt / 1000;
    amt = 1000 * k;
  } else {
    env->item[x][y] = OKGOLD;
    k = 32767; 
    amt = 32767000L; 
  }
  env->cdude[GOLD] -= amt;
  StrPrintF(buf, "You drop %ld gold pieces", (Long) amt);
  env->iarg[x][y] = k;
  env->know[x][y] = 0;
  print_stats();
  message(buf);
  /* say dropped an item so won't pick it up right away */
  env->dropflag = true;
}

// This is the initialization for a multipurpose form that
// mostly just displays a bunch of text.
// It is used for when you owe taxes and try to enter a store,
// and also for your home.
const Char at_home[23][38] = {
  "Welcome home . . . . Latest",
  "word from the doctor is not good.",
  //
  "Congratulations.  You found the",
  "potion of cure dianthroritis!",
  //
  "The diagnosis is confirmed as",
  "dianthroritis.  The doctor estimates",
  "that your daughter has only %ld",
  "mobuls left in this world.",
  "Her only hope is the very rare potion",
  "of cure dianthroritis, rumored to be",
  "found in the depths of the caves.",
  //
  "The doctor is now administering the",
  "potion, and in a few moments your",
  "daughter should be well on her way",
  "to recovery.  The potion is . . .",
  "working!  The doctor thinks that",
  "your daughter will recover in a few",
  "days.  Congratulations!",
  //
  "The doctor has the sad duty to",
  "inform you that your daughter has",
  "died!  In your agony, you kill the",
  "doctor, your spouse, and yourself!",
  "Too bad."
};

void evader_init(FormPtr frm, Short in_store)
{
  Char buf[80];
  Short y = 12, i;
  Long amt;
  Boolean has_potion, time_up;

  if (in_store == STORE_BANK) {
    FrmCopyTitle(frm, "First National Bank of Ularn");
    // print stuff
    StrPrintF(buf, "The Larn Revenue Service has");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "ordered that your account be frozen");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "until all levied taxes of %ldgp", env->outstanding_taxes);
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "are paid.");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    if (has_tax_form) {
      // if you have the tax form, dock some funds
      //      nap(1000);
      y += 11;
      StrPrintF(buf, "I see that you brought a signed form");
      WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
      StrPrintF(buf, "authorizing a transfer of up to");
      WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
      StrPrintF(buf, "4095gp from your account to theirs.");
      WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
      if (env->cdude[BANKACCOUNT] > 0) {
	amt = min(4095, env->cdude[BANKACCOUNT]);
	amt = min(amt, env->outstanding_taxes);
	env->cdude[BANKACCOUNT] -= amt;
	pay_tax(amt);
	//      nap(1000);
	y += 11;
	StrPrintF(buf, "We transferred %ld gp to the LRS", amt);
	WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
	StrPrintF(buf,"leaving %ld in your account.", env->cdude[BANKACCOUNT]);
	WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
	has_tax_form = false;
      } else {
	y += 11;
	StrPrintF(buf, "Unfortunately your balance is ZERO");
	WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
	StrPrintF(buf, "so we can't help you.");
	WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
      }
    }
  } else if (in_store == STORE_DND) {
    FrmCopyTitle(frm, "Ye Olde Dungeon Emporium");
    // print stuff
    StrPrintF(buf, "The Larn Revenue Service has");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "ordered us not to do business with");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "tax evaders.");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "They have also told us that you owe");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "%ld gp in back taxes..", env->outstanding_taxes);
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "As we must comply with the law, we");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "cannot serve you at this time.");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
    StrPrintF(buf, "Soo sorry.");
    WinDrawChars(buf, StrLen(buf), 5, y);    y += 11;
  } else if (in_store == STORE_HOME) {
    time_up = (get_mobuls() < 0);
    has_potion = has_potion_cured();
    FrmCopyTitle(frm, "Your Home");
    i = (has_potion) ? 2 : 0;
    StrPrintF(buf, at_home[i++]);
    WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
    StrPrintF(buf, at_home[i++]);
    WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
    y += 11;
    if (!time_up) {
      i = (has_potion) ? 11 : 4;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++], get_mobuls());
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      y += 11;
      if (has_potion) nap(1000);
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
    } else {
      i = 18;
      if (has_potion) {
	StrPrintF(buf, "However...");
	WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      }
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
      y += 11;
      StrPrintF(buf, at_home[i++]);
      WinDrawChars(buf, StrLen(buf), 2, y);    y += 11;
    }
  }
  // else, someone screwed up.
}

// used in stores where you buy stuff, to show how much cash you have
void store_gold(Long gold)
{
  Char buf[80];
  RectangleType r;
  Short y = 11 * store_itmlst_len + 18; // 128 for 10; ...
  RctSetRectangle(&r, 5, y, 150, 11);
  WinEraseRectangle(&r, 0);
  StrPrintF(buf, "You have %ld gold pieces.", gold);
  WinDrawChars(buf, StrLen(buf), 5, y);
}
/******************************************************************
                      BANK
 ******************************************************************/

void bank_gold(Long gold)
{
  Char buf[80];
  RectangleType r;
  Short y = 11 * store_itmlst_len + 18 + 11;
  RctSetRectangle(&r, 5, y, 150, 11);
  WinEraseRectangle(&r, 0);
  StrPrintF(buf, "You have %ld in the bank.", gold);
  WinDrawChars(buf, StrLen(buf), 5, y);
}
void bank_interest()
{
  Short i; // Should not need to be a long.. it can't be more than max mobuls
    if (env->cdude[BANKACCOUNT] <= 0) 
      env->cdude[BANKACCOUNT] = 0;
    else if (env->cdude[BANKACCOUNT] < 1000000) {
      i = (env->gtime - env->lasttime) / 100; /* # mobuls elapsed */
      //      i = (env->gtime - env->lasttime) / 128; /* # mobuls elapsed */
      while ((i-- > 0) && (env->cdude[BANKACCOUNT] < 1000000))
	env->cdude[BANKACCOUNT] += env->cdude[BANKACCOUNT]/250;
	//	env->cdude[BANKACCOUNT] += env->cdude[BANKACCOUNT]/256;
      if (env->cdude[BANKACCOUNT] > 1000000) 
	env->cdude[BANKACCOUNT] = 1000000; /* interest limit */
    }
    //env->lasttime = env->gtime;
    env->lasttime = (env->gtime/100)*100;
}

// Ok, here the list choices are gemstones in your inventory.
// Welcome to the 8th level branch office of the First National Bank of Ularn.
//
//      Gemstone              Appraisal         Gemstone              Appraisal
//d) a brilliant diamond           1600   f) an enchanting emerald         1100
//                              You have        0 gold pieces in the bank.
//                                       You have      212 gold pieces
//Your wish? [(d) deposit, (w) withdraw, (s) sell a stone, or Esc]  
// ...I'm not going to do prev/next buttons.  So there.

void bank_init(FormPtr frm)
{
  ListPtr lst;
  Short i, j, gem;
  Char c;

  // don't forget to CALCULATE INTEREST:
  bank_interest();

  store_itmlst_len = 8;
  store_select_items = (Char **) md_malloc(sizeof(Char *) * store_itmlst_len);
  for (i = 0, j = 0 ; i < store_itmlst_len && j < NINVT ; j++) {
    gem = env->iven[j];
    switch(gem) {
    case OLARNEYE: 
    case ODIAMOND: 
    case OEMERALD: 
    case ORUBY: 
    case OSAPPHIRE: 
      // calc. value
      if (gem == OLARNEYE) {
	// //	gemvalue[i] = 250000 - ((env->gtime*7)/100)*100; // shrug.
	//	gemvalue[i] = 250000 - env->gtime*7; // won't look as tidy..
	//	if (gemvalue[i] < 50000)
	  gemvalue[i] = 50000;
      } else
	gemvalue[i] = (255 & env->ivenarg[j]) * 100; // shrug.
      // get name
      store_select_items[i] = (Char *) md_malloc(sizeof(Char) * 25);
      //      StrPrintF(store_select_items[i], "%c) %s", 'a' + j,
      //		get_objectname(gem));
      c = 'a' + j;
      StrPrintF(store_select_items[i], "%c) %s", c, get_objectname(gem));
      i++;
      break;
    default:
      break; // not a gem
    }
  }
  if (i < store_itmlst_len) store_itmlst_len = i; // might look better
  // if store_itmlst_len==0, it seems to DTRT and disable the list
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, store_select_items, store_itmlst_len);
  LstSetSelection(lst, -1);
  LstSetHeight(lst, store_itmlst_len);
  // title
  FrmCopyTitle(frm, "First National Bank of Ularn");
}
void bank_init_buttons(FormPtr frm)
{
  ControlPtr btn;
  // buttons
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_sell));
  CtlSetLabel(btn, "Sell");
  CtlShowControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_prev));
  CtlHideControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_next));
  CtlHideControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_trans));
  CtlShowControl(btn);
}

void bank_mumble()
{
  Short i, tmp, y = 15;
  Char buf[10];

  for (i = 0 ; i < store_itmlst_len ; i++) {
    // print the gem values
    StrPrintF(buf, "%ld", gemvalue[i]);
    tmp = 6 - StrLen(buf); // lame attempt at right-justification...
    WinDrawChars(buf, StrLen(buf), 126 + tmp*5, y);
    y += 11;
  }
  store_gold(env->cdude[GOLD]);
  bank_gold(env->cdude[BANKACCOUNT]);
}

void bank_sell(FormPtr frm, Char inv_i, Short list_j)
{
  ListPtr lst;
  RectangleType r;
  Long newg;
  Short i = inv_i - 'a';

  if (i < 0 || i >= NINVT ||
      list_j < 0 || list_j >= store_itmlst_len) return; // only if bug
  if (env->iven[i] == 0) return; // only if bug
  newg = env->cdude[GOLD] + gemvalue[list_j];
  if (newg > 0) env->cdude[GOLD] = newg;   // fear of overflow
  else if (env->cdude[GOLD]) env->cdude[GOLD] = MAXGOLD;
  // now destroy the inventory item
  env->iven[i] = 0;
  if (env->iven[i] == OLARNEYE)
    ungot_larneye(); // make demons invisible again
  gemvalue[list_j] = 0;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, NULL, 0);
  store_select_items[list_j][0] = '\0';
  LstSetListChoices(lst, store_select_items, store_itmlst_len);
  LstSetSelection(lst, -1);
  LstDrawList(lst);
  RctSetRectangle(&r, 126, 15 + 11*list_j, 30, 11); // this right xy? I'm lazy.
  WinEraseRectangle(&r, 0);
  store_gold(env->cdude[GOLD]);
}

// if 'bank' print bank account info also
void bank_trans_init(FormPtr frm, Boolean bank)
{
  ControlPtr btn;
  Char buf[40];
  if (bank) {
    // ok done with that.
    StrPrintF(buf, "You: %ld", env->cdude[GOLD]);
    WinDrawChars(buf, StrLen(buf), 2, 13);
    StrPrintF(buf, "Bank: %ld", env->cdude[BANKACCOUNT]);
    WinDrawChars(buf, StrLen(buf), 80, 13);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_dp));
    CtlSetLabel(btn, "Deposit");
    if (env->cdude[GOLD] > 0)
      CtlShowControl(btn);
    else 
      CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_wd));
    if (env->cdude[BANKACCOUNT] > 0)
      CtlShowControl(btn);
    else 
      CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_allw));
    CtlShowControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_cancel));
    CtlSetLabel(btn, "Cancel");
  } else {
    StrPrintF(buf, "You have: %ld", env->cdude[GOLD]);
    WinDrawChars(buf, StrLen(buf), 2, 13);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_dp));
    CtlSetLabel(btn, "Drop");
    if (env->cdude[GOLD] > 0)
      CtlShowControl(btn);
    else 
      CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_wd));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_allw));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_trans_cancel));
    CtlSetLabel(btn, "None");
  }
}

// true if success, false if overdrawn
Boolean bank_transact(Long amt, Boolean deposit)
{
  Long tmp;
  if (amt > 0) {
    if (deposit) {
      if (amt <= env->cdude[GOLD]) {
	env->cdude[GOLD] -= amt;
	tmp = env->cdude[BANKACCOUNT] + amt;
	if (tmp > 0) env->cdude[BANKACCOUNT] = tmp;
	return true;
      }
      // insert snide message here, or at least a beep.
      return false;
    } else {
      if (amt <= env->cdude[BANKACCOUNT]) {
	env->cdude[BANKACCOUNT] -= amt;
	tmp = env->cdude[GOLD] + amt;
	if (tmp > 0) env->cdude[GOLD] = tmp;
	else if (env->cdude[GOLD]) env->cdude[GOLD] = MAXGOLD;
	return true;
      }
      // insert snide message here, or at least a beep.
      return false;
    }
  }
  // insert snide message here, or at least a beep.
  return false;
}

/******************************************************************
                      DND STORE
 ******************************************************************/

void dndstore_buy(FormPtr frm, Short store_item)
{
  Short j = store_item + store_itmlst_len * dndstore_current_tens;
  VoidHand vh;
  VoidPtr p;
  Short *store_data_p; // in quadruples ($, obj, arg, itm_qty)
  Long itm_cost;
  Short itm_type, itm_arg, itm_qty;
  Char buf[80];
  ListPtr lst;
  RectangleType r;
  RctSetRectangle(&r, 5,128, 150, 11); // matches store_gold
  WinEraseRectangle(&r, 0);

  if (j >= 92) {
    StrPrintF(buf, "Sorry, we're out of that item."); // also if slot >92*5
  } else {
    j *= 5;
    vh = DmQueryRecord(iLarnDB, DNDSTORE_RECORD); // "read-only, i promise"
    p = MemHandleLock(vh);
    if (!p) return;   // if !p, we are all in trouble.
    store_data_p = (Short *) p;
    itm_cost = store_data_p[j++];
    itm_cost *= 10;
    itm_type = store_data_p[j++];
    itm_arg  = store_data_p[j++];
    itm_qty  = store_data_p[j];
    MemHandleUnlock(vh);
    // hang on to j (as a p-offset) so that we can, maybe, decrement qty.

    if (env->cdude[GOLD] < itm_cost)
      StrPrintF(buf, "You don't have enough gold to pay!");
    else if (itm_qty <= 0)
      StrPrintF(buf, "Sorry, we're out of that item."); // also if slot >92*5
    else if (pocketfull())
      StrPrintF(buf, "You can't carry anything more!");
    else {
      itm_qty--;
      // update the quantity in the database record
      vh = DmGetRecord(iLarnDB, DNDSTORE_RECORD);
      p = MemHandleLock(vh);
      if (!p) return;   // if !p, we are all in trouble.
      DmWrite(p, j*sizeof(Short), &itm_qty, sizeof(Short));
      MemHandleUnlock(vh);
      DmReleaseRecord(iLarnDB, DNDSTORE_RECORD, true);
      // ok we survived that.
      // -  take(obj,arg)
      take(itm_type, itm_arg, false);
      // -  'know' it if it's a scroll or potion
      if (itm_type == OPOTION && itm_arg >= 0 && itm_arg < MAXPOTION) {
	env->potion_known[itm_arg] = true;
	StrPrintF(buf, "you buy: potion%s", get_Of_potion(itm_arg, true));
      } else if (itm_type == OSCROLL && itm_arg >= 0 && itm_arg < MAXSCROLL) {
	env->scroll_known[itm_arg] = true;
	StrPrintF(buf, "you buy: scroll%s", get_Of_scroll(itm_arg, true));
      } else {
	StrPrintF(buf, "you buy: %s", get_objectname(itm_type));
      }
      // -  subtract gold 
      env->cdude[GOLD] -= itm_cost; // we checked so this should be >= 0
      // -  subtract qty; redraw list if it's now 0
      if (itm_qty == 0) {
	lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
	LstSetListChoices(lst, NULL, 0);
	StrPrintF(store_select_items[store_item], "*** sold out ***");
	LstSetListChoices(lst, store_select_items, store_itmlst_len);
	LstSetSelection(lst, -1);
	LstDrawList(lst);
	RctSetRectangle(&r, 126, 15 + 11*store_item, 30, 11);
	WinEraseRectangle(&r, 0);
      }
    }
  }
  WinDrawChars(buf, StrLen(buf), 5, 128);
  nap(1000); // was 2000, seemed too slow on xcopilot, probably too fast now
  store_gold(env->cdude[GOLD]);
}

// Call this for a new player.
void dndstore_restock()
{
  VoidHand vh;
  VoidPtr p;
  Short *store_data_p;
  Short restock_qty, i, j;
  vh = DmGetRecord(iLarnDB, DNDSTORE_RECORD);
  p = MemHandleLock(vh);
  if (!p) return;   // if !p, we are all in trouble.
  store_data_p = (Short *) p;
  for (i = 0, j = 3 ; i < 92 ; i++) {
    // cost, type, arg, >curr_qty<, restock_qty
    restock_qty = store_data_p[j+1];
    DmWrite(p, j*sizeof(Short), &restock_qty, sizeof(Short));
    j += 5; // skip to next curr_qty
  }
  MemHandleUnlock(vh);
  DmReleaseRecord(iLarnDB, DNDSTORE_RECORD, true);
}


void dndstore_init(FormPtr frm)
{
  Short i;
  ListPtr lst;
  dndstore_current_tens = 0;
  // alloc the store_select_items
  store_itmlst_len = 10;
  store_select_items = (Char **) md_malloc(sizeof(Char *)*store_itmlst_len);
  for (i = 0 ; i < store_itmlst_len ; i++)
    store_select_items[i] = (Char *) md_malloc(sizeof(Char) * 50); // 35+3+x
  //  StrPrintF(store_select_items[0], "just a test");
  //  dndstore_list_tens(frm, 0); // testing
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetHeight(lst, store_itmlst_len);
  FrmCopyTitle(frm, "Ye Olde Dungeon Emporium");
}

void dndstore_list_tens(FormPtr frm, Short plus)
{
  Short i, j, itm_type, itm_arg, itm_qty, tmp;
  Int itm_cost;
  ListPtr lst;
  Short *store_data_p; // in quints ($, obj, arg, itm_qty, restock_qty)
  VoidHand vh;
  VoidPtr p;
  Char buf[16];
  RectangleType r;
  Short y = 15;
  
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, NULL, 0);

  dndstore_current_tens += plus;
  if (dndstore_current_tens < 0) dndstore_current_tens = 9; //wrap-around
  if (dndstore_current_tens > 9) dndstore_current_tens = 0; //92 items= 10 tens
  StrPrintF(buf, "%d", dndstore_current_tens);
  WinDrawChars(buf, 1, 76, 141); // show 'page' number

  vh = DmQueryRecord(iLarnDB, DNDSTORE_RECORD); // "read-only, i promise"
  p = MemHandleLock(vh);
  if (!p) return;   // if !p, we are all in trouble.
  store_data_p = (Short *) p;
  //  MemHandleUnlock(vh);
  //  return;
  for (i = 0 ; i < store_itmlst_len ; i++)
    store_select_items[i][0] = '\0';
  //  x = 126; y = 15;
  // erase the rectangle (126,15) to (156,15+110)
  RctSetRectangle(&r, 126,15, 30, 110);
  WinEraseRectangle(&r, 0);
  for (i = 0, j = dndstore_current_tens * 50 ; i < store_itmlst_len && j < 5*92 ; i++) {
    //    StrPrintF(store_select_items[i], "-");
    itm_cost = store_data_p[j++];  // * 10; heck, I'm just printing extra 0.
    itm_type = store_data_p[j++];
    itm_arg  = store_data_p[j++];
    itm_qty  = store_data_p[j++];
    j++; // 5th is "restock qty", skip.

    if (itm_qty == 0) {
      StrPrintF(store_select_items[i], "...");
    } else {
      if (itm_type == OPOTION) {
	StrPrintF(store_select_items[i], "potion%s",
		  //		  i, " of foo", itm_cost);
		  get_Of_potion(itm_arg, true));
      } else if (itm_type == OSCROLL) {
	StrPrintF(store_select_items[i], "scroll%s",
		  //		  i, " of foo", itm_cost);
		  get_Of_scroll(itm_arg, true));
      } else {
	StrPrintF(store_select_items[i], "%s",
		  get_objectname(itm_type));
      }
      // consider make-it-fit'ing, into 124 pixels width.
      // now draw cost (6 chars, padded on left with ' ')
      StrPrintF(buf, "%d0", itm_cost);
      tmp = 6 - StrLen(buf); // lame attempt at right-justification...
      // it happens that all numbers are 5 pixels wide.
      WinDrawChars(buf, StrLen(buf), 126 + tmp*5, y);
    }
    y += 11;
  }
  MemHandleUnlock(vh);
  //  return;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, store_select_items, store_itmlst_len);
  LstSetSelection(lst, -1);
  LstDrawList(lst);
}

void dndstore_btns(FormPtr frm)
{
  ControlPtr btn;

  store_gold(env->cdude[GOLD]);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_sell));
  CtlSetLabel(btn, "Buy");
  CtlShowControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_prev));
  CtlSetLabel(btn, "< -");
  CtlShowControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_next));
  CtlSetLabel(btn, "+ >");
  CtlShowControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_trans));
  CtlHideControl(btn);
}

#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
void store_cleanup(FormPtr frm)
{
  // free the store_select_items
  ListPtr lst;
  VoidHand h;
  Short i;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, NULL, 0);
  if (store_select_items) {
    for (i = 0 ; i < store_itmlst_len ; i++) {
      free_me(store_select_items[i]);
    }
    free_me(store_select_items);    
  }
}
#undef free_me


/******************************************************************
                      COLLEGE
 ******************************************************************/

void college_init(FormPtr frm)
{
  ListPtr lst;
  Short i;
  ControlPtr btn;

  store_itmlst_len = 8;
  store_select_items = (Char **) md_malloc(sizeof(Char *) * store_itmlst_len);
  for (i = 0 ; i < store_itmlst_len ; i++) {
    store_select_items[i] = (Char *) md_malloc(sizeof(Char) * 25);
    if (! (env->courses_taken & (TAKEN i)))
      StrCopy(store_select_items[i], college_courses[i]);
    else store_select_items[i][0] = '\0';
  }

  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, store_select_items, store_itmlst_len);
  LstSetSelection(lst, -1);
  LstSetHeight(lst, store_itmlst_len);

  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_sell));
  CtlSetLabel(btn, "Enroll"); // no need to 'show'
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_prev));
  CtlHideControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_next));
  CtlHideControl(btn);
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_st_trans));
  CtlHideControl(btn);

  FrmCopyTitle(frm, "Courses Offered");
}

void college_mumble(Long gold)
{
  Char buf[80];
  RectangleType r;
  Short i, x, y=15;

  //  RctSetRectangle(&r, 5,106, 150, 11);
  RctSetRectangle(&r, 5,117, 150, 11);
  WinEraseRectangle(&r, 0);
  StrCopy(buf, "Each course costs 250 gold.");
  //  WinDrawChars(buf, StrLen(buf), 5, 106);
  WinDrawChars(buf, StrLen(buf), 5, 117);
  for (i = 0 ; i < store_itmlst_len ; i++, y+=11) {
    StrPrintF(buf, "%d mb.", units[i]);
    x = (units[i] > 9) ? 128 : 133;
    WinDrawChars(buf, StrLen(buf), x, y);
  }
  //  StrCopy(buf, "(mobuls)");
  //  WinDrawChars(buf, StrLen(buf), 120, y+2); // might need to ditch that
  store_gold(gold);
}

static void college_remark(Char *buf)
{
  RectangleType r;
  RctSetRectangle(&r, 5,117, 150, 11);
  WinEraseRectangle(&r, 0);
  WinDrawChars(buf, StrLen(buf), 5, 117);
  // take a nap and redraw "Each costs 250 gold" ?
}

void college_enroll(FormPtr frm, Short i)
{
  //  Char buf[50];
  ListPtr lst;
  RectangleType r;
  // store_item assumed to be in bounds
  if (env->cdude[GOLD] < 250) {
    college_remark("You don't have enough gold!");
    return;
  }
  if (env->courses_taken & (TAKEN i)) {
    college_remark("Sorry, that course is full");
    return;
  }
  switch(i) {
  case 0:
    env->cdude[STRENGTH] += 2;  
    env->cdude[CONSTITUTION]++;
    college_remark("You feel stronger!");
    break;
  case 1:
    if (! (env->courses_taken & (TAKEN 0))) {
      college_remark("You must take its prerequisite first.");
      return;
    }
    env->cdude[STRENGTH] += 2;  
    env->cdude[CONSTITUTION] += 2;
    college_remark("You feel much stronger!");
    break;
  case 2:
    env->cdude[INTELLIGENCE] += 2; 
    college_remark("Your quest seems more attainable!");
    break;
  case 3:
    if (! (env->courses_taken & (TAKEN 2))) {
      college_remark("You must take its prerequisite first.");
      return;
    }
    env->cdude[INTELLIGENCE] += 2; 
    college_remark("Your quest seems very attainable!");
    break;
  case 4:
    env->cdude[CHARISMA] += 3; 
    college_remark("You feel like a born leader!");
    break;
  case 5:
    env->cdude[WISDOM] += 2; 
    college_remark("You know you'll find that potion!");
    break;
  case 6:
    env->cdude[DEXTERITY] += 3; 
    college_remark("You feel like dancing!");
    break;
  case 7:
    env->cdude[INTELLIGENCE] += 1; 
    college_remark("Wow!! E = MC^2!!!");
    break;
  }
  env->cdude[GOLD] -= 250;
  store_gold(env->cdude[GOLD]);
  env->gtime += units[i] * 100;    // also, *** ADD TO env->gtime ***
  env->courses_taken |= (TAKEN i); // remember that it's been taken now
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_st));
  LstSetListChoices(lst, NULL, 0);
  store_select_items[i][0] = '\0';
  LstSetListChoices(lst, store_select_items, store_itmlst_len);
  LstSetSelection(lst, -1);
  LstDrawList(lst);
  RctSetRectangle(&r, 126, 15 + 11*i, 30, 11); // these the right xy? I'm lazy.
  WinEraseRectangle(&r, 0);
  // Since time has passed, regenerate things:
  env->cdude[HP] = env->cdude[HPMAX];            /* player regenerated : */
  env->cdude[SPELLS] = env->cdude[SPELLMAX]; 
  if (env->cdude[BLINDCOUNT])       	         /* cure blindness too! */
    env->cdude[BLINDCOUNT]=1;  
  if (env->cdude[CONFUSE])	                 /* end confusion */
    env->cdude[CONFUSE]=1;	
  // also, *** call 'adjtime' *** like it says.
  adjtime((Long) (units[i] * 100), false); // make your effects wear off.
  // and take a nap, maybe
  return;
}


/******************************************************************
                   TRADING POST
 ******************************************************************/

void tpost_print_value(Short k)
{
  Char buf[40];
  Long value;
  Int itm_cost;
  Short itm, itm_type, itm_arg, izarg, j;
  RectangleType r;
  Short *store_data_p; // in quints ($, obj, arg, itm_qty, restock_qty)
  VoidHand vh;
  VoidPtr p;

  RctSetRectangle(&r, 40, 141, 70, 11); // is 60 long enough? check this
  WinEraseRectangle(&r, 0);
  if (k < 0 || k >= NINVT || env->iven[k] == 0) return; // OOB or unused slot

  value = -1;
  last_item_value = -1;
  last_item_qty_index = -1;
  itm = env->iven[k];

  // if unidentified,
  // print a "we don't want that" message
  j = env->ivenarg[k];
  if ((itm == OSCROLL && j >= 0 && j < MAXSCROLL && !env->scroll_known[j]) ||
      (itm == OPOTION && j >= 0 && j < MAXPOTION && !env->potion_known[j])) {
      //      StrPrintF(buf, "unidentified");
      StrPrintF(buf, "We don't want it.");
      WinDrawChars(buf, StrLen(buf),  40, 141);
      return;
  }
  if (itm == OSPEED || itm == OACID || itm == OHASH || 
      itm == OSHROOMS || itm == OCOKE) {
      StrPrintF(buf, "try DopeWars.");
      WinDrawChars(buf, StrLen(buf),  40, 141);
      return;
  }

  if (itm == ODIAMOND ||
      itm == ORUBY ||
      itm == OEMERALD ||
      itm == OSAPPHIRE) {
    value = 20 * (Long)env->ivenarg[k];
  } else {
    vh = DmQueryRecord(iLarnDB, DNDSTORE_RECORD); // "read-only, i promise"
    p = MemHandleLock(vh);
    if (!p) return;   // if !p, we are all in trouble.
    store_data_p = (Short *) p;
    for (j = 0 ; j < 5*92 ; ) {
      itm_cost = store_data_p[j++];  // * 10; heck, I'm just printing extra 0.
      itm_type = store_data_p[j++];
      itm_arg  = store_data_p[j++];
      if (itm_type == itm) {
	if (itm == OSCROLL || itm == OPOTION) {
	  if (itm_arg == env->ivenarg[k]) {
	    value = 2 * (Long)itm_cost;
	    break;
	  }
	} else {
	  // make +n objects more valuable
	  izarg = env->ivenarg[k];  
	  value = tpost_calc_val(itm_cost, izarg);
	  break;
	}
      }
      j+=2; // skip stock qty, restock qty
    }
    if (value < 0) value = 0;
    else {
      if (store_data_p[j] < store_data_p[j+1] || store_data_p[j] == 0) {
	last_item_qty_index = j; // restock the other store, if sold
	last_item_qty = store_data_p[j] + 1;
      }
    }
    MemHandleUnlock(vh);
  }
  StrPrintF(buf, "Value: %ld", value);
  //  StrPrintF(buf, "Value: %d %d", last_item_qty, last_item_qty_index);
  WinDrawChars(buf, StrLen(buf),  40, 141);
  last_item_value = value;
}


// I suppose if you want to get very fancy, you could
// cause the other store to restock a sold-out item.
Boolean tpost_sell_item(Short k)
{
  VoidHand vh;
  VoidPtr p;
  Short m = last_item_qty;
  Long tmp;
  if (k < 0 || k >= NINVT || env->iven[k] == 0 || last_item_value <= 0)
    return false;
  tmp = env->cdude[GOLD] + last_item_value;
  if (tmp > 0) env->cdude[GOLD] = tmp;
  else if (env->cdude[GOLD]) env->cdude[GOLD] = MAXGOLD;

  if (env->cdude[WEAR] == k)    env->cdude[WEAR] = -1;
  if (env->cdude[WIELD] == k)   env->cdude[WIELD] = -1;
  if (env->cdude[SHIELD] == k)  env->cdude[SHIELD] = -1;
  //  adjustcvalues(env->iven[inventory_item],
  //		env->ivenarg[inventory_item]);
  //  env->iven[inventory_item] = 0;
  adjustcvalues(env->iven[k], env->ivenarg[k], false);
  env->iven[k] = 0;
  if (last_item_qty_index != -1) {
    vh = DmGetRecord(iLarnDB, DNDSTORE_RECORD);
    p = MemHandleLock(vh);
    if (!p) return true;   // if !p, we are all in trouble.
    DmWrite(p, last_item_qty_index*sizeof(Short), &m, sizeof(Short)); // kaput
    MemHandleUnlock(vh);
    DmReleaseRecord(iLarnDB, DNDSTORE_RECORD, true);
  }
  return true;
  //   caller will redraw the inventory.
}

/******************************************************************
                             PAD
 ******************************************************************/

const Short pad_warez[5] = { OSPEED, OACID, OHASH, OSHROOMS, OCOKE };
const Short pad_prices[5] = { 100, 250, 500, 1000, 5000 };
//Boolean pad_stock[5] = { 1,1,1,1,1 }; // replace with warez_taken and TAKEN
void pad_gold()
{
  //  ListPtr lst;
  Char buf[80];
  RectangleType r;
  Short y = 90;
  RctSetRectangle(&r, 5, y, 150, 11);
  WinEraseRectangle(&r, 0);
  StrPrintF(buf, "Looks like ya got %ld bucks.", env->cdude[GOLD]);
  WinDrawChars(buf, StrLen(buf), 5, y);
  // also remove sold items
  // I'm too lazy to actually do that.
  return;
}
void pad_buy(FormPtr frm, Short store_item)
{
  Char buf[80];
  Short y = 105;
  RectangleType r;
  RctSetRectangle(&r, 5, y, 150, 22);
  WinEraseRectangle(&r, 0);
  if (store_item >= 0 && store_item < 5) {
    if (env->cdude[GOLD] < pad_prices[store_item]) {
      StrPrintF(buf, "Whattaya trying to pull on me?");
      WinDrawChars(buf, StrLen(buf), 5, y);
      StrPrintF(buf, "You aint got the cash!");
      WinDrawChars(buf, StrLen(buf), 5, y+11);
      return;
    } 
    if (!(env->warez_taken & (TAKEN store_item))) {
      if (pocketfull())
	StrPrintF(buf, "Hey, you can't carry no more.");
      else {
	take(pad_warez[store_item], 0, false);
	env->cdude[GOLD] -= pad_prices[store_item];
	env->warez_taken |= (TAKEN store_item);
	pad_gold();
	StrPrintF(buf, "Ok, here ya go.");
      }
      WinDrawChars(buf, StrLen(buf), 5, y);
      return;
    }
  }
  StrPrintF(buf, "Sorry man, I ain't got no more");
  WinDrawChars(buf, StrLen(buf), 5, y);
  StrPrintF(buf, "of that shit.");
  WinDrawChars(buf, StrLen(buf), 5, y+11);

  return;
}

/******************************************************************
                             MISC
 ******************************************************************/
Long tpost_calc_val(Int itm_cost, Short izarg)
{
  Long value = itm_cost;
  if (izarg >= 0) value *= 2;
  while ( (izarg > 0) && (value < 500000) ) {
    value = 14 * (67 + value)/ 10; // that's really quite.. bizarre
    //value = 11 * (67 + value)/8; // 14 ~= 11/8; it's smart enough to optimize
    izarg--;
  }
  return value;
}
Long get_mobuls()
{
  //  return ( (Long) ((TIMELIMIT - env->gtime + 127) / 128) );
  return ( (Long) ((TIMELIMIT - env->gtime + 99) / 100) );
}
void add_winner_bonus()
{
  Long t, g;
  // taxrate was originally 1/20.
  // a reasonable approximation is 13/256.
  //  tmp = env->cdude[GOLD] * 13 / 256;
  t = env->cdude[GOLD] / TAXRATEinv;
  env->outstanding_taxes += t;

  // score bonus for winning... AFTER the first time you've won...
  g = env->cdude[GOLD] + 10000000L * env->cdude[HARDGAME];
  if (g > 0) env->cdude[GOLD] = g;
  else if (env->cdude[GOLD]) env->cdude[GOLD] = MAXGOLD;

}

// This one is ELAPSED TIME, not time left.
Long get_mobuls_elapsed()
{
  return (Long) (env->gtime / 100);
}
