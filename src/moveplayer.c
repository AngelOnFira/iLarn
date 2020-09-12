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


// Passage types: rock, tomb, dead end, corner, straight, 3way, 4way, other.
#define PA_ROCK     0
#define PA_TOMB     1
#define PA_CULDESAC 2 
#define PA_CORNER   3
#define PA_3WAY     4
#define PA_4WAY     5
#define PA_STRAIGHT 6
#define PA_OPEN     7 
static Boolean bordering_unknown(Short px, Short py) SEC_4;
static Short passage_type(Short px, Short py) SEC_4;
static void frob_dir(Short x, Short y, Short *diag_dir, Short *next_dir) SEC_4;

/*
  showplayer()
  
  subroutine to show where the player is on the screen
  cursor values start from 1 up
  */
Short oldx;
Short oldy;
Short yrepcount;
Boolean nomove;
Short lastpx;
Short lastpy;
void unmoveplayer()
{
  Short x = env->playerx, y = env->playery;
  env->playerx = lastpx;  
  env->playery = lastpy;
  showcell(env->playerx, env->playery);
  showplayer(env->playerx, env->playery);
  update_screen_cell(x, y); // also redraw the place you were (door)
  refresh();
}
void showplayer(Short playerx, Short playery)
{
  oldx = playerx;  
  oldy = playery;
  setscreen(playerx, playery, '@');
}


/*
  moveplayer(dir)
  
  subroutine to move the player from one room to another
  returns false if can't move that way, or hit a monster, or on an object,
                   or some other freaky thing happened
  else returns true
  nomove is set to 1 to stop the next move (inadvertent monsters hitting
  players when walking into walls) if player walks off screen or into wall
  */
// xxx shouldn't I just use externs?
Short diroffx[] = { 0,  0, 1,  0, -1,  1, -1, 1, -1 };
Short diroffy[] = { 0,  1, 0, -1,  0, -1, -1, 1,  1 };
extern LarnPreferenceType my_prefs;

/* from = present room #  direction = [1-north]
   [2-east] [3-south] [4-west] [5-northeast]
   [6-northwest] [7-southeast] [8-southwest]
   if direction=0, don't move--just show where he is */
// Returns 0 on suceess, otherwise a code saying why we stopped
// 1 = you've reached a 0/MAX boundary
// 2 = there's a wall there and you can't walk through it
// 3 = you went up to the ground level
// 4 = there's a monster there
// 5 = you're on an object
// (6 = entering intersection)
// (7 = hit while running)
// (8 = entering unknown territory)
Short moveplayer(Short dir)
{
  Short k,m,i,j;
  Boolean exploring;

  // If you are confused -- you generally move in a random direction
  if (env->cdude[CONFUSE]) 
    if (env->cdude[LEVEL] < rnd(30))
      dir = rund(9);

  // calculate new x,y as k,m; see if they're reasonable.
  k = env->playerx + diroffx[dir];
  m = env->playery + diroffy[dir];

  exploring = bordering_unknown(k, m);
  
  if (k < 0 || k >= MAXX || m < 0 || m >= MAXY) { 
    // This is as far as you can move.
    nomove = true; // "don't count next iteration as a move" ??
    yrepcount = 0; // hello, what is this? oh like 'count' in iRogue
    return 1;
  }
  
  i = env->item[k][m];
  j = env->mitem[k][m];
  
  if (i == OWALL && (env->cdude[WTW] == 0 || env->level == 0)) { 
    // You hit a wall, and you can't walk through it.
    nomove = true;
    yrepcount = 0;
    return 2;
  }		
  
  if (k == 33 && m == MAXY-1 && env->level == 1) {
    // You have found the exit up from dungeon level 1.
    newcavelevel(0); 
    // find the dungeon entrance on level 0, put yourself there.
    //    for (k = 0 ; k < MAXX ; k++) {
    //      for (m = 0; m < MAXY ; m++) {
    //    env->playerx = MAXX_0/2;
    //    env->playery = MAXY_0/2;
    //    for (k = 1 ; k <= MAXX_0 ; k++) {
    //      for (m = 1; m <= MAXY_0 ; m++) {
    for (k = 0 ; k < MAXX ; k++) {
      for (m = 0; m < MAXY ; m++) {
	if (env->item[k][m] == OENTRANCE) { 
	  env->playerx = k;
	  env->playery = m;
	  k = MAXX; m = MAXY; // break, break.
	}
      }
    }
    /* deleted, ws, 08.08.93	positionplayer();  */ 
    clear_visible();
    check_player_position(env->playerx, env->playery, false);
    recalc_screen(); 
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh();
    print_stats();
    //	  env->dropflag = true; /* no stupid questions */ // eh?
    return 3;
  }

  if (j > 0) { 	
    /* You hit a monster. */
    hitmonster(k, m, false);
    yrepcount = 0;
    return 4; 
  }
  
  lastpx = env->playerx;
  lastpy = env->playery;
  env->playerx = k;
  env->playery = m;

  update_screen_cell(lastpx, lastpy);


  if (i && i != OTRAPARROWIV
      && i != OIVTELETRAP
      && i != OIVDARTRAP 
      && i != OIVTRAPDOOR
      && i != OWALL) {
    /* You're on an item, other than like a trap or a wall. */
    yrepcount = 0;
    return 5;
  }

  // Looks like you have successfully moved, without anything freaky.
  if (exploring && my_prefs.coward_on)
    return 8;
  return 0;

}



