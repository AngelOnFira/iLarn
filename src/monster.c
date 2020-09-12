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
#include "iLarnRsc.h"


Char lastmonst[40];  // I MUST move everything that uses it; or, it to env.
extern Boolean keep_fighting;
extern Short fight_monster_i;
extern Short fight_monster_j;
//extern const struct monst monster[];
// These I found in display.c.. used there, monster.c, and movem.c.
const Short diroffx2[] = { 0,  0, 1,  0, -1,  1, -1, 1, -1 };
const Short diroffy2[] = { 0,  1, 0, -1,  0, -1, -1, 1,  1 };

// nobjtab is used only by newobject.
const Char nobjtab[38] = { 
  0, OSCROLL, OSCROLL, OSCROLL, OSCROLL, OPOTION,
  OPOTION, OPOTION, OPOTION, OGOLDPILE, OGOLDPILE, OGOLDPILE,
  OGOLDPILE, OBOOK, OBOOK, OBOOK, OBOOK, ODAGGER,
  ODAGGER, ODAGGER, OLEATHER, OLEATHER, OLEATHER, OREGENRING,
  OPROTRING, OENERGYRING, ODEXRING, OSTRRING, OSPEAR, OBELT,
  ORING, OSTUDLEATHER, OSHIELD, OFLAIL, OCHAIN, O2SWORD,
  OPLATE, OLONGSWORD
};

// these little things are used by spattack
#define ARMORTYPES 6
const Char rustarm[ARMORTYPES][2] = { 
  {OSTUDLEATHER,-2},
  {ORING,-4},
  {OCHAIN,-5},
  {OSPLINT,-6},
  {OPLATE,-8},
  {OPLATEARMOR,-9}
};
const Char spsel[10] = { 
  1, 2, 3, 5, 6, 8, 9, 11, 13, 14
};


static void dropsomething(Short monst) SEC_5;
static Boolean emptyhanded() SEC_5;
static Boolean stealsomething() SEC_5;
static Boolean spattack(Short attack, Short xx, Short yy) SEC_5;


// Formerly in data.c.


// So, like, maybe this should be a DATABASE RECORD.
// That reminds me - am I remembering to un-genocide things when you die? xxx

