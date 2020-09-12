/*********************************************************************
 * iLarn - Ularn adapted for the PalmPilot.                          *
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
#include "win.h"
/* maybe all this should be in "display.c" */


//#define MAX_SPLASH_LINES 4
#define SPLASH_WIDTH 160
Short stats_area_dirty = 0;
Boolean message_area_dirty = false;
extern Boolean itsy_on;
//Short splash_lines_used = 0;
//Char splash_lines[MAX_SPLASH_LINES][80];
Char *old_messages[SAVED_MSGS];
Short last_old_message_shown;
//extern const Char levelname[21][3];
static const Char levelname[21][3] = {
" H"," 1"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10","11","12","13","14","15",
"V1","V2","V3","V4","V5"};


static void erase_stuff(Short lines) SEC_5;

//Boolean message_more = false;
// Clear the message area
Boolean message_clear()
{
  RectangleType r;
  Short msgtop = 160 - (itsy_on ? 22 : 11);
  RctSetRectangle(&r,0,msgtop,160,160-msgtop);
  qWinEraseRectangle(&r, 0);
  if (stats_area_dirty > (itsy_on ? 2 : 1))
    print_stats();
  if (last_old_message_shown < SAVED_MSGS-1)  return true;
  else return false;
}

// Log a message for later display (if it is not null)
void message(Char *msg)
{
  Short i;
  Char *tmp;
  if (!msg) return;
  // Log the message for later display.....
  tmp = old_messages[0];
  for (i = 0 ; i < SAVED_MSGS-1 ; i++)
    old_messages[i] = old_messages[i+1];
  StrNCopy(tmp, msg, SAVED_MSG_LEN-2); /* leave room for \n.. */
  tmp[SAVED_MSG_LEN-2] = '\0'; /* making sure it's terminated.. */
  StrCat(tmp, "\n");
  old_messages[SAVED_MSGS-1] = tmp;
  // this part is for "--more--" ability:
  last_old_message_shown--;
  if (last_old_message_shown < 0) // "none of these msgs have been shown"
    last_old_message_shown = -1;
  // Remember that we have something to display
  env->pending_splash = true;
}

/*
 * Blow away the message queue
 * (Use this when you die)
 */
void preempt_messages()
{
  env->pending_splash = false;
  last_old_message_shown = SAVED_MSGS - 1;  
}

/*
 * Print things that are in the queue for the message area:
 * While there is space on screen and unused strings in message log,
 *     While word wrapping the current string,
 *          print line and advance the space-left counter.
 *     If we printed the whole string, advance the unused-strings counter.
 */
// Cookies are not working.  I wonder why.  --- I think they work now?
void show_messages()
{
  Short lines_avail = (itsy_on ? 4 : 3);
  Short len_line, len_str, base_y = 160;
  Short lines_used = 0, i = last_old_message_shown;
  Char *msg;
  Boolean more = false;
  // if there's nothing to print, bail out
  if (last_old_message_shown >= SAVED_MSGS-1) {
    env->pending_splash = false;
    return;
  }
  // messages each use 1-2 lines; figure out how many lines we'll use
  while (lines_used < lines_avail && i < SAVED_MSGS-1) {
    len_str = StrLen(old_messages[i+1]);
    if (FntCharsWidth(old_messages[i+1], len_str-1) > SPLASH_WIDTH)
      lines_used += 2;
    else lines_used++;
    i++;
  }
  // if more messages than space, jump up a line
  if (i < SAVED_MSGS-1 || lines_used > lines_avail) {
    more = true;
    lines_used = lines_avail + 1;
    base_y = 160-11; // leave space for --more--
  }
  // else don't use more space than necessary; align with bottom of screen.
  if (lines_used < lines_avail) lines_avail = lines_used;
  // ok, erase the area that will be used, then print stuff on it
  erase_stuff(lines_used);
  while (lines_avail > 0 && last_old_message_shown < SAVED_MSGS-1) {
    msg = old_messages[last_old_message_shown+1];
    while (lines_avail > 0) {
      len_str = StrLen(msg);
      len_line = FntWordWrap(msg, SPLASH_WIDTH);
      if (len_line >= len_str) { // we have finished the string!
	qWinDrawChars(msg, len_str-1, 0, base_y-11*lines_avail);
	lines_avail--;
	last_old_message_shown++;
	break;
      }
      qWinDrawChars(msg, len_line, 0, base_y-11*lines_avail);
      lines_avail--;
      msg += len_line;
    }    // we finished the string, or we ran out of space.
  }  // we finished all messages, or we ran out of space.
  if (more)
    qWinDrawChars("--more--", 8, 10, base_y);//(doesn't really need to be last)
  if (lines_used > (itsy_on ? 2 : 1))
    WinDrawGrayLine(0,160-11*lines_used,160,160-11*lines_used);
  env->pending_splash = false;
  stats_area_dirty = lines_used; // remember how much to clean up, later
}