// I should swipe some code from Moria, it has a smarter run.
void a_better_run(Short dir)
{
  Short i = 0;
  Short x, y, xold, yold;
  Short x2, y2, count, dir2=0;
  //  Short moves = 0;
  Boolean walk_thru_walls;
  while (!i) {
    xold = env->playerx;
    yold = env->playery;
    dir2 = 0;
    walk_thru_walls = (env->cdude[WTW] > 0);
    // be clever about corners
    if (!walk_thru_walls && !env->cdude[CONFUSE]) {
      x2 = env->playerx + diroffx[dir];
      y2 = env->playery + diroffy[dir];
      if (diroffx[dir] == 0 || diroffy[dir] == 0) {
	// direction is not a diagonal
	if (!bordering_unknown(x2, y2)) {
	  // the square we WERE going to move onto is KNOWN.
	  if (passage_type(x2, y2) == PA_CORNER) {
	    // the square we WERE going to move onto is a CORNER.
	    if (!env->item[x2][y2] && !env->mitem[x2][y2]) {
	      // there is NO object or monster there; let's SKIP it.
	      // dir2 = non-diagonal direction, dir = diagonal direction
	      frob_dir(x2, y2, &dir, &dir2);
	      //	      message("skip");
	    }
	  }
	}
      }
    }
    i = moveplayer(dir); // 0 on success
    if (dir2 != 0) dir = dir2; // undiagonalize the special cornerskipping case

    x = env->playerx;
    y = env->playery;

    ////// a long bunch of ifs /////////////////////
    if (!walk_thru_walls && i < 3) {
      // do special things: (0) stop at side-passages, and (1,2) follow turns
      if (dir == NORTH || dir == SOUTH || dir == EAST || dir == WEST) {
	if (i == 0) { // "stop at side-passages"
	  // if there is NO wall in FRONT of you
	  // but you have a passage to the left or right or both, STOP.
	  //  [***]
	  // <- . ->
	  //   * 

	  // you're heading DIR (e)
	  // opening to perpdir (s), wall to at least one perpdir diagonal
	  x2 = x + diroffx[dir];
	  y2 = y + diroffy[dir];
	  // is 'ahead' a legal space and not a wall?  if so then...
	  if (x2 >= 0 && x2 < MAXX && y2 >= 0 && y2 < MAXY &&
	      env->item[x2][y2] != OWALL) { 
	    // if ahead of you is a TRAP that you KNOW ABOUT, then STOP.
	    // XXXX well, I will stop when I figure out how to tell I know.
	    // first consider north/south
	    if ( dir == NORTH || dir == SOUTH ) {
	      // is there an east passage.. in other words...
	      if ( (x+1 <= MAXX-1) &&
		   (env->item[x+1][y] != OWALL) ) {
		// ... is there a SE or NE wall, but no E wall.
		if ( (y==MAXY-1 || env->item[x+1][y+1] == OWALL) ||
		     (y==0      || env->item[x+1][y-1] == OWALL)) {
		  // yes.  stop.
		  i = 6;
		}
	      }
	      // is there a west passage.. in other words...
	      if ( (x-1 >= 0)      &&
		   (env->item[x-1][y] != OWALL)) {
		// ... is there a SW or NW wall, but no W wall.
		if ( (y==MAXY-1 || env->item[x-1][y+1] == OWALL) ||
		     (y==0      || env->item[x-1][y-1] == OWALL)) {
		  i = 6;
		}
	      }
	    } // done considering north/south
	    // now consider east/west
	    if ( dir == EAST || dir == WEST ) {
	      // is there an south passage.. in other words...
	      if ( (y+1 <= MAXY-1) &&
		   (env->item[x][y+1] != OWALL) ) {
		// ... is there a SE or SW wall, but no S wall.
		if ( (x==MAXX-1 || env->item[x+1][y+1] == OWALL) ||
		     (x==0      || env->item[x-1][y+1] == OWALL)) {
		  i = 6;
		}
	      }
	      // is there a north passage... in other words...
	      if ( (y-1 >= 0)      &&
		   (env->item[x][y-1] != OWALL)) {
		// ... is there a NE or NW wall, but no N wall.
		if ( (x==MAXX-1 || env->item[x+1][y-1] == OWALL) ||
		     (x==0      || env->item[x-1][y-1] == OWALL)) {
		  i = 6;
		}
	      }
	    } // done considering east/west
	  }
	} // DONE WITH i=0 (the "STOP if you see a side passage" trick)
	else { // "follow turns"
	  // i==1 or i==2 You tried to walk thru a wall, or off the edge.
	  if (  (x==MAXX-1 || y==MAXY-1 || env->item[x+1][y+1] == OWALL) &&
		(x==MAXX-1 || y==0      || env->item[x+1][y-1] == OWALL) &&
		(x==0      || y==MAXY-1 || env->item[x-1][y+1] == OWALL) &&
		(x==0      || y==0      || env->item[x-1][y-1] == OWALL) ) {
	    
	    if (dir == NORTH || dir == SOUTH) {
	      y2 = y + diroffy[dir];
	      count = 0;
	      x2 = x + 1;
	      if ((x2 <= MAXX-1) && (env->item[x2][y] != OWALL)) {
		count++; // legal and clear space
		dir2 = EAST;
	      }
	      x2 = x - 1;
	      if ((x2 >= 0) && (env->item[x2][y] != OWALL)) {
		count++; // legal and clear space
		dir2 = WEST;
	      }
	      if (count == 1) {
		dir = dir2;
		i = 0;
	      }
	    } else if (dir == EAST || dir == WEST) {
	      x2 = x + diroffx[dir];
	      if ( (x2 < 0) || (x2 > MAXX-1) || (env->item[x2][y]==OWALL)) {
		// wall ahead of us
		count = 0;
		y2 = y + 1;
		if ((y2 <= MAXY-1) && (env->item[x][y2] != OWALL)) {
		  count++; // legal and clear space
		  dir2 = SOUTH;
		}
		y2 = y - 1;
		if ((y2 >= 0) && (env->item[x][y2] != OWALL)) {
		  count++; // legal and clear space
		  dir2 = NORTH;
		}
		if (count == 1) {
		  dir = dir2;
		  i = 0;
		}
	      }
	    }
	    // Now we have to do one more move!  otherwise you get stuck
	    // it's the only REAL move, I don't think I need to movemonst first
	    i = moveplayer(dir); // 0 on success
	    // hopefully this won't interfere with the other stop-thingy
	  } // end diagonals-all-walls
	} // end else i==1 or i==2 (follow turns)
      } // end if n/s/e/w
    } // end if i < 3 and not walk-through-walls
    ////// (end of the long bunch of ifs) /////////////////////

    // this used to be before the "if i < 3" stuff

    // You used a turn if: legal, or hit monster, or item, or intersect, or..
    if (i == 0 || i >= 4) {
      if (i != 5) { // on a !trap!wall item ==> DON'T make time tick... YET.
	if (env->cdude[HASTEMONST]) 
	  movemonst();  
	movemonst(); 
	//      randmonst(); 
	regen(true);  // we might drop things here.
	// regen(false, env);  // we might drop things here.
      }
    }

    if (env->hitflag) i = 7; // has player been hit while running
    if (env->dropflag) i = 5; // has clumsy player dropped something

    if (i == 0) {
      // Stop run-through-walls if this was a transition.
      if (env->cdude[WTW] > 0) {
	if ((env->item[xold][yold] != OWALL && env->item[x][y] == OWALL) ||
	    (env->item[xold][yold] == OWALL && env->item[x][y] != OWALL)) {
	  break;
	}
      } else if (walk_thru_walls) {
	// player just LOST the ability to walk through walls
	// (hopefully not in an inconvenient place.)
	message("You feel yourself solidify...");
	break;
      }
      check_player_position(env->playerx, env->playery, true);
      showcell(env->playerx, env->playery);
      setscreen(env->playerx, env->playery, '@');
      refresh();
    }

  } // end WHILE i
  check_player_position(env->playerx, env->playery, false);
  showcell(env->playerx, env->playery);
  setscreen(env->playerx, env->playery, '@');
}


