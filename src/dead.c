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
//#include "proto3.h"
#include "iLarnRsc.h"
#ifndef I_AM_COLOR
#include <SystemMgr.rh> // for user name
#else /* I_AM_COLOR */
#include <SystemMgr.h> // needed? or not?
#endif /* I_AM_COLOR */
#include <DLServer.h> // for user name


extern LarnPreferenceType my_prefs;

static void print_cod(Short cod, Short x, Short y, Short lev) SEC_4;
static void print_scores() SEC_4;
static void boot_loser_clones(ULong birthdate) SEC_4;
// All kinds of ways to die:

const Char whydead[30][40] = {
  "quit", 					/* 0 */
  "suspended", 
  "self - annihilated", 
  "shot by an arrow",
  "hit by a dart",
  "fell into a pit", 				/* 5 */
  "fell into a pit to HELL",
  "a winner", 
  "trapped in solid rock", 
  "killed by a missing save file",
  "killed by an old save file", 			/* 10 */
  "caught by the greedy cheater checker",
  "killed by a protected save file", 
  "killed family, committed suicide", /* 13 */
  "erased by a wayward finger",
  "fell through a trap door to HELL",		/* 15 */
  "fell through a trap door",
  "drank some poisonous water",
  "fried by an electric shock", 
  "slipped on a volcano shaft",
  "killed by a stupid act of frustration", 	/* 20 */
  "attacked by a revolting demon",
  //  "hit by %s own magic",			/* 22 */
  "hit by self-inflicted magic",			/* 22 */
  "demolished by an unseen attacker",
  "fell into the dreadful sleep",
  "killed by an exploding chest",			/* 25 */
  "killed by a missing maze data file",
  "annihilated in a sphere",
  "died a post mortem death",
  "wasted by a malloc() failure"			/* 29 */
};

#define WINNER 263



// Get a default username of NAMELEN-1, truncating it if necessary.
// xxx I should ask for user name in 'preferences' and remember it there.
void get_default_username()
{
  Char *tmp, *first_wspace;
  VoidHand h;
  tmp = md_malloc(sizeof(Char) * (dlkMaxUserNameLength + 1));
  DlkGetSyncInfo(NULL, NULL, NULL, tmp, NULL, NULL);
  /* if it's too long, use the first name only */
  if (StrLen(tmp) > NAMELEN-1) {
    first_wspace = StrChr(tmp, spaceChr);
    if (first_wspace)
      *(first_wspace) = '\0';
    else
      tmp[NAMELEN-1] = '\0';
  }
  if (StrLen(tmp))
    StrNCopy(my_prefs.name, tmp, NAMELEN);
  else
    StrPrintF(my_prefs.name, "anonymous");
  h = MemPtrRecoverHandle(tmp);
  if (h) MemHandleFree(h);  
}

Short get_suid()
{
  Char *tmp;
  VoidHand h;
  Short suid;
  // On OS 3.x Palms, allegedly you can read a 12 digit ROM serial number.
  // this does not work on my Visor, though, and would not work
  // for 0S 2.x devices either.
  // ok, what's some nice cheesy way to hash the user name, instead ?
  tmp = md_malloc(sizeof(Char) * (dlkMaxUserNameLength + 1));
  DlkGetSyncInfo(NULL, NULL, NULL, tmp, NULL, NULL);
  suid = do_sum(tmp);
  h = MemPtrRecoverHandle(tmp);
  if (h) MemHandleFree(h);  
  return suid;
}


/*
 * Find a "previous incarnation" (if any) in the WINNERS list.
 * Do not assume that there is only one per suid.
 * If more than one, use the MOST RECENT one: largest 'date' field.
 *
 * From this, get taxes, dmg, hardlev, and bank balance.
 */
