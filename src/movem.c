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
// monster is in data.c
//extern struct monst monster[];
extern Short diroffx[];
extern Short diroffy[];
extern Boolean keep_fighting;
extern Short fight_monster_i;
extern Short fight_monster_j;


static Short w1[9], w1x[9], w1y[9];
static Short tmp1, tmp2, tmp3, tmp4, distance;
static Short tmpitem, xlo, xhi, ylo, yhi;
// SIGH
Char screen[MAXX][MAXY]; // used only by movemt

static void movemt(Short i, Short j) SEC_2;
static void mmove(Short aa, Short bb, Short cc, Short dd) SEC_2;


/*
 * move mnster
 */
void movemonst()
{
  Short i,j, playerx, playery;
  if (env->cdude[HP] <= 0)
    return; // stop beating dead horse
  if (env->cdude[TIMESTOP])
    return;	/* no action if time is stopped */
  /* // XXXX DEBUG
  {
    Char buf[20];
    static Short counter = 0;
    StrPrintF(buf, "%d", counter++);
    WinDrawChars(buf, StrLen(buf), 0,0);
  }
  */ // XXXX END DEBUG
  if (env->cdude[HASTESELF])
    if ((env->cdude[HASTESELF] & 1) == 0)
      return;
  movsphere(); /* move the spheres of annihilation if any */
  if (env->cdude[HOLDMONST])
    return;	/* no action if monsters are held */
	
  playerx = env->playerx;
  playery = env->playery;
  if (env->cdude[AGGRAVATE]) {
    // determine window of monsters to move
    tmp1 = playery - 5; 
    tmp2 = playery + 6; 
    tmp3 = playerx - 10; 
    tmp4 = playerx + 11;
    distance = 40; /* depth of intelligent monster movement */
  } else {
    tmp1 = playery - 3; 
    tmp2 = playery + 4; 
    tmp3 = playerx - 5; 
    tmp4 = playerx + 6;
    distance = 17; /* depth of intelligent monster movement */
  }
	
  if (env->level == 0) {
    // if on outside level monsters can move in perimeter
    tmp1 = max(0,    tmp1);
    tmp2 = min(tmp2, MAXY);
    tmp3 = max(0,    tmp3);
    tmp4 = min(tmp4, MAXX);
  } else {
    // if in a dungeon monsters can't be on the perimeter (wall there) */
    tmp1 = max(1,    tmp1);
    tmp2 = min(tmp2, MAXY-1);
    tmp3 = max(1,    tmp3);
    tmp4 = min(tmp4, MAXX-1);
  }
	
  for (j = tmp1 ; j < tmp2 ; j++) // un-set the monster moved flags
    for (i = tmp3 ; i < tmp4 ; i++)
      env->moved[i][j] = false;
  env->moved[env->lasthx][env->lasthy] = false;
	
  if (env->cdude[AGGRAVATE] || !env->cdude[STEALTH]) {
    // "who gets moved?" split for "efficiency"
    for (j = tmp1 ; j < tmp2 ; j++) // look thru all locations in window
      for (i = tmp3 ; i < tmp4 ; i++) {
	if (env->cdude[HP] <= 0)
	  return; // stop beating dead horse
	if (env->mitem[i][j])     // if there is a monster to move
	  if (!env->moved[i][j])  // if it has not already been moved
	    movemt(i,j);      // go and move the monster
      }
  } else {
    // not aggravated and not stealth
    for (j = tmp1 ; j < tmp2 ; j++) // look thru all locations in window 
      for (i = tmp3 ; i < tmp4 ; i++) {
	if (env->cdude[HP] <= 0)
	  return; // stop beating dead horse
	if (env->mitem[i][j])     // if there is a monster to move
	  if (!env->moved[i][j])  // if it has not already been moved
	    if (env->stealth[i][j])    // if it is NOT asleep-due-to-stealth
	      movemt(i,j);    // go and move the monster
      }
  }
	
  if (env->mitem[env->lasthx][env->lasthy]) {
    // now move monster last hit by player.. if not already moved.
    if (!env->moved[env->lasthx][env->lasthy]) {
      if (env->cdude[HP] <= 0)
	return; // stop beating dead horse
      movemt(env->lasthx,env->lasthy);
      env->lasthx = w1x[0];
      env->lasthy = w1y[0];
    }
  }
}



/*
 *	This routine is responsible for determining where one monster at (x,y) 
 *	will move to.  Enter with the monsters coordinates in (x,y).
 *	Returns no value.
 */