const struct monst monster[66] = {
/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HP	EXP
	--------------------------------------------------------- */
{ "", 0,	0,	0,	0,	0,   1,   0,	0,	0}, // 0
{ "lemming", 				   
	1,	3,	0,	0,	0,   3,   0,	0,	1},
{ "gnome", 				   
	1,	10,	1,	0,	0,   8,   30,   2,	2},
{ "hobgoblin", 				   
	1,	13,	2,	0,	0,   5,   25,	3,	2},
{ "jackal", 				   
	1,	7,	1,	0,	0,   4,   0,	1,	1},
{ "kobold",				   
	1,	15,	1,	0,	0,   7,  10,	1,	1},
{ "orc", 				   
	2,	15,	3,	0,	0,   9,  40,	5,	2},
{ "snake",				   
	2,	10,	1,	0,	0,   3,   0,	3,	1},
{ "giant centipede",			   
	2,	13,	1,	4,	0,   3,   0,	2,	2},
{ "jaculi",				   
	2,	9,	1,	0,	0,   3,   0,	2,	1} ,
{ "troglodyte",				   
       2,	10,	2,	0,	0,   5,  80,	5,	3} , // 10
{ "giant ant",				   
	2,	8,	1,	4,	0,   4,   0,	5,	4} ,

/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HP	EXP
	--------------------------------------------------------- */
{ "floating eye",			   
	3,	8,	2,	0,	0,   3,   0,	 7,	 2},
{ "leprechaun",				   
	3,	3,	0,	8,	0,   3,1500,    15,	40},
{ "nymph",				   
	3,	3,	0,	14,	0,   9,   0,	20,	40},
{ "quasit",				   
	3,	5,	3,	0,	0,   3,   0,	14,	10},
{ "rust monster",			   
	3,	5,	0,	1,	0,   3,   0,	18,	20} ,
{ "zombie",				   
	3,	12,	3,	0,	0,   3,   0,	 9,	 7} ,
{ "assassin bug",			   
	4,	4,	3,	0,	0,   3,   0,	23,	13} ,
{ "bitbug",				   
	4,	5,	4,	15,	0,   5,  40,	24,	33} ,
{ "hell hound",				   
	4,	5,	2,	2,	0,   6,   0,	20,	33} , // 20
{ "ice lizard",				   
	4,	11,	3,	10,	0,   6,  50,	19,	23} ,
{ "centaur",				   
	4,	6,	4,	0,	0,  10,  40,	25,	43} ,
					   
/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HP	EXP
	--------------------------------------------------------- */
{ "troll",				   
	5,	9,	5,	0,	0,   9,  80,	55,	250} ,
{ "yeti",				   
	5,	8,	4,	0,	0,   5,  50, 	45,	90} ,
{ "white dragon",			   
	5,	4,	5,	5,	0,  16, 500,	65,	1000} ,
{ "elf",				   
	5,	3,	3,	0,	0,  15,  50,	25,	33} ,
{ "gelatinous cube",			   
	5,	9,	3,	0,	0,   3,   0,	24,	43} ,
{ "metamorph",				   
	6,	9,	3,	0,	0,   3,  0,	32,	40} , 
{ "vortex",				   
	6,	5,	4,	0,	0,   3,  0,	33,	53} ,
{ "ziller",				   
	6,	15,	3,	0,	0,   3,  0,	34,	33} , // 30
{ "violet fungi",			   
	6,	12,	3,	0,	0,   3,  0,	 39,	90} ,
{ "wraith",				   
	6,	3,	1,	6,	0,   3,  0,	36,	300} ,
{ "forvalaka",				   
	6,	3,	5,	0,	0,   7,  0,	55,	270} ,

/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HP	EXP
	--------------------------------------------------------- */
{ "lama nobe", 				   
	7,	14,	7,	0,	0,   6,  0,	36,	70} ,
{ "osequip", 				   
	7,	4,	7,	16,	0,   4,  0,	36,	90} ,
{ "rothe", 				   
	7,	15,	5,	0,	0,   3,  100,	53,	230} ,
{ "xorn", 				   
	7,	6,	7,	0,	0,  13,  0,	63,	290} ,
{ "vampire", 				   
	7,	5,	4,	6,	0,  17,  0,	55,	950} ,
{ "invisible stalker", 			   
	7,	5,	6,	0,	0,  5,  0,	55,	330} ,
{ "poltergeist",			   
	8,	1,	8,	0,	0,   3,  0,	55,	430} , // 40
{ "disenchantress",			   
	8,	3,	1,	9,	0,   3,  0,	57,	500} ,
{ "shambling mound", 			   
	8,	13,	5,	0,	0,   6,  0,	47,	390} ,
{ "yellow mold",			   
	8,	12,	4,	0,	0,   3,  0,	37,	240} ,
{ "Umber Hulk",				   
	8,	6,	7,	11,	0,  14,  0,	67,	600} ,
					   
/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HPEXP
	--------------------------------------------------------- */
{ "gnome king",				   
	9,	-1,	10,	0,	0,  18,  2000,	120,3000} ,
{ "mimic",				   
	9,	 9,	7,	0,	0,   8,  0,	57,	100} ,
{ "water lord",				   
	9, 	-10,	15,	7,	0,  20,  0,	155,15000} ,
{ "bronze dragon",			   
	9,	 5,	9,	3,	0,  16,  300,	90, 4000} ,
{ "green dragon",			   
	9,	 4,	4,	10,	0,  15,  200,	80, 2500} ,
{ "purple worm",			   
	9,	-1,	13,	0,	0,   3,  100,	130,15000} , // 50
{ "xvart",				   
	9,	-2,	14,	0,	0,  13,  0,	100,	1000} ,
{ "spirit naga",			   
	10, 	-20,	15,	12,	0,  23,  0,	100, 20000} ,
{ "silver dragon",			   
	10,	-4,	10,	3,	0,  20,  700,	110,10000} ,
{ "platinum dragon", 			   
	10,	-7,	15,	13,	0,  22,  1000,	150,25000} ,
{ "green urchin",			   
	10,	-5,	12,	0,	0,   3,  0,	95, 5000} ,
{ "red dragon",				   
	10,	-4,	13,	3,	0,  19,  800,	120,14000} ,

/*NAME	LV	AC	DAM	ATT	DEF INT GOLD	HP	EXP
	------------------------------------------------------------- */
{ "type I demon lord",			   
	12,    -40,	20,	3,	 0, 20,  0,	150,50000} ,
{ "type II demon lord",			   
	13,    -45,	25,	5,	 0, 22,	0,	200,75000} ,
{ "type III demon lord", 		   
	14,    -50,	30,	9,	 0, 24,	0,	250,100000} ,
{ "type IV demon lord",			   
	15,    -55,	35,	11,	 0, 26,	0,	300,125000} , // 60
{ "type V demon lord",			   
	16,    -60,	40,	13,	 0, 28,	0,	350,150000} ,
{ "type VI demon lord",			   
	17,    -65,	45,	13,	 0, 30,	0,	400,175000} ,
{ "type VII demon lord", 		   
	18,    -70,	50,	6,	 0, 32,	0,	450,200000} ,
{ "demon prince", 			   
	19,   -100,	80,	6,	 0, 40,	0,	1000,500000} ,
{ "Prince of Darkness",
	20,   -127,	127,	6,	 0, 100, 0,	32767,1000000} // 65
// previously God of Hellfire ... PoD sounds classier though
};

Short m_hitpoints(Short i) {
  return monster[i].hitpoints;
}
const Char *m_name(Short i) {
  return monster[i].name;
}
Short m_intelligence(Short i) {
  return monster[i].intelligence;
}



// End formerly in data.c.


/* 
 * Restore all monsters to full hit points
 */
void heal_monsters()
{
  Short i, j;
  for (i=0; i<MAXY; i++)  
    for (j=0; j<MAXX; j++)
      if (env->mitem[j][i])
	env->hitp[j][i] = m_hitpoints(env->mitem[j][i]);
}

/*
 *  Function to check location for emptiness (no item or monster there)
 *   i.e. returns true if 'empty' in that sense
 *	Enter with itm or monst TRUE or FALSE if checking it
 *	Example:  if itm==TRUE check for no item at this location
 *			  if monst==TRUE check for no monster at this location
 *  This routine will return FALSE if at a wall or the dungeon exit on level 1
 */
Boolean cgood(Short x, Short y, Short itm, Short monst)
{
  if ((y>=0) && (y<=MAXY-1) 
      && (x>=0) && (x<=MAXX-1))                         /* within bounds? */
    if (env->item[x][y]!=OWALL)	                   /* can't make on walls */
      if (itm==0 || (env->item[x][y]==0))         /* is it free of items? */
	if (monst==0 || (env->mitem[x][y]==0))  /*is it free of monsters? */
	  if ((env->level!=1) || (x!=33) || (y!=MAXY-1))  /* not the exit */
	    return true;
  return false;
}

/*
 *		Function to create a monster next to the player
 *	Enter with the monster number (1 to MAXMONST+8)
 */
