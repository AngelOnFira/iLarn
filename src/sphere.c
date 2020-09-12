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
extern Short diroffx[];
extern Short diroffy[];

// SPHERES OF ANNIHILATION

//struct sphere spheres[MAXSPHERE]; // how many is enough? 6 bytes apiece
// I should just make this a database record, entirely! !!

// don't forget to save these when we leave the application.

// also check on: case 33 in speldamage in cast.c

/*
 *	Routine to annihilate all monsters around player (playerx,playery)
 *	annihilate() 	
 *
 *	Gives player experience, but no dropped objects
 *	Returns the experience gained from all monsters killed    nah.
 */
void annihilate()
{
  Short i,j, playerx = env->playerx, playery = env->playery;
  Boolean some = false;
  Char *p;
  Char buf[80];

  for (i = playerx - 1 ; i <= playerx + 1 ; i++) {
    for (j = playery - 1 ; j <= playery + 1 ; j++) {
      if (vxy(&i, &j)) continue; // out of bounds
      if (! env->mitem[i][j]) continue; // no monster is there
      p = &(env->mitem[i][j]);
      if (*p < DEMONLORD) {
	//	k += monster[*p].experience;	 // uh oh; can I access this?
	some = true;
	clobberm(*p);
	*p = env->know[i][j] = 0;
      }
      else {
	StrPrintF(buf, "The %s barely escapes annihilation!", m_name(*p));
	message(buf);
	env->hitp[i][j] = (env->hitp[i][j] >> 1) + 1;  // lose 1/2 hitpts
      }
    }
  }
  if (some)
    message("You hear loud screams of agony!");	
  return;
}

/*
 *		Function to create a new sphere of annihilation
 *	newsphere(x,y,dir,lifetime)  
 *		int x,y,dir,lifetime;
 *
 *	Enter with the coordinates of the sphere in x,y
 *	  the direction (0-8 diroffx format) in dir, and the lifespan of the
 *	  sphere in lifetime (in turns)
 *
 * if you have no suggestion for the index, i should be -1
 */