static Short passage_type(Short px, Short py)
{
  Short x = px, y = py, i, w;
  Boolean walls[4];
  if (x == 0 || y == 0 || x == MAXX-1 || y == MAXY-1 ||
      env->item[x][y] == OWALL) {
    // you are in a rock (or maybe dungeon entrance on 1).  hope you're WTW.
    return PA_ROCK;
  }
  if ( env->item[x+1][y+1] == OWALL &&
       env->item[x+1][y-1] == OWALL &&
       env->item[x-1][y+1] == OWALL &&
       env->item[x-1][y-1] == OWALL ) {
    // the corners around x,y are walls
    // now count the number of non-corner walls
    walls[0] = (env->item[x-1][y] == OWALL);
    walls[1] = (env->item[x+1][y] == OWALL);
    walls[2] = (env->item[x][y-1] == OWALL);
    walls[3] = (env->item[x][y+1] == OWALL);
    for (i = 0, w = 0; i < 4 ; i++)
      if (walls[i]) w++;
    if (w != 2) return w+1;// PA_TOMB, PA_CULDESAC; PA_3WAY, PA_4WAY
    if ((walls[0] && walls[1]) || (walls[2] && walls[3]))
      return PA_STRAIGHT;
    else
      return PA_CORNER;
  } else
    return PA_OPEN;
}