void createmonster(Short mon)
{
  Short x, y, k, i;
  //  if (mon < 1 || mon > MAXMONST+8)
  if (mon < LEMMING || mon > LUCIFER)
    return;
  while (env->genocided[mon] && mon < MAXMONST) 
    mon++; /* genocided? */
  /* choose dir, then try all */
  for (k = rnd(8), i = -8 ; i < 0 ; i++, k++) {
    if (k > 8) k = 1;		/* wraparound the diroff arrays */
    x = env->playerx + diroffx2[k];		
    y = env->playery + diroffy2[k];
    /* if we can create here */
    if (cgood(x,y,0,1)) {
      env->mitem[x][y] = mon;
      env->hitp[x][y] = m_hitpoints(mon);
      env->stealth[x][y] = 0;
      env->know[x][y] = 0;
      switch(mon) {
      case ROTHE: 
      case POLTERGEIST: 
      case VAMPIRE:    
	env->stealth[x][y] = 1;
      }
      return;
    }
  }
}

/*
 * 	Routine to place an item next to the player
 *	Enter with the item number and its argument (iven[], ivenarg[])
 *	Returns no value, thus we don't know about createitem() failures.
 */
void createitem(Short it, Long arg)
{
  Short x, y, k, i;
  
  if (it >= MAXOBJ) 
    return;	/* no such object */
  
 /* choose direction, try all */
  for (k = rnd(8), i= -8 ; i < 0 ; i++, k++) {
    if (k > 8) k = 1;		/* wraparound the diroff arrays */
    x = env->playerx + diroffx2[k];
    y = env->playery + diroffy2[k];
    if ( (it != OMAXGOLD && it != OGOLDPILE) ) {
      // see if we can create a non-gold item here
      if (cgood(x, y, 1, 0)) {
	env->item[x][y] = it;  
	env->know[x][y] = 0;  
	env->iarg[x][y] = arg;  
	return; // success!
      }
    } else {
      // 'it' is omaxgold or ogoldpile; can combine if gold present
      switch (env->item[x][y]) {
      case OGOLDPILE :
	if ( (env->iarg[x][y] + arg) < 32767) {
	  env->iarg[x][y] += arg;
	  return;
	}
      case ODGOLD :
	if ( (10L * env->iarg[x][y] + arg) < 327670L) {
	  i = env->iarg[x][y] ;
	  env->iarg[x][y] = (10L * i + arg) / 10;
	  env->item[x][y] = ODGOLD;
	  return;
	}
      case OMAXGOLD :
	if ( (100L * env->iarg[x][y] + arg) < 3276700L) {
	  i = ((100L * env->iarg[x][y]) + arg)/100;
	  env->iarg[x][y] = i;
	  env->item[x][y] = OMAXGOLD;
	  return;
	}
      case OKGOLD :
	if ( (1000L * env->iarg[x][y] + arg) < 32767000L) { 
	  i = env->iarg[x][y];
	  env->iarg[x][y] = (1000L * i + arg) / 1000;
	  env->item[x][y] = OKGOLD;
	  return;
	}
	else env->iarg[x][y] = 32767000L;   // XXX danger will robinson
	return;
      default :
	// no gold present yet, behave normally
	if (cgood(x, y, 1, 0)) {
	  env->item[x][y] = it;  
	  env->know[x][y] = 0;  
	  if (it == OMAXGOLD)
	    env->iarg[x][y] = arg / 100;
	  else env->iarg[x][y] = arg;
	  return;
	}
      } /* end switch */
    } /* end else */
  } /* end for */
}

/*
 *	Function to return hp damage to monster due to a number of full hits
 *        (then we'll compute a random fraction and damage it that much.)
 *	Enter with the number of full hits being done
 */
Short fullhit(Short xx)
{
  Short i, tmp;

  if (xx < 0 || xx > 20) return(0);	/* fullhits are out of range */
  if (env->cdude[LANCEDEATH]) return(10000);	/* lance of death */

  tmp = (env->cdude[WCLASS] >>1) + env->cdude[STRENGTH] + env->cdude[STREXTRA];
  //  i = xx * (tmp - env->cdude[HARDGAME] - 12 + env->cdude[MOREDAM]); 
  // Make the compiler happier:
  i = xx * (tmp - (Short) env->cdude[HARDGAME] - 12 + (Short) env->cdude[MOREDAM]); 

  return( (i >= 1) ? i : xx );
}

/*
 *     I have to say, this is a wrongly named function.
 *
 *	Subroutine to copy the word "monster" into lastmonst if the player is 
 * 	blind, otherwise the monster name.
 *      Enter with the coordinates (x,y) of the monster
 */
void ifblind(Short x, Short y)
{
  const Char *p;
  vxy(&x,&y);	/* verify correct x,y coordinates */
  if (env->cdude[BLINDCOUNT]) { 
    env->lastnum = 279;
    p = "monster"; 
  }
  else { 
    env->lastnum = env->mitem[x][y];  
    p = m_name(env->lastnum);
  }
  StrCopy(lastmonst, p);
}

/*
 *	Function to verify x & y are within the bounds for a level
 *	If *x or *y is not within the absolute bounds for a level, FIX them
 *	 so that they are on the level.
 *	Returns TRUE if it was out of bounds, and the *x & *y in the calling
 *	routine have been altered.
 */
Boolean vxy(Short *x, Short *y)
{
  Short flag = 0;
  if (*x < 0) { 
    *x = 0; 
    flag++; 
  }
  if (*y < 0) { 
    *y = 0; 
    flag++; 
  }
  if (*x >= MAXX) { 
    *x = MAXX-1; 
    flag++; 
  }
  if (*y >= MAXY) { 
    *y = MAXY-1; 
    flag++; 
  }
  return(flag > 0);
}

/*
 *	This routine is used for a bash & slash type attack on a monster
 *	Enter with the coordinates of the monster in (x,y).
 */
