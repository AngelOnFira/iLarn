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
//#define newpotion() (potprob[rund(41)])
//#define newscroll() (scprob[rund(81)])

// This was in data.c

// newpotion() and newscroll() are used in create.c and monster.c

/*
 *	function to create scroll numbers with appropriate probability of 
 *	occurrence
 *
 *	0 - armor		1 - weapon	
 *	2 - enlightenment	3 - paper
 *	4 - create monster	5 - create item	
 *	6 - aggravate		7 - time warp
 *	8 - teleportation	9 - expanded awareness			
 *	10 - haste monst	11 - heal monster	
 *	12 - spirit protection	13 - undead protection
 *	14 - stealth		15 - magic mapping			
 * 	16 - hold monster 	17 - gem perfection 
 *	18 - spell extension	19 - identify
 *	20 - remove curse	21 - annihilation			
 *	22 - pulverization	23 - life protection
 */
const Char scprob[81]= { 
	0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3,
	3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
	9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 
	14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 20,
	21, 22, 22, 22, 23 };

/*
 *	function to return a potion number created with appropriate probability
 *	of occurrence
 *
 *	0 - sleep		1 - healing		2 - raise level
 *	3 - increase ability	4 - gain wisdom		5 - gain strength
 *	6 - charisma		7 - dizziness		8 - learning
 *	9 - gold detection	10 - monster detection	11 - forgetfulness
 *	12 - water		13 - blindness		14 - confusion
 *	15 - heroism		16 - sturdiness		17 - giant strength
 *	18 - fire resistance	19 - treasure finding	20 - instant healing
 *	21 - cure dianthroritis	22 - poison		23 - see invisible
 */
const Char potprob[41] = { 
  0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 9, 10, 10, 10, 
  11, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 20, 22, 22, 23, 23 };

Char newpotion() {
  return potprob[ rund(41) ];
}
Char newscroll() {
  return scprob[ rund(81) ];
}

const Char nlpts[15] = { 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7 };
const Char nch[10] =   { 0, 0, 0, 1, 1, 1, 2, 2, 3, 4 };
const Char nplt[10] =  { 0, 0, 0, 0, 1, 1, 2, 2, 3, 4 };
const Char ndgg[13] =  { 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 5 };
const Char nsw[13] =   { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 3 };

/* return the + points on created leather armor */
// hardgame = env->cdude[HARDGAME];
Char newleather(Boolean hardgame) {
  return nlpts[ rund( hardgame ? 10 : 13 ) ];
}
/* return the + points on chain armor */
Char newchain() {
  return nch[ rund(10) ];
}
/* return + points on plate armor */
Char newplate(Boolean hardgame) {
  return nplt[ rund( hardgame ? 3 : 10 ) ];
}
/* return + points on new daggers */
Char newdagger() {
  return ndgg[ rund(13) ];
}
/* return + points on new swords */
Char newsword(Boolean hardgame) {
  return nsw[ rund( hardgame ? 6 : 13 ) ];
}


//#define disappear(x,y) (mitem[x][y]=know[x][y]=0)
void disappear(Short x, Short y)
{
  env->mitem[x][y] = 0;
  env->know[x][y] = 0;
}


// Char objnamelist[128]=" AToP%^F&^+M=%^$$f*OD#~][[)))(((||||||||{?!BC.o...<<<<EVV))([[]]](^ [H*** ^^ S tsTLcu...//))]:::::@.............................";

const Char objectname[110][35] = {
  {0},
  "a holy altar",
  //  "a handsome jewel encrusted throne",
  "a handsomely jeweled throne",
  "an orb of enlightenment",
  "a pit",
  "a staircase leading upwards",
  "the elevator going up",
  "a bubbling fountain",
  "a great marble statue",
  "a teleport trap",
  "the college of Larn",
  "a mirror",
  "the DND store",
  "a staircase going down",
  "the elevator going down",
  "the bank of Larn",
  "the 8th branch of the Bank of Larn",
  "a dead fountain",
  "gold",
  "an open door",
  "a closed door",
  "a wall",
  "The Eye of Larn",
  "plate mail",
  "chain mail",
  "leather armor",
  "a sword of slashing",
  "Bessman's flailing hammer",
  "a sunsword",
  "a two handed sword",
  "a spear",
  "a dagger",
  "ring of extra regeneration",
  "a ring of regeneration",
  "a ring of protection",
  "an energy ring",
  "a ring of dexterity",
  "a ring of strength",
  "a ring of cleverness",
  "a ring of increase damage",
  "a belt of striking",
  "a magic scroll",
  "a magic potion",
  "a book",
  "a chest",
  "an amulet of invisibility",
  "an orb of dragon slaying",
  "a scarab of negate spirit",
  "a cube of undead control",
  "a device of theft prevention",
  "a brilliant diamond",
  "a ruby",
  "an enchanting emerald",
  "a sparkling sapphire",
  "the dungeon entrance",
  "a volcanic shaft leaning downward",
  "the base of a volcanic shaft",
  "a battle axe",
  "a longsword",
  "a flail",
  "ring mail",
  "studded leather armor",
  "splint mail",
  "plate armor",
  "stainless plate armor",
  "a lance of death",
  "an arrow trap",
  "an arrow trap",
  "a shield",
  "your home",
  "gold",
  "gold",
  "gold",
  "a dart trap",
  "a dart trap",
  "a trapdoor",
  "a trapdoor",
  "the local trading post",
  "a teleport trap", 
  "a massive throne",
  "a sphere of annihilation",
  //  "a handsome jewel encrusted throne",
  "a handsomely jeweled throne",
  "the Larn Revenue Service",
  "a fortune cookie",
  "a golden urn",
  "a brass lamp",
  "The Hand of Fear",
  "The Talisman of the Sphere",
  "a wand of wonder",
  "a staff of power",
  "Vorpal",
  "Slayer",
  "elven chain",
  "some speed",
  "some LSD",
  "some hashish",
  "some magic mushrooms",
  "some cocaine",
  "Dealer McDope's Pad",
  "","","","","","","","","","",""
};




