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

// oaltar
static void ohear() SEC_2;
// othrone
// odeadthrone
// ochest
// ofountain
//    needs fntchange, fch
static void fntchange(Boolean how) SEC_2;
//static void fch(Boolean how_up, Long *x );
/////////////////////////////////////////////////////////////////////
#define FOR_SHORT 26
const Char fortune_short[FOR_SHORT][38] = {
  "gem value = gem * 2 ^ perfection",
  "don't pry into the affairs of others",
  "beware of the gusher!",
  "some monsters are greedy",
  "nymphs have light fingers",
  "try kissing a disenchantress!",
  "hammers and brains don't mix",
  "ask the genie",
  "be sure to pay your taxes",
  "are monsters afraid of something?",
  "some dragons can fly",
  "dos thou strive for perfection?",
  "a level 25 player casts like crazy!",
  "difficulty affects regeneration",
  "don't fall into a bottomless pit",
  "watch out for trap doors",
  "dexterity allows you to carry more",
  "take the express",
  "identify things before you use them",
  "who is tougher than a demon prince?",
  "Slayer has a grudge",
  "you'll do better if you stop playing",
  "a bad line helps you through doors",
  "play on a VT100 via a 100 m V.24 line",
  "control praying",
  "all men are equal, but ..."
};
#define FOR_LONG 18
  /*
const Char fortune_long[FOR_LONG*2][38] = {
  "sitting down can have unexpected", "results",
  "drinking can be hazardous to your", "health",
  "what does a potion of cure", "dianthroritis taste like?",
  "hit point gain/loss when raising a", "level depends on constitution",
  "patience is a virtue, unless your", "daughter dies",
  "what can the Eye of Larn see for its", "guardian?",
  "energy rings affect spell", "regeneration",
  "control of the pesty spirits is most", "helpful",
  "you can get 2 points of WC for the", "price of one",
  "never enter the dungeon naked!  the", "monsters will laugh at you!",
  "did someone put itching powder in", "your armor?",
  "avoid opening doors.  you never know", "what's on the other side.",
  "the greatest weapon in the game has", "not the highest Weapon Class",
  "you can't buy the most powerful", "scroll",
  "there's more than one way through a", "wall",
  "Try Dealer McDope's for all your", "recreational needs!",
  "wonderful wands prevent you from", "falling",
  "fight the LRS and they'll call you", "names"
};
  */
const Char fortune_long[FOR_LONG][65] = {
  "sitting down can have unexpected results",
  "drinking can be hazardous to your health",
  "what does a potion of cure dianthroritis taste like?",
  "hit point gain/loss when raising a level depends on constitution",
  "patience is a virtue, unless your daughter dies",
  "what can the Eye of Larn see for its guardian?",
  "energy rings affect spell regeneration",
  "control of the pesty spirits is most helpful",
  "you can get 2 points of WC for the price of one",
  "never enter the dungeon naked!  the monsters will laugh at you!",
  "did someone put itching powder in your armor?",
  "avoid opening doors.  you never know what's on the other side.",
  "the greatest weapon in the game has not the highest Weapon Class",
  "you can't buy the most powerful scroll",
  "there's more than one way through a wall",
  "Try Dealer McDope's for all your recreational needs!",
  "wonderful wands prevent you from falling",
  "fight the LRS and they'll call you names"
};
/////////////////////////////////////////////////////////////////////
/*
 *  eat a fortune cookie, see a message (unless blind)
 */
extern Boolean itsy_on; // just for the ocookie...
void ocookie()
{
  Short i;
  i = rund(FOR_SHORT+FOR_LONG);
  if (itsy_on || i >= FOR_LONG || env->cdude[BLINDCOUNT])
    message("The cookie tasted good..."); // ocookie not implemented yet
  if (!env->cdude[BLINDCOUNT]) {
    message("A message inside the cookie reads:");
    if (i < FOR_LONG) {
      //      message(fortune_long[i*2]);
      //      message(fortune_long[i*2+1]);
      message(fortune_long[i]);
    } else {
      message(fortune_short[i-FOR_LONG]);
    }
  }
}

/*
 *	subroutine to process an altar object
 */