void hitmonster(Short x, Short y, Boolean silent)
{
  Short tmp, monst, flag, wield, damag=0;
  Char buf[80];
  
  if (env->cdude[TIMESTOP])  return;  // not if time stopped
  
  vxy(&x,&y);	// verify coordinates are within range
  
  if ((monst = env->mitem[x][y]) == 0) return;
  
  //  hit3flag = 1;   I don't think I need that
  ifblind(x, y);
  
  tmp = monster[monst].armorclass + env->cdude[LEVEL] + env->cdude[DEXTERITY];
  tmp += env->cdude[WCLASS]/4 - 12 - env->cdude[HARDGAME];
  
  // need at least random chance to hit
  if ((rnd(20) < tmp) || (rnd(71) < 5)) { 	
    StrPrintF(buf, "You hit the %s", lastmonst);
    flag = 1;
    damag = fullhit(1);  
    if (damag < 9999) damag = rnd(damag) + 1;
  } else { 	
    StrPrintF(buf, "You missed the %s", lastmonst);
    flag = 0;
  }
  if (!silent)
    message(buf);
  buf[0] = '\0';

  wield = env->cdude[WIELD];
  if (flag)
    if (wield > 0)
      if (env->iven[wield] != OSWORDofSLASHING) // and it could be dulled
	if ((monst==RUSTMONSTER) || (monst==DISENCHANTRESS) || (monst==CUBE))
	  if (env->ivenarg[wield] > -10) {
	    StrPrintF(buf, "Your weapon is dulled by the %s", lastmonst); 
	    // do_feep(800, 80); // good pitch, maybe a tad short in duration.
	    if (!silent)
	      message(buf);
	    env->ivenarg[wield] -= 1;
	    recalc();
	    //	    print_stats();
	  }
  if (flag) {
    hitm(x,y,damag); // this only prints "The %s died" so ignore 'silent'
    if ((monst >= DEMONLORD) && (env->cdude[LANCEDEATH]) && (env->hitp[x][y])){
      StrPrintF(buf, "Your lance of death tickles the %s!", lastmonst);
      if (!silent)
	message(buf);
    }
  }

  if (monst == METAMORPH)
    if (env->hitp[x][y] < 25 && env->hitp[x][y] > 0)
      env->mitem[x][y] = BRONZEDRAGON + rund(9);

  // HERE is where to tweak numbers if lemmings are too irritating.  
  if (env->mitem[x][y] == LEMMING)
    //    if (rnd(100) <= 40) {
    if (rnd(100) <= 10) {
      createmonster(LEMMING);
      showcell(env->playerx, env->playery);
      refresh();
    }
}
/*
 *		Function to just hit a monster at a given coordinates
 *	Returns the number of hitpoints the monster absorbed
 * This routine is used to specifically damage a monster at a location (x,y)
 *	Called by hitmonster(x,y)
 */
Short hitm(Short x, Short y, Short amt)
{
  Short monst;
  Short hpoints, amt2;
  Char buf[40];
  
  vxy(&x,&y);	// verify coordinates are within range 
  amt2 = amt;	// save initial damage so we can return it 
  monst = env->mitem[x][y];
  
  // if you are under a half damage curse, adjust damage points 
  if (env->cdude[HALFDAM]) {
    amt >>= 1;
  }
  if (amt <= 0) {
    amt2 = amt = 1;
  }
  
  env->lasthx = x;
  env->lasthy = y;
  
  // hitting a monster breaks stealth and hold-monster.
  env->stealth[x][y] = true;	
  env->cdude[HOLDMONST] = 0;
  // do extra damage to dragons if you have an orb of dragon slaying
  switch(monst) {
  case WHITEDRAGON:
  case REDDRAGON:
  case GREENDRAGON:
  case BRONZEDRAGON:
  case PLATINUMDRAGON:
  case SILVERDRAGON:
    if (env->cdude[SLAYING])
      amt *= 3;
    break;
  }
  // invincible monster fix is here
  // So I see.
  if (env->hitp[x][y] > monster[monst].hitpoints)
    env->hitp[x][y] = monster[monst].hitpoints;

  if (monst >= DEMONLORD) {
    if (env->cdude[LANCEDEATH]) 
      amt = 300;
    if ((env->cdude[WIELD] != -1) &&
	(env->iven[env->cdude[WIELD]] == OSLAYER))
      amt = 10000;
  }

  hpoints = env->hitp[x][y];
  if (hpoints <= amt) {
    keep_fighting = false; // notify fight() in case we're in it
    StrPrintF(buf, "The %s died!", lastmonst);
    message(buf);
    raiseexperience( (Long) monster[monst].experience);
    amt = monster[monst].gold;  
    if (amt > 0)
      dropgold(rnd(amt) + amt);
    dropsomething(monst);
    disappear(x, y);	
    env->hitp[x][y] = 0;
    env->know[x][y] = 1; // at least *I* think so
    update_screen_cell(x, y);
    recalc_screen(); // I think I need this
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh(); // I think I need this
    return hpoints;
  }
  env->hitp[x][y] = hpoints - amt;
  return amt2;
}
// This is needed for a spell
void clobberm(Short monst)
{
  raiseexperience( (Long) monster[monst].experience);
}

/*
 *	Function for the monster at location x,y to hit the player
 */
