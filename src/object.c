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

static void finditem_part1(Short item) SEC_2;

extern Short yrepcount;


// Identify a trap next to the player.
// This is in this file for no particular reason.
void id_trap()
{
  Short i, j, flag=0;
  Char buf[40];
  for (j = env->playery - 1 ; j < env->playery + 2 ; j++) {
    if (j < 0) continue;
    if (j >= MAXY) break;
    for (i = env->playerx - 1 ; i < env->playerx + 2 ; i++) {
      if (i < 0) continue;
      if (i >= MAXX) break;
      switch(env->item[i][j]) {
      case OTRAPDOOR:     // a trapdoor
      case ODARTRAP:      // a dart trap
      case OTRAPARROW:    // an arrow trap
      case OTELEPORTER:   // a teleport trap
      case OELEVATORUP:   // the elevator going up
      case OELEVATORDOWN: // the elevator going down
	StrPrintF(buf, "It's %s", get_objectname(env->item[i][j]) );
	flag++;
      };
    }
  }
  if (flag==0) 
    StrPrintF(buf, "No traps are visible");
  message(buf);
}

/*
 *  return true if a monster is next to the player
 */
Boolean nearbymonst()
{
  Short tmp,tmp2;
  for (tmp=env->playerx-1; tmp<env->playerx+2; tmp++)
    for (tmp2=env->playery-1; tmp2<env->playery+2; tmp2++)
      if (env->mitem[tmp][tmp2]) return true; /* if monster nearby */
  return false;
}

/*
 * thing to call when you pick up gold
 */
void ogold(Short arg)
{
  Long i, tmp;
  //  Char buf[40];
  
  i = env->iarg[env->playerx][env->playery];

  if (arg == ODGOLD) i *= 10;
  else if (arg == OMAXGOLD) i *= 100;
  else if (arg == OKGOLD) i *= 1000;

  // ok, this was cute for testing the splash, but it is also DAMNED ANNOYING
  //  StrPrintF(buf, "It is worth %d!",(Int)i); // but i might be a long
  //  message(buf);
  tmp = env->cdude[GOLD] + i;
  if (tmp > 0) env->cdude[GOLD] = tmp;
  else if (env->cdude[GOLD]) env->cdude[GOLD] = MAXGOLD;

  print_stats();
  /*destroy gold*/
  env->item[env->playerx][env->playery] = 0;
  env->know[env->playerx][env->playery] = 0;
}

// These are all the different verbs possible when you stand on an object.
#define BURN 0
#define CLUP 1
#define CLDO 2
#define CLOS 3
#define DESE 4
#define DRIN 5 
#define EATI 6 
#define GIVE 7 
#define GOIN 8 
#define IGNO 9 
#define JPRA 10
#define OPEN 11
#define PRYO 12
#define READ 13
#define RUBI 14
#define SITD 15
#define SMOK 16
#define SNOR 17
#define SPIL 18
#define STAY 19
#define TAKE 20
#define WASH 21
#define ZZZZ 22

// These are all the button labels corresponding to the verbs.
Char button_labels[23][16] = {
  "burn it",
  "climb up",
  "climb down",
  "close it",
  "desecrate",
  "drink it",
  "eat it",
  "give money",
  "go inside",
  "ignore it",
  "just pray",
  "open it",
  "pry off jewels",
  "read it",
  "rub it",
  "sit down",
  "smoke it",
  "snort it",
  "spill it",
  "stay here",
  "take it",
  "wash yourself",
  "-"
};

Char question_hotkeys[4] = "----";

/*
 * Set up the "what do you want to do" form
 * for when you are standing on an object
 */
