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
/* #include <UI/UIGlobals.h> */ // not needed?  see ITSYON [commented out]
#include "iLarnRsc.h" // itsyfont

#ifndef I_AM_COLOR
#include <System/Globals.h> // for font munging
#else /* I_AM_COLOR */
// hm, do I need something for font munging? guess not
#include "color.h" // for color stuff
#endif /* I_AM_COLOR */
#include "win.h"

static void put_char_at(Short row, Short col, Char ch, Boolean bold) SEC_5;

/* The screen can have 15 rows, but I took 1 for messages and 3 for stats */
#define visible_h 11
/* You can put about 19.9 m's on a memo line.. so.. we'll have 20 cols */
#define visible_w 20
/* Height in pixels of one character ... in this case an 'M' */
#define visible_char_h 10
/* Width in pixels of one character ... 160 pixels / 20 col = 8 pix/col */
#define visible_char_w 8

// itsy is a smaller font
#define visible_h_itsy 17
#define visible_w_itsy 32
#define visible_char_h_itsy 6
#define visible_char_w_itsy 5

Boolean itsy_on = false;

/* This is the offset to convert character-positions in
   the "visible" part of the screen represented by 'terminal'
   to the larger "real" dungeon represented by 'buffer'. */
Short visible_x = 0, visible_y = 0;

extern LarnPreferenceType my_prefs;
Boolean disable_autocentering = false;
Boolean autocenter_just_this_once;
Boolean level0 = false;
void level0_itsy_fix(Boolean yes)
{
  level0 = yes;
  if (level0 && itsy_on)
    toggle_itsy(); // do not do the small font in the town.
}
void am_i_on_level_0(Boolean yes)
{
  disable_autocentering = yes;
  autocenter_just_this_once = yes;
}

// From Ularn's header.h, these are 17 down by 67 across.
#define DROWS MAXY
#define DCOLS MAXX

Char terminal[visible_h_itsy][visible_w_itsy];/*relative: model of the screen*/
Char buffer[DROWS][DCOLS]; /* absolute - a model of the dungeon */
Boolean buffer_bold[DROWS][DCOLS]; /* absolute - a model of the dungeon */
Boolean screen_dirty;
Boolean lines_dirty[DROWS]; /* absolute */


/*
#define ITSY     void *font128 
#define ITSYON   font128 = MemHandleLock(DmGetResource('NFNT', ItsyFont)); UICurrentFontPtr = font128
#define ITSYOFF  MemHandleUnlock(font128)
*/

/*
 * Switch between the small font and the regular font.
 * (Do NOT allow the small font if the SDK that I compiled with
 * is inappropriate for your operating system version.)
 */
Boolean toggle_itsy()
{
  DWord version;
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
#ifdef I_AM_OS_2
  if (version < 0x03000000l) {
    itsy_on = !itsy_on;
    WinEraseWindow();
    return true;
  } else itsy_on = false;
#else
  if (version >= 0x03000000l) {
    itsy_on = !itsy_on;
    WinEraseWindow();
    return true;
  } else itsy_on = false;
#endif
  return false;
}



/* 
 *  This will clear the part of the display that belongs to
 *  "the dungeon".  It also clears 'terminal' since that is
 *  a representation of the display.
 *  This should be called before the offset for 'terminal' is
 *  moved to show a different part of 'buffer'.
 */
void clear_visible() 
{
  Short i, j;
  Short v_h = itsy_on ? visible_h_itsy : visible_h;
  Short v_w = itsy_on ? visible_w_itsy : visible_w;
  Short dunbottom = (itsy_on ? 105 : 114) - 1;

  /* Clear the physical screen. */
  RectangleType r;
  //  RctSetRectangle(&r, 0, 23, 160, 114);
  //  RctSetRectangle(&r, 0, 12, 160, 114);
  RctSetRectangle(&r, 0, 0, 160, dunbottom);
  qWinEraseRectangle(&r, 0);

  /* Update 'terminal' to represent the cleared screen. */
  screen_dirty = true;
  //  spell_dirty = true;
  for (i = 0 ; i < v_h; i++) {
    lines_dirty[i+visible_y] = true;
    for (j = 0; j < v_w; j++) {
      terminal[i][j] = ' ';
    }
  }
}