/*
	if (i == 0 && !exploring) {
	  // We did not stop at a side passage.  Current square was known.
	  x2 = x + diroffx[dir];
	  y2 = y + diroffy[dir];
	  if (!bordering_unknown(x2, y2)) {
	    Short x3, y3;
	    // NEXT MOVE's square is also known, so if it's a corner, SKIP it.
	    // we skip it by moving to the appropriate diagonal.
	    x3 += diroffx[dir];
	    y3 += diroffy[dir];
	    if ((x3 < MAXX-1 && x3 > 0) && (y3 < MAXY-1 && y3 > 0) &&
		env->item[x3][y3] == OWALL) {
	      // NEXT NEXT MOVE's square is a WALL (and not the edge).
	      // 
	    }
	  }
	}

*/


// Sorry this is so incomprehensible.
// Its purpose is to tell you how to move diagonally around a corner, AND
// set up the proper direction for your next move after that.
static void frob_dir(Short x, Short y, Short *diag_dir, Short *next_dir)
{
  Short orig_dir = *diag_dir;
  Short cheat = 0;
  // x,y is the position of a 'corner' cell to be skipped
  // diag_dir is initially the direction we are moving to get to it.
  // (caller promises this is not a diagonal direction)
  // but will be set to the diagonal direction we move to avoid it.
  // next_dir is the direction we want to be moving after avoiding it
  // (the passable direction perpendicular to original value of diag_dir).
  switch (orig_dir) {
  case SOUTH: // next_dir will be e or w, diag_dir will be se or sw
    cheat = 2;
  case NORTH: // next_dir will be e or w, diag_dir will be ne or nw
    // is the clear space to the E or to the W..
    *next_dir = (env->item[x+1][y] != OWALL) ? EAST : WEST;
    *diag_dir = ((*next_dir == EAST) ? NORTHEAST : NORTHWEST) + cheat;
    break;
  case WEST:  // next_dir will be n or s, diag_dir will be nw or sw
    cheat = 1;
  case EAST:  // next_dir will be n or s, diag_dir will be ne or se
    // is the clear space to N or to S.
    *next_dir = (env->item[x][y+1] != OWALL) ? SOUTH : NORTH;
    *diag_dir = ((*next_dir == NORTH) ? NORTHEAST : SOUTHEAST) + cheat;
    break;
  }
}




// adapted from 'showcell'
static Boolean bordering_unknown(Short x, Short y)
{
  Short i, j;
  Short minx, maxx, miny, maxy;
  if (env->cdude[BLINDCOUNT])  return true; // see nothing if blind!
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

  for (j = miny ; j <= maxy ; j++)
    for (i = minx ; i <= maxx ; i++)
      if ((env->know[i][j] & 1) == 0)	// if it's 0 or 2, we don't know it
	return true;
  return false;
}

/*
static Boolean bordering_unknown(Short px, Short py)
{
  // Are any of the 3-8 tiles around this square 'unknown'
  Short x, y;
  for (x = px - 1 ; x <= px + 1 ; x++) {
    if (x < 0 || x >= MAXX) continue;
    for (y = py - 1 ; y <= py + 1 ; y++) {
      if (y < 0 || y >= MAXY) continue;
      if (env->know[x][y] == 0) // Hey, what if it's 2.
	return true;
    }
  }
  return false;
}
*/