void hitplayer(Short x, Short y)
{
  Short dam, tmp, thismonster, bias;
  Char buf[80];
  Boolean silent = false;

  vxy(&x,&y);	// verify coordinates are within range

  thismonster = env->mitem[x][y];
  env->lastnum = thismonster;

  if (keep_fighting && x == fight_monster_i && y == fight_monster_j)
    silent = true;
  else if (thismonster != LEMMING) // (ignore lemmings, who do no damage)
    keep_fighting = false; // we're in a fight and got hit by ANOTHER monster

  if ((env->know[x][y] & 1) == 0) {
    // (know is 0 or 2)
    env->know[x][y] = 1; 
    update_screen_cell(x, y);
  }

  bias = (env->cdude[HARDGAME]) + 1;
  //  hitflag = hit2flag = hit3flag = 1;  // unneeded now?
  //  env->hitflag = true; // "hello, you've been hit."
  //  yrepcount = 0;
  ifblind(x, y);

  if (thismonster < DEMONLORD)
    if ( (env->cdude[INVISIBILITY]) && (rnd(33)<20) ) {
      StrPrintF(buf, "The %s misses wildly", lastmonst);
      if (!silent)
	message(buf);
      return;
    }

  if ( (thismonster < DEMONLORD) &&
       (thismonster != PLATINUMDRAGON) &&
       (env->cdude[CHARMCOUNT]) ) {
    if (rnd(30) + 5 * monster[thismonster].level - env->cdude[CHARISMA] < 30) {
      StrPrintF(buf, "The %s is awestruck at your magnificence!", lastmonst);
      if (!silent)
	message(buf);
      return;
    }
  }

  if (thismonster==LEMMING)
    return;
  else {
    dam = monster[thismonster].damage;
    dam += rnd( ( (dam<1) ? 1 : dam) ) + monster[thismonster].level;
  }
  // demon-dude damage is reduced if wielding Slayer
  if ( (thismonster >= DEMONLORD) &&
       (env->cdude[WIELD] != -1) &&
       (env->iven[env->cdude[WIELD]]==OSLAYER) )
    //    dam = 1 - (0.1 * rnd(5)) * dam; // hello???  well, i'll make a guess:
    dam -= (dam * (10 - (rnd(5)))) / 10; // reduce to 10%-50% of original
	
  // spirit naga and poltergeist damage is halved by scarab of negate spirit
  if (env->cdude[NEGATESPIRIT] || env->cdude[SPIRITPRO])  
    if ((thismonster == POLTERGEIST) || (thismonster == SPIRITNAGA)) 
      dam = dam/2; 
  
  // halved if undead and cube of undead control
  if (env->cdude[CUBEofUNDEAD] || env->cdude[UNDEADPRO])
    if ((thismonster == VAMPIRE) ||
	(thismonster ==WRAITH) ||
	(thismonster ==ZOMBIE))
      dam = dam/2;
  
  tmp = 0;
  if (monster[thismonster].attack > 0)
    // decide whether to perform special-attack
    if ( ((dam + bias + 8) > env->cdude[AC]) ||
	 (1 == rnd( (env->cdude[AC]>0) ? env->cdude[AC] : 1)) ) {
      // heck, let's say that a special attack always interrupts a fight:
      if (keep_fighting && x == fight_monster_i && y == fight_monster_j)
	keep_fighting = false;
      if (spattack(monster[thismonster].attack, x, y)) {
	// The monster vanished.  Poof.  Skip the rest of this function.
	return;
      }
      tmp = 1;  
      bias -= 2; 
      // Hey, the monster gets a chance to HIT you too?
      // (in addition to special attack?)
    }
	
  if ( ((dam + bias) > env->cdude[AC]) ||
       (1 == rnd( (env->cdude[AC]>0) ? env->cdude[AC] : 1)) ) {
    StrPrintF(buf, "The %s hit you", lastmonst);	
    if (!silent)
      message(buf);
    tmp = 1;
    dam -= env->cdude[AC];
    if (dam < 0) dam = 0;
	  
    if (dam > 0) { 
      losehp(dam); 
      print_stats();
    }
  }
	
  if (tmp == 0) {
    StrPrintF(buf, "The %s missed", lastmonst);	
    if (!silent)
      message(buf);
  }
}


/*
 *	Function to create an object near the player when 
 *	certain monsters are killed
 *	Enter with the monster number 
 */
static void dropsomething(Short monst)
{
  switch(monst) {
  case ORC:  
  case NYMPH:
  case ELF:
  case TROGLODYTE:
  case TROLL:
  case ROTHE:
  case VIOLETFUNGI:
  case PLATINUMDRAGON:
  case GNOMEKING:
  case REDDRAGON:
    something(env->level);
    break;
  case LEPRECHAUN: 
    do {
      if (rnd(101) >= 75)
	creategem();
    } while (rnd(5) == 1);
    break;
  case LEMMING: // I found it like this.
    /*		createitem(OGOLDPILE,1);
     */		break;
  }
}

/*
 *	Function to drop some gold around player
 *	Enter with the number of gold pieces to drop
 */
void dropgold(Short amount)
{
  if (amount > 250)
    createitem(OMAXGOLD, (Long) amount);
  else
    createitem(OGOLDPILE,(Long) amount);
}

/*
 *	Function to create an item from a designed probability around player
 *	Enter with the cave level on which something is to be dropped
 *	Returns nothing of value.
 */
void something(Short lev)
{
  Short j;
  Short i;

  if (lev<0 || lev>MAXLEVEL+MAXVLEVEL) 
    return;	/* you're on an imaginary level? */
  do {
    j = newobject(lev, &i);
    createitem(j, (Long)i);
  } while (rnd(101) < 8);  /* possibly more than one item */
}

/*
 *	Routine to return a randomly selected object to be created
 *	Returns the object number created, and sets *i for its argument
 *	Enter with the cave level and a pointer to the items arg
 */