static void movemt(Short i, Short j)
{
  Short k, m, z, tmp, xtmp, ytmp, thismonster;
  switch(thismonster = env->mitem[i][j]) {
    // for half speed monsters
  case TROGLODYTE:  
  case HOBGOBLIN:  
  case METAMORPH:  
  case XVART:
  case INVISIBLESTALKER:  
  case ICELIZARD: 
    if ((env->gtime & 1) == 1) return;
  }

  /* choose destination randomly if scared */
  tmp = (has_object(OHANDofFEAR) ? 1 : 0);
  if (env->cdude[SCAREMONST]) {
    if (tmp==1) tmp=2;
  } else {
    // hand of fear only works some of the time, on its own..
    if ((tmp==1) && (rnd(10) > 4)) tmp=0;
  }
  if ((thismonster > DEMONLORD) || (thismonster == PLATINUMDRAGON)) {
    // unimpressed monsters..
    tmp = (tmp==1) ? 0 : (rnd(10) > 5);
  }
  if (tmp) {
    // ok, monster is scared
    xlo = i + rnd(3) - 2;
    xlo = max(0,   xlo);
    xlo = min(xlo, MAXX-1);

    ylo = j + rnd(3) - 2;
    ylo = max(0,   ylo);
    ylo = min(ylo, MAXY-1);

    tmp = env->item[xlo][ylo];
    if (tmp != OWALL)
      if (env->mitem[xlo][ylo] == 0)
	if ((thismonster != VAMPIRE) || (tmpitem != OMIRROR))
	  if (tmp != OCLOSEDDOOR)
	    mmove(i, j, xlo, ylo);
    return;
  } // done moving scared monster

	
  if (m_intelligence(thismonster) > 10 - env->cdude[HARDGAME]) {
    // if smart monster
    // intelligent movement here -- first set up screen array
    // Try to move toward the player; if we can't find, or move in, the
    // best direction, then don't bother moving.
    //
    // oh fsck me harder.  [tmp1 thru tmp4, globals, were set in caller]
    xlo = tmp3 - 2; 
    ylo = tmp1 - 2; 
    xhi = tmp4 + 2;  
    yhi = tmp2 + 2;
    vxy(&xlo, &ylo);   // set lo to 0 or higher
    vxy(&xhi, &yhi);   // set hi to MAX{x,y}-1 or lower
    for (k = ylo ; k < yhi ; k++) {
      for (m = xlo ; m < xhi ; m++) {
	if (thismonster >= DEMONPRINCE) {
	  screen[m][k] = 0;
	} else {
	  switch(env->item[m][k]) {
	  case OWALL: 
	  case OELEVATORUP:
	  case OELEVATORDOWN:
	  case OPIT: 
	  case OTRAPARROW: 
	  case ODARTRAP:
	  case OCLOSEDDOOR: 
	  case OTRAPDOOR: 
	  case OTELEPORTER:
	    screen[m][k] = 127;  
	    break;
	  case OMIRROR: 
	    //	    if (env->mitem[m][k] == VAMPIRE)  screen[m][k] = 127;
	    // I do not think that was right.
	    if (thismonster == VAMPIRE)  screen[m][k] = 127;
	    break;
	  default:  
	    screen[m][k] = 0;
	    break;
	  }
	}
      }
    }
    screen[env->playerx][env->playery] = 1;

    // now perform proximity ripple from playerx,playery to monster
    // you're sick, sick, SICK
    // Ok, here's the story.
    // xlo,ylo and xhi,yhi are the corners of a square-to-search.
    // We are looking for low numbers starting with 1.
    //   (If 1 is not in the square, we will just fumble around)
    // If we find 1, then in every non-numbered square around it,
    // we will write a "2" (then we grovel the rest of the square.)
    // Whether we found 1 or not,
    // we will then start searching the square looking for "2"s.
    // (seems like we can terminate early if there's no 1 in here; OH WELL)
    // Eventually we should reach our own position.
    xlo = tmp3 - 1; 
    ylo = tmp1 - 1; 
    xhi = tmp4 + 1;  
    yhi = tmp2 + 1;
    vxy(&xlo, &ylo);  
    vxy(&xhi, &yhi);
    for (tmp = 1 ; tmp < distance ; tmp++) // only up to <distnce> squares away
      for (k = ylo ; k < yhi ; k++)
	for (m = xlo ; m < xhi ; m++)
	  if (screen[m][k] == tmp) // found a square in the current goal-area
	    for (z = 1 ; z < 9 ; z++) {
	      // mark the 8 squares around it, if they're unmarked
	      xtmp = m + diroffx[z];
	      ytmp = k + diroffy[z];
	      if (screen[xtmp][ytmp] == 0)
		screen[xtmp][ytmp] = tmp + 1;
	      if (xtmp==i && ytmp==j) goto out;
	    }
    // this goto is sadly necessary (lacking a mega-'break')
  out:  
    if (tmp < distance) {
      // we did find connectivity, hurrah
      // now move in the direction that we are connected at.
      for (z = 1 ; z < 9 ; z++) {
	// go in a circle around monster's current position
	xlo = i + diroffx[z];
	ylo = j + diroffy[z];
	// HEY - don't include squares that are out of bounds!!
	// (This bug was letting the gnome king walk into South wall.)
	if (xlo <= 0 || ylo <= 0 || xlo >= MAXX-1 || ylo >= MAXY-1)
	  continue;
	if (screen[xlo][ylo] == tmp) {
	  if (!env->mitem[xlo][ylo]) { 
	    //	    Char buf[20]; // xxx
	    w1x[0]=xlo;
	    w1y[0]=ylo; 
	    mmove(i, j, w1x[0], w1y[0]); 
	    //	    StrPrintF(buf, "%d %d %d %d", i, j, w1x[0], w1y[0]); // xxx
	    //	    WinDrawChars(buf, StrLen(buf), 0, 0);// xxx
	    return; 
	  }
	}
      } // end for z
    } // nope didn't find connectivity.
  } // end intelligence
	
  // dumb monsters move here (oblivious to obstacles)
  // Construct a small square, with the monster at the edge or corner
  // that is farthest from the player...
  xlo = i - 1;
  ylo = j - 1;
  xhi = i + 2;
  yhi = j + 2;
  if (i < env->playerx)      xlo++; 
  else if (i > env->playerx) --xhi;
  if (j < env->playery)      ylo++; 
  else if (j > env->playery) --yhi;
  for (k = 0 ; k < 9 ; k++)
    w1[k] = 10000; // initialize to "infinite" distance/undesirability
	
  for (k = xlo ; k < xhi ; k++) {
    for (m = ylo ; m < yhi ; m++) {
      // for each square, compute w1=distance^2 to player
      // (note: tmp is an index for a flat array that represents
      // a square 3x3 array in someone's imagination.)
      tmp = 3 * (1 + m - j) + (1 + k - i);
      tmpitem = env->item[k][m];
      // ok if: not a wall, or it's a wall but player is standing on it;
      // no monster there already; no vamp/mirror deal; no closed door
      if (tmpitem != OWALL || (k==env->playerx && m==env->playery))
	if (env->mitem[k][m] == 0)
	  if ((thismonster != VAMPIRE) || (tmpitem != OMIRROR))
	    if (tmpitem != OCLOSEDDOOR) {
	      w1[tmp] = (env->playerx - k) * (env->playerx - k) +
		(env->playery - m) * (env->playery - m); // dist^2
	      w1x[tmp] = k;
	      w1y[tmp] = m;
	    }
    }
  } // end for k,m
  // now we have an imaginary 3x3 array of {undesirability,(x,y)}
  tmp = 0;
  for (k = 1 ; k < 9 ; k++)
    if (w1[tmp] > w1[k])
      tmp = k; // find index of most desirable square
	
  // if it's not infinitely lame, and we're not already there, go there.
  if (w1[tmp] < 10000)
    if ((i!=w1x[tmp]) || (j!=w1y[tmp]))
      mmove(i, j, w1x[tmp], w1y[tmp]);
}