void get_taxes()
{
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID;
  scf * win_scores; // or is it ** ?  uncertain.
  Short i, i_max = -1;
  ULong date_max = 0;

  Short suid = get_suid();
  uniqueID = REC_WINSCORE;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);

  if (err)
    // no winners scoreboard.. no taxes.  leave everything as 0.
    return;

  // scoreboard exists!
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return;
  win_scores = (scf *) p; // array of score-file entries
  for (i = 0 ; i < SCORESIZE ; i++)
    if (win_scores[i].suid == suid) { // [heh, here I've outsmarted myself]
      if (win_scores[i].date > date_max) {
	i_max = i;
	date_max = win_scores[i].date;
      }
    }
  if (i_max != -1) {
    // Note: these will be overwritten by restore_all() if not a new character.
    env->outstanding_taxes = win_scores[i_max].taxes;
    env->damage_taxes = win_scores[i_max].dmg;
    env->cdude[HARDGAME] = win_scores[i_max].hardlev + 1; // bwaha
    env->cdude[BANKACCOUNT] = win_scores[i_max].bank;
  }
  MemHandleUnlock(vh);
}


/*
 * Find a "previous incarnation" (if any) in the WINNERS list.
 * Do not assume that there is only one per suid.
 * If more than one, use the MOST RECENT one: largest 'date' field.
 *
 * In this record, if we find it, *set* taxes and dmg to new values.
 * Returns true if we succeed.
 */
Boolean set_taxes(Long tax, Long dmg)
{
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID = REC_WINSCORE;
  scf my_sc, *win_scores;
  Short i, i_max = -1;
  ULong date_max = 0;
  Short suid = get_suid();

  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  // scoreboard exists!
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  win_scores = (scf *) p; // array of score-file entries
  for (i = 0 ; i < SCORESIZE ; i++)
    if (win_scores[i].suid == suid) { // [heh, here I've outsmarted myself]
      if (win_scores[i].date > date_max) {
	i_max = i;
	date_max = win_scores[i].date;
      }
    }
  if (i_max == -1) {
    MemHandleUnlock(vh);
    return false;
  }
  my_sc = win_scores[i_max];
  my_sc.taxes = tax;
  my_sc.dmg = dmg;
  MemHandleUnlock(vh);
  vh = DmGetRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  DmWrite(p, sizeof(scf) * i_max, &my_sc, sizeof(scf));
  MemPtrUnlock(p); // release record...
  DmReleaseRecord(iLarnSaveDB, recindex, true); // dirty=true
  return true;
}

/*
 *  If your life is protected, some ways of dying will not kill you
 */
Boolean life_protected(Short x)
{
  if (env->cdude[LIFEPROT] <= 0)
    return false;
  switch (x) {
  case 256:  // quit
  case 257:  // suspended
  case 262:  // bottomless pit
  case 263:  // winner
  case 265:  // missing save file
  case 266:  // old save file
  case 267:  // greedy
  case 268:  // protected save file
  case 269:  // ran out of time
  case 271:  // bottomless trapdoor
  case 282:  // missing data file
  case 284:  // post mortem
  case 285:  // malloc failed
  case 300:  // unscored quit
    return false;
  }
  env->cdude[LIFEPROT]--;
  env->cdude[HP]=1;
  env->cdude[CONSTITUTION]--;
  message("Your life passes before your eyes..");
  message("You feel weeiiirrrrd all over!");
  return true;
}

/*
 * Put a new score in the scoreboard
 * returns true if successful, false on failure
 * note to self - people would rather have n scores for themselves than 1
 */