// used in monster.c and once in create.c
Short newobject(Short lev, Short *i)
{
  Short j;
  Short tmp = 32;

  if (env->level < 0 || env->level > MAXLEVEL+MAXVLEVEL) 
    return(0);	/* you're on an imaginary level? */

  if (lev > 6)      tmp = 37; 
  else if (lev > 4) tmp = 35; 
  // else tmp = 32.

  tmp = rnd(tmp);
  j = nobjtab[tmp];	/* the object type */
  switch(tmp) {
  case 1: 
  case 2: 
  case 3: 
  case 4:	
    *i=newscroll();	
    break;
  case 5: 
  case 6: 
  case 7: 
  case 8:	
    *i=newpotion();	
    break;
  case 9: 
  case 10: 
  case 11: 
  case 12: 
    *i = rnd( (lev+1)*10 ) + lev*10 + 10; // basically 10(lev+1) to 20(lev+1)
    break;
  case 13: 
  case 14: 
  case 15:
  case 16:
    *i=lev;	
    break;
  case 17: 
  case 18: 
  case 19: 
    *i=newdagger();
    if (*i==0) return(0);  
    break;
  case 20: 
  case 21: 
  case 22: 
    *i=newleather(env->cdude[HARDGAME]);
    if (*i==0) return(0);  
    break;
  case 23: 
  case 32: 
  case 35: 
    *i=rund(lev/3+1); 
    break;
  case 24: 
  case 26: 
    *i=rnd(lev/4+1);   
    break;
  case 25: 
    *i=rund(lev/4+1); 
    break;
  case 27: 
    *i=rnd(lev/2+1);   
    break;
  case 28: 
    *i=rund(lev/3+1); 
    if (*i==0) return(0); 
    break;
  case 29: 
  case 31: 
    *i=rund(lev/2+1); 
    if (*i==0) return(0); 
    break;
  case 30: 
  case 33: 
    *i=rund(lev/2+1);   
    break;
  case 34: 
    *i=newchain();   	
    break;
  case 36: 
    *i=newplate(env->cdude[HARDGAME]);
    break;
  case 37: 
    *i=newsword(env->cdude[HARDGAME]);
    break; 
  }
  return(j);
}

/*
 *	Subtract hp from user and update display
 *	Enter with the number of hit points to lose
 *	Note: if x > c[HP] this routine could kill the player!
 */
void checkloss(Short x)
{
  if (x>0) { 
    losehp(x);  
    print_stats(); // noop if hp is <=0
  }
}
/*
 *  return true if player inventory is empty, else false
 */
static Boolean emptyhanded()
{
  Short i;
  Long *c = env->cdude;
  
  for (i=0; i < NINVT; i++)
    if( (env->iven[i]) && (i!=c[WIELD]) && (i!=c[WEAR]) && (i!=c[SHIELD]) )
      return false;
  return true;
}
/*
 *  function to steal an item from the players pockets
 *  returns  true if something was stolen
 * (used only by the special attack thingy)
 */ 
static Boolean stealsomething()
{
  Short i,j;
  Long *c = env->cdude;
  
  j=100;
  while (1) {
    i = rund(NINVT);
    // find something in inventory but not 'in hand'
    if (env->iven[i] && (c[WEAR]!=i) && (c[WIELD]!=i) && (c[SHIELD]!=i)) {
      show3(i); // print item name to splash
      adjustcvalues(env->iven[i], env->ivenarg[i], true);
      env->iven[i] = 0; 
      return(1);
    }
    if (--j <= 0) return(0);
  }
}

/*
 *	Function to process special attacks from monsters
 *	Enter with the special attack number, and the coordinates (xx,yy)
 *		of the monster that is special attacking
 *	Returns 1 if must do a show1cell(xx,yy) upon return, 0 otherwise
// <zephyr>I'M SICK OF YOUR LIES</zephyr>
// Returns 1 if the monster vanished and shouldn't act anymore...
 *
 * atckno   monster     effect
 * ---------------------------------------------------
 *	0	none
 *	1	rust monster		eat armor
 *	2	hell hound		breathe light fire
 *	3	dragon			breathe fire
 *	4	giant centipede		weakening sing
 *	5	white dragon		cold breath
 *	6	wraith			drain level
 *	7	waterlord		water gusher
 *	8	leprechaun		steal gold
 *	9	disenchantress		disenchant weapon or armor
 *	10	ice lizard		hits with barbed tail
 *	11	umber hulk		confusion
 *	12	spirit naga		cast spells taken from special attacks
 *	13	platinum dragon		psionics
 *	14	nymph			steal objects
 *	15	bugbear			bite
 *	16	osequip			bite
 *
 *	char rustarm[ARMORTYPES][2];
 *	special array for maximum rust damage to armor from rustmonster
 *	format is: { armor type , minimum attribute 
 */