void newsphere(Short x, Short y, Short dir, Short life, Short i)
{
  UInt recindex = SPHERES_RECORD;
  VoidHand vh;
  VoidPtr p;
  //  Err err;
  //ULong uniqueID = REC_SPHERES;
  struct sphere *spp;
  struct sphere sp_tmp;

  Short j, m, sp_i=-1, min_i=0, min_len=99;
  Boolean kept;
  //  struct sphere *sp;
  Char buf[80];

  if (dir >= 9) dir=0; // no movement if direction not found

  // find an unused sphere index, sp_i
  if (i != -1) sp_i = i; // someone gave us a hint
  else {
    // find an empty slot, or evict the next to die
    //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
    if (DmNumRecords(iLarnDB) <= recindex) return;
    vh = DmQueryRecord(iLarnDB, recindex);
    p = MemHandleLock(vh);
    if (!p) return;
    spp = (struct sphere *) p;
    for (j = 0 ; j < MAXSPHERE ; j++) {
      if (spp[j].lifetime <= 0) {
	sp_i = j;
	break;
      } else if (spp[j].lifetime < min_len) {
	min_len = spp[j].lifetime;
	min_i = j;
      }
    }
    MemHandleUnlock(vh);
    if (sp_i == -1) {
      sp_i = min_i;
      rmsp(sp_i, spp[sp_i].x, spp[sp_i].y); // kill the evicted sphere
    }
  }
  //  StrPrintF(buf, "dir %d life %d i %d", dir, life, sp_i);
  //  message(buf);

  // ok, we're ready to rumble.
  i = 0;

  if (env->level == 0) {
    vxy(&x,&y); // don't go out of bounds
  } else {
    x = min( max (x, 1), MAXX-2);
    y = min( max (y, 1), MAXY-2);
  }
  
  kept = false;
  if (env->item[x][y]==OANNIHILATION) {
    // collision of spheres detonates them
    message("Two spheres of annihilation collide!");
    message("You hear a great earth shaking blast!"); 
    //      beep();
    rmsp(sp_i, x, y);;  // remove this sphere (do I need to?)
    rmsphere(x, y);;   // and the other sphere
    sphboom(x, y);;    // blow up stuff around sphere
    // sphere does not survive
  } else if (env->item[x][y] == OLRS ) {
    rmsp(sp_i, x, y);;
    env->item[x][y] = 0;
    update_screen_cell(x, y);
    nap(1200);
    //      beep();
    env->item[x][y] = OLRS;
    update_screen_cell(x, y);
    message("You curse: \"DEATH and TAXES!\""); 
    add_tax(19930813L, true);
    // sphere does not survive
  } else if ( has_object(OSPHTALISMAN) ) {
    // sphere can't be dispelled + player can't be annihilated
    kept = true;
    // sphere is kept
  } else if (env->mitem[x][y] >= DEMONLORD) {
    // demons dispel spheres
    m = env->mitem[x][y];
    i = env->hitp[x][y];
    env->know[x][y] = 1; 
    //      show1cell(x,y);	            // show the demon (ha ha)
    update_screen_cell(x,y);
    StrPrintF(buf, "The %s dispels the sphere!", m_name(m));
    message(buf);
    //      beep(); 
    rmsp(sp_i, x, y);;
    env->mitem[x][y] = m;
    env->hitp[x][y] = i;
    env->know[x][y] = 0; 
    // sphere does not survive
  } else if (env->mitem[x][y] == DISENCHANTRESS) {
    // disenchantress cancels spheres
    m = env->mitem[x][y];
    StrPrintF(buf, "The %s causes cancellation of the sphere!", m_name(m));
    message(buf);
    //      beep();
    rmsp(sp_i, x,y);;
    sphboom(x,y);;
    // sphere does not survive
  } else if (env->cdude[CANCELLATION]) {
    // cancellation cancels spheres
    message("As the cancellation takes effect,");
    message("you hear a great earth shaking blast!"); 
    //      beep();
    rmsp(sp_i, x,y);;
    sphboom(x,y);;
    // sphere does not survive
  } else if (env->playerx==x && env->playery==y) {
    // collision of sphere and player!
    message("AIEEEeeeee.....");
    message("You have been enveloped by the");
    message("zone of nothingness!");
    //      beep(); 
    rmsp(sp_i, x,y);;
    nap(4000);  
    died(258, true);
    // sphere does not survive (well, neither do you)
  } else {
    kept = true;
    // sphere is kept
  }
  // ok, is the sphere still around?
  if (kept) {
    env->item[x][y] = OANNIHILATION;  
    env->mitem[x][y] = 0;  
    env->know[x][y] = 1;
    //      show1cell(x,y);	// show the new sphere
    update_screen_cell(x,y);
    sp_tmp.x = x;  
    sp_tmp.y = y;  
    sp_tmp.lev = env->level;
    sp_tmp.dir = dir;  
    sp_tmp.lifetime = life;  
    // now, write the record
    //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
    if (DmNumRecords(iLarnDB) <= recindex) return;
    vh = DmGetRecord(iLarnDB, recindex); // since we're going to modify it
    p = MemHandleLock(vh);
    if (!p) return;
    // write sp_tmp to the ith index
    DmWrite(p, sizeof(struct sphere) * sp_i, &sp_tmp, sizeof(struct sphere));
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnDB, recindex, true);

    env->cdude[SPHCAST] += 1; // keep track of number of active spheres
  }
}

/*
 *	rmsphere(x,y)	Function to delete a sphere of annihilation from list
 *		int x,y;
 *
 *	Enter with the coordinates of the sphere (on current level)
 */
void rmsphere(Short x, Short y)
{
  UInt recindex = SPHERES_RECORD;
  VoidHand vh;
  VoidPtr p;
  //  Err err;
  //ULong uniqueID = REC_SPHERES;
  struct sphere *spp;
  Short j;

  //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
  if (DmNumRecords(iLarnDB) <= recindex) return;
  vh = DmQueryRecord(iLarnDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return;
  spp = (struct sphere *) p;
  for (j = 0 ; j < MAXSPHERE ; j++) {
    if (spp[j].lifetime <= 0)
      continue;
    if ( (env->level == spp[j].lev) && (x == spp[j].x) && (y == spp[j].y) ) {
      /* delete from list */
      MemHandleUnlock(vh);
      rmsp(j, x, y);
      return;
    }
  }
  MemHandleUnlock(vh);
}


void rmsp(Short i, Short x, Short y)
{
  UInt recindex = SPHERES_RECORD;
  VoidHand vh;
  VoidPtr p;
  //  Err err;
  //ULong uniqueID = REC_SPHERES;
  //  struct sphere *spp;
  struct sphere sp_tmp = {0,0,0,0,0}; //  sp_tmp.lifetime = 0;

  env->item[x][y] = env->mitem[x][y] = 0;  
  env->know[x][y]=1;
  //  show1cell(x,y);  	// show the now missing sphere
  update_screen_cell(x,y);
  env->cdude[SPHCAST] -= 1;	

  //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
  if (DmNumRecords(iLarnDB) <= recindex) return;
  vh = DmGetRecord(iLarnDB, recindex); // since we're going to modify it
  p = MemHandleLock(vh);
  if (!p) return;
  // write sp_tmp to the ith index
  DmWrite(p, sizeof(struct sphere) * i, &sp_tmp, sizeof(struct sphere));
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnDB, recindex, true);
}