/*	name array for scrolls		*/
const Char scrollname[MAXSCROLL][20] = {
  " enchant armor",
  " enchant weapon",
  " enlightenment",
  " blank paper",
  " create monster",
  " create artifact",
  " aggravate monsters",
  " time warp",
  " teleportation",
  " expanded awareness",
  " haste monsters",
  " monster healing",
  " spirit protection",
  " undead protection",
  " stealth",
  " magic mapping",
  " hold monsters",
  " gem perfection",
  " spell extension",
  " identify",
  " remove curse",
  " annihilation",
  " pulverization",
  " life protection",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 "
};

/*	name array for magic potions	*/
const Char potionname[MAXPOTION][20] = {
  " sleep",
  " healing",
  " raise level",
  " increase ability",
  " wisdom",
  " strength",
  " raise charisma",
  " dizziness",
  " learning",
  " gold detection",
  " monster detection",
  " forgetfulness",
  " water",
  " blindness",
  " confusion",
  " heroism",
  " sturdiness",
  " giant strength",
  " fire resistance",
  " treasure finding",
  " instant healing",
  " cure dianthroritis", // initially KNOWN  #21 or P_CURED
  " poison",
  " see invisible",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 ",
  "\0 "
};

//Boolean potion_known[MAXPOTION];
//Boolean scroll_known[MAXSCROLL];

// set argument to 'false' unless you have just achieved omniscience
/*
void init_scroll_potion(Boolean mister_know_it_all) {
  Short i;
  for (i = 0 ; i < MAXPOTION ; i++)
    potion_known[i] = mister_know_it_all;
  for (i = 0 ; i < MAXSCROLL ; i++)
    scroll_known[i] = mister_know_it_all;
  potion_known[P_CURED] = true; // cure dianthroritis
}
*/
// make one scroll known; or all if given negative argument.
/*
void know_scroll(Short j) {
  if (j >= 0 && j < MAXSCROLL)     scroll_known[j] = true;    
}
void know_potion(Short j) {
  if (j >= 0 && j < MAXPOTION)     potion_known[j] = true;    
}
Boolean potscr_known(Short j, Boolean pot_not_scr) {
  if (pot_not_scr) return potion_known[j];
  else return scroll_known[j];
}
*/

Char buf[40];

Char * get_Of_scroll(Short j, Boolean known)
{
  buf[0] = '\0';
  if (known && j >= 0 && j < MAXSCROLL)
    StrPrintF(buf, " of%s", scrollname[j]);
  return buf;
}

Char * get_Of_potion(Short j, Boolean known)
{
  buf[0] = '\0';
  if (known && j >= 0 && j < MAXPOTION)
    StrPrintF(buf, " of%s", potionname[j]);
  return buf;
}

const Char * get_objectname(Short j)
{
  if (j >= 0 && j < 110)
    return objectname[j];
  else return objectname[0];
}

// show3 was in main.c
// no more show1!
void show3(Short index)
{
  Char buf[80];
  show3_cp(index, buf);
  message(buf);
}
//
//
void show3_cp(Short index, Char *buf)
{
  Short itm = env->iven[index];
  Short arg = env->ivenarg[index];
  buf[0] = '\0';
  StrPrintF(buf, "foo");
  switch(itm) {
  case OPOTION:
    //    return; // testing
    if (env->potion_known[arg])
      StrPrintF(buf, "%c) %s of%s",
		(Char)(index+'a'), objectname[itm], potionname[arg]);
    else
      StrPrintF(buf, "%c) %s", (Char)(index+'a'), objectname[itm]);
    break;
  case OSCROLL:
    //    return; // testing
    if (env->scroll_known[arg])
      StrPrintF(buf, "%c) %s of%s",
		(Char)(index+'a'), objectname[itm], scrollname[arg]);
    else
      StrPrintF(buf, "%c) %s", (Char)(index+'a'), objectname[itm]);
    break;
  case OLARNEYE:		
  case OBOOK:			
  case OSPIRITSCARAB:
  case ODIAMOND:		
  case ORUBY:			
  case OCUBEofUNDEAD:
  case OEMERALD:		
  case OCHEST:		
  case OCOOKIE:
  case OSAPPHIRE:		
  case OORB:	
  case OHANDofFEAR:
  case OBRASSLAMP:
  case OURN:
  case OWWAND:
  case OSPHTALISMAN:
  case ONOTHEFT:
    //    return; // testing
    StrPrintF(buf, "%c) %s", (Char)(index+'a'), objectname[itm]);
    break;
  default:	
    StrPrintF(buf, "%c) %s", (Char)(index+'a'), objectname[itm]);
    //    return; // testing
    // StrPrintF(buf, "%c) %s",index+'a',objectname[(Short) env->iven[index]]);
    if (arg > 0) 
      StrPrintF(buf + StrLen(buf), " +%d", arg);
    else if (arg < 0) 
      StrPrintF(buf + StrLen(buf), " %d", arg);
    break;
  }
  //  return; // testing
  if (env->cdude[WIELD]==index) 
    //    StrCat(buf, " (weapon in hand)");
    StrCat(buf, " (wielded)");
  if ((env->cdude[WEAR]==index) || (env->cdude[SHIELD]==index))  
    //    StrCat(buf, " (being worn)");
    StrCat(buf, " (worn)");
}

// "z) a device of theft prevention + 99 (weapon in hand)"