// col,row = x,y
void setscreen(Short col, Short row, Short ch)
{
  buffer[row][col] = ch;
  buffer_bold[row][col] = false;
  lines_dirty[row] = true;
  screen_dirty = true;
}

// these are needed in some other places too.
Char monstnamelist[128]=" lGHJKOScjtAELNQRZabhiCTYdegmvzFWflorXV pqsyUkMwDDPxnDDuD         ..............................................................";

Char get_monstnamelist(Short i) {
  if (i < 0 || i > LUCIFER) i = 0;
  return monstnamelist[i];
}
// make the invisible stalker visible
void see_invisible() {
  monstnamelist[INVISIBLESTALKER] = 'I';
}
// make the demon lords visible
void got_larneye() {
  monstnamelist[DEMONLORD]   = '1';
  monstnamelist[DEMONLORD+1] = '2';
  monstnamelist[DEMONLORD+2] = '3';
  monstnamelist[DEMONLORD+3] = '4';
  monstnamelist[DEMONLORD+4] = '5';
  monstnamelist[DEMONLORD+5] = '6';
  monstnamelist[DEMONLORD+6] = '7';
  monstnamelist[DEMONPRINCE] = '9';
  monstnamelist[LUCIFER] = '0';
}

Char objnamelist[128]=" AToP%^F&^+M=%^$$f*OD#~][[)))(((||||||||{?!BC.o...<<<<EVV))([[]]](^ [H*** ^^ S tsTLcu...//))]:::::@.............................";

// make the invisible stalker invisible again
void unsee_invisible() {
  monstnamelist[INVISIBLESTALKER] = objnamelist[0];
}
// make the demon lords invisible again
void ungot_larneye() {
  monstnamelist[DEMONLORD]   = 
  monstnamelist[DEMONLORD+1] = 
  monstnamelist[DEMONLORD+2] = 
  monstnamelist[DEMONLORD+3] = 
  monstnamelist[DEMONLORD+4] = 
  monstnamelist[DEMONLORD+5] = 
  monstnamelist[DEMONLORD+6] = 
  monstnamelist[DEMONPRINCE] = 
  monstnamelist[LUCIFER] = objnamelist[0];
}

/*
 * Draw the whole screen (as the player knows it)
 */
void recalc_screen()
{
  Char draw_me;
  Short i, j, k;
  for (i = 0 ; i < MAXY ; i++) {
    lines_dirty[i] = true;
    for (j = 0 ; j < MAXX ; j++) {
      buffer_bold[i][j] = false;
      if (env->know[j][i] == 0) {
	draw_me = ',';
      } else if (env->know[j][i] == 2) {
	k = env->mitem[j][i];
	draw_me = objnamelist[0]; // if not 0 or OWALL, draw in 'bold'?
	if (k != 0 && k != OWALL)
	  buffer_bold[i][j] = true;
      } else {
	k = env->mitem[j][i];
	if (k != 0) {
	  if (k == MIMIC) {
	    // select a random monster character
	    draw_me = monstnamelist[ rund(MAXMONST) ];
	  } else {
	    // draw the monster character
	    draw_me = monstnamelist[ k ];
	  }
	} else {
	  k = env->item[j][i];
	  draw_me = objnamelist[k]; // if not 0 or OWALL, draw in 'bold'?
	  if (k != 0 && k != OWALL)
	    buffer_bold[i][j] = true;
	}
      }
      buffer[i][j] = draw_me;
    }
  }
  screen_dirty = true;
  //  spell_dirty = true;
}


/*
 * The funtion to actually draw a character ch at a col,row position
 * (row, col are RELATIVE i.e. this is used in model-of-screen)
 * note the row+1 offset is so that we have a line for msgs.
 * The character will be drawn in either the small or normal font.
 */
