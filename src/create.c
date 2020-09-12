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

//Short oldx,oldy; // they were extern (def'd data.c, used display.c)
extern LarnPreferenceType my_prefs;

static Boolean cannedlevel(Short k) SEC_2;
static void decode(Short k, Char c, Short x, Short y) SEC_2;
static void makemaze(Short k) SEC_2;
static Boolean eat_this_way_p(Short x, Short y, Short dir) SEC_2;
static void makeobject(Short j) SEC_2;
static void multfillroom(Short n, Char what, Short arg) SEC_2;
static void froom(Short n, Char itm, Short arg) SEC_2;
static void fillroom(Char what, Short arg, Boolean first) SEC_2;
static void treasureroom(Short lv) SEC_2;
static void troom(Short lv, Short xsize, Short ysize,
		  Short tx, Short ty, Short glyph) SEC_2;
static void checkgen() SEC_2;


/*
  Subroutine to create the player and the players attributes
  This is called when you start a new character (i.e. when you
  enter the app without a saved character, or when you die,
  you get a pick-character form and then we call this)
  */
void makeplayer(Short selected_class)
{
  pick_char(selected_class);
  env->cdude[BIRTHDATE] = TimGetSeconds(); // Unique ID for Characters.
  env->gtime = 0; // the clock is ticking
  recalc();
  newcavelevel(0);
  env->playerx = rnd(MAXX_0);
  env->playery = rnd(MAXY_0);
}

/*
  function to enter a new level.  This routine must be called anytime the
  player changes levels.  If that level is unknown it will be created.
  A new set of monsters will be created for a new level, and existing
  levels will get a few more monsters.
  Note that it is here we remove genocided monsters from the present level
  (hence when you cast genocide, we call this on current level to 'clean up')
  */
void newcavelevel(Short x)
{
  Short i, j;
  Boolean saving_implemented = true; // turn this off to get random levels :)
  Boolean found = false;

  if (env->beenhere[(Short) env->level]) 
    savelevel(env);     // save the current (old) level to the database
  // NOTE: it's possible for savelevel to fail and return false.  xxxxxxx
  // It would be polite for me to detect this and warn the user  xxxxxxx
  // that he/she is, presumably, low on memory.   xxxxxxxxxxxxxxx
  env->level = x;       // get the new level
  level0_itsy_fix(x==0); // this is font-related

  if (env->beenhere[x]) 
    found = getlevel(env);

  if (found)
    sethp(false); // add some monsters
  else {
    // create level from scratch
    for (i = 0 ; i < MAXY ; i++)
      for (j = 0 ; j < MAXX ; j++)
	env->know[j][i] = env->mitem[j][i] = 0;
    makemaze(x);
    makeobject(x);
    env->beenhere[x] = saving_implemented;
    sethp(true); // populate the level
    if (x == 0) // town
      for (j = 0 ; j < MAXY ; j++)
	for (i = 0 ; i < MAXX ; i++)
	  env->know[i][j] = 1;
  }

  checkgen(); // wipe out any genocided monsters

  am_i_on_level_0(x==0); // this is for display scrolling

}

/*
 *	function to read in a maze from a data file!
 */