// So, apparently something is fucked up in here.   xxxxxx
void newscore(Short cod)
{
  Short my_location = -1; // my place (0 = 1st place, n-1 = nth place)
  Short ousted_fellow = SCORESIZE-1; // place of the entry that's being booted
  Short i, offset;
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID;
  scf * win_scores; // or is it ** ?  uncertain.
  scf * my_sc; // my score

  // might as well do these even if I don't use them.
  my_sc = (scf *) md_malloc(sizeof(scf));
  if (WINNER == cod)
    add_winner_bonus(); // adds to taxes and to score
  my_sc->birthdate = env->cdude[BIRTHDATE];
  my_sc->score = env->cdude[GOLD]; // ensure even 0-scorers get a shot
  my_sc->timeused = get_mobuls_elapsed();
  my_sc->timeused = max(my_sc->timeused, 1);
  my_sc->taxes = env->outstanding_taxes; // need to change store.c
  my_sc->dmg = env->damage_taxes;
  my_sc->bank = env->cdude[BANKACCOUNT];
  my_sc->date = TimGetSeconds();
  my_sc->suid = get_suid();
  my_sc->hardlev = env->cdude[HARDGAME];
  my_sc->cod = cod;
  my_sc->level = env->level; // dungeon floor
  if (env->char_class < 0 || env->char_class > 8) env->char_class = 5;//in case
  my_sc->char_class = env->char_class;
  my_sc->is_male = is_male();
  //get_default_username(); // Really I should be recording this in prefs.
  StrNCopy(my_sc->who, my_prefs.name, NAMELEN);

  // note: wizards should not be entered in scoreboard
  // read, sort, re-write the two scoreboards

  // Do this TWICE
  // First, do it with     uniqueID = REC_SCORE;
  // Second, if winner, do it with     uniqueID = REC_WINSCORE;

  uniqueID = REC_SCORE; // REC_WINSCORE = REC_SCORE + 1   (very important!)
  if (WINNER == cod) uniqueID = REC_WINSCORE;

  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);

  if (err) {
    // scoreboard does not exist; create it.
    // this seems to work, in general.
    recindex = DmNumRecords(iLarnSaveDB);
    vh = DmNewRecord(iLarnSaveDB, &recindex, sizeof(scf) * SCORESIZE);
    p = MemHandleLock(vh);
    DmSet(p, 0, sizeof(scf) * SCORESIZE, 0); // zero the unused area
    DmWrite(p, 0, my_sc, sizeof(scf)); // write me.  whee!!!
    MemPtrUnlock(p);  // release the record.......
    DmReleaseRecord(iLarnSaveDB, recindex, true); // dirty=true
    DmSetRecordInfo(iLarnSaveDB, recindex, NULL, &uniqueID);
  } else {
    // scoreboard exists
    vh = DmGetRecord(iLarnSaveDB, recindex);
    p = MemHandleLock(vh);
    if (!p) return;   // we're in some kind of trouble
    
    win_scores = (scf *) p; // array of 'em.
    
    ousted_fellow = SCORESIZE-1; // index of the lowest score..
    for (i = 0 ; i < SCORESIZE ; i++) {
      if (win_scores[i].birthdate == my_sc->birthdate) {
	ousted_fellow = i; // yo!  it's your clone!
	break;
      }
    }
    // if you're less than the lowest score (or old self), don't bother
    if (my_sc->score > win_scores[ousted_fellow].score ||
	!win_scores[ousted_fellow].timeused) {
      // if there's no clone of you, find the first unused slot.
      if (ousted_fellow == SCORESIZE-1) {
	for (i = 0 ; i < SCORESIZE ; i++) {
	  if (win_scores[i].timeused == 0) {
	    ousted_fellow = i; // it's a blank slot
	    break;
	  }
	}
      }
      // ok, either:
      // clone exists (clone > ousted_fellow), or not.
      // if clone exists,
      //   find the space for you.  copy from there to clone.
      //   everything lower than clone stays where it is!
      // else
      //   find the space for you.  copy from there to ousted fellow.
      //   everything lower than ousted fellow stays where it is!
      // Gosh, they're the same.  How excellent.

      // so, you are booting the lowest score, or the first unused score,
      // or your own clone.  this might not be the relative ordering that
      // you deserve, so look at the higher entries and see if you outrank.
      my_location = ousted_fellow - 1; // look at next-highest
      while (my_location >= 0 &&
	     (my_sc->score > win_scores[my_location].score ||
	      !win_scores[my_location].timeused))
	my_location--;
      my_location++; // this is the place you deserve.  now move
      // 0  ... my_location ...         foo ousted_fellow
      // to
      // 0  ... ME          my_location ... foo
      if (my_location < ousted_fellow) {
	for (i = ousted_fellow ; i > my_location ; i--) {
	  // Move the entry at "i-1" to "i"
	  // I think this works now.  Let's test it.  (xxx)
	  offset = sizeof(scf) * i;                            // TO i
	  DmWrite(p, offset, &(win_scores[i-1]), sizeof(scf)); // FROM i-1
	}
      }
      // ousted_fellow now clobbered; my_location now 'empty'
      // um, I don't think 0 is the right offset?
      offset = sizeof(scf) * my_location;
      DmWrite(p, offset, my_sc, sizeof(scf)); // write me..
    } // else you're (yeah again) too pathetic to write
    MemPtrUnlock(p); // release record...
    DmReleaseRecord(iLarnSaveDB, recindex, true); // dirty=true
  } // done finding or creating scoreboard
  if (WINNER == cod)
    boot_loser_clones(my_sc->birthdate);
  // Hey, we're done.  How about that.
}