void setupQuestionForm(FormPtr frm)
{
  ControlPtr btn; 
  Short lab[4], i;
  for (i = 0 ; i < 3 ; i++) lab[i] = ZZZZ;
  lab[3] = IGNO;
  switch(env->ko_object) {
  case OPOTION:
    lab[0] = DRIN;    lab[1] = TAKE;    lab[2] = SPIL;    lab[3] = IGNO;
    break;
  case OSCROLL:
    lab[0] = READ;    lab[1] = TAKE;    lab[2] = BURN;    lab[3] = IGNO;
    break;
  case OALTAR:
    lab[0] = GIVE;    lab[1] = JPRA;    lab[2] = DESE;    lab[3] = IGNO;
    break;
  case OBOOK:
    lab[0] = READ;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OCOOKIE:
    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OTHRONE:
  case OTHRONE2:
    lab[0] = PRYO;    lab[1] = SITD;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case ODEADTHRONE:
    lab[0] = SITD;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
    // OORB: finditem
  case OBRASSLAMP:
    lab[0] = RUBI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
    // OWWAND: OHANDofFEAR: finditem
    // OPIT: no choice
  case OSTAIRSUP:
    lab[0] = CLUP;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
    // elevators: no choice
  case OFOUNTAIN:
    lab[0] = DRIN;    lab[1] = WASH;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
    // statue: noop
  case OCHEST:
    // lab[0] = TAKE;    lab[1] = OPEN;    lab[2] = ZZZZ;    lab[3] = IGNO;
    lab[0] = OPEN;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
    // tele: no choice
  case OSCHOOL:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
    // mirror: noop
  case OBANK2:
  case OBANK:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
    // deadfountain: blah
  case ODNDSTORE:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OSTAIRSDOWN:
    lab[0] = CLDO;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OOPENDOOR:
    lab[0] = CLOS;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OCLOSEDDOOR:
    lab[0] = OPEN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OENTRANCE:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OVOLDOWN:
    lab[0] = CLDO;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OVOLUP:
    lab[0] = CLUP;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
    // various traps
  case OTRADEPOST:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OHOME:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OPAD:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  case OSPEED:
    lab[0] = SNOR;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OSHROOMS:
    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OACID:
    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OHASH:
    lab[0] = SMOK;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  case OCOKE:
    lab[0] = SNOR;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
    // sphere
  case OLRS:
    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    break;
  default:
    lab[0] = TAKE;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = IGNO;
    break;
  }
  for (i = 0 ; i < 4 ; i++) {
    question_hotkeys[i] = button_labels[lab[i]][0];
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, i + btn_qst_1));
    CtlSetLabel(btn, button_labels[lab[i]]);
    if (lab[i] == ZZZZ)
      CtlHideControl(btn);
    else
      CtlShowControl(btn);
  }
}

#undef BURN
#undef CLUP
#undef CLDO
#undef CLOS
#undef DESE
#undef EATI
#undef GIVE
#undef GOIN
#undef IGNO
#undef JPRA
#undef OPEN
#undef PRYO
#undef READ
#undef SITD
#undef SMOK
#undef SNOR
#undef SPIL
#undef STAY
#undef TAKE
#undef WASH




/*
 * return true if caller should FrmPopupForm(QuestionForm)
 */