// I have monster names in comments - make sure the length of the
// resulting string will fit on the screen, if you change it
static Boolean spattack(Short attack, Short xx, Short yy)
{
  Short i=0, k, m;
  Short thismonster = env->mitem[xx][yy];
  Boolean affected = false;
  Char buf[80];
  //  buf[0] = '\0'; // check this at end to see whether to update status

  if (attack==12) attack = spsel[rund(10)]; // saves me a tail-recurse call!!!

  vxy(&xx,&yy);	// verify x & y coordinates

  // cancel only works 5% of time for demon prince and god
  if (env->cdude[CANCELLATION]) {
    if (thismonster >= DEMONPRINCE) {
      if (rnd(100) >= 95)
	return false;
    } else
      return false;
  }
	
  // staff of power cancels demonlords/wraiths/vampires 75% of time
  if (thismonster != LUCIFER) {
    if ( (thismonster >= DEMONLORD) ||
	 (thismonster == WRAITH) ||
	 (thismonster == VAMPIRE) )
      if ( has_object(OPSTAFF) && (rnd(100) < 75) )
	return false;
  }
	
  // if have cube of undead control,  wraiths and vampires do nothing 
  if ( (thismonster==WRAITH) || (thismonster==VAMPIRE) )
    if ( (env->cdude[CUBEofUNDEAD]) || (env->cdude[UNDEADPRO]) )
      return false;

  affected = false;

  switch(attack) {

  case 1:	// rust your armor, affected=1 when rusting has occurred
    m = k = env->cdude[WEAR];
    i = env->cdude[SHIELD];
    if (i != -1) {
      env->ivenarg[i] -= 1;  // avoid TC bug again...
      if (env->ivenarg[i] < -1) 
	env->ivenarg[i]= -1;  // shield is already at max rust
      else
	affected = true;
    }
    if ( !affected && (k != -1) ) {
      m = env->iven[k];
      for (i = 0 ; i < ARMORTYPES ; i++)
	if (m == rustarm[i][0]) {
	  // find his armor in table: how rusty can *it* get
	  env->ivenarg[k] -= 1;
	  if (env->ivenarg[k] < rustarm[i][1])
	    env->ivenarg[k] = rustarm[i][1];  // armor is already at max rust
	  else affected = true; 
	  break;
	}
    }
    StrPrintF(buf, "The %s hit you...", lastmonst); // "rust monster"
    message(buf);
    if ( !affected ) { // if rusting did not occur
      switch(m) {
      case OLEATHER:	
	message("...You are lucky you have leather on");
	break;
      case OSSPLATE:	
	message("...Lucky you have stainless steel!");
	// p = "You are fortunate to have stainless steel armor!";
	break;
      case OELVENCHAIN:
	message("...Good thing you have elven chain!");
	// p = "You are very lucky to have such strong elven chain!";
	break;
      }
    } else { 
      //      beep(); 
      message("...your armor feels weaker");
      recalc();
      //      print_stats();
    }
    break;

  case 2:		
    i =                rnd(15) +  8 - env->cdude[AC];
    affected = true;
    // FALL THROUGH!
  case 3:
    if (!affected) i = rnd(20) + 25 - env->cdude[AC];
    StrPrintF(buf, "The %s breathes fire!", lastmonst);
    // "hell hound", "{bronze,red,silver} dragon", "type I demon lord"
    message(buf);
    if (env->cdude[FIRERESISTANCE]) {
      StrPrintF(buf, "(you toast some marshmallows)");
      //      StrPrintF(buf, "The %s's flame doesn't faze you!", lastmonst);
      message(buf);
      i = 0; // I think!
    }
    checkloss(i);
    break;

  case 4:	
    StrPrintF(buf, "The %s stung you!", lastmonst);  // "giant {centipede,ant}"
    message(buf);
    if (env->cdude[STRENGTH]>3) {
      message("You feel weaker"); 
      //      beep();
      env->cdude[STRENGTH] -= 1;
      if (env->cdude[STRENGTH] < 3)
	env->cdude[STRENGTH] = 3;
      print_stats();
    }
    break;

  case 5:		
    StrPrintF(buf, "The %s blasts you with his cold breath", lastmonst);
    // "white dragon", "type II demon lord"
    message(buf);
    i = rnd(15)+18-env->cdude[AC];  
    checkloss(i);
    break;

  case 6:
    StrPrintF(buf, "The %s drains you of your life energy!",lastmonst);
    // "wraith",
    // "vampire", "type VII demon lord", "demon prince", "God of Hellfire"
    message(buf);
    loselevel();
    if (thismonster==DEMONPRINCE) losemspells(1);
    if (thismonster==LUCIFER) {
      loselevel();
      losemspells(2);
      for (i = 0 ; i <= 5 ; i++) 
	if (env->cdude[i]-- < 3) env->cdude[i] = 3; // "ouchie"
      print_stats();
    }
    //    beep();  
    break;

  case 7:
    StrPrintF(buf, "The %s got you with a gusher!", lastmonst); // "water lord"
    message(buf);
    i = rnd(15) + 25 - env->cdude[AC];  
    checkloss(i);
    break;

  case 8:	
    if (env->cdude[NOTHEFT]) return false;  // player has a device of no theft
    if (env->cdude[GOLD]) {
      StrPrintF(buf, "The %s hit you...  Your purse feels lighter",
		lastmonst); // "leprechaun"
      message(buf);
      if (env->cdude[GOLD] > 32767)
	env->cdude[GOLD] >>= 1;
      else
	env->cdude[GOLD] -= rnd((Int)(1 + (env->cdude[GOLD] >> 1) ));
      if (env->cdude[GOLD] < 0)  env->cdude[GOLD] = 0;
      print_stats();
    } else {
      StrPrintF(buf, "The %s couldn't find any gold to steal", lastmonst);
      message(buf);
    }
    disappear(xx,yy); 
    //    beep();
    update_screen_cell(xx,yy); // 'cause it disappears
    return true;

  case 9:	
    i = rund(NINVT);  
    m = env->iven[i]; // randomly select item
    if (m > 0 && env->ivenarg[i] > 0 && m!=OSCROLL && m!=OPOTION) {
      env->ivenarg[i] -= 3;
      if (env->ivenarg[i] < 0) env->ivenarg[i] = 0;
      // "disenchantress", "type III demon lord"
      StrPrintF(buf, "The %s hits you with a spell of disenchantment!",
		lastmonst);
      message(buf);
      //	  beep(); 
      recalc();
      //      print_stats();
      show3(i);   // shows item that got toasted
    }		
    break;

  case 10:   
    StrPrintF(buf, "The %s hit you with his barbed tail", lastmonst);
    // "ice lizard", "green dragon"
    message(buf);
    i = rnd(25) - env->cdude[AC];  
    checkloss(i);
    break;

  case 11:
    StrPrintF(buf, "The %s has confused you", lastmonst);
    // "Umber Hulk", "type IV demon lord"
    message(buf);
    //    beep();
    env->cdude[CONFUSE] += 10 + rnd(10);		
    break;

  case 12: // select one of a subset of the special attacks
    //    return spattack(spsel[rund(10)], xx, yy); // "spirit naga"
    // We should never arrive here, 'cause I made it do this ABOVE.  ha.
    break;

  case 13:	
    StrPrintF(buf, "The %s flattens you with his psionics!", lastmonst);
    // "platinum dragon", "type V/VI demon lord"
    message(buf);
    i = rnd(15) + 30 - env->cdude[AC];  
    checkloss(i);
    break;

  case 14:	
    if (env->cdude[NOTHEFT]) return(0); 
    // he has device of no theft
    if (emptyhanded()) {
      StrPrintF(buf, "The %s finds nothing to steal!", lastmonst);
      // "nymph"[spirit naga]
      message(buf);
      break;
    }
    StrPrintF(buf, "From your pack, the %s takes:", lastmonst);
    //    StrPrintF(buf, "The %s picks your pocket and takes", lastmonst);
    message(buf);
    //    beep();
    if (!stealsomething()) // if not nothing, this prints the stolen item
      message("  nothing"); 
    disappear(xx,yy);
    update_screen_cell(xx,yy); // 'cause it disappears
    return true; // 'cause it disappears

  case 15:
    affected = true;
    i =               rnd(10) +  5 - env->cdude[AC];
    // FALL THROUGH!
  case 16:	
    if (!affected) i= rnd(15) + 10 - env->cdude[AC];  
    StrPrintF(buf, "The %s bit you!", lastmonst); // "bitbug", "osequip"
    message(buf);    
    checkloss(i);
    break;

  default:
    // this should not happen
    break;
  } // end switch attack!

  return false;
}