Char unconvert[16] = " !#-.D~         ";
//                    0123456
static Boolean cannedlevel(Short k)
{ 
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID;
  Short offset, row, col;
  Char c,x,y, *charp;

  //uniqueID = rund(20) + 10; // there are 20 canned levels: uids 10-29
  //err = DmFindRecordByID(iLarnDB, uniqueID, &recindex);
  //if (err) return false; // couldn't find.. create an uncanned level instead
  // There are 20 canned levels, at indices 1 through 20. (recs start at 0).
  recindex = rund(20) + 1;
  if (DmNumRecords(iLarnDB) <= recindex) return false; // database is scrod!
  vh = DmQueryRecord(iLarnDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  charp = (Char *) p;

  for (row = 0, offset = 0 ; row < MAXY ; row++) {
    for (col = 0 ; col < MAXX ; col++) {
      // Canned levels have the top and bottom row and left col stripped (#)
      // They could also be packed more tightly than they are..  ah well
      if (col == 0 || row == 0 || row == MAXY-1) {
	env->item[col][row] = OWALL;
	env->iarg[col][row] = 0;
	env->mitem[col][row] = 0;
	env->hitp[col][row] = 0;
      } else {
	c = charp[offset]; // "get a char from p+offset";
	x = (c & 0xF0) >> 4; // high 4
	y = c & 0x0F; // low  4
	decode(k, x, col, row);
	decode(k, y, col+1, row);
	offset++;
	col++; // we read two at a go
      }
    }
  }
  // close the record
  MemHandleUnlock(vh);
  return true;
}
// This is used in decoding the canned levels
// for a particular canned cell, figure out what to put there.
static void decode(Short k, Char c, Short x, Short y)
{
  Short it, arg, mit, marg;
  it = mit = arg = marg = 0;
  switch(c) {
  case 0: // ' '
    break;
  case 1: // '!'
    if (k!=MAXLEVEL+MAXVLEVEL-1)  
      break;
    it = OPOTION;	
    arg = 21;
    mit = LUCIFER;
    marg = m_hitpoints(mit);
    break;
  case 2: // '#'
    it = OWALL;
    break;
  case 3: // '-'
    it = newobject(k+1, &arg);
    break;
  case 4: // '.'
    if (k < MAXLEVEL)  break;
    mit = makemonst(k+1);
    marg = m_hitpoints(mit);
    break;
  case 5: // 'D'
    it = OCLOSEDDOOR;  	
    arg = rnd(30);		
    break;
  case 6: // '~'
    if (k != MAXLEVEL-1) break;
    it = OLARNEYE;
    mit = DEMONPRINCE;
    marg = m_hitpoints(mit);
    break;
  }
  env->item[x][y] = it;
  env->iarg[x][y] = arg;
  env->mitem[x][y] = mit;
  env->hitp[x][y] = marg;
}


/*
 * Make the caverns for a given level.  only walls are made.
 */
static Short mx,mxl,mxh,my,myl,myh,tmp2;
static void makemaze(Short k)
{
  Short i,j;
  Short tmp, z;

  // decide whether to make it a canned level
  if (k > 1 && ( rnd(17) <= 4
		 || k == MAXLEVEL-1 
		 || k == MAXLEVEL+MAXVLEVEL-1) ) {
    if (cannedlevel(k))
      return; // we successfully read the maze
  }

  tmp = OWALL;

  for (i = 0; i < MAXY; i++)	
    for (j = 0; j < MAXX; j++)	
      env->item[j][i] = tmp;

  // let's make level 0 smaller so it fits on your screen!
  if (k == 0) {
    for (i = 1 ; i <= MAXY_0 ; i++)
      for (j = 1 ; j <= MAXX_0 ; j++)
	env->item[j][i] = 0;
    return;
  }	

  eat(1,1);

  if (k == 1)  // make the dungeon exit
    env->item[33][MAXY-1] = 0;
  
  // now for open spaces, except if it is level 10.
  if (k != MAXLEVEL-1) {
    tmp2 = rnd(3) + 3;
    for (tmp = 0 ; tmp < tmp2 ; tmp++) { 	
      my = rnd(11) + 2;   
      myl = my - rnd(2);  
      myh = my + rnd(2);
      if (k < MAXLEVEL) { 	
	mx = rnd(44) + 5;  
	mxl = mx - rnd(4);  
	mxh = mx + rnd(12) + 3;
	z = 0;
      } else { 	
	mx = rnd(60) + 3;  
	mxl = mx - rnd(2);  
	mxh = mx + rnd(2);
	z = makemonst(k);
      }
      for (i = mxl ; i < mxh ; i++)		
	for (j = myl ; j < myh ; j++) {  	
	  env->item[i][j] = 0;
	  if ( (env->mitem[i][j] = z) != 0) 
	    env->hitp[i][j] = m_hitpoints(z);
	}
    }
  } // done creating open spaces

  // make a horizontal passageway on one whole random row (except last volcano)
  if (k != MAXLEVEL-1) { 	
    my = rnd(MAXY-2);  
    for (i = 1 ; i < MAXX - 1; i++)	
      env->item[i][my] = 0; 
  }
  if (k > 1) 
    treasureroom(k);  // have I tested this?
}

/*
 *  'eat' is a function to eat away a filled in maze
 *  it was recursive, and I had to fix that (limited stack depth!)
 *  hence more primitive approach with eat_stack and eat_this_way_p
 */

/*
 * The most I've seen used, so far, is 172.  so 200 should be plenty.
 * If it is not enough you will see a level with some un-tunnelled area.
 */
#define BIGEAT 200
struct eat_stack {
  Short x;
  Short y;
  Char dir; // 4 values, this is an untried direction to try next
  Char tries; // decrement when you try; don't save again if it's now 0
};
/*
 * eat_this_way_p returns false if you're done here, true if you should eat it
 */
static Boolean eat_this_way_p(Short x, Short y, Short dir)
{
  switch(dir) {
  case 1:	
    if (x <= 2) return false;         // west	
    if ((env->item[x-1][y]!=OWALL) || (env->item[x-2][y]!=OWALL)) return false;
    return true;
    break;
  case 2:	
    if (x >= MAXX-3) return false;    // east	
    if ((env->item[x+1][y]!=OWALL) || (env->item[x+2][y]!=OWALL)) return false;
    return true;
    break;
  case 3:	
    if (y <= 2) return false;         // south	
    if ((env->item[x][y-1]!=OWALL) || (env->item[x][y-2]!=OWALL)) return false;
    return true;
    break;
  case 4:	
    if (y >= MAXY-3 ) return false;   // north	
    if ((env->item[x][y+1]!=OWALL) || (env->item[x][y+2]!=OWALL)) return false;
    return true;
    break;
  }
  // If we get to here, someone passed in a bad value for dir...
  return false;
}

/*
 * Ok, here is eat
 * It is also (re)used in monster.c to "alter reality"
 * I'll leave in all my old comments so you have a prayer
 */
#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
void eat(Short x0, Short y0)
{ 
  struct eat_stack *my_eat_stack;
  VoidHand h; // for free_me

  Short i, x, y, dir;
  Short stack_i = 0; // the lowest 'empty' location; if 0, it's all empty

  my_eat_stack = (struct eat_stack*) md_malloc(sizeof(struct eat_stack)
					       * BIGEAT);

  for (i = 0 ; i < BIGEAT ; i++) {
    my_eat_stack[i].x = my_eat_stack[i].y = my_eat_stack[i].dir = 0;
  }

  x = x0;
  y = y0;
  dir = 0;
  // Hm, I should push "65,15" on the stack so that if it gets stuck,
  my_eat_stack[stack_i].x = 65;
  my_eat_stack[stack_i].y = 15;
  my_eat_stack[stack_i].dir = rnd(4);
  my_eat_stack[stack_i].tries = 4; // cause there have been NO tries of this
  stack_i++;
  // ok see if that worked.
  while (stack_i < BIGEAT) {
    // it will try the other side.
    if (!dir) {  // "try 0" -- fresh, not from the stack.
      // pick direction: [1,4]
      dir = rnd(4);
      //      if (max_stack_size < stack_i) max_stack_size = stack_i;
      my_eat_stack[stack_i].x = x;
      my_eat_stack[stack_i].y = y;
      my_eat_stack[stack_i].dir =  (dir < 4) ? (dir+1) : 1;// Order Matters! 
      my_eat_stack[stack_i].tries = 3; // 3cause we are doing the 1st try NOW
      stack_i++;
      //      if (stack_i >= BIGEAT) {
	// DANGER WILL ROBINSON. for debugging to detect too-small BIGEAT.
	//env->cdude[INTELLIGENCE] = 97;
      //	break;
      //      }
    } // else "try 1,2,or3": direction (and x and y) were pulled from stack.
    if (!eat_this_way_p(x, y, dir)) {
      // we've reached AN end
      if (stack_i <= 0) { // this line is the real thing
	// hey, we ran out of stack, we've reached THE end
	break;
      }
      // nope, pop someone off the stack, and start through the loop again.
      stack_i--;
      x = my_eat_stack[stack_i].x;
      y = my_eat_stack[stack_i].y;
      dir = my_eat_stack[stack_i].dir;
      my_eat_stack[stack_i].tries -= 1;
      if (my_eat_stack[stack_i].tries) {
	my_eat_stack[stack_i].dir =  (dir < 4) ? (dir+1) : 1;// Order Matters! 
	// push it back on the stack, slightly mutated
	stack_i++;
      }
      continue;
    }
    // nope, eat in the current direction, then inc/dec x/y and loop.
    switch(dir) {
    case 1:
      env->item[x-1][y] = env->item[x-2][y] = 0;
      x -= 2; // west
      dir = 0;
      break;
    case 2:
      env->item[x+1][y] = env->item[x+2][y] = 0;
      x += 2; // east
      dir = 0;
      break;
    case 3:
      env->item[x][y-1] = env->item[x][y-2] = 0;
      y -= 2; // north
      dir = 0;
      break;
    case 4:
      env->item[x][y+1] = env->item[x][y+2] = 0;
      y += 2; // south
      dir = 0;
      break;
    }
  } // end while
  //  env->cdude[GOLD] = max_stack_size; // To view used-space, while debugging
  free_me(my_eat_stack);
}


/*
 ***********
 MAKE_OBJECT
 ***********
 subroutine to create the objects in the maze for the given level
 used by newcavelevel
 */
static void makeobject(Short j)
{
  Short i;
  
  if (j == 0) {
    fillroom(OENTRANCE, 0, true);	/*	entrance to dungeon*/
    fillroom(ODNDSTORE, 0, true);	/*	the DND STORE	*/
    fillroom(OSCHOOL, 0, true);	/*	college of Larn	*/
    fillroom(OBANK, 0, true);	/*	1st national bank of larn*/
    fillroom(OVOLDOWN, 0, true);	/*	volcano shaft to temple*/
    fillroom(OHOME, 0, true);	/*	the players home & family*/
    fillroom(OTRADEPOST, 0, true);	/*  	the trading post	*/
    fillroom(OLRS, 0, true);	/*  	the larn revenue service */
    return;
  }
  if (j == MAXLEVEL) 
    fillroom(OVOLUP, 0, false); /* volcano shaft up from the temple */
  
  /*	make the fixed object in the maze STAIRS and 
	random object ELEVATORS */
  
  if ((j>0) && (j != MAXLEVEL-1) && (j < MAXLEVEL+MAXVLEVEL-3)) 
    fillroom(OSTAIRSDOWN, 0, false);
  
  if ((j > 1) && (j != MAXLEVEL))	
    fillroom(OSTAIRSUP, 0, false);
  
  if ((j>3) && (j != MAXLEVEL))
    if (env->cdude[ELVUP]==0)
      if (rnd(100) > 85) {
	fillroom(OELEVATORUP, 0, false);
	env->cdude[ELVUP]++;
      }
  
  if ((j>0) && (j<=MAXLEVEL-2))
    if (env->cdude[ELVDOWN]==0)
      if (rnd(100) > 85) {
	fillroom(OELEVATORDOWN, 0, false);
	env->cdude[ELVDOWN]++;
      }
  
  /*	make the random objects in the maze		*/
  multfillroom(rund(3),OBOOK,j);				
  multfillroom(rund(3),OCOOKIE, 0);
  multfillroom(rund(3),OALTAR, 0);
  multfillroom(rund(3),OSTATUE, 0);
  multfillroom(rund(3),OFOUNTAIN, 0);			
  multfillroom(rund(2),OTHRONE, 0);			
  multfillroom(rund(2),OMIRROR, 0);
  
  if (j >= MAXLEVEL+MAXVLEVEL-3)
    fillroom(OPIT, 0, false);
  multfillroom(rund(3),OPIT, 0);
  
  if (j >= MAXLEVEL+MAXVLEVEL-3)
    fillroom(OIVTRAPDOOR, 0, false);
  multfillroom(rund(2),OIVTRAPDOOR, 0);
  multfillroom(rund(2),OTRAPARROWIV, 0);		
  multfillroom(rnd(3)-2,OIVTELETRAP, 0);
  multfillroom(rnd(3)-2,OIVDARTRAP, 0);
  
  if (j==1) multfillroom(1,OCHEST,j);
  else 	  multfillroom(rund(2),OCHEST,j);
  
  if (j<MAXLEVEL-1) {
    multfillroom(rund(2),ODIAMOND,rnd(10*j+1)+10);
    multfillroom(rund(2),ORUBY,rnd(6*j+1)+6);
    multfillroom(rund(2),OEMERALD,rnd(4*j+1)+4);
    multfillroom(rund(2),OSAPPHIRE,rnd(3*j+1)+2);
  }
  
  for (i=0; i<rnd(4)+3; i++)
    fillroom(OPOTION, newpotion(), false);	/*	make a POTION	*/
  
  for (i=0; i<rnd(5)+3; i++)
    fillroom(OSCROLL, newscroll(), false);	/*	make a SCROLL	*/
  
  for (i=0; i<rnd(12)+11; i++)
    fillroom(OGOLDPILE, 12*rnd(j+1)+(j<<3)+10, false); /* make GOLD */
  
  if (j==8)	
    fillroom(OBANK2, 0, false);	/*	branch office of the bank */


  //  if ( my_prefs.L18  &&  (env->cdude[PAD]==0)  &&  (j>=4) ) 
  if ( (env->cdude[PAD] == 0) && (j >= 4) && (rnd(100) > 75) ) {
    fillroom(OPAD, 0, false);	/* Dealer McDope's Pad */
    env->cdude[PAD]++;
  }
  
  froom(2,ORING, 0);		/* a ring mail 	*/
  froom(1,OSTUDLEATHER, 0);	/* a studded leather	*/
  froom(3,OSPLINT, 0);		/* a splint mail*/
  froom(5,OSHIELD,rund(3));	/* a shield	*/
  froom(2,OBATTLEAXE,rund(3));	/* a battle axe	*/
  froom(5,OLONGSWORD,rund(3));	/* a long sword	*/
  froom(5,OFLAIL,rund(3));	        /* a flail	*/
  froom(7,OSPEAR,rnd(5));		/* a spear	*/
  froom(4,OREGENRING,rund(3));	/* ring of regeneration */
  froom(1,OPROTRING,rund(3));	/* ring of protection	*/
  froom(2,OSTRRING,rund(5)); 	/* ring of strength  */
  froom(2,ORINGOFEXTRA, 0);	/* ring of extra regen	*/
  
  if (env->cdude[LAMP]==0) {
    if (rnd(120) < 8) {
      fillroom (OBRASSLAMP, 0, false);
      env->cdude[LAMP]++;
      goto zug;
    }
  }
  
  if (env->cdude[WAND]==0) {	/* wand of wonder */
    if (rnd(120) < 8) {
      fillroom(OWWAND, 0, false);
      env->cdude[WAND]++;
      goto zug;
    }
  }
  
  if (env->cdude[DRAGSLAY]==0) /* orb of dragon slaying */
    if(rnd(120) < 8) {
      fillroom(OORBOFDRAGON, 0, false);
      env->cdude[DRAGSLAY]++;
      goto zug;
    }
  
  if (env->cdude[NEGATE]==0)	/* scarab of negate spirit */
    if(rnd(120) < 8) {
      fillroom(OSPIRITSCARAB, 0, false);
      env->cdude[NEGATE]++;
      goto zug;
    }
  
  if (env->cdude[CUBEUNDEAD]==0)		/* cube of undead control */
    if (rnd(120) < 8) {
      fillroom(OCUBEofUNDEAD, 0, false);	
      env->cdude[CUBEUNDEAD]++;
      goto zug;
    }
  
  if (env->cdude[DEVICE]==0)	/* device of antitheft */
    if (rnd(120) < 8) {
      fillroom(ONOTHEFT, 0, false);		
      env->cdude[DEVICE]++;
      goto zug;
    }
  
  if(env->cdude[TALISMAN]==0) 		/* talisman of the sphere */
    if(rnd(120) < 8) {
      fillroom(OSPHTALISMAN, 0, false);
      env->cdude[TALISMAN]++;
      goto zug;
    }
  
  if (env->cdude[HAND]==0)		/* hand of fear */
    if (rnd(120) < 8) {
      fillroom(OHANDofFEAR, 0, false);	
      env->cdude[HAND]++;
      goto zug;
    }
  
  if (env->cdude[ORB] == 0) 	/* orb of enlightenment */
    if (rnd(120) < 8) {
      fillroom(OORB, 0, false);
      env->cdude[ORB]++;
      goto zug;
    }
  
  if (env->cdude[ELVEN]==0)	/* elven chain */
    if (rnd(120) < 8) {
      fillroom(OELVENCHAIN, 0, false);
      env->cdude[ELVEN]++;
    }
 zug:
  if (env->cdude[SLASH]==0)	/* sword of slashing */
    if (rnd(120) < 8) {
      fillroom(OSWORDofSLASHING, 0, false); 	
      env->cdude[SLASH]++;
    }
  
  if (env->cdude[BESSMANN]==0)	/* Bessman's flailing hammer */ 
    if(rnd(120) < 8) {
      fillroom(OHAMMER, 0, false);	
      env->cdude[BESSMANN]++;
    }
  
  if ((j>=10)&&(j<=20)&&(env->cdude[SLAY]==0))	/* Slayer */
    if (rnd(100) > 85-(j-10)) {
      fillroom (OSLAYER, 0, false);
      env->cdude[SLAY]++;
    }
  
  if ((env->cdude[STAFF]==0) && (j>=8) && (j<=20))	/* staff of power */
    if (rnd(100) > 85-(j-10)) {
      fillroom(OPSTAFF, 0, false);
      env->cdude[STAFF]++;
    }
  
  if (env->cdude[HARDGAME]<3 || (rnd(4)==3)) { 	
    if (j>3) { 	
      froom(3,OSWORD,rund(6));  /* sunsword */
      froom(5,O2SWORD,rnd(6));  /* a two handed sword	*/
      froom(3,OBELT,rund(7));	  /* belt of striking	*/
      froom(3,OENERGYRING,rund(6));	/* energy ring	*/
      froom(4,OPLATE,rund(8));  /* platemail */
    }
  }
}


/*
  subroutine to fill in a number of objects of the same kind
  */
static void multfillroom(Short n, Char what, Short arg)
{
  Short i;
  
  for (i = 0 ; i < n ; i++)		
    fillroom(what, arg, false);
}
/* routine to maybe fill a room */
static void froom(Short n, Char itm, Short arg)
{	
  if (rnd(151) < n) 
    fillroom(itm, arg, false);	
}

/*
 *	subroutine to put an object into an empty room
 *	uses a random walk
 */
static void fillroom(Char what, Short arg, Boolean first)
{
  Short x, y, mx = MAXX, my = MAXY;
  if (first) { mx = 22; my = 13; }
  
  x = rnd(mx-2);  
  y = rnd(my-2);
  while (env->item[x][y]) {
    x += rnd(3)-2;		
    y += rnd(3)-2;
    if (x > mx-2)  x = 1;		
    if (x < 1)       x = mx - 2;
    if (y > my-2)  y = 1;		
    if (y < 1)       y = my - 2;
  }
  env->item[x][y] = what;
  env->iarg[x][y] = arg;
}


/*
  function to make a treasure-room on a level
  note:   level 10's treasure room has the eye in it and demon lords
          level V3 has potion of cure dianthroritis and demon prince
  */
static void treasureroom(Short lv)
{
  Short tx, ty, xsize, ysize;
  
  for (tx = 1+rnd(10) ; tx < MAXX-10 ; tx += 10)
    if ( (lv==MAXLEVEL-1) || (lv==MAXLEVEL+MAXVLEVEL-1) || rnd(13)==2) {
      xsize = rnd(6)+3;  	    
      ysize = rnd(3)+3;  
      ty = rnd(MAXY-9)+1;  /* upper left corner of room */
      if (lv==MAXLEVEL-1 || lv==MAXLEVEL+MAXVLEVEL-1) {
	tx += rnd(MAXX-24);
	troom(lv, xsize, ysize, tx, ty, rnd(3)+6);
      } else {
	troom(lv, xsize, ysize, tx, ty, rnd(9));
      }
    }
}

/*
 *	subroutine to create a treasure room of any size at a given location 
 *	room is filled with objects and monsters 
 *	the coordinate given is that of the upper left corner of the room
 */
static void troom(Short lv, Short xsize, Short ysize,
		  Short tx, Short ty, Short glyph)
{
  Short i, j;
  Short tp1, tp2;
  
  for (j = ty - 1 ; j <= ty + ysize ; j++)
    for (i = tx - 1 ; i <= tx + xsize ; i++)	/* clear out space for room */
      env->item[i][j] = 0;
  for (j = ty ; j < ty + ysize ; j++)
    for (i = tx ; i < tx + xsize ; i++) { /* now put in the walls */
      env->item[i][j] = OWALL; 
      env->mitem[i][j] = 0; 
    }
  for (j = ty + 1 ; j < ty + ysize - 1 ; j++)
    for (i = tx + 1 ; i < tx + xsize - 1 ; i++)	/* now clear out interior */
      env->item[i][j] = 0;
  // did that seem inefficient to you?

  switch(rnd(2)) { /* locate the door on the treasure room */
  case 1:
    i = tx + rund(xsize);
    j = ty + (ysize-1) * rund(2);
    env->item[i][j] = OCLOSEDDOOR;
    env->iarg[i][j] = glyph;  /* on horizontal walls */
    break;
  case 2:
    i = tx + (xsize-1) * rund(2);
    j = ty + rund(ysize);
    env->item[i][j] = OCLOSEDDOOR;
    env->iarg[i][j] = glyph;	/* on vertical walls */
    break;
  };
  
  tp1 = env->playerx;  
  tp2 = env->playery;  
  env->playery = ty + (ysize>>1);
  if (env->cdude[HARDGAME] < 2)
    for (env->playerx = tx+1 ; env->playerx <= tx+xsize-2 ; env->playerx+=2)
      for (i=0, j=rnd(6) ; i<=j ; i++) { 	
	something(lv+2); 
	createmonster(makemonst(lv+2)); 
      }
  else
    for (env->playerx = tx+1 ; env->playerx <= tx+xsize-2 ; env->playerx+=2)
      for (i=0, j=rnd(4) ; i<=j ; i++) { 	
	something(lv+2); 
	createmonster(makemonst(lv+4)); 
      }
  env->playerx = tp1;
  env->playery = tp2;
}

/*
  subroutine to put a monster into an empty room without hitting
  walls or other monsters.  return false on failure.
  */
Boolean fillmonst(Char what)
{
  Short x, y, trys;
  
  for (trys = 10 ; trys > 0 ; --trys) { /* max # of creation attempts */
    x = rnd(MAXX-2);  
    y = rnd(MAXY-2);
    // x,y has no item/wall, monster, or player in it...
    if ( (env->item[x][y] == 0) &&   // no item or wall
	 (env->mitem[x][y] == 0) &&  // no monster
	 ((env->playerx != x) || (env->playery != y))) { // no player
      env->mitem[x][y] = what;  
      if (env->know[x][y])
	env->know[x][y] = 2;
      env->hitp[x][y] = m_hitpoints(what);
      return true;
    }
  }
  return false; /* creation failure */
}



/*
  creates an entire set of monsters for a level!!!
  must be done when entering a new level
  if flg == true then wipe out old monsters? else leave them there and
  just add level/2 new 'wandering' monsters that have moved in.
  */
void sethp(Boolean flg)
{
  Short i,j;
  Short elevel = env->level;
  
  if (flg) 
    for (i = 0 ; i < MAXY ; i++) 
      for (j = 0 ; j < MAXX ; j++) 
	env->stealth[j][i]=0;
  if (elevel == 0) { 	
    env->cdude[TELEFLAG] = 0; 
    return; 
  } /*	if teleported and finally found level 1, then no longer lost */
  // I don't know, I feel un-lost as soon as I get to a level I know..
  // maybe it would be less annoying to change that
  // (Also, why is it in *this* procedure?)
  
  // decide how many monsters to add
  if (flg)  
    j = rnd(12) + 2 + (elevel>>1);   
  else   
    j = (elevel>>1) + 1;
  
  for (i = 0 ; i < j ; i++)  
    fillmonst(makemonst(elevel));

  // add demons in nasty levels
  if ((elevel >= 11) && (elevel <= 15)) {
    i = elevel - 10;
    for (j = 1 ; j <= i ; j++)
      if (!fillmonst(DEMONLORD+rund(7)))
	j--;
  }
  // else add demon princes in really nasty levels
  if (elevel > 15 ) {
    i = elevel - 15;
    for (j = 1 ; j <= i ; j++)
      if (!fillmonst(DEMONPRINCE))
	j--;
  }
  positionplayer();
}


/*
 *	Function to destroy all genocided monsters on the present level
 */
static void checkgen()
{
  Short x,y;
  
  for (y=0; y<MAXY; y++)
    for (x=0; x<MAXX; x++)
      if (env->genocided[(Short) env->mitem[x][y]])
	env->mitem[x][y]=0; /* no more monster */
}