/*
 *	sphboom(x,y)	Function to perform the effects of a sphere detonation
 *		int x,y;
 *
 *	Enter with the coordinates of the blast, Returns no value
 */
void sphboom(Short x, Short y)
{
  Short i,j;
  Long *c = env->cdude;

  if (c[HOLDMONST]) c[HOLDMONST] = 1;
  if (c[CANCELLATION]) c[CANCELLATION] = 1;
  for (j = max(1, x-2) ; j < min(x+3 , MAXX-1) ; j++)
    for (i = max(1, y-2) ; i < min(y+3 , MAXY-1) ; i++) {
      env->item[j][i] = env->mitem[j][i] = 0;
      //      show1cell(j,i);
      update_screen_cell(x,y);
      if (has_object(OSPHTALISMAN))
	return;
      if (env->playerx == j && env->playery == i) {
	//	beep();
	message("You were too close to the sphere!");
	nap(3000);
	died(283, true); // player killed in explosion
      }
    }
}



/*
 *	movsphere() 	Function to look for and move spheres of annihilation
 *
 *	This function works on the sphere linked list, first duplicating the list
 *	(the act of moving changes the list), then processing each sphere in order
 *	to move it.  They eat anything in their way, including stairs, volcanic
 *	shafts, potions, etc, except for upper level demons, who can dispel
 *	spheres.
 *	No value is returned.
 */
//#define SPHMAX 20	/* maximum number of spheres movsphere can handle */
void movsphere()
{
  UInt recindex = SPHERES_RECORD;
  VoidHand vh;
  VoidPtr p;
  //  Err err;
  //ULong uniqueID = REC_SPHERES;
  struct sphere *spp;
  //  struct sphere sp_tmp;

  Short j, x, y, dir, len=0;
  // look through sphere list.
  //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
  if (DmNumRecords(iLarnDB) <= recindex) return;
  vh = DmQueryRecord(iLarnDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return;
  spp = (struct sphere *) p;
  for (j = 0 ; j < MAXSPHERE ; j++) {
    if (spp[j].lifetime <= 0) continue; // this slot's not actually in use
    if (spp[j].lev == env->level) {
      x = spp[j].x;
      y = spp[j].y;
      dir = spp[j].dir;
      len = spp[j].lifetime - 1; // age the sphere
      if ((len <= 0) || (env->item[x][y] != OANNIHILATION)) {
	rmsp(j, x, y); // i THINK that Get is ok while a Query is open
	continue;
      }
      // move the sphere
      switch(rnd((Short)max(7,env->cdude[INTELLIGENCE]>>1))) {
      case 1:
      case 2:
	// the sphere wins free of your control!
	dir = rnd(8);
      default:
	rmsp(j, x, y); // i THINK that Get is ok while a Query is open
	newsphere(x + diroffx[dir], y + diroffy[dir], dir, len, j);
      }
    }
  }
  MemHandleUnlock(vh);
}

// don't leave them hanging about for the next player...
// well, it would be fun in a cruel sort of way
void clearspheres()
{
  UInt recindex = SPHERES_RECORD;
  VoidHand vh;
  VoidPtr p;
  //ULong uniqueID = REC_SPHERES;
  Int size;

  size = sizeof(struct sphere) * MAXSPHERE;
  //if (DmFindRecordByID(iLarnDB, uniqueID, &recindex)) return;
  if (DmNumRecords(iLarnDB) <= recindex) return;
  vh = DmGetRecord(iLarnDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return;
  DmSet(p, 0, size, 0);  // set it all to 0.  i think that will work.
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnDB, recindex, true);
}