static void put_char_at(Short row, Short col, Char ch, Boolean bold)
{
  //  Short cheat, vcheat = itsy_on ? 14 : 5; // center the dungeon vertically
  Short cheat, vcheat = itsy_on ?  1 : 2;
  /* the space has already been cleared with clear_line */
  /* on second thought, no it hasn't. */
  RectangleType r;
  Short vc_w = itsy_on ? visible_char_w_itsy : visible_char_w;
  Short vc_h = itsy_on ? visible_char_h_itsy : visible_char_h;

#ifdef I_AM_OS_2
  if (itsy_on && !level0)
    FntSetFont(ledFont);
#else
  if (itsy_on && !level0)
    FntSetFont(129);
#endif

  //  RctSetRectangle(&r, col * vc_w, (row+1)*vc_h+vcheat, vc_w, vc_h);
  RctSetRectangle(&r, col * vc_w, row * vc_h+vcheat, vc_w, vc_h);
  qWinEraseRectangle(&r, 0); /* 0 for square corners */
    
  // calculate pixel position of "row, col" and put char there
  cheat = vc_w - FntCharWidth(ch); // center the variable width characters

  if (cheat <= 1)   cheat = 0;
  else              cheat /= 2;

  if (ch != ' ') {
    if (!itsy_on && (ch== 'g' || ch== 'j' || ch== 'p' ||ch == 'q' ||ch == 'y'))
      vcheat--; // unfortunately, letters with dangling bits are a pain.
#ifdef I_AM_COLOR
    if (IsColor) {
      if (bold) {
	WinSetTextColor((my_prefs.black_bg ? black : white));
	WinSetBackColor(get_color(ch, bold));
	WinEraseRectangle(&r, 0); // 'erase' draws in 'background' color? hope.
      } else {
	WinSetTextColor(get_color(ch, bold));
	// an afterthought for dragons
	if (ch == DRAGON_CHAR) {
	  Short x, y, mon;
	  // Calculate absolute row,col from this relative row,col.
	  x = col + visible_x;
	  y = row + visible_y;
	  mon = env->mitem[x][y];
	  WinSetTextColor(get_dragon_color(mon));
	}
      }
    }
#endif // I_AM_COLOR
    if (!my_prefs.black_bg
#ifdef I_AM_COLOR
	|| IsColor
#endif // I_AM_COLOR
	)
      WinDrawChars(&ch, 1, col * vc_w + cheat, row * vc_h + vcheat);
    else 
      WinDrawInvertedChars(&ch, 1, col * vc_w + cheat, row * vc_h + vcheat);
#ifdef I_AM_COLOR
    if (bold) {
      if (IsColor) {
	// set bg color back to normal
	WinSetBackColor((my_prefs.black_bg)?black:white);
	qWinEraseLine(col * vc_w, (row+1) * vc_h+vcheat,
		      (col+1) * vc_w, (row+1) * vc_h+vcheat); // gaaah
      }
      else WinInvertRectangle(&r, 0); /* 0 for square corners */
    }
#else // I_AM_COLOR
    if (bold)  WinInvertRectangle(&r, 0); /* 0 for square corners */
#endif // I_AM_COLOR
  }
  terminal[row][col] = ch;

#ifdef I_AM_OS_2
  if (itsy_on && !level0)
    FntSetFont(stdFont);
#else
  if (itsy_on && !level0)
    FntSetFont(stdFont);
#endif
}


/*
 *  Actually redraw the screen.  (if it is 'dirty'.)
 */
void refresh()
{
  Short col, line, absline;
  Short v_h = itsy_on ? visible_h_itsy : visible_h;
  Short v_w = itsy_on ? visible_w_itsy : visible_w;

  if (screen_dirty) {

#ifdef I_AM_COLOR
    if (IsColor) {
      WinPushDrawState();
      if (my_prefs.black_bg) WinSetBackColor(black);
    }
#endif // I_AM_COLOR

    for (line = 0 ; line < v_h ; line++) {
      //    for (line = v_h - 1; line >= 0; line--) {
      /* line is RELATIVE, line+visible_y is ABSOLUTE. */
      absline = line + visible_y;
      if (lines_dirty[absline]) {
	for (col = 0; col < v_w; col++) {
	  /* col is RELATIVE, col+visible_x is ABSOLUTE. */
	  if (col+visible_x < DCOLS)
	    if (buffer[absline][col+visible_x] != terminal[line][col]) {
	      put_char_at(line, col, buffer[absline][col+visible_x],
			  buffer_bold[absline][col+visible_x]);
	    }
	}
	lines_dirty[absline] = false;
      }
    }
#ifdef I_AM_COLOR
    if (IsColor) {
      WinPopDrawState();
    }
#endif // I_AM_COLOR

    screen_dirty = false;

  }
  //  WinDrawLine(0,12,160,12);
  spell_stats(); // don't forget the spell-status on right hand side.
}