static void boot_loser_clones(ULong birthdate)
{
  Short i, offset, victim;
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID;
  scf * win_scores; // or is it ** ?  uncertain.

  uniqueID = REC_SCORE;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);

  if (err) {
    // scoreboard does not exist; illogical, but means we have no work to do.
  } else {
    // scoreboard exists
    vh = DmGetRecord(iLarnSaveDB, recindex);
    p = MemHandleLock(vh);
    if (!p) return;   // we're in some kind of trouble
    
    win_scores = (scf *) p; // array of 'em.

    // Assume only one clone exists!
    victim = -1;
    for (i = 0 ; i < SCORESIZE ; i++) {
      if (win_scores[i].birthdate == birthdate) {
	victim = i;
	break;
      }
    }
    if (victim == -1) {
      MemPtrUnlock(p); // release record...
      DmReleaseRecord(iLarnSaveDB, recindex, false); // dirty=true
      return;
    }
    for (i = victim ; i < SCORESIZE-1 ; i++) {
      // Move the entry at "i+1" to "i"
      // I think this works now.  Let's test it.  (xxx)
      offset = sizeof(scf) * i;                            // TO i
      DmWrite(p, offset, &(win_scores[i+1]), sizeof(scf)); // FROM i+1
    }
    // don't forget to zero the last entry!
    offset = sizeof(scf) * (SCORESIZE-1);
    DmSet(p, offset, sizeof(scf), 0);

    MemPtrUnlock(p); // release record...
    DmReleaseRecord(iLarnSaveDB, recindex, true); // dirty=true
  }

}


/*
 * Call this when you die.  cod= cause of death
 *
 * cod MUST be something I can pass to m_name,
 * or cod-256 MUST be less than length of whydead.
 */
Short cause_of_death; // we need to remember 'cod' til we get into ScoreForm
extern Boolean i_am_dead; 
void died(Short cod, Boolean show_scores)
{
  if (env->cdude[HPMAX] == 0 && env->cdude[SPELLMAX] == 0 && env->gtime == 0)
    // hm, I think we've been here already.....
    return;

  if (cod < 0) cod = -cod; // let's get real here

  if (life_protected(cod)) // check the Cause Of Death
    return;

  i_am_dead = true;
  preempt_messages();
  // so, everything above here *seems* ok.
  cause_of_death = cod;

  remove_save_records();
  if (WINNER == cause_of_death)
    destroy_sketch_if_any();
  // if cod is 300 or 257, 'quit': go directly to the APPLICATIONS LAUNCHER
  // instead of to ScoreForm.  Not sure how to do this.
  //  WinDrawChars("cod", 3, 0, 11);
  newscore(cod); // dude, this needs debugging.
  //  WinDrawChars("clear", 5, 0, 11);
  clear_env(env);
  //  WinDrawChars("score", 5, 0, 11);
  FrmGotoForm(ScoreForm);
  // enter the player into the scoreboards!  (if he is deserving.)
  return;
  //  if (WINNER(cod)) 
  //    mailbill(); // it WOULD be amusing to put mail in your Mail inbox..
}
/*
 *  when we are in the scores form, display the reason for your demise
 *  and also have a scrollable window of top scores.
 */