/* these are just a couple of wrappers */
void munge_lastmonst(Char * mname) {
  if (mname)
    StrNCopy(lastmonst, mname, 40);
}
Char * read_lastmonst() {
  return lastmonst;
}


// Initialize the select-from-list form so that it holds monster names.
// e.g. "c) giant centipede"
// NOT in alphabetical order.  Do NOT leave out any monsters even
// if genocided (that will throw off the count when I examine selection.)
// See also:  init_cast_select, init_known_items.
// (Each of these initialization things lives in the file its data is in)
// Note - Ularn only allowed people to genocide the first
// instance of the character in monstnamelist.  This means that
// non-bronze dragons, and the lama nobe, were not genocidable.
// I could make these exceptions if it seems like a good idea.
extern Char **cast_select_items; // [SPNUM][27];
extern Short cast_qty;
void init_doomed_monsters(FormPtr frm)
{
  // initialize the list
  ListPtr lst;
  Short i;
  Char c;  // from monstnamelist
  const Char *mn;

  FrmCopyTitle(frm, "Genocide");
  cast_select_items = (Char **) md_malloc(sizeof(Char *) * MAXMONST);

  cast_qty = 0;
  for (i = 1 ; i < MAXMONST ; i++) {
    c = get_monstnamelist(i);
    mn = monster[i].name;
    cast_select_items[cast_qty] = md_malloc(sizeof(Char) * (StrLen(mn) + 5));
    StrPrintF(cast_select_items[cast_qty], "(%c) %s", c, mn);
    cast_qty++; // how many to free later
  }

  // Yay.
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
  LstSetListChoices(lst, cast_select_items, cast_qty);
  LstSetSelection(lst, -1);

}






/**********************************************************************
                       FIGHT
 IN: dir = the direction to attack in
 PURPOSE:
 Let user fight on autopilot so s/he doesn't have to keep tapping the
 screen and seeing hit/miss messages.  Theoretically (not sure if still true)
 the fight will be interrupted when the monster might be able to
 kill the rogue with one hit.  I stole this from Rogue.  Damn lemmings.
 **********************************************************************/
Boolean keep_fighting = false;
Short fight_monster_i = -1;
Short fight_monster_j = -1;
extern Boolean i_am_dead;
void fight(Short dir)
{
  Short obj, mon, k, m, possible_dam;
  if (env->cdude[CONFUSE]) {
    message("You are too confused.");
    return;
  }
  k = env->playerx + diroffx2[dir];
  m = env->playery + diroffy2[dir];
  if (k < 0 || k >= MAXX || m < 0 || m >= MAXY)
    return; // don't fight past boundary
  obj = env->item[k][m];
  mon = env->mitem[k][m];
  if (obj == OWALL && (env->cdude[WTW] == 0 || env->level == 0))
    return; // don't hit a cell you can't walk through
  if (mon <= 0) {
    message("I see no monster there.");
    return;
  }
  possible_dam = 2 * monster[mon].damage + monster[mon].level;
  if (env->cdude[HP] <= possible_dam) {
    message("You hesitate to commit yourself.");
    return;
  }
  // set globals; callees will need them
  keep_fighting = true;
  fight_monster_i = k;
  fight_monster_j = m;
  // Record which monster we are fighting (in fight_monster)
  // Figure out the maximum damage the monster can inflict in one hit
  //  When we are looping, if cur hp drops to that, we get interrupted.
  while (!i_am_dead && keep_fighting) {
    if (env->cdude[HP] <= possible_dam) {
      message("Perhaps it's time to consider retreat.");
      break;
    }
    hitmonster(k, m, true); // might set keep_fighting if mon dies
    if (env->cdude[HASTEMONST]) 
      movemonst();  // might set keep_fighting if mon moves away ...
    if (i_am_dead) break;
    movemonst();   // ... or if a different monster hits you
    if (i_am_dead) break;
    regen(true); // might set keep_fighting ... if we drop stuff.
  }
  showcell(env->playerx, env->playery);
  showplayer(env->playerx, env->playery);
  print_stats();
  refresh();
  //  update_screen_cell(env->playerx, env->playery);
  // may also need that or showplayer
  //  showplayer(env->playerx, env->playery);
  fight_monster_i = -1;
  fight_monster_j = -1;
}