Boolean lookforobject(Short auto_pray)
{
  //  Short j;
  Short i, px, py;
  Char buf[80];
  px = env->playerx;
  py = env->playery;
  if (env->cdude[TIMESTOP]) return false;
  i = env->item[px][py];
  if (!i) return false;

  showcell(px, py);
  //  yrepcount = 0;
  env->ko_object = i;

  switch(i) {
  case OGOLDPILE:	
  case OMAXGOLD:
  case OKGOLD:	
  case ODGOLD:	    // DONE
    message("You have found some gold!");
    ogold(i);
    break;
  case OPOTION:
    i = env->iarg[px][py];
    StrPrintF(buf, "%s%s",
	      //	      "You have found a magic potion",
	      "You find a potion",
	      get_Of_potion(i, env->potion_known[i]));
    message(buf);
    return true;    // opotion
    break;
  case OSCROLL:
    i = env->iarg[px][py];
    StrPrintF(buf, "%s%s",
	      //	      "You have found a magic scroll",
	      "You find a scroll",
	      get_Of_scroll(i, env->scroll_known[i]));
    message(buf);
    return true;    // oscroll
    break;
  case OALTAR:
    if (nearbymonst()) return false;
    message("There is a Holy Altar here!");
    return true;
    break;  // oaltar
  case OBOOK:
    message("You have found a book.");
    return true;
    break; // obook
  case OCOOKIE:
    message("You have found a fortune cookie.");
    return true;
    break; // ocookie
  case OTHRONE:
  case OTHRONE2:
    if (nearbymonst()) return false;
    StrPrintF(buf, "You see %s!", get_objectname(i));
    message(buf);
    return true; // othrone 0||1
    break;
  case ODEADTHRONE:
    StrPrintF(buf, "You see %s!", get_objectname(i));
    message(buf);
    return true; // odeadthrone
    break;
  case OORB:
    if (nearbymonst())
      return false;
    finditem_part1(i); // hey, you should change i and return true here
    return true;
    break;
  case OBRASSLAMP:
    message("You have found a brass lamp.");
    return true; // need to create obrasslamp
    break;
  case OWWAND:
    if (nearbymonst())
      return false;
    finditem_part1(i); // hey, you should change i and return true here
    return true;
    break;
  case OHANDofFEAR:
    if (nearbymonst())
      return false;
    finditem_part1(i); // hey, you should change i and return true here
    return true;
    break;
  case OPIT:
    message("You're standing at the top of a pit.");
    opit();
    break; // opit
  case OSTAIRSUP:	
    message("There is a circular staircase here"); 
    return true;
    break; // ostairs
  case OELEVATORUP:
    message("You're on an express elevator going up");
    //nap(2000);
    oelevator(true);
    break; // oelevator 1
  case OELEVATORDOWN:	
    message("You're on an express elevator down!");
    //nap(2000);
    oelevator(false);
    break; // oelevator -1
  case OFOUNTAIN:	
    if (nearbymonst()) return false;
    message("There is a fountain here"); 
    return true; // ofountain
    break;
  case OSTATUE:	 // DONE
    if (nearbymonst()) return false;
    message("You are standing in front of a statue"); 
    break;
  case OCHEST:	
    message("There is a chest here");  
    return true; // ochest
    break;
  case OIVTELETRAP:	
    if (rnd(11)<6) return false;
    env->item[px][py] = OTELEPORTER;
    env->know[px][py] = 1;
    // fall through.
  case OTELEPORTER:
    message("Zaaaappp!  You've been teleported!");
    //beep(); 
    //nap(3000); // oteleport
    oteleport(false);
    break; 
  case OSCHOOL:	
    if (nearbymonst()) return false;
    message("You have found the College of Ularn.");
    return true; // oschool
    break;
  case OMIRROR:	 // DONE
    if (nearbymonst()) return false;
    message("There is a mirror here");	
    break; // omirror is a NOOP
  case OBANK2:
  case OBANK:	
    if (nearbymonst()) return false;
    if (i == OBANK)
      message("You have found the bank of Ularn.");
    else
      //	     : "You have found a branch office of the bank of Ularn.");
      message("You've found a BankUlarn branch.");
    return true; // obank
    break;
  case ODEADFOUNTAIN:	// DONE
    if (nearbymonst()) return false;
    message("There is a dead fountain here"); 
    break;
  case ODNDSTORE:	
    if (nearbymonst()) return false;
    message("There is a DND store here.");
    return true; // dndstore
    break;
  case OSTAIRSDOWN:
    message("There is a circular staircase here"); 
    return true; // ostairs -1
    break;
  case OOPENDOOR:
    message("open door");
    return true; // create an odoor
    break;
  case OCLOSEDDOOR:
    message("closed door");
    return true; // create an odoor
    break;
  case OENTRANCE:
    message("dungeon entrance");
    return true; // create an oenter
    break;
  case OVOLDOWN:
    message("volcano shaft");
    return true; // create an ovold
    break;
  case OVOLUP:
    message("volcano shaft leading up");
    return true; // create an ovolu
    break;
  case OTRAPARROWIV:	 // DONE
    if (rnd(17)<13) return false;	/* for an arrow trap */
    env->item[px][py] = OTRAPARROW;
    env->know[px][py] = 0;
    // fall through
  case OTRAPARROW:	// DONE
    message("You are hit by an arrow"); 
    // beep
    env->lastnum = 259;
    losehp(rnd(10) + env->level);
    print_stats();
    return false;
  case OIVDARTRAP:	 // DONE
    if (rnd(17)<13) return false;		/* for a dart trap */
    env->item[px][py] = ODARTRAP;
    env->know[px][py] = 0;
    // fall through
  case ODARTRAP:		// DONE
    message("You are hit by a dart"); 
    // beep
    env->lastnum = 260;
    losehp(rnd(5));
    env->cdude[STRENGTH] = max(3, env->cdude[STRENGTH] - 1);
    print_stats();
    return false;
  case OIVTRAPDOOR:	
    if (rnd(17)<13) return false;		/* for a trap door */
    env->item[px][py] = OTRAPDOOR;
    env->know[px][py] = 1;
    // fall through
  case OTRAPDOOR:		
    // beep
    env->lastnum = 272; // this has not been tested
    if (has_object(OWWAND))
      message("You escape a trap door");
    else if ( (env->level == MAXLEVEL - 1) ||
              (env->level == MAXLEVEL + MAXVLEVEL - 1) ) {
      if (env->cdude[HARDGAME] == 0 && (rnd(100) > 5)) {
	// I will try to be nice to 'inexperienced' players.
	message("You ALMOST fell into a trap door!");
      } else {
	message("You found a trap door to HELL!");
	//      beep();  
	nap(3000);  
	env->cdude[HP] = -1;// just to be sure
	died(271, true);
      }
    } else {
      message("You fall through a trap door!");
      newcavelevel(env->level + 1);
      losehp(rnd(5) + env->level);
      o_draws();// includes print_stats
    }
    break;
  case OTRADEPOST:
    if (nearbymonst()) return false;
    message("You have found the Ularn trading post");
    return true; // otradepost
    break;
  case OHOME:
    if (nearbymonst()) return false;
    message("You have found your way home.");
    return true; // ohome
    break;
  case OPAD:
    if (nearbymonst()) return false;
    message("You've found Dealer McDope's hideout!");
    return true; // opad
    break;
  case OSPEED:
    message("You have found some speed");
    return true; // create ospeed, etc.
    break;
  case OSHROOMS:
    message("You've found some magic mushrooms");
    return true;
    break;
  case OACID:
    message("You have found some LSD");
    return true;
    break;
  case OHASH:
    message("You have found some hashish");
    return true;
    break;
  case OCOKE:
    message("You have found some cocaine");
    return true;
    break;
  case OWALL: // DONE, ha
    break;
  case OANNIHILATION:
    if ( has_object(OSPHTALISMAN) ) {
      message("The Talisman of the Sphere protects you from annihilation!");
    }
    //    message("annihilated! (DEATH NOT IMPL.)");
    env->cdude[HP] = -1; // just to be sure
    died(283, true);
    return false;
    break;
  case OLRS:
    if (nearbymonst()) return false;
    message("There is an LRS office here.");
    return true;
    break;
  default:
    // default object! at last
    //    message("You have found a thingy.");
    // Hey, I should return true here.
    finditem_part1(i);
    return true; // NEW
    break;
  }
  return false;

}