/*
Char level_name[3];
static void levelname(Short lev) {
  if (lev <= 0)
    StrPrintF(level_name, " H");
  else if (lev <= 9)
    StrPrintF(level_name, " %d", lev);
  else if (lev <= 15)
    StrPrintF(level_name, "%d", lev);
  else
    StrPrintF(level_name, "V%d", lev-15);
}
*/
static const Char levelname[21][3] = {
" H"," 1"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10","11","12","13","14","15",
"V1","V2","V3","V4","V5"};

const Char class_name[9][10] = {
  "Ogre", "Wizard", "Klingon", "Elf",
  "Rogue", "Geek", "Dwarf", "Rambo", "Rincewind"
};
Short scoreStart; // what entry to start at
Boolean scoreWinners; // print winners or all
#define good_y 24
#define good_y2 15
Boolean can_scroll_up;
Boolean can_scroll_down;
static void update_score_scrollers(FormPtr frm)
{
  FrmUpdateScrollers(frm, 
		     FrmGetObjectIndex(frm, repeat_score_up),
		     FrmGetObjectIndex(frm, repeat_score_down),
		     can_scroll_up, can_scroll_down);
}
// Initialize the score form
void init_death(FormPtr frm)
{
  ControlPtr pushbtn;
  // First, print out why you died.
  if (i_am_dead)
    print_cod(cause_of_death, 0, good_y2, -1);
  else
    FrmCopyTitle(frm, "High Scores");
  // Then, print some other winners/losers
  scoreStart = 0; // what entry to start at
  if (i_am_dead && WINNER == cause_of_death) {
    // note: take care that this strlen is not longer than the default title
    FrmCopyTitle(frm, "You Have Won");
    scoreWinners = true;
    pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_score_win));
  } else {
    scoreWinners = false;
    pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_score_all));
  }
  print_scores();
  CtlSetValue(pushbtn, true);
  update_score_scrollers(frm);
}
// 'scroll' current set of scores up or down
void scroll_scores(Boolean up, FormPtr frm)
{
  Short newstart;
  if (up) {
    if (!can_scroll_up) return;
    newstart = max(0, scoreStart-5);
  }
  else {
    if (!can_scroll_down) return;
    newstart = min(scoreStart+5, SCORESIZE-5);
  }
  if (newstart == scoreStart) return;
  scoreStart = newstart;
  print_scores();
  update_score_scrollers(frm);
}
// swap from winners' scores to everyone's scores
void which_scores(Boolean winners, FormPtr frm)
{
  if (winners == scoreWinners)    return;
  scoreWinners = winners;
  scoreStart = 0;
  print_scores();
  update_score_scrollers(frm);
}

