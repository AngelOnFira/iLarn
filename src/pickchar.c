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

typedef struct CHAR_TAG {
  Short strength;
  Short intelligence;
  Short wisdom;
  Short constitution;
  Short dexterity;
  Short charisma;
  Short hitpts;
  Short wield;
  Short wear;
  Short n_spells;
  Short spellmax;
  Short *init_arr; // will this work right?
  //  Char *name;
} CHARTYP;

//#define Si       static int 
//#define Si static Short
#define Si Short
#define Rbt      (0x800)    /* call rund() ? */
#define Rb6      (Rbt | 6)
#define Rms      (0x7FF)    /* mask to delete Rbt */

//             (object [arg])*  NO_OBJ (spell)* S_NON
Si init_o[] = { OPOTION, Rb6,
		OPOTION, Rb6,
		NO_OBJ,            S_MLE,        S_NON, S_NON}; // 7+1
Si init_w[] = { OPOTION, P_TRS_F,
		OSCROLL, Rb6,
		OSCROLL, Rb6,
		NO_OBJ,            S_MLE, S_CHM, S_NON}; // 10
Si init_k[] = { OSTUDLEATHER, 1,
		OPOTION, Rb6,
		NO_OBJ,            S_SSP,        S_NON,S_NON}; // 7+1
Si init_e[] = { OLEATHER, 1,
		OSCROLL, Rb6,
		NO_OBJ,            S_PRO,        S_NON,S_NON}; // 7+1
Si init_r[] = { ODAGGER, 1,
		OLEATHER, 1,
		OSCROLL, R_STEAL,
		NO_OBJ,            S_MLE,        S_NON,S_NON}; // 9+1
//Si init_r[] = { ODAGGER, 1,
//		OLEATHER, 1,
//		ODIAMOND, 50,
//		NO_OBJ,            S_MLE,        S_NON,S_NON}; // 9+1
Si init_g[] = { OLEATHER, Rb6,
		ODAGGER, Rb6,
		NO_OBJ,            S_PRO, S_MLE, S_NON}; // 8
Si init_d[] = { OSPEAR, 0,
		NO_OBJ,            S_PRO,        S_NON,S_NON}; // 5+1
Si init_b[] = { OLANCE, 0,
		NO_OBJ,                          S_NON}; // 4
Si init_l[] = { NO_OBJ,            S_STP,        S_NON,S_NON}; // 3+1

const CHARTYP chartyps[] = {
  { 18,  4,  6, 16,  6,  4,   16, -1, -1, 1, 1, init_o},  // "Ogre"      
  {  8, 16, 16,  6,  6,  8,    8, -1, -1, 2, 2, init_w},  // "Wizard"    
  { 14, 12,  4, 12,  8,  3,   14, -1,  0, 1, 1, init_k},  // "Klingon "  
  {  8, 14, 12,  8,  8, 14,    8, -1,  0, 1, 2, init_e},  // "Elf "      
  {  8, 12,  8, 10, 14,  6,   12,  0,  1, 1, 1, init_r},  // "Rogue "    
  { 12, 12, 12, 12, 12, 12,   10, -1, -1, 1, 1, init_g},  // "Geek"      
  { 16,  6,  8, 16,  4,  4,   12,  0, -1, 1, 1, init_d},  // "Dwarf "    
  {  3,  3,  3,  3,  3,  3,    1,  0, -1, 0, 0, init_b},  // "Rambo "    
  {  6, 18, 12,  4,  4,  4,    1, -1, -1, 0, 9, init_l}	  // "Rincewind "
};
//  {  8, 12,  8, 10, 14,  6,   12,  1,  0, 1, 1, init_r, "Rogue "    },


void pick_char(Short i)
{
  //  Short i, j, inx;
  Short j, inx;
  Short *pi;

  // Initialize the inventory (it has NINVT==26 max items)
  // this couldn't possbly be right: "for (j = 0 ; i < NINVT ; i++)"
  for (j = 0 ; j < NINVT ; j++)  
    env->iven[j] = 0;
  env->cdude[SHIELD]       = -1;
  env->cdude[LEVEL]        = 1;     /* player starts at level one */
  env->cdude[REGENCOUNTER] = 16;
  env->cdude[ECOUNTER]     = 96;    /* start regeneration correctly */

  // this should all work ok (char_class is char[20]) // not anymore.  haha!
  //  StrCopy(env->char_class, chartyps[i].name);
  env->char_class = i; // remember this when printing top ten scores...
  env->cdude[STRENGTH]     = chartyps[i].strength;
  env->cdude[INTELLIGENCE] = chartyps[i].intelligence;
  env->cdude[WISDOM]       = chartyps[i].wisdom;
  env->cdude[CONSTITUTION] = chartyps[i].constitution;
  env->cdude[DEXTERITY]    = chartyps[i].dexterity;
  env->cdude[CHARISMA]     = chartyps[i].charisma;
  env->cdude[HPMAX]        = chartyps[i].hitpts;
  env->cdude[HP]           = env->cdude[HPMAX];

  // init_arr looks like this:
  //  (object [arg])*  NO_OBJ (spell)* S_NON
  // convert this to inventory and spells.
  // hell, let's try it as-is and see if it seems to "work".
  for (pi = chartyps[i].init_arr, inx=0 ;
       (j = pi[0]) != NO_OBJ ;
       pi +=2, inx++)
    {
      env->iven[inx] = j;
      j = pi[1];
      if ( j & Rbt )
	j = rund( j & Rms );
      env->ivenarg[inx] = j;
    }
  for ( ; (inx = pi[0]) != S_NON ; pi++) {
    env->spelknow[inx] = 1;
  }; 
  // Ok.  Perhaps that worked.

  env->cdude[WIELD]        = chartyps[i].wield;
  env->cdude[WEAR]         = chartyps[i].wear;
  env->cdude[SPELLMAX]     = chartyps[i].spellmax;
  env->cdude[SPELLS]       = chartyps[i].n_spells;

  if ( env->cdude[SPELLMAX] == 9 ){     /* Rincewind: run fast, live longer */
    env->cdude[AGGRAVATE]     = 99999L;
    env->cdude[INVISIBILITY]  = 99999L;
    env->cdude[HASTESELF]     = 99999L;
    env->cdude[CANCELLATION]  = 99999L;
    env->cdude[WTW]           = 99999L;
    env->cdude[TIMESTOP]      = 45;
    env->cdude[AWARENESS]     = 99999L;
  };

} /* end pick_char */