static void erase_stuff(Short lines)
{
  RectangleType r;
  Short erase = lines;
  // maybe need to erase the spare line
  if (itsy_on && stats_area_dirty > 1) erase = max(erase,2);
  // if the stats are messed up from the last splash, redraw them
  if (stats_area_dirty > lines && (!itsy_on || stats_area_dirty > 2))
    print_stats();
  erase *= 11;
  RctSetRectangle(&r,0,160-erase,160,erase);
  qWinEraseRectangle(&r, 0);
}

/*
void show_messages()
{
  Short i, erase, lines = (itsy_on ? 4 : 3);
  Short msgtop; //, dunbottom = (itsy_on ? 105 : 116);
  RectangleType r;

  lines = min(splash_lines_used, lines);
  msgtop = 160 - 11*lines;

  erase = lines; // maybe need to erase the spare line
  if (itsy_on && stats_area_dirty > 1) erase = max(erase,2);
  // if the stats are messed up from the last splash, redraw them
  if (stats_area_dirty > lines && (!itsy_on || stats_area_dirty > 2))
    print_stats();
  erase *= 11;
  RctSetRectangle(&r,0,160-erase,160,erase);
  WinEraseRectangle(&r, 0);

  for (i = 0 ; i < lines ; i++) {
    //  WinDrawChars(splash_lines[i], StrLen(splash_lines[i]), 2, 11*i);
    WinDrawChars(splash_lines[i], StrLen(splash_lines[i]), 2, msgtop+11*i);
  }
  stats_area_dirty = lines;
  if (splash_lines_used > (itsy_on ? 2 : 1))
    WinDrawGrayLine(0,msgtop,160,msgtop);
  splash_lines_used = 0;
  env->pending_splash = false;
  // should add some provision for "--more--" perhaps
}
*/

/***************************************************************
         MESSAGE  LOG   (alloc_message_log, log_message)
 IN: msg = the message to add
 PURPOSE:
 Record the new message (there is sort of a circular array
 of the Last N messages to display in the message log.) 
****************************************************************/
// call alloc_message_log initially when all stuff is being allocated..
// it has to be dynamic so we can play games with pointers in log_message.
void alloc_message_log()
{
  Short i;
  for (i = 0 ; i < SAVED_MSGS ; i++) {
    old_messages[i] = (Char *) md_malloc(sizeof(Char) * SAVED_MSG_LEN);
    old_messages[i][0] = '\0';
  }
  // the log is all null strings.. so (we pretend) all of it has been shown
  last_old_message_shown = SAVED_MSGS - 1;
}

//01234567  log new message 'n'..
//abc_____  becomes
//bc_____n

/*
 * Add a line to the 'popup' message window
 * It will be displayed later
void add_to_splash(Char *msg, struct everything *env)
{
  if (!msg) return;
  log_message(msg);
  if (splash_lines_used < MAX_SPLASH_LINES) {
    StrNCopy(splash_lines[splash_lines_used], msg, 80);
    splash_lines_used++;
  }
  env->pending_splash = true;
}
 */

// Add a multiple-line message to the 'popup' message window
/*
void add_long_to_splash(Char *msg, struct everything *env)
{
  Short len_line;
  if (!msg) return;
  log_message(msg);
  while (splash_lines_used < MAX_SPLASH_LINES) {
    len_line = FntWordWrap(msg, SPLASH_WIDTH);
    StrNCopy(splash_lines[splash_lines_used], msg, len_line);
    splash_lines[splash_lines_used][len_line] = '\0';
    splash_lines_used++;
    if (len_line >= StrLen(msg)) break;
    msg += len_line;
  }
  env->pending_splash = true;
}
*/

/*
 * Print things that are in the queue for the 'popup' message window
 */
/*
void print_splash()
{
  Short i;

  for (i = 0 ; i < min(splash_lines_used,3) ; i++) {
    // xxx I *should* be clever and print no more text than fits.
    WinDrawChars(splash_lines[i], StrLen(splash_lines[i]), 2, 11*i);
  }
  splash_lines_used = 0;
  env->pending_splash = false;
}
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/*
 *  Print what spells are active on the right side of the screen
 */

static const Short idx[17] = { STEALTH, UNDEADPRO, SPIRITPRO, CHARMCOUNT,
			       TIMESTOP, HOLDMONST, GIANTSTR, FIRERESISTANCE,
			       DEXCOUNT, STRCOUNT, SCAREMONST, HASTESELF,
			       CANCELLATION, INVISIBILITY, ALTPRO,
			       PROTECTIONTIME, WTW };

// We probably only need to call this when screen is re-drawn completely.
// i.e. put this somewhere conditional in 'refresh'
void spell_stats()
{
  DWord version;
  Char buf[40] = "S U SpChT HoG F D StScHaC I P'P W ";
  Short i, y, dunbottom = (itsy_on ? 105 : 114) - 2;
  RectangleType r;
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);

  // if we are in the town, return
  if (env->level == 0) return;
  // if we are in the wrong os to use a small font, return
#ifdef I_AM_OS_2
  if (!(version < 0x03000000l)) return;
#else
  if (!(version >= 0x03000000l)) return;