/*
 *  First part of a do-thing-to-object.
 */
static void finditem_part1(Short item)
{
  Short tmp; //, i;
  Char buf[80];
  //  StrPrintF(buf, "You have found %s", get_objectname(item));
  StrPrintF(buf, "Hey! %s", get_objectname(item));
  tmp = env->iarg[env->playerx][env->playery];
  switch(item) {
  case ODIAMOND:		
  case ORUBY:			
  case OEMERALD:
  case OSAPPHIRE:		
  case OSPIRITSCARAB:	
  case OORBOFDRAGON:
  case OORB:
  case OHANDofFEAR:
  case OWWAND:
  case OCUBEofUNDEAD:	
  case ONOTHEFT:	
    break;
  default:
    if (tmp>0) 
      StrPrintF(buf + StrLen(buf), " +%d", tmp);
    else if (tmp<0)
      StrPrintF(buf + StrLen(buf), " %d", tmp);
  }
  message(buf);
  // THe popup should offer take/ignore
  // if you take:
  //      if (take(itm,tmp)==0)  
  //	forget();	
  // otherwise:
  //  ignore();
}





// return true on success, false if pack FULL.
//Boolean take(Short item, Short arg) {
//  return true;
//} // now in object_blah_more

#define forget() (env->item[px][py]=env->know[px][py]=0)

