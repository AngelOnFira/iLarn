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

extern Boolean keep_fighting;

/*
 * subroutine to regenerate player hp and spells
 */
void regen(Boolean show_stats)
{
  Short i;
  Boolean flag = false;
  Long *d;

  //  d = c; // Was there some point to this?
  d = env->cdude;

  if (d[TIMESTOP])  { 
    if(--d[TIMESTOP]<=0 && show_stats) 
      print_stats();
    return; 
  }	/* for stop time spell */

  if (d[STRENGTH]<3) { 
    d[STRENGTH]=3; 
    flag = true; 
  }
  if ((d[HASTESELF]==0) || ((d[HASTESELF] & 1) == 0))
    env->gtime++;

  if (d[HP] != d[HPMAX])
    if (d[REGENCOUNTER]-- <= 0)	/*regenerate hit points	*/
      {
	d[REGENCOUNTER] = 22 + (d[HARDGAME]<<1) - d[LEVEL];
	if ((d[HP] += d[REGEN]) > d[HPMAX])  
	  d[HP] = d[HPMAX];
	flag = true;
      }

  if (d[SPELLS] < d[SPELLMAX])		/*regenerate spells	*/
    if (d[ECOUNTER]-- <= 0) {
      d[ECOUNTER] = 100 + 4 * (d[HARDGAME]-d[LEVEL]-d[ENERGY]);
      d[SPELLS]++;	
      flag = true;
    }

  if (d[HERO])	
    if (--d[HERO]<=0) { 
      for (i=0; i<6; i++) 
	d[i] -= 10; 
      flag = true; 
    }
  if (d[COKED])
    if (--d[COKED]<=0) {
      for (i=0; i<6; i++)
	d[i] -= 34;
      flag = true;
    }
  if (d[ALTPRO])	
    if (--d[ALTPRO]<=0) { 
      d[MOREDEFENSES]-=3; 
      flag = true; 
    }
  if (d[PROTECTIONTIME])	
    if (--d[PROTECTIONTIME]<=0) {
      d[MOREDEFENSES]-=2; 
      flag = true; 
    }
  if (d[DEXCOUNT])
    if (--d[DEXCOUNT]<=0)	{ 
      if ( (d[DEXTERITY]-=3) < 3 )
	d[DEXTERITY] = 3;
      flag = true; 
    }
  if (d[STRCOUNT])
    if (--d[STRCOUNT]<=0)	{ 
      d[STREXTRA]-=3; 
      flag = true; 
    }
  if (d[BLINDCOUNT])
    if (--d[BLINDCOUNT]<=0) { 
      if (show_stats)
	message("The blindness lifts  "); 
      //      beep(); 
    }
  if (d[CONFUSE])	
    if (--d[CONFUSE]<=0) { 
      if (show_stats)
	message("You regain your senses"); 
      //      beep(); 
    }
  if (d[GIANTSTR])
    if (--d[GIANTSTR]<=0) { 
      d[STREXTRA] -= 20; 
      flag = true; 
    }
  if (d[CHARMCOUNT])     if ((--d[CHARMCOUNT]) <= 0)   flag = true;
  if (d[INVISIBILITY])	 if ((--d[INVISIBILITY]) <= 0) flag = true;
  if (d[CANCELLATION])   if ((--d[CANCELLATION]) <= 0) flag = true; 
  if (d[WTW])            if ((--d[WTW]) <= 0)          flag = true; 
  if (d[HASTESELF])      if ((--d[HASTESELF]) <= 0)    flag = true;
  if (d[AGGRAVATE])
    --d[AGGRAVATE]; 
  if (d[SCAREMONST])     if ((--d[SCAREMONST]) <= 0)   flag = true; 
  if (d[STEALTH])	 if ((--d[STEALTH]) <= 0)      flag = true; 
  if (d[AWARENESS])
    if (env->cdude[ORB] != 2)
      --d[AWARENESS];
  if (d[HOLDMONST])      if ((--d[HOLDMONST]) <= 0)    flag = true;
  if (d[HASTEMONST])		
    --d[HASTEMONST];
  if (d[FIRERESISTANCE]) if ((--d[FIRERESISTANCE]) <= 0) flag = true;
  if (d[GLOBE])
    if (--d[GLOBE]<=0) { 
      d[MOREDEFENSES] -= 10; 
      flag = true; 
    }
  if (d[SPIRITPRO])      if (--d[SPIRITPRO] <= 0)       flag = true;
  if (d[UNDEADPRO])	 if (--d[UNDEADPRO] <= 0)       flag = true;
  if (d[HALFDAM])
    if (--d[HALFDAM]<=0)  { 
      if (show_stats)
	message("You now feel better "); 
      //      beep(); 
    }
  if ( d[SEEINVISIBLE] && has_object(OAMULET) &&
       (--d[SEEINVISIBLE]<=0) ){
    unsee_invisible();
    if (show_stats)
	message("You feel your vision return to normal"); 
    //    beep(); 
  }
	
  if (d[ITCHING]) {
    if (d[ITCHING]>1)
      if ((d[WEAR]!= -1) || (d[SHIELD]!= -1))
	if (rnd(100)<50) { 		
	  d[WEAR]=d[SHIELD]= -1; 
	  //	  lprcat("\nThe hysteria of itching forces you to remove your armor!"); 
	  if (show_stats)
	    message("Itchy!! You take off your armor."); 
	  //	  beep(); 
	  recalc();  
	  flag = true;
	}
    if (--d[ITCHING]<=0) { 
      if (show_stats)
	message("You feel the irritation subside!"); 
      //      beep(); 
    }
  }
  if (d[CLUMSINESS]) {
    if (d[WIELD] != -1)
      if (d[CLUMSINESS]>1)
	if (env->item[env->playerx][env->playery]==0)/* if nothing there */
	  if (rnd(100)<33) /* drop your weapon */ {
	    drop_object(d[WIELD]);
	    keep_fighting = false;
	  }
	    //	    drop_object(0);//	    drop_object(d[WIELD]); drop_object not implemented
    if (--d[CLUMSINESS]<=0) { 
      if (show_stats)
	message("You now feel less awkward!"); 
      //      beep(); 
    }
  }
  if (flag && show_stats) {
    print_stats();
    spell_stats();
  }
}