static void print_scores()
{
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  ULong uniqueID;
  scf * win_scores;
  //
  RectangleType r;
  Short i, j, x, y = (i_am_dead ? good_y : good_y2);
  DateType date; // year (since 1904), month, day
  DateFormatType datetype;
  Char buf[80];

  can_scroll_up = can_scroll_down = false;
  // Clear a rectangle in case we are printing over people.
  RctSetRectangle(&r, 0, (i_am_dead ? good_y : good_y2)+2, 160, 119);
  WinEraseRectangle(&r, 0);

  uniqueID = (scoreWinners ? REC_WINSCORE : REC_SCORE);
  if (DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex)) goto NOSCORES;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) goto NOSCORES;
  win_scores = (scf *) p; // array of 'em.
  // Ok, there is room for 5 two-line entries on the screen.
  if (win_scores[0].timeused == 0) {
    MemHandleUnlock(vh);
    goto NOSCORES;
  }
  // Print nice scores....
  i = (i_am_dead ? 2 : 1);
  StrPrintF(buf, "Score");
  WinDrawChars(buf, StrLen(buf), 22, y+i);
  StrPrintF(buf, "Hard");
  WinDrawChars(buf, StrLen(buf), 53, y+i);
  StrPrintF(buf, "Class");
  WinDrawChars(buf, StrLen(buf), 76, y+i);
  StrPrintF(buf, "Date");
  WinDrawChars(buf, StrLen(buf), 116, y+i);
  //  StrPrintF(buf, "Score");
  //  WinDrawChars(buf, StrLen(buf), 12, y+2);
  //  StrPrintF(buf, "Difficulty   Class  Date");
  //  WinDrawChars(buf, StrLen(buf), 40, y+2);
  datetype = PrefGetPreference(prefDateFormat);
  for (i = max(scoreStart,0), j = 0 ; i < SCORESIZE && j < 5 ; i++, j++) {
    if (win_scores[i].timeused > 0) {
      x = 0;
      StrPrintF(buf, "%ld", win_scores[i].score); // 10 digits
      x = 50 - FntCharsWidth(buf, StrLen(buf));
      WinDrawChars(buf, StrLen(buf), x, y + 11 + j*22);
      StrPrintF(buf, "%d", win_scores[i].hardlev); // 3 digits
      x = 68 - FntCharsWidth(buf, StrLen(buf));
      WinDrawChars(buf, StrLen(buf), x, y + 11 + j*22);
      StrPrintF(buf, "(%s)", class_name[win_scores[i].char_class]);
      x = 76;
      WinDrawChars(buf, StrLen(buf), x, y + 11 + j*22);
      // now get the date in a string format
      DateSecondsToDate(win_scores[i].date, &date);
      DateToAscii(date.month, date.day, date.year+firstYear, datetype, buf);
      x = 116; // allow 40 pixels for class name
      WinDrawChars(buf, StrLen(buf), x, y + 11 + j*22);
      if (scoreWinners) {
	StrPrintF(buf, "%ld Mobuls, %s", win_scores[i].timeused,
		  win_scores[i].who);
	x = 158 - FntCharsWidth(buf, StrLen(buf));
	WinDrawChars(buf, StrLen(buf), x, y + 22 + j*22);
      } else {
	print_cod(win_scores[i].cod, 0, y + 22 + j*22, win_scores[i].level);
      }
    }
  }
  WinDrawGrayLine(0, y+2+9, 160, y+2+9);
  if (scoreStart > 0 && win_scores[scoreStart-1].timeused > 0)
    can_scroll_up = true;
  if (scoreStart+5 < SCORESIZE && win_scores[scoreStart+5].timeused > 0)
    can_scroll_down = true;
  // Ok, done.
  MemHandleUnlock(vh);
  return;
 NOSCORES:
  if (scoreWinners)
    WinDrawChars("--- The winners list is empty ---", 33, 12, good_y+2+11);
  else
    WinDrawChars("--- The high score list is empty ---", 36, 12, good_y+2+11);
  return;
}