/*
 *  This will ensure that the player remains "on screen",
 *  i.e. it scrolls the 'terminal' so that it includes the player.
 *  (when refresh is called, this will be seen.)
 *  I had to patch this up a bit to make the spell-stats column all nice.
 */
void check_player_position(Short playerx, Short playery, Boolean running)
{
  Short v_h = itsy_on ? visible_h_itsy : visible_h;
  Short v_w = itsy_on ? visible_w_itsy : visible_w;
  Short spell_space = itsy_on ? 2 : 1;

  if (disable_autocentering) {
    if (autocenter_just_this_once) {
      move_visible_window(1, 1, false);
      autocenter_just_this_once = false;
    }
    return;
  }
  if (!running && my_prefs.stay_centered) {
    Short x, y, tx, ty;
    x = playerx - (itsy_on ? visible_w_itsy : visible_w) / 2;
    y = playery - (itsy_on ? visible_h_itsy : visible_h) / 2;
    tx = visible_x; ty = visible_y;
    move_visible_window(x, y, false);
  }
  if (playerx < visible_x ||
      playerx >= visible_x + v_w - spell_space ||
      playery < visible_y ||
      playery >= visible_y + v_h) {
    /* I guess the player teleported or something */
    //    clear_visible(); // moved to move_visible_window
    move_visible_window(playerx, playery, true);
    return;
  }
  /* The player is able to move off the screen from here, so scroll it. */
  if (0 < visible_x && playerx <= visible_x + 1) {
    clear_visible();
    visible_x = max(visible_x - v_w / 2, 0);
  } else if ((MAXX+spell_space) - v_w > visible_x &&
	     playerx >= visible_x + (v_w-spell_space) - 2) {
    clear_visible();
    visible_x = min(visible_x + v_w / 2, (MAXX+spell_space) - v_w);
  }
  if (0 < visible_y && playery <= visible_y + 1) {
    clear_visible();
    visible_y = max(visible_y - v_h / 2, 0);
  } else if (MAXY - v_h > visible_y && 
	     playery >= visible_y + v_h - 2) {
    clear_visible();
    visible_y = min(visible_y + v_h / 2, MAXY - v_h);
  }
}

/*
 *  This will move 'terminal' either so that it's centered on the
 *  buffer-coordinate x,y, or so that the buffer-coordinate x,y is the
 *  left top position of 'terminal'.
 *  (when refresh is called, this will be seen.)
 *  I had to patch this up a bit to make the spell-stats column all nice.
 */
void move_visible_window(Short left_x, Short top_y, Boolean centered)
{
  Short v_h = itsy_on ? visible_h_itsy : visible_h;
  Short v_w = itsy_on ? visible_w_itsy : visible_w;
  Short spell_space = itsy_on ? 2 : 1;
  //  clear_visible();
  if (centered) {
    left_x -= v_w / 2;
    top_y -= v_h / 2;
  }
  if (left_x < 0) left_x = 0;
  if (top_y < 0) top_y = 0;
  if (left_x + v_w > (MAXX + spell_space)) left_x = (MAXX + spell_space) - v_w;
  if (top_y + v_h > MAXY) top_y = MAXY - v_h;
  if (visible_x != left_x || visible_y != top_y) {
    clear_visible();
    visible_x = left_x;
    visible_y = top_y;
  } // else we didn't really move the window.
}

/*
 * Set the buffer value to what it "should" be "now"
 * but it will get actually drawn when the screen is refreshed
 */
void update_screen_cell(Short j, Short i)
{
  Short k;
  Char draw_me;   //j,i == x,y
  buffer_bold[i][j] = false;
  if (env->know[j][i] == 0) {
    draw_me = ',';
  } else if (env->know[j][i] == 2) {
    k = env->mitem[j][i];
    draw_me = objnamelist[k]; // if not 0 or OWALL, draw in 'bold'?
    if (k != 0 && k != OWALL)
      buffer_bold[i][j] = true;
  } else {
    k = env->mitem[j][i];
    if (k != 0) {
      if (k == MIMIC) {
	// select a random monster character
	draw_me = monstnamelist[ rund(MAXMONST) ];
      } else {
	// draw the monster character
	draw_me = monstnamelist[ k ];
      }
    } else {
      k = env->item[j][i];
      draw_me = objnamelist[k]; // if not 0 or OWALL, draw in 'bold'?
      if (k != 0 && k != OWALL)
	buffer_bold[i][j] = true;
    }
  }
  if (buffer[i][j] != draw_me) {
    buffer[i][j] = draw_me;
    lines_dirty[i] = true;
    screen_dirty = true;
  }
}