#endif
  // if we have no active spells, return
  y = 0;
  for (i = 0, y=0 ; i < 17 ; i++)
    if (env->cdude[idx[i]]) y++;
  if (!y) return;

  // ok... clear the rightmost column or so, and draw spell status in it.
#ifdef I_AM_OS_2
  FntSetFont(ledFont);
#else
  FntSetFont(129);
#endif
  RctSetRectangle(&r, 152, 0, 152, dunbottom);
  qWinEraseRectangle(&r, 0);
  //  for (i = 0, y = 11+6 ; i < 17 ; i++, y += 6)
  for (i = 0, y = 0 ; i < 17 ; i++, y += 6)
    if (env->cdude[idx[i]])
      qWinDrawChars(buf+i*2, 2, 152, y);
  FntSetFont(stdFont);
  WinDrawGrayLine(151,0,151,dunbottom);
}


/*
 * Print three lines of status at the bottom of screen
 */
extern Boolean recalc_changed_stats;
void print_stats()
{
  RectangleType r;
  //  Char buf[20];
  Char buf[80];
  // Short w, hi, x;
  Short hi, x;
  // If itsy ON, the dungeon takes up 103 vertical pixels, else it takes 112.
  // We center the 103 with 1 pixel margin and 5 rows of stats/messages.
  // We center the 112 with 1 pixel margin and 4 rows of stats/messages.
  Short dunbottom = (itsy_on ? 105 : 114); // 127-11=116 // 127
  Short msgtop = 160 - (itsy_on ? 22 : 11);
  Long *cdude = env->cdude;
  if (cdude[HP] <= 0) return; // we're probably not in the main screen

  recalc_changed_stats = false;

  /*
  RctSetRectangle(&r,0,dunbottom,160,msgtop-dunbottom);
  qWinEraseRectangle(&r, 0);
  */

  qWinDrawLine(0,dunbottom-1,160,dunbottom-1);
  hi = dunbottom;
  
  RctSetRectangle(&r,0,hi,160,11);
  qWinEraseRectangle(&r, 0); /* 0 for square corners */
  x = 1;
  // Swap spells with hp
  //StrCopy(buf, "Hp 014/014");
  StrPrintF(buf, "Hp %d/%d", (Int) cdude[HP], (Int) cdude[HPMAX]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 55;
  //  StrCopy(buf, "Ac 004");
  StrPrintF(buf, "Ac %d", (Int) cdude[AC]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 31;
  //  StrCopy(buf, "Wc 000");
  StrPrintF(buf, "Wc %d", (Int) cdude[WCLASS]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 35;
  //  StrCopy(buf, "Floor 01");
  if (cdude[TELEFLAG] != 0) {
    StrPrintF(buf, "Floor  ?");
  } else {
    //    StrPrintF(buf, "Floor %d", env->level);
    StrPrintF(buf, "Floor %s", levelname[(Short) env->level]);
  }
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x = 1;
  hi += 11;
  RctSetRectangle(&r,0,hi,160,11);
  qWinEraseRectangle(&r, 0); /* 0 for square corners */
  //  StrCopy(buf, "Spells 01/01");
  StrPrintF(buf, (cdude[SPELLMAX]<100)?"Spells %d/%d":"Spell%d/%d",
	    (Int) cdude[SPELLS], (Int) cdude[SPELLMAX]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 56;
  //  StrCopy(buf, "s14  i12 w04 c12 d08 ch03");
  if (cdude[SPELLMAX] < 10) {
    StrPrintF(buf, "s %d  i %d w %d c %d d %d ch %d", 
	      (Int) (cdude[STRENGTH]+cdude[STREXTRA]), (Int) cdude[INTELLIGENCE],
	      (Int) cdude[WISDOM], (Int) cdude[CONSTITUTION],
	      (Int) cdude[DEXTERITY], (Int) cdude[CHARISMA]);
    x -= 13;
  } else {
    StrPrintF(buf, "s%d  i%d w%d c%d d%d ch%d", 
	      (Int) (cdude[STRENGTH]+cdude[STREXTRA]), (Int) cdude[INTELLIGENCE],
	      (Int) cdude[WISDOM], (Int) cdude[CONSTITUTION],
	      (Int) cdude[DEXTERITY], (Int) cdude[CHARISMA]);
  }
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x = 1;
  hi += 11;
  RctSetRectangle(&r,0,hi,160,hi+11);
  qWinEraseRectangle(&r, 0); /* 0 for square corners */
  //  StrCopy(buf, "Level 001");
  StrPrintF(buf, "Level %d", (Int) cdude[LEVEL]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 45;
  //  StrCopy(buf, "Exp 0123456789");
  //  StrPrintF(buf, "Exp %d", (Int) cdude[EXPERIENCE]);
  StrPrintF(buf, "Exp %ld", cdude[EXPERIENCE]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  x += 73;
  //  StrCopy(buf, "$ 000123");
  //  StrPrintF(buf, "$ %d", (Int) cdude[GOLD]);
  StrPrintF(buf, "$ %ld", cdude[GOLD]);
  qWinDrawChars(buf, StrLen(buf), x, hi);
  stats_area_dirty = 0;
  return;
}

