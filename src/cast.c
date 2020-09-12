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
#include "iLarnRsc.h" // DirectionForm


static Boolean nospell(Short x, Short monst) SEC_1;
extern Short cast_form_use;

// Used for alter-reality spell.
struct isave
{
  Boolean is_mon;	// false = item,     true = monster
  Char id;	        // item number   or  monster number
  Short arg;	        // item type     or  monster hitpoints
};

// Can't we make this nasty thing a record or something?
const Char spelweird[MAXMONST+8][SPNUM] = {
/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* lemming */
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,  0,0,0,0,0,0 	} ,
/*gnome */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      hobgoblin */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         jackal */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         kobold */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
 /*            orc */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          snake */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*giant centipede */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         jaculi */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     troglodyte */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*      giant ant */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   floating eye */ 
{  0,0,0,8,0,10,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     leprechaun */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          nymph */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         quasit */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*   rust monster */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         zombie */ 
{  0,0,0,8,0,4,   0,0,0,0,0,0,4,0,   0,0,0,0,0,4,0,   4,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   assassin bug */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        bugbear */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     hell hound */ 
{  0,6,0,0,0,0,   12,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*     ice lizard */ 
{  0,0,0,0,0,0,   11,0,0,0,0,0,0,0,  0,15,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        centaur */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          troll */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   4,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*           yeti */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,15,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   white dragon */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,14,0,  0,15,0,0,0,0,0,  4,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*       elf */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,14,5,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*gelatinous cube */ 
{  0,13,0,8,0,10, 2,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      metamorph */ 
{  0,13,0,0,0,0,  2,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   4,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         vortex */ 
{  0,13,0,0,0,10, 1,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   4,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         ziller */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i     f c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*   violet fungi */ 
{  0,0,0,8,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*         wraith */
{  0,13,0,8,0,4,   0,0,0,0,0,0,14,0,   0,0,0,0,0,4,0,   4,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      forvalaka */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*      lama nobe */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        osequip */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*          rothe */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*           xorn */
{  0,7,0,8,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   4,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*        vampire */
{  0,0,0,0,0,4,   2,0,0,0,0,0,14,0,   0,0,0,0,0,4,0,   0,0,4,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*invisible staker*/
{  0,0,0,0,0,0,   1,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    poltergeist */ 
{  0,13,0,8,0,4,  1,0,0,0,0,0,0,0,   0,4,0,0,0,4,0,   4,0,4,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* disenchantress */
{  0,0,0,8,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*shambling mound */ 
{  0,0,0,0,0,10,  0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    yellow mold */ 
{  0,0,0,8,0,10,  1,0,0,0,0,0,0,0,   0,0,0,0,0,4,0,   0,0,0,0,0,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     umber hulk */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,5,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     gnome king */ 
{  0,7,0,0,3,0,   0,0,0,0,0,0,0,5,   0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*          mimic */
{  0,0,0,0,0,0,   2,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     water lord */ 
{  0,13,0,8,3,4,  1,0,0,0,0,0,0,0,   0,0,9,0,0,4,0,   0,0,0,0,16,4,  0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*  bronze dragon */ 
{  0,7,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   green dragon */ 
{  0,7,0,0,0,0,   11,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    purple worm */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*          xvart */ 
{  0,13,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*    spirit naga */
{  0,13,0,8,3,4,  1,0,0,0,0,0,14,5,   0,4,9,0,0,4,0,   4,0,4,0,4,4,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*  silver dragon */
{  0,6,0,9,0,0,   12,0,0,0,0,0,0,0,  0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*platinum dragon */ 
{  0,7,0,9,0,0,   11,0,0,0,0,0,14,0, 0,0,9,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*   green urchin */ 
{  0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,
/*     red dragon */ 
{  0,6,0,0,0,0,   12,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,   0,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,9,0,0,4,0,   4,0,9,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,0,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*demon lord */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,0,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,

/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/*demon prince */ 
{ 0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  0,0,4,0,0,4,0,   4,0,4,0,4,4,   3,0,0,9,4,   9,0,0,0,0,0 	} ,
/*p m d s c s    w s e h c c p i    b c p c h c v    d l d g f f    s h s t m    s g s w a p */
/* God of Hellfire */ 
{  0,13,0,8,3,10,   1,0,0,0,0,0,14,5,  18,0,9,0,0,4,0,   4,18,4,0,0,4,   4,4,0,9,4,   9,0,17,0,0,0 	}
};

// Special-case messages
const Char spelmes[19][40] = { 
  "",
  /*  1 */	"the web had no effect on the %s",
  /*  2 */	"the %s changed shape to avoid the web",
  /*  3 */	"the %s isn't afraid of you",
  /*  4 */	"the %s isn't affected",
  /*  5 */	"the %s can see you with his infravision",
  /*  6 */	"the %s vaporizes your missile",
  /*  7 */	"your missile bounces off the %s",
  /*  8 */	"the %s doesn't sleep",
  /*  9 */	"the %s resists",
  /* 10 */	"the %s can't hear the noise",
  /* 11 	"the %s's tail cuts it free of the web", */
  /* 11 */	"the %s's tail cuts through the web",
  /* 12 */	"the %s burns through the web",
  /* 13 */	"your missiles pass right through the %s",
  /* 14 */	"the %s sees through your illusions",
  /* 15 */	"the %s loves the cold!",
  /* 16 */	"the %s loves the water!",
  /* 17 */	"the demon is terrified of the %s!",
  /* 18 */	"the %s loves fire and lightning!"
};
// init-cast-select could probably go here.

/* The 'task' remembers what you're doing, while you take a time-out
   to figure out what direction to do it IN.  */
#define DIR_MAKEWALL 0
#define DIR_TDIRECT  1
#define DIR_POLY     2
#define DIR_DIRECT   3
#define DIR_GODIRECT 4
#define DIR_ANNIHIL  5
Short task;
struct taskdata_s {
  Short spnum;
  Short dam;
  Char *str;
  Short arg;
  Short delay;
  Char cshow;
};
struct taskdata_s taskdata;


// Call this instead of FrmPopupForm(DirectionForm)
// This superimposes some lines on the screen for you to tap within.
extern Boolean casting_a_spell;
void draw_cast_slices()
{
  casting_a_spell = true;
  WinInvertLine(0,  40, 60, 70);
  WinInvertLine(100,90,160,120);
  WinInvertLine(0,  120,   60, 90);
  WinInvertLine(100, 70,  160, 40);
  WinInvertLine(40,  0, 70, 60);
  WinInvertLine(90,100,120,159);
  WinInvertLine(120,  0,90, 60);
  WinInvertLine(70, 100,40,159);
}

/*
 *	Routine to check to see if player is confused
 *      if yes, prints a message and returns true.  if no, returns false
 */
Boolean isconfuse()
{
  Boolean b = env->cdude[CONFUSE];
  if (b)
    message("You can't aim your magic!");  // and you used a spell point. d'oh
  return b;
}

/*
 *	Function to genocide (remove) a monster type from game
 *	This is done by setting a flag in the monster[] structure
 */
const Char no_more[] = "There will be no more %s%s";
void genmonst(Short j)
{
  //  Short j = 1;
  Char buf[80];

  if (!env->genocided[j]) { // also should check that j is within bounds!
    env->genocided[j] = true;
    switch(j) {
    case ELF:
      StrPrintF(buf, no_more, "elves");
      break;
    case VORTEX:
      StrPrintF(buf, no_more, "vortices");
      break;
    case VIOLETFUNGI:
      StrPrintF(buf, no_more, m_name(j));
      break;
    case DISENCHANTRESS:
      StrPrintF(buf, no_more, m_name(j), "es");
      break;
    default:
      StrPrintF(buf, no_more, m_name(j), "s");
      break;
    }
    message(buf);
    /* now wipe out monsters on this level */
    newcavelevel(env->level);  // intriguing.
    o_draws();
  } else
    message("You sense failure!");

  cast_form_use = CAST_NONE;
  start_of_Play_Level_loop();
}

// Make a wall, if the space is available.
void makewall(Short x, Short y)
{
  if ((y>=0) && (y<=MAXY-1) && (x>=0) && (x<=MAXX-1)) { /* within bounds? */
    if (env->item[x][y] != OWALL) {	/* can't make anything on walls */
      if (env->item[x][y] == 0){	/* is it free of items? */
	if (env->mitem[x][y] == 0){  /*is it free of monsters? */
	  if ((env->level != 1) || (x != 33) || (y != MAXY-1)) {
	    env->item[x][y] = OWALL;
	    env->know[x][y] = 1;
	    update_screen_cell(x,y);
	  }
	  else message("you can't make a wall there!");
	}
	else message("there's a monster there!");
      }
      else message("there's something there already!");
    }
    else message("there's a wall there already!");
  }
  
}

/*
 *	Routine to teleport-away a monster
 */
void tdirect(Short x, Short y)
{
  Short m; //  Short x,y, m;
  Short spnum = 30;

  m = env->mitem[x][y];
  if (m == 0) {	
    message("There wasn't anything there!");	
    return;  
  }
  ifblind(x,y); // side effect
  if (nospell(spnum,m)) { 
    env->lasthx = x;  
    env->lasthy = y; 
    return; 
  }
  fillmonst(m);  
  env->mitem[x][y] = env->know[x][y] = 0;
}

/*
 *	Routine to damage all monsters that are 1 square from player
 *      Call with
 *	the spell number in sp, the damage done to each square in dam,
 *	  and the lprintf string to identify the spell in str.
 */
void omnidirect(Short spnum, Short dam, Char *str)
{
  Short x,y,m;
  Char buf[80];
  if (spnum < 0 || spnum >= SPNUM || str == 0) return; /* bad args */
  for (x = env->playerx-1 ; x < env->playerx+2 ; x++) {
    for (y = env->playery-1 ; y < env->playery+2 ; y++) {
      if ((m = env->mitem[x][y]) != 0) {
	if (nospell(spnum,m) == 0) {
	  ifblind(x,y); // side effect
	  StrPrintF(buf, str, read_lastmonst());
	  message(buf);
	  hitm(x,y,dam);  
	  //	  nap(800);
	} else { 
	  env->lasthx = x;  
	  env->lasthy = y; 
	}
      }
    }
  }
}
/*
 *	Polymorph a monster
 *	Enter with the spell number in spmun.
 *	Returns no value.
 */
void dirpoly(Short x, Short y)
{
  Short m, tmphp;
  Short spnum = 16;

  if (env->mitem[x][y] == 0) {	
    message("There wasn't anything there!");	
    return;  
  }
  ifblind(x,y); // side effect
  if (nospell(spnum, env->mitem[x][y])) { 
    env->lasthx = x;   // what are these?
    env->lasthy = y; 
    return; 
  }
  do {
    m = rnd(MAXMONST+7);  // polymorph, but not into something genocided
  } while (env->genocided[m]);
  env->mitem[x][y] = m;

  tmphp = env->hitp[x][y];
  env->hitp[x][y] = m_hitpoints(m);
  if (env->hitp[x][y] / tmphp >= 2)
    message("maybe this was a bad idea...");
  update_screen_cell(x, y);  /* show the new monster */
}

/*
 *	Return true (and print an appropriate message)
 *      if the spell can't affect the monster; otherwise return false
 *	Enter with the spell number in x, and the monster number in monst.
 */
static Boolean nospell(Short x, Short monst)
{
  Short tmp;
  Char buf[80];

  // bad spell or monst
  if (x>=SPNUM || monst>MAXMONST+8 || monst<0 || x<0) return false;

  if ((tmp = spelweird[monst-1][x]) == 0) 
    return false;
  // print "the monster is unaffected" type of message
  StrPrintF(buf, spelmes[tmp], m_name(monst));
  //  message(buf);
  message(buf);
  return true;
}

/*
 *	Directional spell damage (nail the monster you pointed at)
 *      This is the first half... it saves the task, preparatory to
 *      being asked what direction to cast in.
 *	Enter with the spell number in spnum, the damage to be done in dam,
 *	  lprintf format string in str, and lprintf's argument in arg.
 */
void direct(Short spnum, Short dam, Char *str, Short arg)
{
  if (spnum<0 || spnum>=SPNUM || str==0) return; // bad arguments
  if (isconfuse()) return; // side effect
  // save task
  task = DIR_DIRECT;
  taskdata.spnum = spnum;
  taskdata.dam = dam;
  taskdata.str = str;
  taskdata.arg = arg;
  // get direction
  draw_cast_slices();
  return;
}
/* 
 * This is the second half that restores the task and performs it,
 * now that we know what direction to do it in.
 */
void direct_helper(Short x, Short y)
{
  Short m;
  Short spnum, dam, arg;
  Char *str;
  Char buf[80];
  // restore task saved in 'direct'
  spnum = taskdata.spnum;
  dam = taskdata.dam;
  str = taskdata.str;
  arg = taskdata.arg;

  m = env->mitem[x][y];
  if (env->item[x][y] == OMIRROR) { // cute, mirror reflects some spells
    if (spnum == 3) {
      message("You fall asleep! "); 
      arg += 2;
      while (arg-- > 0)
	nap(1000);  // xxxx monsters should move, also
      return;
    }
    else if (spnum == 6) {
      message("You get stuck in your own web! "); 
      arg += 2;
      while (arg-- > 0)
	nap(1000);   // xxxx monsters should move, also
      return;
    }
    else {
      env->lastnum = 278; // in case you kill yourself
      StrPrintF(buf, str,"spell caster (that's you)", arg);
      message(buf);
      losehp(dam); 
      return;
    }
  }
  if (m == 0) {	
    message("There wasn't anything there!");	
    return;  
  }
  ifblind(x,y); // side effect
  if (nospell(spnum,m)) { 
    env->lasthx = x;   // hey what are these
    env->lasthy = y; 
    return; 
  }
  //  StrPrintF(str,read_lastmonst(),(Long)arg); // why did I comment this?xxx
  StrPrintF(buf, str, read_lastmonst(), arg);
  message(buf);
  hitm(x,y,dam);
}

/*
 *	Function to hit in a direction from a missile weapon and have it keep
 *	on going in that direction until its power is exhausted
 *	Enter with the spell number in spnum, the power of the weapon in hp,
 *	lprintf format string in str, the # of milliseconds to delay between 
 *	locations in delay, and the character representing the weapon in cshow.
 *      (Erstunken und erlogen - globale Vaiablen! ws, 12.8.93)
 *      (These German comments alarm me.  bs, 6.18.00)
 *      This is the first half and just saves the task before getting dir.
 */
void godirect(Short spnum, Short dam, Char *str, Short delay, Char cshow)
{
  if (spnum<0 || spnum>=SPNUM || str==0 || delay<0) return; /* bad args */
  if (isconfuse())
    return;
  // save task
  task = DIR_GODIRECT;
  taskdata.spnum = spnum;
  taskdata.dam = dam;
  taskdata.str = str;
  taskdata.delay = delay;
  taskdata.cshow = cshow;
  // get direction
  draw_cast_slices();
  return;
}
/*
 * The other half of godirect that actually does the work
 */
void godirect_helper(Short dx, Short dy)
{
  Char *p;
  Short x,y,m, i;
  Char buf[80];
  
  Short spnum, dam, delay;
  Char cshow, *str;
  Boolean frobbed_object = false;
  spnum = taskdata.spnum;
  dam = taskdata.dam;
  str = taskdata.str;
  delay = taskdata.delay;
  cshow = taskdata.cshow;

  x = env->playerx;	
  y = env->playery;
  
  i=0;
  while (dam > 0) {
    x += dx;    
    y += dy;
    if ((x > MAXX-1) || (y > MAXY-1) || (x < 0) || (y < 0)) {
      dam = 0;	
      break;  // out of bounds
    }
    if ((x == env->playerx) && (y == env->playery)) {
      message("You are hit by your own magic!"); 
      env->lastnum = 278;  // in case it kills you
      losehp(dam);  
      return;
    }
    if (env->cdude[BLINDCOUNT] == 0) {
      // if not blind, show effect
      // XXXXxxxx need to debug this animation, it seems.. odd
      nap(delay); // this might be too long
      setscreen(x, y, cshow);
      refresh();
      update_screen_cell(x, y); // back to 'normal' for next refresh
    }
    if ((m = env->mitem[x][y]) != 0) {
      // ok, is there a monster there
      ifblind(x,y); // side effect
      if (m == LUCIFER || (m >= DEMONLORD && rnd(100) < 10) ) {
	dx *= -1; // cute, eh
	dy *= -1;
	StrPrintF(buf, "the %s returns your puny missile!", m_name(m));
	message(buf);
      } else {
	if (nospell(spnum,m)) { 
	  env->lasthx = x;  // eh?
	  env->lasthy = y; 
	  return; 
	}
	StrPrintF(buf, str, read_lastmonst());
	//	message("something happened"); // but why is this here? xxx
	message(buf);
	dam -= hitm(x,y,dam);
	update_screen_cell(x,y);
	//	nap(1000);
	x -= dx;
	y -= dy;
      }
    } else
      // No monster there, but maybe something else interesting.
      switch (*(p=&env->item[x][y])) {
	// LRS either doesn't notice, or hits you back harder.
      case OLRS:	
	if (dam < 60 + env->cdude[HARDGAME]) {
	  dam = 0;
	  //	  message("The LRS absorbs your effort");
	  StrPrintF(buf, str, get_objectname(OLRS));
	  message(buf);
	} else {
	  env->item[x][y] = 0;
	  update_screen_cell(x, y);
	  nap(1200);
	  env->item[x][y] = OLRS;
	  update_screen_cell(x, y);
	  message("You curse: \"DEATH, TAXES, and HARD RESETS!\"");
	  add_tax(45054L, true);
	  nap(1200);
	  dam *= 2;  // LRS reflects your puny missile, with a bonus.
	  dx  *= -1;
	  dy  *= -1;
	};
	break;

	// Wall stops spells, but might crumble (except on volcano level 3)
      case OWALL:	
	StrPrintF(buf, str, "wall");
	message(buf);
	if (dam >= 50 + env->cdude[HARDGAME])
	  if ((env->level > 0) && (env->level < MAXLEVEL + MAXVLEVEL - 3))
	    /* not in town, not on V3 */
	    if ((x < MAXX - 1) && (y < MAXY - 1) && (x) && (y)) {
	      message("The wall crumbles");
	      *p=0;
	      env->know[x][y]=0;
	      update_screen_cell(x,y);
	      frobbed_object = true;
	    }
	dam = 0;	
	break;

	// Closed door stops spells but might be destroyed.
      case OCLOSEDDOOR:	
	StrPrintF(buf, str, "door");
	message(buf);
	if (dam >= 40) {
	  message("The door is blasted apart");
	  *p=0;
	  env->know[x][y] = 0;
	  update_screen_cell(x, y);
	  frobbed_object = true;
	}
	dam = 0;	
	break;

	// Statue might crumble (revealing a book)
      case OSTATUE:	
	StrPrintF(buf, str, "statue");
	message(buf);
	if (dam > 44) {
	  if (env->cdude[HARDGAME] > 3 && rnd(60) < 30) {
	    dam = 0;	
	    break;
	  }
	  message("The statue crumbles");
	  *p=OBOOK; 
	  env->iarg[x][y] = env->level;
	  env->know[x][y] = 0;
	  update_screen_cell(x, y);
	  frobbed_object = true;
	}
	dam = 0;	
	break;
	    
	// Hitting throne might piss off the gnome king
      case OTHRONE:	
	StrPrintF(buf, str, "throne");
	message(buf);
	if (dam > 33) {
	  env->mitem[x][y] = GNOMEKING; 
	  env->hitp[x][y] = m_hitpoints(GNOMEKING);
	  *p = OTHRONE2;
	  env->know[x][y] = 0;
	  update_screen_cell(x, y);
	  frobbed_object = true;
	}
	dam = 0;	
	break;

	// Mirror reflects your spell
	// (I guess the mirror is spherical, since it reverses any direction)
      case OMIRROR:
	dx *= -1;	
	dy *= -1;	
	break;
      }
    dam -= 3 + (env->cdude[HARDGAME]>>1);
  }
  if (frobbed_object) {
    // so, like, redraw the screen too.
    recalc_screen();
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
  }
  refresh();// I think I need this
}

// Cast a spell... decrement spell points and hand off to speldamage.
void cast(Short i)
{
  message_clear(); // clear away any stale message
  if (i < 0 || i >= SPNUM) return; // "should not happen"
  env->cdude[SPELLS] -= 1; // use up a spell point
  print_stats(); // will this be happy?
  // select spell by index
  if (env->spelknow[i])
    speldamage(i);
  else // presumably this should not happen
    message("nothing happened");
  //  if (!casting_a_spell)
  //    print_stats(env); // else we'll do that in just a bit...
}


/*
 *	Function to perform spell functions cast by the player
 *	Enter with the spell number
 */
void speldamage(Short x)
{
  Short i,j,clev;
  Short xl,xh,yh; // yl unused
  Char *p,*kn,*pm;
  Long *c = env->cdude;
  
  if (x >= SPNUM) return; // no such spell (should not happen)
  
  // can't cast a spell if you stop time
  if (c[TIMESTOP])  { 	
    message("It didn't seem to work"); 
    return; 
  }
  
  // are you feeling smart enough?
  if ((rnd(23)==7) || (rnd(18) > c[INTELLIGENCE])) { 	
    message("It didn't work!");  
    start_of_Play_Level_loop();
    return;
  }

  // are you feeling experienced enough?
  clev = c[LEVEL];
  if (clev * 3 + 2 < x) { 	
    message("You're too inexperienced."); 
    start_of_Play_Level_loop();
    return;
  }
  
  switch(x) {
    /* ----- LEVEL 1 SPELLS ----- */
    
  case 0:
    // protection field +2
    if (c[PROTECTIONTIME]==0)
      c[MOREDEFENSES]+=2;
    c[PROTECTIONTIME] += 250;   
    message("you feel protected");
    break;

  case 1: 
    // magic missile
    i = rnd(((clev+1)<<1)) + clev + 3;
    godirect(x, i, (clev>=2)?"Your missiles hit the %s":
	                     "Your missile hit the %s", 100, '+');
    return;

  case 2:	
    // dexterity
    if (c[DEXCOUNT]==0)	
      c[DEXTERITY]+=3;
    c[DEXCOUNT] += 400;  	
    message("you feel more agile");
    break;

  case 3: 
    // sleep
    i=rnd(3)+1;
    p="While the %s slept, you smashed it %d times";
    direct(x,fullhit(i),p,i);
    return;

  case 4:
    // charm monster (so that is what charisma is good for..)
    c[CHARMCOUNT] += c[CHARISMA]<<1;	
    message("you feel.. dignified");
    break;

  case 5:
    // sonic spear
    godirect(x, rnd(10)+15+clev, "The sound damages the %s", 70, '@'); 
    return;
	  
    /* ----- LEVEL 2 SPELLS ----- */
	  
  case 6: 
    // web
    i=rnd(3)+2;	
    p="While the %s is entangled, you hit %d times";
    direct(x,fullhit(i),p,i);
    return;
	  
  case 7:	
    // strength
    if (c[STRCOUNT]==0) c[STREXTRA]+=3;
    c[STRCOUNT] += 150+rnd(100);    
    message("you feel stronger");
    break;
	  
  case 8:	
    // enlightenment
    enlighten(-5, +6, -15, +16);
    break;
	  
  case 9:	
    // healing
    raisehp(20+(clev<<1));  
    message("you feel better");
    break;
	  
  case 10:	
    // cure your own blindness
    if (c[BLINDCOUNT]) message("you can see again!");
    else  message("was there a point to that?");
    c[BLINDCOUNT]=0;
    break;
	  
  case 11:	
    // create a random monster
    createmonster(makemonst(env->level+1)+8);  
    recalc_screen(); // I think I need this
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh(); // I think I need this
    message("poof!");
    break;
	  
  case 12: 
    // create damaging illusions
    if (rnd(11)+7 <= c[WISDOM]) {
      direct(x,rnd(20)+20+clev,"The %s believed!",0);
      return;
    } else
      message("It didn't believe the illusions!");
    break;
	  
  case 13:
    // invisibility
    j = 0;
    // if he has the amulet of invisibility then add more time
    if ( (i = has_object_at(OAMULET)) >= 0 ) 
      j += 1 + env->ivenarg[i];
    c[INVISIBILITY] += (j<<7)+12;   
    message("you feel transparent");
    break;
	  
    /* ----- LEVEL 3 SPELLS ----- */
	  
  case 14:	
    // fireball
    godirect(x, rnd(25+clev)+25+clev,
	        "The fireball hits the %s", 40, '*'); 
    return;
	  
  case 15:	
    // cold
    godirect(x, rnd(25)+20+clev,
	        "Your cone of cold strikes the %s", 60, 'O');
    return;
	  
  case 16:	
    // polymorph
    if (isconfuse())
      break;
    task = DIR_POLY;
    draw_cast_slices();
    return;
	  
  case 17:	
    // cancellation
    c[CANCELLATION] += 5 + clev;	
    message("poof!");
    break;
	  
  case 18:	
    // haste self
    c[HASTESELF] += 7 + clev;  
    message("you feel hasty");
    break;
	  
  case 19:    
    // cloud kill
    omnidirect(x, 30+rnd(10), "The %s gasps for air");
    break; // (not return, because we don't need a direction form)
	  
  case 20:	
    // vaporize rock
    xh = min(env->playerx+1, MAXX-2);		
    yh = min(env->playery+1, MAXY-2);
    for (i = max(env->playerx-1, 1) ; i <= xh ; i++) 
      for (j = max(env->playery-1, 1) ; j <= yh ; j++) {
	kn = &env->know[i][j];
	pm = &env->mitem[i][j];
	switch(*(p = &env->item[i][j])) {
	  // wall crumbles, unless you are deep in the volcano
	case OWALL:
	  if ((env->level > 0) && (env->level < MAXLEVEL + MAXVLEVEL - 3)) {
	    *p = *kn = 0;
	    //	    message("the wall is vaporized");
	  }
	  break;

	  // statue might crumble
	case OSTATUE:
	  if (c[HARDGAME] > 3 && rnd(60) < 30) {
	    //	    message("the statue is unaffected");
	    break;
	  }
	  *p=OBOOK; 
	  env->iarg[i][j]=env->level;  
	  *kn=0;
	  message("the statue crumbles");
	  break;

	  // attempt to vaporize throne is certain to piss off gnome king
	case OTHRONE: 
	  *pm=GNOMEKING;  
	  *kn=0;  
	  *p= OTHRONE2;
	  env->hitp[i][j]=m_hitpoints(GNOMEKING); 
	  //	  message("uh oh");
	  break;

	  // attempt to vaporize altar is certain to result in demon horde
	case OALTAR:	
	  *pm=DEMONPRINCE;  
	  *kn=0;
	  env->hitp[i][j]=m_hitpoints(DEMONPRINCE);
	  createmonster(DEMONPRINCE);
	  createmonster(DEMONPRINCE);
	  createmonster(DEMONPRINCE);
	  createmonster(DEMONPRINCE);
	  //	  message("uh oh");
	  recalc_screen(); // I think I need this
	  showcell(env->playerx, env->playery);
	  showplayer(env->playerx, env->playery);
	  refresh(); // I think I need this
	  break;
	} // end wall/statue/throne/altar
	switch(*pm) {
	  // certain rocklike monsters also take damage.
	  case XORN:	
	    ifblind(i,j);  // side effect
	    hitm(i,j,200); 
	    break; 
	  case TROLL:
	    ifblind(i,j);  // side effect
	    hitm(i,j,200); 
	    break; 
	  }
      } // end for j
    // so, like, redraw the screen too.
    recalc_screen();
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh();
    break; // end of vaporize

    /* ----- LEVEL 4 SPELLS ----- */

  case 21:
    // dehydration
    direct(x,100+clev,"The %s shrivels up",0); 
    return;

  case 22:
    //	lightning
    // xxx this made my visor lock up, earlier
    godirect(x, rnd(25)+20+(clev<<1),
	     //	       "A lightning bolt hits the %s", 1, '~', env);
	       "A lightning bolt hits the %s", 10, '~');
    return;

  case 23:	
    // drain life
    i=min(c[HP]-1,c[HPMAX]/2);
    direct(x, i+i,
	   "a life-sapping force surrounds you, like a really bad sitcom",
	   0);
    c[HP] -= i;  	
    return;

  case 24:	
    // globe of invulnerability
    if (c[GLOBE]==0) c[MOREDEFENSES] += 10;
    c[GLOBE] += 200;  
    c[INTELLIGENCE] = max(3,c[INTELLIGENCE]-1);
    message("you feel nigh-invulnerable!");
    break;

  case 25:	
    // flood
    omnidirect(x,32+clev,"The %s struggles for air in your flood!");
    break;

  case 26:
    // finger of death
    if (rnd(151)==63) { 
      message("\nYour heart stopped!\n"); 
      nap(4000);
      env->cdude[HP] = -1;// just to be sure
      died(270, true);
      return;
    } else if (c[WISDOM] > rnd(10)+10) {
      direct(x,2000,"The %s's heart stopped",0); 
      return;
    } else
      message("It didn't work"); 
    break;

    /* ----- LEVEL 5 SPELLS ----- */

  case 27:	
    // scare monster
    c[SCAREMONST] += rnd(10)+clev;
    // if you have HANDofFEAR it lasts longer
    if (has_object(OHANDofFEAR)) {
      c[SCAREMONST] *= 3;
    }
    message("you feel intimidating");
    break;

  case 28:
    // hold monster
    c[HOLDMONST] += rnd(10)+clev;
    message("you feel riveting");
    break;

  case 29:	
    // time stop
    c[TIMESTOP] += rnd(20)+(clev<<1);  
    message("\"time stand still...\"");
    return;

  case 30:	
    // teleport monster away
    if (isconfuse())
      break;
    task = DIR_TDIRECT;
    draw_cast_slices();
    return;

  case 31:	
    // magic fire
    omnidirect(x,35+rnd(10)+clev,"The %s cringes from the flame"); 
    break;

    /* ----- LEVEL 6 SPELLS ----- */

  case 32:
    // make wall
    if (isconfuse())
      break;
    task = DIR_MAKEWALL;
    draw_cast_slices();
    return;

  case 33:	
    // sphere of annihilation!
    if (rnd(23)==5) {
      //      message("You have been enveloped by the zone of nothingness!\n");
      message("You have been annihilated!\n");
      nap(1000);
      env->cdude[HP] = -1;// just to be sure
      died(258, true);  // ow
      return;
    }
    c[INTELLIGENCE] = max(3,c[INTELLIGENCE]-1);
    task = DIR_ANNIHIL;
    draw_cast_slices();
    return;

  case 34:	
    // genocide!
    //     genmonst();  // Go to genocide-monster form.
    cast_form_use = CAST_GENOCIDE;
    FrmPopupForm(CastForm);
    env->spelknow[34] = false;
    c[INTELLIGENCE] = max(3,c[INTELLIGENCE]-1);
    return;

  case 35:
    // summon demon
    if (rnd(100) > 30) { 
      direct(x-1,150,"The demon strikes at the %s",0);  
      return;
    } else if (rnd(100) > 15) { 
      message("Nothing seems to have happened");  
    } else {
      message("The demon clawed you and vanished!"); 
      i=rnd(40)+30;  
      env->lastnum=277;
      losehp(i); /* must say killed by a demon */
    };
    break;

  case 36:
    // walk through walls
    c[WTW] += rnd(10)+5;	
    message("you feel phase-modulated");
    break;

  case 37:
    // alter reality
    {
      struct isave *save;
      /* pointer to item save structure */
      Short sc = 0;  /* # items saved */
      save = (struct isave *) md_malloc(sizeof(struct isave)*MAXX*MAXY*2);

      for (j = 0 ; j < MAXY ; j++) // save all items and monsters
	for (i = 0 ; i < MAXX ; i++) {
	  xl = env->item[i][j];
	  if (xl && xl != OWALL && xl != OANNIHILATION) {
	    save[sc].is_mon = false;  
	    save[sc].id = env->item[i][j];
	    save[sc].arg = env->iarg[i][j];
	    sc += 1;
	  }
	  if (env->mitem[i][j]) {
	    save[sc].is_mon = true;  
	    save[sc].id = env->mitem[i][j];
	    save[sc].arg = env->hitp[i][j];
	    sc += 1;
	  }
	  env->item[i][j] = OWALL;   
	  env->mitem[i][j] = 0;
	  env->know[i][j] = 0;
	} // done saving items and monsters
      // redo the passages
      eat(1,1);	
      if (env->level == 1)   env->item[33][MAXY-1] = 0; // entrance
      j = rnd(MAXY-2); // make straight the way
      for (i = 1 ; i < MAXX-1 ; i++)
	env->item[i][j] = 0;
      // now put the items and monsters back
      for ( ; sc>0 ; sc--) {
	if (!save[sc].is_mon) {  // it's an object
	  Short trys;
	  // this for statement looks for env->item[i][j]==0
	  for (trys = 100, i = j = 1 ;
	       trys > 0 && env->item[i][j] ;
	       i = rnd(MAXX-1), j = rnd(MAXY-1), trys--)  ; 
	  if (trys) { 
	    env->item[i][j] = save[sc].id; 
	    env->iarg[i][j] = save[sc].arg; 
	  } // otherwise we're out of space, too bad for you
	} else {  // it's a monster
	  Short trys;
	  // this for statement looks for !(item[i][j]==OWALL || mitem[i][j])
	  for (trys = 100, i = j = 1 ;
	       trys > 0 && (env->item[i][j] == OWALL || env->mitem[i][j]) ;
	       i = rnd(MAXX-1), j = rnd(MAXY-1), trys--)   ;
	  if (trys) { 
	    env->mitem[i][j] = save[sc].id; 
	    env->hitp[i][j] = save[sc].arg; 
	  }
	}
      } // done replacing items and monsters
      c[INTELLIGENCE] = max(3,c[INTELLIGENCE]-1);
      env->spelknow[37] = false; // you forget this spell
      // xxxxx need to free 'save', need to position the player.
      //      free_me(save);
      //      positionplayer(); // hm need to pick random px, py
    }
    message("yow!");
    // so, like, redraw the screen too.
    recalc_screen();
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh();
    break;

  case 38:
    // permanence
    adjtime(-99999L, true); // you go BACK in time, gtime stays the SAME
    env->spelknow[38] = false; // you forget this spell
    c[INTELLIGENCE] = max(3,c[INTELLIGENCE]-1);
    message("that ought to do it...");
    break;

  default:	
    message("that spell is not available!");
    return;
  }

  start_of_Play_Level_loop();
}


/*
 * dir_task_dispatch is called by the handler for the form
 * that asks you what direction to cast the spell in.
 * it takes care of dispatching the spell you wanted to cast.
 */
extern Short diroffx[];
extern Short diroffy[];
void dir_task_dispatch(Short click_dir)
{
  Short dx, dy;
  Short x, y;
  dx = diroffx[click_dir];
  dy = diroffy[click_dir];
  x = env->playerx + dx;
  y = env->playery + dy;
  // dispatch by TASK
  switch(task) {
  case DIR_MAKEWALL:
    makewall(x, y);
    break;
  case DIR_TDIRECT:
    tdirect(x, y);
    break;
  case DIR_POLY:
    dirpoly(x, y);
    break;
  case DIR_DIRECT:
    direct_helper(x, y);
    break;
  case DIR_GODIRECT:
    godirect_helper(dx, dy);
    break;
  case DIR_ANNIHIL:
    newsphere(x, y, click_dir, rnd(20)+11, -1);
    // (location direction lifetime; where to store)
    break;
  }
  if (env->cdude[HP] <= 0) return; // dude, you are dead
  start_of_Play_Level_loop();
}
#undef DIR_MAKEWALL
#undef DIR_TDIRECT 
#undef DIR_POLY    
#undef DIR_DIRECT  
#undef DIR_GODIRECT