// print just the cause of death on a line
static void print_cod(Short cod, Short x, Short y, Short lev)
{
  Short len;
  Char buf[80];
  const Char *nstr;
  Char *mod, ch;
  if (cod < 256) { // monster
    if (cod > 0) {
      nstr = m_name(cod);
      if (cod < LUCIFER) {
	ch = nstr[0];
	mod = (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
	  ? "an" : "a";
      } else mod = "the"; // definite article looks better
      StrPrintF(buf, "killed by %s %s", mod, nstr);
    } else {
      StrPrintF(buf, "killed by nothing");
    }
  } else {
    // I believe that any of these will fit on one line.....
    cod -= 256;
    if (cod > 29)
      cod = 0; // should not happen
    StrPrintF(buf, "%s", whydead[cod]);
  }
  if (lev >= 0) {
    len = StrLen(buf);
    //    StrPrintF(buf+len, " on %d", lev);
    StrPrintF(buf+len, " on %s", levelname[lev]);
  }
  WinDrawChars(buf, StrLen(buf), x, y);
}




// from version 0.30
// moves a record from db to savedb
void topten_move_db(ULong uniqueID)
{
  UInt index;
  Handle h;
  Err err;
  //  WinDrawChars("FOO", 3, 70, 70);
  err = DmFindRecordByID(iLarnDB, uniqueID, &index);
  if (!err) {
    //    WinDrawChars("BAR", 3, 70, 70);
    err = DmDetachRecord(iLarnDB, index, &h);
    if (!err) {
      //      WinDrawChars("QUX", 3, 70, 70);
      index = DmNumRecords(iLarnSaveDB);// Yet they end up at 0 and 1 anyway!!!
      err = DmAttachRecord(iLarnSaveDB, &index, h, NULL);
      if (!err) {
	// do I need a DmReleaseRecord? probably not
	DmReleaseRecord(iLarnSaveDB, index, true);
	// CAN'T use the old uniqueID.. if you do it will just silently vanish!
	uniqueID -= 4;
	DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
	//	WinDrawChars("MOO", 3, 70, 70);
      }
    }
  }
}

// from version 0.22
// I would like to get rid of this... sigh
void topten_fwdcompatibl()
{
  UInt recindex, recindex2; //, obsolete;
  Short twice, i;
  VoidHand vh, vh2;
  VoidPtr p, p2;
  Err err;
  ULong uniqueID;
  scf_vx01 * old_score;
  scf new_score;
  // Read stuff as scf_vx01
  // Write stuff as scf

  // (note: yes, env has been allocated and character (if any) loaded by now)
  env->cdude[BIRTHDATE] = TimGetSeconds(); // in case saved character existed
  // There could also be an outstanding sketch.  But only for beta testers
  // of version 0.21, so I will just ignore that case.

  uniqueID = OLD_REC_SCORE; // REC_WINSCORE = REC_SCORE + 1   (very important!)
  for (twice = 0 ; twice < 2 ; twice++, uniqueID++) {
    err = DmFindRecordByID(iLarnDB, uniqueID, &recindex);
    if (err) {
      // scoreboard does not exist; no need to do anything.
    } else {
      // Get ready to read the old record
      vh = DmQueryRecord(iLarnDB, recindex);
      p = MemHandleLock(vh);
      if (!p) return;   // we're in some kind of trouble
      old_score = (scf_vx01 *) p; // array of 'em.

      // Get ready to write the new record.
      recindex2 = DmNumRecords(iLarnDB);
      vh2 = DmNewRecord(iLarnDB, &recindex2, sizeof(scf) * SCORESIZE);
      p2 = MemHandleLock(vh2);
      DmSet(p2, 0, sizeof(scf) * SCORESIZE, 0); // just to be sure

      // Copy stuff!  and add Birthdate.
      for (i = 0; i < SCORESIZE; i++) {
	new_score.birthdate = i + 1; // well, it's unique and non-zero
	new_score.score = old_score[i].score;
	new_score.timeused = old_score[i].timeused;
	new_score.taxes = old_score[i].taxes;
	new_score.dmg = old_score[i].dmg;
	new_score.bank = old_score[i].bank;
	new_score.date = old_score[i].date;
	new_score.suid = old_score[i].suid;
	new_score.hardlev = old_score[i].hardlev;
	new_score.cod = old_score[i].cod;
	new_score.level = old_score[i].level;
	new_score.char_class = old_score[i].char_class;
	new_score.is_male = old_score[i].is_male;
	StrNCopy(new_score.who, old_score[i].who, NAMELEN);
	DmWrite(p2, i * sizeof(scf), &new_score, sizeof(scf));
      }
      MemPtrUnlock(p2);  // release the new record.......
      DmReleaseRecord(iLarnDB, recindex2, true); // dirty=true
      DmSetRecordInfo(iLarnDB, recindex2, NULL, &uniqueID);
      // Delete the old record!
      MemHandleUnlock(vh);
      DmRemoveRecord(iLarnDB, recindex);
    }
  } // end for
}

