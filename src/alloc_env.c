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

// will this work?
Short hitp[MAXX][MAXY];
Long  iarg[MAXX][MAXY];
Char  item[MAXX][MAXY];
Char  know[MAXX][MAXY];
Char mitem[MAXX][MAXY];
Boolean stealth[MAXX][MAXY];
Boolean moved[MAXX][MAXY];

static void clear_env_globals(struct everything *env) SEC_4;

// Free the giant 'env' struct and the stuff it points to.
#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
void free_env(struct everything *env) {
  VoidHand h;
  /*
  Short i;
  for (i = 0 ; i < MAXX ; i++) {
    free_me(env->hitp[i]);
    free_me(env->iarg[i]);
    free_me(env->item[i]);
    free_me(env->know[i]);
    free_me(env->mitem[i]);
    free_me(env->stealth[i]);
    free_me(env->moved[i]);
  }
  */
  free_me(env->cdude);
  free_me(env->beenhere);
  free_me(env->genocided);
  //  free_me(env->char_class);
  free_me(env->iven);
  free_me(env->spelknow);
  free_me(env->ivenarg);
  /*
  free_me(env->hitp);
  free_me(env->iarg);
  free_me(env->item);
  free_me(env->know);
  free_me(env->mitem);
  free_me(env->stealth);
  free_me(env->moved);
  */
}

// Allocate the giant 'env' struct of global stuff.
// Call this once when you start the app.. if you die, just zero it out
//
// It would be better to have these things be global variables
// or database records.  The heap is like the first thing to run out.
// The main hogs are the 2-dimensional arrays:
// iarg; hitp; item, know, mitem; stealth, moved  (in order of size)
void alloc_env(struct everything *env) {
  //  Short i;
  // perhaps I should check that the result is non-nil.
  env->cdude = (Long *) md_malloc(sizeof(Long) * 100);
  get_taxes(env); // read the high-score records
  env->beenhere = (Boolean *) md_malloc(sizeof(Boolean)*(MAXLEVEL+MAXVLEVEL));
  env->genocided = (Boolean *) md_malloc(sizeof(Boolean)*66);
  env->iven = (Char *) md_malloc(sizeof(Char) * NINVT);
  env->spelknow = (Boolean *) md_malloc(sizeof(Boolean) * SPNUM);
  env->potion_known = (Boolean *) md_malloc(sizeof(Boolean) * MAXPOTION);
  env->potion_known[P_CURED] = true; // cure dianthroritis
  env->scroll_known = (Boolean *) md_malloc(sizeof(Boolean) * MAXSCROLL);
  env->ivenarg = (Short *) md_malloc(sizeof(Short) * NINVT);
  // everything that is dungeon-shaped goes here:
  env->hitp = hitp;
  env->iarg = iarg;
  env->item = item;
  env->know = know;
  env->mitem = mitem;
  env->stealth = stealth;
  env->moved = moved;
  clear_env_globals(env);
  /*
  env->hitp = (Short **) md_malloc(sizeof(Short *) * MAXX);
  env->iarg = (Long **) md_malloc(sizeof(Long *) * MAXX);
  env->item = (Char **)  md_malloc(sizeof(Char *)  * MAXX);
  env->know = (Char **)  md_malloc(sizeof(Char *)  * MAXX);
  env->mitem = (Char **) md_malloc(sizeof(Char *)  * MAXX);
  env->stealth = (Boolean **) md_malloc(sizeof(Boolean *) * MAXX);
  env->moved = (Boolean **) md_malloc(sizeof(Boolean *) * MAXX);
  for (i = 0 ; i < MAXX ; i++) {
    env->hitp[i] = (Short *) md_malloc(sizeof(Short) * MAXY);
    env->iarg[i] = (Long *) md_malloc(sizeof(Long) * MAXY);
    env->item[i] = (Char *)  md_malloc(sizeof(Char)  * MAXY);
    env->know[i] = (Char *)  md_malloc(sizeof(Char)  * MAXY);
    env->mitem[i] = (Char *) md_malloc(sizeof(Char)  * MAXY);
    env->stealth[i] = (Boolean *) md_malloc(sizeof(Boolean) * MAXY);
    env->moved[i] = (Boolean *) md_malloc(sizeof(Boolean) * MAXY);
  }
  */
}


// Zero out the giant 'env' struct after you die and restart.
extern Char *old_messages[SAVED_MSGS];
extern Short last_old_message_shown;
void clear_env(struct everything *env) {
  Short i;
  for (i = 0 ; i < 100 ; i++)
    env->cdude[i] = 0;
  get_taxes(env); // read the high-score records
  for (i = 0 ; i < MAXLEVEL+MAXVLEVEL ; i++)
    env->beenhere[i] = 0;
  for (i = 0 ; i < 66 ; i++)
    env->genocided[i] = 0;
  for (i = 0 ; i < NINVT ; i++) {
    env->iven[i] = 0;
    env->ivenarg[i] = 0;
  }
  for (i = 0 ; i < SPNUM ; i++)
    env->spelknow[i] = 0;
  for (i = 0 ; i < MAXPOTION ; i++)
    env->potion_known[i] = 0;
  env->potion_known[P_CURED] = true; // cure dianthroritis
  for (i = 0 ; i < MAXSCROLL ; i++)
    env->scroll_known[i] = 0;
  clear_env_globals(env);
  env->gtime = 0;
  env->playerx = 0;
  env->playery = 0;
  env->level = 0;
  // also clear the message log
  for (i = 0 ; i < SAVED_MSGS ; i++)
    old_messages[i][0] = '\0';
  // the log is all null strings.. so (we pretend) all of it has been shown
  last_old_message_shown = SAVED_MSGS - 1;
}

static void clear_env_globals(struct everything *env) {
  Short i,j;
  for (i = 0 ; i < MAXX ; i++) {
    for (j = 0 ; j < MAXY ; j++) {
      env->hitp[i][j] = 0;
      env->iarg[i][j] = 0;
      env->item[i][j] = 0;
      env->know[i][j] = 0;
      env->mitem[i][j] = 0;
      env->stealth[i][j] = 0;
    }
  }
}