/*
 * Second part of do something to object.
*************** user has answered the question.. now do the thing *****
*/
// button should be 0-4
// Probably I should have a lookup table of "which ones are 'take'"
// and save a little space that way.
Short in_store;
// Returns true if action took time.
Boolean dspchQuestionForm(Short button) SEC_2;
/*
static Boolean bite_me_palmos(Short button, Short i, Short px, Short py)
{
    switch(button) {
    case 0:
      quaffpotion(i);
      forget();
      break;
    case 1:
      if (take(OPOTION, i, true))   forget();
      else return false;
      break;
    case 2:
      spill();
      forget();
      break;
    }
    return true;
} 
*/
Boolean dspchQuestionForm(Short button)
{
  Short i,px,py;
  px = env->playerx;
  py = env->playery;
  i = env->iarg[px][py];
  if (button == 3 &&
      env->ko_object != OCLOSEDDOOR &&
      env->ko_object != OALTAR)
    return false; // button3 is always 'do nothing'; and in most cases it does.
  
  switch(env->ko_object) {
  case OPOTION: // done!
    //    if (!bite_me_palmos(button, i, px, py)) return false;
    switch(button) {
    case 0:
      quaffpotion(i);
      forget();
      break;
    case 1:
      if (take(OPOTION, i, true))   forget();
      else return false;
      break;
    case 2:
      spill();
      forget();
      break;
    }
    break;
  case OSCROLL: // done!
    // XXX if you are blind you should not be able to read
    switch(button) {
    case 0:
      if (read_scroll(i)) {
	forget();
	if (i==R_PULVR) return false; // only 1 scroll type needs a direction.
      } else return false;
      break;
    case 1:
      if (take(OSCROLL, i, true))   forget();
      else return false;
      break;
    case 2:
      forget();
      if (env->cdude[BLINDCOUNT]) break;
      enlighten(-4, 5, -13, 14);
      break;
    }
    break;
  case OALTAR:  // done! except popup
    switch(button) {
    case 0:
      if (env->cdude[GOLD] <= 0) {
	if (omoney(0))
	  return false; // created monster
      } else {
	in_store = STORE_ALTAR;
	FrmPopupForm(TransForm);
	return false; // TransForm itself needs to take care of taking-a-turn.
      }
      break;
    case 1:
      if (ojustpr()) return false; // created monster
      break;
    case 2:
      if (odesecrate()) return false; // created monster
      break;
    case 3: // ouchie
      if (rnd(100)<30) { 
	message("uh oh");
	createmonster(makemonst(env->level+2)); 
	env->cdude[AGGRAVATE] += rnd(450);
	recalc_screen(); // I think I need this
	showcell(env->playerx, env->playery);
	showplayer(env->playerx, env->playery);
	refresh(); // I think I need this
	return false; // be nice - give the player a turn to run away!!!
      }
      break;
    }
    break;
  case OBOOK: // done!
    // XXX if you are blind you should not be able to read
    switch(button) {
    case 0:
      if (readbook(i))	forget();
      else return false;
      break;
    case 1:
      if (take(OBOOK, i, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OCOOKIE:
    //    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      forget();
      ocookie();
      break;
    case 1:
      if (take(OCOOKIE, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OTHRONE:
  case OTHRONE2: // done!
    switch(button) {
    case 0:
      if (othrone_pry((env->ko_object==OTHRONE2)))
	return false; // summoned monster
      break;
    case 1:
      if (othrone_sit((env->ko_object==OTHRONE2)))
	return false; // summoned monster
      break;
    case 2:
      return false;
    }
    break;
  case ODEADTHRONE: // done!
    switch(button) {
    case 0:
      odeadthrone();
      break;
    case 1:
    case 2:
      return false;
    }
    break;
    // OORB: finditem
  case OBRASSLAMP: // done! except popup
    switch(button) {
    case 0:
      obrasslamp();
      // xxxx taking-a-turn here might screw up the spell selection form...
      break;
    case 1:
      if (take(OBRASSLAMP, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
    // OWWAND: OHANDofFEAR: finditem
    // OPIT: no choice
  case OSTAIRSUP:   // done!
    switch(button) {
    case 0:
      ostairs(true);
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
    // elevators: no choice
  case OFOUNTAIN:  // done!
    switch(button) {
    case 0:
      ofountain_drink();
      break;
    case 1:
      if (ofountain_wash()) return false; // created monster
      break;
    case 2:
      return false;
    }
    break;
    // statue: noop
  case OCHEST:  // done!
    switch(button) {
      // case 1:
    case 0:
      ochest();
      break;
      // case 0:
    case 1:
      if (take(OCHEST, i, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
    // tele: no choice
  case OSCHOOL:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_COLLEGE;
      FrmPopupForm(StoreForm); // test...
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
    // mirror: noop
  case OBANK2:
  case OBANK:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_BANK;
      if (OWES_TAXES)
	FrmPopupForm(EvaderForm);
      else 
	FrmPopupForm(StoreForm); // test...
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
    // deadfountain: blah
  case ODNDSTORE:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_DND;
      if (OWES_TAXES)
	FrmPopupForm(EvaderForm);
      else 
	FrmPopupForm(StoreForm); // test...
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OSTAIRSDOWN: // done!
    switch(button) {
    case 0:
      ostairs(false);
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OOPENDOOR: // done!
    switch(button) {
    case 0:
      oopendoor();
      return false; // seems reasonable to me not to take a turn
    case 1:
    case 2:
      return false;
    }
    break;
  case OCLOSEDDOOR: // done!
    switch(button) {
    case 0:
      oclosedoor();
      return false; // seems reasonable to me not to take a turn
    case 1:
    case 2:
    case 3:
      unmoveplayer();
      return false; // this is a 'cancel', should not take a turn
    }
    break;
  case OENTRANCE: // done!
    switch(button) {
    case 0:
      oentrance();
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OVOLDOWN: // done!
    switch(button) {
    case 0:
      ovoldown();
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OVOLUP: // done!
    switch(button) {
    case 0:
      ovolup();
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
    // various traps
  case OTRADEPOST:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_TPOST;
      FrmPopupForm(InvSelectForm); // ha! not what you were expecting!
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OHOME:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_HOME;
      FrmPopupForm(EvaderForm);
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OPAD:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      FrmPopupForm(PadForm);
      return false;
    case 1:
    case 2:
      return false;
    }
    break;
  case OSPEED:
    //    lab[0] = SNOR;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      drug_speed();
      forget();       // make sure we're updating stats line..
      break;
    case 1:
      if (take(OSPEED, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OSHROOMS:
    //    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      drug_shrooms();
      forget();      
      break;
    case 1:
      if (take(OSHROOMS, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OACID:
    //    lab[0] = EATI;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      drug_acid();
      forget();      
      break;
    case 1:
      if (take(OACID, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OHASH:
    //    lab[0] = SMOK;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      drug_hash();
      forget();      
      break;
    case 1:
      if (take(OHASH, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
  case OCOKE:
    //    lab[0] = SNOR;    lab[1] = TAKE;    lab[2] = ZZZZ;    lab[3] = IGNO;
    switch(button) {
    case 0:
      drug_coke();
      forget();      
      break;
    case 1:
      if (take(OCOKE, 0, true))   forget();
      else return false;
      break;
    case 2:
      return false;
    }
    break;
    // sphere
  case OLRS:
    //    lab[0] = GOIN;    lab[1] = ZZZZ;    lab[2] = ZZZZ;    lab[3] = STAY;
    switch(button) {
    case 0:
      in_store = STORE_LRS;
      FrmPopupForm(LrsForm); // test...
      return false;
      break;
    case 1:
    case 2:
      return false;
    }
    break;
  default:
    switch(button) {
    case 0:
      if (take(env->ko_object, i, true))   forget();
      else return false;
      // take
      break;
    case 1:
    case 2:
      return false; // ignore
    }
    break;
  }
  return true; //any action that took no time should've returned false already
}
#undef forget