/*
 *  subroutine to display a cell location on the screen
 *  (i.e. reveal the AREA AROUND the player that can be "seen" "currently")
 *  This will be seen when 'refresh' is called
 */
void showcell(Short x, Short y)
{
  Short i,j,k,foo;
  Short minx, maxx, miny, maxy;
  
  if (env->cdude[BLINDCOUNT])  return;	// see nothing if blind!
  if (env->cdude[AWARENESS]) { 
    minx = x-3;	
    maxx = x+3;	
    miny = y-3;	
    maxy = y+3; 
  } else { 
    minx = x-1;	
    maxx = x+1;	
    miny = y-1;	
    maxy = y+1; 
  }
  
  if (minx < 0)      minx = 0;		
  if (maxx > MAXX-1) maxx = MAXX-1;
  if (miny < 0)      miny = 0;		
  if (maxy > MAXY-1) maxy = MAXY-1;

  for (j = miny ; j <= maxy ; j++) {
    for (i = minx ; i <= maxx ; i++) {
      if ((env->know[i][j] & 1) == 0) {	// if it's 0 or 2, reveal it
	buffer_bold[j][i] = false;
	if ((k = env->mitem[i][j]) != 0) {
	  // if it's a monster, do the monster thing
	  if (k == MIMIC)
	    foo = monstnamelist[rund(MAXMONST)];
	  else
	    foo = monstnamelist[k];
	} else {
	  // otherwise see if there's an item
	  buffer_bold[j][i] = true;
	  switch(k = env->item[i][j]) {
	  case 0: 
	  case OWALL:  
	    buffer_bold[j][i] = false;
	  case OIVTELETRAP:  
	  case OTRAPARROWIV:
	  case OIVDARTRAP: 
	  case OIVTRAPDOOR:	
	    foo = objnamelist[k];
	    break;
	    
	  default: 
	    foo = objnamelist[k]; 
	  }
	}
	env->know[i][j] = 1;
	if (foo != buffer[j][i]) {
	  buffer[j][i] = foo;
	  lines_dirty[j] = true;
	  screen_dirty = true;
	}
      }
    }
  }
}





// Given a (row, col) that is promised to be on the screen,
// tell what screen coordinate it was drawn at
static void what_x_y(Short row, Short col, Short *x, Short *y) {
  Short vc_w = itsy_on ? visible_char_w_itsy : visible_char_w;
  Short vc_h = itsy_on ? visible_char_h_itsy : visible_char_h;
  // these calculations must be same as in put_char_at!
  Short vcheat = itsy_on ? 1 : 2;
  Short cheat = vc_w - FntCharWidth('@');
  if (cheat <= 1)  cheat = 0;
  else             cheat /= 2;
  // ok done making those calculations

  row -= visible_y;
  col -= visible_x;
  *x = col * vc_w + cheat;
  *y = row * vc_h + vcheat;
  // HOWEVER, that is one corner of the cell, and we want the "center",
  // whatever that means.  I should fix what_x_y to do that.
  // or do I need to?  iRogue didn't seem to need it.
}

// Convert x,y from screen relative coordinates (ranging -80 to 80)
// to rogue-relative coordinates (ranging -160 to 160)
// row,col = location of rogue.
void rogue_relativize(Short *x, Short *y, Short col, Short row)
{
  Short rx, ry;
  Short tap_x = *x + 80;
  Short tap_y = *y + 80;
  // first, where the heck is the rogue?
  //  what_x_y(sotu->roguep->row, sotu->roguep->col, &rx, &ry);
  what_x_y(row, col, &rx, &ry);
  // HOWEVER, that is one corner of the cell, and we want the "center",
  // whatever that means.  I should fix what_x_y to do that.
  *x = tap_x - rx;
  *y = tap_y - ry;
}

#ifdef I_AM_COLOR
IndexedColorType peek_color(Short y, Short x)
{
  if (x < 0 || x >= DCOLS || y < 0 || y >= DCOLS)
    return white; // arbitrary decision
  return get_color(buffer[y][x], buffer_bold[y][x]);
}
#endif