#define forget() (env->item[px][py]=env->know[px][py]=0)
// desecrate the altar, bad stuff happens
Boolean odesecrate()
{
  Short px = env->playerx;
  Short py = env->playery;
  if (rnd(100)<60) { 
    message("uh oh");
    createmonster(makemonst(env->level+3)+8); 
    env->cdude[AGGRAVATE] += 2500; 
    o_draws();
    return true;
  } else if(rnd(100)<5) { 
    raiselevel();
  } else if (rnd(101)<30) {
    message("The altar crumbles into a pile of dust!");
    forget();    //remember to destroy the altar*
  } else { 
    message("nothing happens");
  }
  return false;
}
#undef forget
// just pray at the altar, random stuff happens
Boolean ojustpr()
{
  if (rnd(100)<75) 
    message("nothing happens");
  else if (rnd(13)<4)
    ohear();
  else if (rnd(43) <= 10) {
    if (enchantarmor() && env->cdude[WEAR]) 
      message("You feel your armor vibrate briefly");
  }
  else if (rnd(43) <= 10) {
    if (enchweapon() && env->cdude[WIELD]) 
      message("You feel your weapon vibrate briefly");
  }
  else {
    message("uh oh");
    createmonster(makemonst(env->level+2));
    o_draws();
    // xxx bug: it's not drawn until the next turn
    return true;
  }
  return false;
}

#define forget() (env->item[px][py]=env->know[px][py]=0)
// give money at the altar, good stuff happens if you were generous
// return true if it created a monster
Boolean omoney(ULong amnt)
{
  Short px = env->playerx;
  Short py = env->playery;
  Boolean uh_oh = false;

  if ( (amnt==0 || (amnt< ( ((Int)(env->cdude[GOLD])) /10)) ) && rnd(60)<30) {
    // message("Cheapskate! The Gods are insulted at such a meager offering!");
    message("Cheapskate! The deity is insulted!");
    forget();
    createmonster(DEMONPRINCE);
    env->cdude[AGGRAVATE] += 1500; 
    o_draws();
    uh_oh = true;
  } else {
    env->cdude[GOLD] -= amnt;
    if (amnt < ((Int)(env->cdude[GOLD]))/10 || amnt<rnd(50)) { 
      createmonster(makemonst(env->level+2)); 
      env->cdude[AGGRAVATE] += 500; 
      o_draws();
      uh_oh = true;
    } else if (rnd(101) > 50)	{ 
      ohear();
    } else if (rnd(43) >= 33) {
      enchantarmor();
      if (enchantarmor() && env->cdude[WEAR]) 
	message("You feel your armor vibrate briefly");
    } else if (rnd(43) >= 33) {
      enchweapon(); 
      if (enchweapon() && env->cdude[WIELD]) 
	message("You feel your weapon vibrate briefly");
    } else {
      message("Thank You.");
    }
  }
  print_stats();	
  return uh_oh;
}
#undef forget
/*
 * function to cast a +5 protection on the player (if deserving)
 */
static void ohear()
{
  message("You have been heard!");
  if (env->cdude[ALTPRO]==0) env->cdude[MOREDEFENSES]+=5;
  env->cdude[ALTPRO] += 800;	// * protection field * /
  spell_stats();
}

/////////////////////////////////////////////////////////////////////
/*
 * subroutine to process a throne object
 */
// pry off gems - might summon gnome king
// returns true if it it did
Boolean othrone_pry(Boolean two)
{
  Short i,k,px,py;
  px = env->playerx;
  py = env->playery;
  k=rnd(101);
  if (k < 25) { // xxxx debugging
    for (i=0; i<rnd(4); i++) 
      creategem(); //gems pop off the throne
    env->item[px][py]=ODEADTHRONE;
    {
      // get a random abuse counter.  shaken not stirred
      Short tmp = 5 - rund(env->cdude[HARDGAME]); // between 5 and 0
      tmp = rund(tmp); // (yes, rund(x<=1) returns 0)
      env->iarg[px][py] = 5 + tmp; // between 5 and 9
    }
    env->know[px][py]=0;
    o_draws();
  } else if (k<40 && !two) { // xxxx debugging
    createmonster(GNOMEKING);
    env->item[px][py]=OTHRONE2;
    env->know[px][py]=0;
    o_draws();
    return true;
  }
  else message("nothing happens");
  return false;

}
// sit on throne - might summon gnome king or teleport you
// returns true if gnome king summoned
Boolean othrone_sit(Boolean two)
{
  Short k,px,py;
  px = env->playerx;
  py = env->playery;
  k=rnd(101);
  if (k<30 && !two) {
    createmonster(GNOMEKING);
    env->item[px][py]=OTHRONE2;
    env->know[px][py]=0;
    o_draws();
    return true;
  }
  else if (k<35) {
    message("Zaaaappp!  You've been teleported!"); 
    //    beep(); 
    oteleport(0); 
  }
  else message("nothing happens");
  return false;
}
// sit on throne - might raise level or teleport you
void odeadthrone()
{
  Short k, px, py;
  px = env->playerx;
  py = env->playery;

  k = rnd(101);
  if (k < 15) {
    if (env->iarg[px][py] <= rund(env->cdude[HARDGAME])) {
      // abused too many times...
      message("A chute opens!  You are transported...");
      if (env->level >= MAXLEVEL - 1) // cave bottom / volcano? just teleport
	oteleport(0); 
      else // if very abused, send them down, else send them up
	oelevator(env->iarg[px][py] >= 0); // express elevator up|down
    } else {
      if (env->iarg[px][py] == env->cdude[HARDGAME])
	message("The floor shudders!");
      else if (rund(2))
	message("You hear a faint creaking.");
      raiselevel();
    }
    env->iarg[px][py]--; // tick the abuse counter
  }
  else if (k<35) {
    message("Zaaaappp!  You've been teleported!"); 
    // beep(); 
    oteleport(0); 
  }
  else message("nothing happens");
  return;
}
/////////////////////////////////////////////////////////////////////
/*
 * subroutine to process a chest object
 */
