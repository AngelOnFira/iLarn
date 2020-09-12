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

/***************************************************************
                   SEED_RANDOM
 IN:
 nothing
 OUT:
 nothing
 PURPOSE:
 This should be called at the start of each game session BEFORE
 doing anything that generates random dungeon guts.  It will
 seed the random number generator.
   (SysRandom returns a random Int between 0 and 'sysRandomMax'
 and takes one argument which, if nonzero, is used to (re)seed it.)
****************************************************************/
//void seed_random_test();
/*
void seed_random()
{
  SysRandom(TimGetSeconds());
  //  seed_random_test();
}
*/

/* macros to generate random numbers   1<=rnd(N)<=N   0<=rund(N)<=N-1 */
/***************************************************************
                   RND
 IN:
 N = upper bound (inclusive)
 OUT:
 a random number between 1 and N inclusive: [1,N]
 PURPOSE:
 duh
****************************************************************/
Int rnd(Int y)
{
  Int r;
  if (y <= 1) return 1;
  r = SysRandom(0);
  r = (r % y) + 1; // 0 to y-1... add 1... 1 to y.
  return r;
}
/***************************************************************
                   RUND
 IN:
 N = upper bound (exclusive)
 OUT:
 a random number between 0 and N-1 inclusive: [0,N)
 PURPOSE:
 duh
****************************************************************/
Int rund(Int y)
{
  Int r;
  if (y <= 1) return 0;
  r = SysRandom(0);
  r = r % y; // 0 to y-1...
  return r;
}
/***************************************************************
                   GET_RAND
 IN:
 x, y = bounds for the return value
 OUT:
 a random number
 PURPOSE:
 Used to obtain a random number between x and y INCLUSIVE.
****************************************************************/
/*
Int get_rand(Int x, Int y) {
  Int r, t;
  if (x == y) return x;
  if (x > y) {
    t = y; y = x; x = t;
  }
  t = (y-x) + 1;
  r = SysRandom(0);
  r = (r % t) + x;
  return r;
}
*/