/*
 *		Function to actually perform the monster movement
 *  Enter with the from coordinates in (aa,bb) and the destination coordinates
 *	in (cc,dd).
 */
static void mmove(Short aa, Short bb, Short cc, Short dd)
{
  Short tmp, i, flag;
  Char *who = 0; //,*p;
  Char buf[80];
  flag = 0;   // set to various values if, e.g, monster hit by arrow trap 
  if ((cc == env->playerx) && (dd == env->playery)) {
    env->hitflag = true; // "hello, you've been hit."
    hitplayer(aa, bb);
    env->moved[aa][bb] = true;
    return;
  }
  if (keep_fighting && aa == fight_monster_i && bb == fight_monster_j) {
    // we're in a fight, and this is the monster being fought,
    // and apparently it's moving somewhere else.
    keep_fighting = false;
  }
  i = env->item[cc][dd]; // what's the item, if any, at the target square
  if ((i==OPIT) || (i==OTRAPDOOR))
    switch(env->mitem[aa][bb]) {
    case SPIRITNAGA:
    case PLATINUMDRAGON:
    case WRAITH:
    case VAMPIRE:
    case SILVERDRAGON:
    case POLTERGEIST:
    case DEMONLORD:	
    case DEMONLORD+1:
    case DEMONLORD+2:
    case DEMONLORD+3:
    case DEMONLORD+4:
    case DEMONLORD+5:
    case DEMONLORD+6:
    case DEMONPRINCE:
    case LUCIFER:
      break; // can walk on air I reckon
    default:
      env->mitem[aa][bb] = 0; // fell in a pit or trapdoor (what a maroon!)
    }

  tmp                = env->mitem[aa][bb];
  env->mitem[cc][dd] = env->mitem[aa][bb];

  if (i == OANNIHILATION) {
    // demons dispel spheres...
    if (tmp >= DEMONLORD) {
      flag = ( has_object(OSPHTALISMAN) ? 1 : 0 ) ;
      if ( !flag || (tmp == LUCIFER && rnd(10) > 7)) {
	// the player has no sphtalisman, or the monster has trumped it
	StrPrintF(buf, "The %s dispels the sphere!", m_name(tmp));
	message(buf);
	rmsphere(cc, dd);
      } else {
	i                  = 0;
	tmp                = 0;
	env->mitem[cc][dd] = 0; // poof!
      }
    } else {
      i                  = 0;
      tmp                = 0;
      env->mitem[cc][dd] = 0; // no more monster.  unless it was a lemming 9%
    }
  } // end Oannihilation
	
  env->stealth[cc][dd] = true;
  flag = 0;

  env->hitp[cc][dd] = env->hitp[aa][bb];
  if (env->hitp[cc][dd] < 0) env->hitp[cc][dd] = 1; // hm. whatif == 0 ????

  if (tmp == LEMMING) {
    // HERE is another place to make lemmings less (or more) annoying:
    if (rnd(100) <= 3) {
      //    if (rnd(100) <= 9) {
      // 9% chance that it multiplies
      env->mitem[aa][bb] = LEMMING;
      env->know[aa][bb] = 1;
    } else {
      env->mitem[aa][bb] = 0;
    }
  } else {
    env->mitem[aa][bb] = 0;
  }

  env->moved[cc][dd] = true;

  if (tmp == LEPRECHAUN) // leprechaun consumes gold
    switch(i) {
    case OGOLDPILE:  
    case OMAXGOLD:  
    case OKGOLD:
    case ODGOLD:
    case ODIAMOND:   
    case ORUBY:     
    case OEMERALD: 
    case OSAPPHIRE:
      env->item[cc][dd] = 0; 
    };

  if (tmp == TROLL)  // if a troll, regenerate him
    if ((env->gtime & 1) == 0)
      if (m_hitpoints(tmp) > env->hitp[cc][dd])
	env->hitp[cc][dd] += 1;

  if (i == OTRAPARROW) {
    // arrow hits monster
    who = "An arrow";  
    flag = 1;
    env->hitp[cc][dd] -= rnd(10) + env->level;
    if (env->hitp[cc][dd] <= 0) { 
      env->mitem[cc][dd] = 0;  
      flag = 2; // killed by arrow
    }
  }

  if (i == ODARTRAP) {
    // dart hits monster
    who = "A dart";  
    flag = 1;
    env->hitp[cc][dd] -= rnd(6);
    if (env->hitp[cc][dd] <= 0) { 
      env->mitem[cc][dd] = 0;  
      flag = 2;
    } 
  }

  if (tmp < DEMONLORD)
    if (i == OTELEPORTER) {
      // monster hits teleport trap
      fillmonst(env->mitem[cc][dd]); // [try to] place the monster elsewhr
      // (we don't particularly care whether we fail)
      env->mitem[cc][dd] = 0; 
      flag = 3; 
    }

  if (tmp < DEMONLORD)
    if ((i == OELEVATORUP) || (i == OELEVATORDOWN)) {
      env->mitem[cc][dd] = 0;
      flag = 4;
    }

  if (env->cdude[BLINDCOUNT]) return;	
  // if blind, don't show where the monsters are
  // otherwise, if the player can see that square, update it
  if (env->know[cc][dd] & 1) {
    buf[0] = 0;
    //    if (flag) cursors();
    switch(flag) {
    case 1: 
      StrPrintF(buf,"%s hits the %s", who, m_name(tmp));
      message(buf);
      break;
    case 2: 
      StrPrintF(buf,"%s kills the %s", who, m_name(tmp));
      message(buf);
      break;
    case 3: 
      StrPrintF(buf,"The %s is teleported", m_name(tmp));
      message(buf);
      break;
    case 4: 
      StrPrintF(buf,"The %s took an elevator", m_name(tmp));
      message(buf);
      break;
    };
  }

  if (env->know[aa][bb] & 1)   update_screen_cell(aa,bb);
  if (env->know[cc][dd] & 1)   update_screen_cell(cc,dd);
}