#define forget() (env->item[px][py]=env->know[px][py]=0)
void ochest()
{
  Short i,k;
  Short px = env->playerx;
  Short py = env->playery;

  k=rnd(101);
  if (k<40) {
    message("The chest explodes as you open it"); 
    //    beep();
    i = rnd(10);  
    env->lastnum=281;  // in case he dies 
    checkloss(i);
    if (env->cdude[HP] <= 0) return; // dude, you are dead
    switch(rnd(10)) {
    case 1:	
      //nap
      env->cdude[ITCHING]+= rnd(1000)+100;
      message("An irritation spreads over your skin!");
      //      beep();
      break;

    case 2:	
      //nap
      env->cdude[CLUMSINESS]+= rnd(1600)+200;
      message("You lose hand-eye coordination!");
      //      beep();
      break;

    case 3:	
      //nap
      env->cdude[HALFDAM]+= rnd(1600)+200;
      message("You feel sick and BARF on your shoes!");	
      //      beep();
      break;
    };
    forget();
    if (rnd(100)<69) 
      creategem(); // gems from the chest 
    dropgold( rnd(110*env->iarg[px][py]+200) );
    for (i=0; i<rnd(4); i++) 
      something(env->iarg[px][py]+2);
    o_draws();
  }
  else message("nothing happens");
  return;

}
#undef forget
/////////////////////////////////////////////////////////////////////
/*
 * process a fountain
 */
// you can drink it for a random result
void ofountain_drink()
{
  Short x;
  if (rnd(1501)<4) {
    message("Oops!  You caught the dreadful sleep!");
    //    beep(); 
    //    lflush();  
    nap(3200);  
    env->cdude[HP] = -1;// just to be sure
    died(280, true);  // DEATH IS NOT IMPLEMENTED YET
    return;
  }
  x = rnd(100);
  if (x==1) raiselevel();
  else if (x < 11) { 	
    x=rnd((env->level<<2)+2);
    message("Bleah! It tastes like stale gatorade!");
    env->lastnum=273; 
    losehp(x); 
    print_stats();
  }
  else if (x<14) { 	
    env->cdude[HALFDAM] += 200+rnd(200);
    message("The water makes you vomit");
  }
  else if (x<17) 
    quaffpotion(17); /* giant strength */
  else if (x < 45)
    message("nothing seems to have happened");
  else if (rnd(3) != 2)
    fntchange(true);	/*change char levels upward*/
  else
    fntchange(false);	/*change char levels downward*/
  if (rnd(12)<3) {      
    // nap
    //    message("The fountain stops bubbling");
    message("The fountain stops bubbling");
    /* dead fountain */
    env->item[env->playerx][env->playery]=ODEADFOUNTAIN; 
    env->know[env->playerx][env->playery]=0;
  }
}
// you can wash in it for a random result
Boolean ofountain_wash()
{
  Short x;
  if (rnd(100) < 11) { 	
    x=rnd((env->level<<2)+2);
    message("Oh no!  The water burns like acid!");
    env->lastnum=273; 
    losehp(x); 
    print_stats();
  }
  else if (rnd(100) < 29)
    message("You are now clean!");
  else if (rnd(100) < 31)
    message("Hm.. It's no good without soap!");
  else if (rnd(100) < 34) {
    createmonster(WATERLORD); 
    o_draws();
    return true;
  }
  else message("nothing seems to have happened");
  return false;
}

/*
 * raise or lower character levels
 * if 'how' > 0 they are raised   if how < 0 they are lowered
 */
static Char went_by_one[2][18] = {
  " went down by one", " went up by one"
};
static void fntchange(Boolean how)
{
  //  Long j;
  Short j;
  Long *c = env->cdude;
  Char buf[80];

  switch(rnd(9)) {
  case 1:	
    StrPrintF(buf, "Your strength%s",
	      went_by_one[how ? 1 : 0]);
    c[0] += (how) ? 1 : -1;
    break;
  case 2:	
    StrPrintF(buf, "Your intelligence%s",
	      went_by_one[how ? 1 : 0]);
    c[1] += (how) ? 1 : -1;
    break;
  case 3:	
    StrPrintF(buf, "Your wisdom%s",
	      went_by_one[how ? 1 : 0]);
    c[2] += (how) ? 1 : -1;
    break;
  case 4:	
    StrPrintF(buf, "Your constitution%s",
	      went_by_one[how ? 1 : 0]);
    c[3] += (how) ? 1 : -1;
    break;
  case 5:	
    StrPrintF(buf, "Your dexterity%s",
	      went_by_one[how ? 1 : 0]);
    c[4] += (how) ? 1 : -1;
    break;
  case 6:	
    StrPrintF(buf, "Your charm%s",
	      went_by_one[how ? 1 : 0]);
    c[5] += (how) ? 1 : -1;
    break;
  case 7:	
    j=rnd(env->level+1);
    if (!how) { 	
      StrPrintF(buf, "You lose %d hit point%s",
		j, (j>1) ? "s!" : "!");
      losemhp(j); 
    }
    else { 	
      StrPrintF(buf, "You gain %d hit point%s",
		j, (j>1) ? "s!" : "!");
      raisemhp(j); 
    }
    break;
  case 8:	
    j=rnd(env->level+1);
    if (how) { 	
      StrPrintF(buf, "You just gained %d spell%s",
		j, (j>1) ? "s!" : "!");
      raisemspells(j); 
    }
    else { 	
      StrPrintF(buf, "You just lost %d spell%s",
		j, (j>1) ? "s!" : "!");
      losemspells(j); 
    }
    break;
  case 9:	
    j = 5*rnd((env->level+1)*(env->level+1));
    if (!how) { 	
      StrPrintF(buf, "You just lost %d experience point%s",
		j, (j>1) ? "s!" : "!");
      loseexperience(j);  // might need to be Long?
    }
    else { 	
      StrPrintF(buf, "You gained %d experience point%s",
		j, (j>1) ? "s!" : "!");
      raiseexperience(j);  // might need to be Long?
    }
    break;
  }
  //  cursors();
  message(buf);
  print_stats();
}

void drug_speed()
{
  Short j;
  message("*snort!*   Oh  wow man likethings");
  message("totallyseemtoslowdown!");
  env->cdude[HASTESELF] += 200 + env->cdude[LEVEL];
  env->cdude[HALFDAM]   += 300 + rnd(200);
  for (j = 0 ; j <= 5 ; j++) {
    if ((env->cdude[j] -= 2) < 3)
      env->cdude[j] = 3; // double-decrement str, int, wis, con, dex.
  }
  print_stats();
}
void drug_shrooms()
{
  message("*eat!*");
  message("Things start to get real spacey...!");
  env->cdude[HASTEMONST] += rnd(75) + 25;
  env->cdude[CONFUSE]    += 30 + rnd(10);
  env->cdude[WISDOM]   += 2;
  env->cdude[CHARISMA] += 2;
  print_stats();
}
void drug_acid()
{
  message("*eat!*");
  message("You are now frying your arse off");
  env->cdude[WISDOM]       += 2;
  env->cdude[INTELLIGENCE] += 2;
  env->cdude[CONFUSE]   += 30 + rnd(10);
  env->cdude[AWARENESS] += 1500;
  env->cdude[AGGRAVATE] += 1500;
  heal_monsters();
  print_stats();
}

void drug_hash()
{
  message("*smoke!*");
  message("WOW! You feel stooooooned...");
  env->cdude[INTELLIGENCE] += 2;
  env->cdude[WISDOM]       += 2;
  if ( (env->cdude[CONSTITUTION] -= 2) < 3) 
    env->cdude[CONSTITUTION] = 3;
  if ( (env->cdude[DEXTERITY] -= 2) < 3) 
    env->cdude[DEXTERITY] = 3;
  env->cdude[HALFDAM]      += 300+rnd(200);
  env->cdude[HASTEMONST]   += rnd(75)+25;
  env->cdude[CLUMSINESS]   += rnd(1800)+200;
  print_stats();
}

void drug_coke()
{
  Short j;
  message("*snort!*");
  message("Your nose begins to bleed");
  if ((env->cdude[DEXTERITY] -= 2) < 3)
    env->cdude[DEXTERITY] = 3;
  if ((env->cdude[CONSTITUTION] -= 2) < 3)
    env->cdude[CONSTITUTION] = 3;
  env->cdude[CHARISMA] += 3;
  for (j = 0 ; j < 6 ; j++)
    // This is a limited-time effect.. after COKED turns, c[j] -= 34  !!!
    env->cdude[j] += 33;
  env->cdude[COKED] += 10;
  print_stats();
}
