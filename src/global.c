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
#include "iLarnRsc.h"


//static void raisespells(Short x, struct everything *env) SEC_5;
//static void losespells(Short x, struct everything *env) SEC_5;



// this was in data.c but only used by makemonst.
const Char monstlevel[12] = {5, 11, 17, 22, 27, 33, 39, 42, 46, 50, 53, 56};
// this is in data.c and I might regret it.
//extern struct monst monster[];
// this was in data.c, and might be used in savelev.c
/*
	table of experience needed to be a certain level of player
	skill[c[LEVEL]] is the experience required to attain the next level
 */
#define MEG 1000000
const Long skill[101] = {
0, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120,                 /*  1-11 */
10240, 20480, 40960, 100000, 200000, 400000, 700000, 1*MEG,         /* 12-19 */
2*MEG,3*MEG,4*MEG,5*MEG,6*MEG,8*MEG,10*MEG,                         /* 20-26 */
12*MEG,14*MEG,16*MEG,18*MEG,20*MEG,22*MEG,24*MEG,26*MEG,28*MEG,     /* 27-35 */
30*MEG,32*MEG,34*MEG,36*MEG,38*MEG,40*MEG,42*MEG,44*MEG,46*MEG,     /* 36-44 */
48*MEG,50*MEG,52*MEG,54*MEG,56*MEG,58*MEG,60*MEG,62*MEG,64*MEG,     /* 45-53 */
66*MEG,68*MEG,70*MEG,72*MEG,74*MEG,76*MEG,78*MEG,80*MEG,82*MEG,     /* 54-62 */
84*MEG,86*MEG,88*MEG,90*MEG,92*MEG,94*MEG,96*MEG,98*MEG,100*MEG,    /* 63-71 */
105*MEG,110*MEG,115*MEG,120*MEG, 125*MEG, 130*MEG, 135*MEG, 140*MEG,/* 72-79 */
145*MEG,150*MEG,155*MEG,160*MEG, 165*MEG, 170*MEG, 175*MEG, 180*MEG,/* 80-87 */
185*MEG,190*MEG,195*MEG,200*MEG, 210*MEG, 220*MEG, 230*MEG, 240*MEG,/* 88-95 */
	260*MEG,280*MEG,300*MEG,320*MEG, 340*MEG, 370*MEG	    /* 96-101*/
};
#undef MEG






/*
***********
RAISE LEVEL
***********
raiselevel()

subroutine to raise the player one level
uses the skill[] array to find level boundarys
uses c[EXPERIENCE]  c[LEVEL]
*/
void raiselevel()
{
  if (env->cdude[LEVEL] < MAXPLEVEL) 
    raiseexperience( (Long) (skill[env->cdude[LEVEL]] -
			     env->cdude[EXPERIENCE]) );
}

/*
***********
LOOSE LEVEL
***********
loselevel()

subroutine to lower the players character level by one
*/
void loselevel()
{
  if (env->cdude[LEVEL] > 1)
    loseexperience( (Long) (env->cdude[EXPERIENCE] -
			    skill[env->cdude[LEVEL]-1] + 1) );
}






/*
 ****************
 RAISE EXPERIENCE
 ****************
 raiseexperience(x)
 
 subroutine to increase experience points
 */
void raiseexperience(Long x)
{
  Short i, j;
  Char buf[40];
  
  i = env->cdude[LEVEL];
  if (i == MAXPLEVEL) return; // let's make sure I don't overflow, eh.
  env->cdude[EXPERIENCE] += x;
  while (env->cdude[EXPERIENCE] >= skill[env->cdude[LEVEL]] 
	 && (env->cdude[LEVEL] < MAXPLEVEL)) {
    j = (env->cdude[CONSTITUTION] - env->cdude[HARDGAME]) >> 1;
    env->cdude[LEVEL]++;	
    raisemhp( rnd(3) + rnd((j>0) ? j : 1));
    raisemspells( rund(3));
    if (env->cdude[LEVEL] < 7 - env->cdude[HARDGAME]) 
      raisemhp( (env->cdude[CONSTITUTION] >> 2));
  }

  if (env->cdude[LEVEL] != i) {
    //    beep(); 
    //    StrPrintF(buf, "Welcome to level %d",(Long) env->cdude[LEVEL]);
    StrPrintF(buf, "Welcome to level %d", (Short) env->cdude[LEVEL]);	
    message(buf);
    /* if we changed levels	*/
    switch (env->cdude[LEVEL]) {
    case 94:	/* earth guardian */
      env->cdude[WTW] = 99999L;
      break;
    case 95: 	/* air guardian */
      env->cdude[INVISIBILITY] = 99999L;
      break;
    case 96:	/* fire guardian */
      env->cdude[FIRERESISTANCE] = 99999L;
      break;
    case 97: 	/* water guardian */
      env->cdude[CANCELLATION] = 99999L;
      break;
    case 98: 	/* time guardian */
      env->cdude[HASTESELF] = 99999L;
      break;
    case 99:	/* ethereal guardian */
      env->cdude[STEALTH] = 99999L;
      env->cdude[SPIRITPRO] = 99999L;
      break;
    case 100:
      message("You are now The Creator");

      for (i=0; i<MAXY; i++)
	for (j=0; j<MAXX; j++)  
	  env->know[j][i]=1;

      for (i=0; i<SPNUM; i++)	
	env->spelknow[i]=1;
      for (i = 0 ; i < MAXPOTION ; i++)
	env->potion_known[i] = true;
      for (i = 0 ; i < MAXSCROLL ; i++)
	env->scroll_known[i] = true;

      env->cdude[STEALTH] = 99999L;
      env->cdude[UNDEADPRO] = 99999L;
      env->cdude[SPIRITPRO] = 99999L;
      env->cdude[CHARMCOUNT] = 99999L;
      env->cdude[GIANTSTR] = 99999L;
      env->cdude[FIRERESISTANCE] = 99999L;
      env->cdude[DEXCOUNT] = 99999L;
      env->cdude[STRCOUNT] = 99999L;
      env->cdude[SCAREMONST] = 99999L;
      env->cdude[HASTESELF] = 99999L;
      env->cdude[CANCELLATION] = 99999L;
      env->cdude[INVISIBILITY] = 99999L;
      env->cdude[ALTPRO] = 99999L;
      env->cdude[WTW] = 99999L;
      env->cdude[AWARENESS] = 99999L;
      break;
    }
  }
  print_stats();
}

/*
 ***************
 LOSE EXPERIENCE
 ***************
 loseexperience(x)
 
 subroutine to lose experience points
 */
void loseexperience(Long x)
{
  Short i,tmp;
  Long *c = env->cdude;
  
  i = c[LEVEL];		
  c[EXPERIENCE] -= x;
  if (c[EXPERIENCE] < 0) c[EXPERIENCE] = 0;
  while (c[EXPERIENCE] < skill[c[LEVEL]-1])
  {
    c[LEVEL] -= 1;
    if (c[LEVEL] < 1) 
      c[LEVEL] = 1;	/*	down one level		*/
    tmp = (c[CONSTITUTION] - c[HARDGAME]) >> 1;	/* lose hpoints */
    losemhp( rnd( (tmp>0) ? tmp : 1)); /* lose hpoints */
    if (c[LEVEL] < 7-c[HARDGAME]) 
      losemhp(c[CONSTITUTION] >> 2);
    losemspells(rund(3));	/*	lose spells */
  }
  if (i != c[LEVEL])
  {
    //    beep(); 
    //    message("You went down to level %d!",(Long) c[LEVEL]);
    message("OUCH!"); // DANGER WILL ROBINSON
  }
  // UPDATE YOUR STATS
  print_stats();
}

/*
 *******
 LOSE HP
 *******
 losehp(x)
 losemhp(x)
 
 subroutine to remove hit points from the player
 warning -- will kill player if hp goes to zero
 */
void losehp(Short x)
{
  //  Char buf[32];
  if ((env->cdude[HP] -= x) <= 0)
  {
    //    StrPrintF(buf, "losehp: dead %d", env->lastnum);
    //    message(buf); // sometimes it freezes here.
    //
    //    beep(); 
    //    lprcat("\n");  
    //    nap(3000);  
    died(env->lastnum, true);
    //    message("DEATH NOT IMPLEMENTED");
  }
}
// lose from max-hp (but don't die)
void losemhp(Short x)
{
  env->cdude[HP] -= x;		
  if (env->cdude[HP] < 1)		
    env->cdude[HP] = 1;
  env->cdude[HPMAX] -= x;	
  if (env->cdude[HPMAX] < 1)	
    env->cdude[HPMAX] = 1;
}


/*
 ********
 RAISE HP
 ********
 raisehp(x)
 raisemhp(x)
 
 subroutine to gain maximum hit points
 */
void raisehp(Short x)
{
  if ((env->cdude[HP] += x) > env->cdude[HPMAX]) 
    env->cdude[HP] = env->cdude[HPMAX];
}
void raisemhp(Short x)
{
  env->cdude[HPMAX] += x;	
  env->cdude[HP] += x;
}

/*
 ************
 RAISE SPELLS
 ************
 raisespells(x)
 raisemspells(x)
 
 subroutine to gain maximum spells
 */
/*
static void raisespells(Short x)
{
  if ((env->cdude[SPELLS] += x) > env->cdude[SPELLMAX])	
    env->cdude[SPELLS] = env->cdude[SPELLMAX];
}
*/
void raisemspells(Short x)
{
  env->cdude[SPELLMAX] += x; 
  env->cdude[SPELLS] += x;
}

/*
 ************
 LOSE SPELLS
 ************
 losespells(x)
 losemspells(x)
 
 subroutine to lose maximum spells
 */
/*
static void losespells(Short x)
{
  if ((env->cdude[SPELLS] -= x) < 0) env->cdude[SPELLS]=0;
}
*/
void losemspells(Short x)
{
  if ((env->cdude[SPELLMAX] -= x) < 0) env->cdude[SPELLMAX]=0;
  if ((env->cdude[SPELLS] -= x) < 0) env->cdude[SPELLS]=0;
}




/*
  positionplayer()
  
  function to be sure player is not in an object (wall etc.)
  */
void positionplayer()
{
  Short try = 2;
  
  while ( try && ( env->item[env->playerx][env->playery] || 
		   env->mitem[env->playerx][env->playery] ) ) {
    env->playerx++;
    if (env->playerx >= MAXX-1) {
      env->playerx = 1;
      env->playery++;
      if (env->playery >= MAXY-1) {	
	env->playery = 1;
	--try;
      }
    }
  }
}





/*
  makemonst(lev)
  int lev;
  
  function to return monster number for a randomly selected monster
  for the given cave level	
  */
Short makemonst(Short lev)
{
  Short tmp, x;
  
  if (lev < 1)	    lev = 1;
  if (lev > 12)	    lev = 12;
  
  tmp = WATERLORD;

  // get a random index
  if (lev < 5) {
    while (tmp == WATERLORD) {
      x = monstlevel[lev-1];  // this level allows lemming-thru-<foo>
      tmp = rnd( (x != 0) ? x : 1); // select from lemming thru <foo>
    }
  } else {
    while (tmp == WATERLORD) {
      x = monstlevel[lev-1] - monstlevel[lev-4];  // move up the window:
      tmp = rnd( (x != 0) ? x : 1);  // select lemming thru <foo>
      tmp += monstlevel[lev-4];      // it's really <bar> thru <qux>
    }
  }

  // make sure it hasn't been genocided
  while (env->genocided[tmp] && tmp<MAXMONST)
    tmp++;

  // then.. after all that.. there's a 10% chance of becoming a lemming
  // (wouldn't it save time to do the lemming first?)
  if (env->level < 16)
    if (rnd(100) < 10)
      tmp = LEMMING;

  // this is just my paranoia
  if (tmp < LEMMING)     tmp = LEMMING;
  if (tmp > LUCIFER)     tmp = LUCIFER;
  
  return(tmp);
}


/*
 * recalculate the armor class, etc, of the player.
 */
extern Boolean recalc_changed_stats;
void recalc()
{
  Short i, j;
  Short wear = env->cdude[WEAR];
  Short shield = env->cdude[SHIELD];
  Short wield = env->cdude[WIELD];
  Long old_AC, old_WCLASS;

  old_AC = env->cdude[AC];
  old_WCLASS = env->cdude[WCLASS];

  env->cdude[AC] = env->cdude[MOREDEFENSES];
  if (wear >= 0) {
    i = env->ivenarg[wear]; 
    switch(env->iven[wear]) {
    case OSHIELD:		
      env->cdude[AC] += 2 + i;
      break;
    case OLEATHER:		
      env->cdude[AC] += 2 + i;
      break;
    case OSTUDLEATHER:	
      env->cdude[AC] += 3 + i;
      break;
    case ORING:			
      env->cdude[AC] += 5 + i;
      break;
    case OCHAIN:		
      env->cdude[AC] += 6 + i;
      break;
    case OSPLINT:		
      env->cdude[AC] += 7 + i;
      break;
    case OPLATE:		
      env->cdude[AC] += 9 + i;
      break;
    case OPLATEARMOR:	
      env->cdude[AC] += 10 + i;
      break;
    case OSSPLATE:		
      env->cdude[AC] += 12 + i;
      break;
    case OELVENCHAIN:
      env->cdude[AC] += 15 + i;
      break;
    }
  }
  if (shield >= 0) 
    if (env->iven[shield] == OSHIELD)
      env->cdude[AC] += 2 + env->ivenarg[shield];

  if (env->cdude[WIELD] < 0) 
    env->cdude[WCLASS] = 0;  
  else {
    i = env->ivenarg[wield];
    switch(env->iven[wield]) {
    case ODAGGER:    
      env->cdude[WCLASS] =  3 + i;  
      break;
    case OBELT:
      env->cdude[WCLASS] =  7 + i;  
      break;
    case OSHIELD:	 
      env->cdude[WCLASS] =  8 + i;  
      break;
    case OPSTAFF:
    case OSPEAR:     
      env->cdude[WCLASS] = 10 + i;  
      break;
    case OFLAIL:     
      env->cdude[WCLASS] = 14 + i;  
      break;
    case OBATTLEAXE: 
      env->cdude[WCLASS] = 17 + i;  
      break;
    case OLANCE:	 
      env->cdude[WCLASS] = 20 + i;  
      break;
    case OLONGSWORD: 
      env->cdude[WCLASS] = 22 + i;  
      break;
    case O2SWORD:    
      env->cdude[WCLASS] = 26 + i;  
      break;
    case OSWORDofSLASHING: 
      env->cdude[WCLASS] = 30 + i; 
      break;
    case OSLAYER:	 
      env->cdude[WCLASS] = 30 + i; 
      break;
    case OSWORD:     
      env->cdude[WCLASS] = 32 + i;  
      break;
    case OHAMMER:    
      env->cdude[WCLASS] = 35 + i;  
      break;
    default:	     
      env->cdude[WCLASS] = 0;
    }
  }
  env->cdude[WCLASS] += env->cdude[MOREDAM];
  /*	now for abilities based on rings	*/
  env->cdude[REGEN]=1;		
  env->cdude[ENERGY]=0;
  for (j = 0 ; j < NINVT ; j++) {
    i = env->ivenarg[j];
    switch(env->iven[j]) {
    case OPROTRING: 
      env->cdude[AC]     += i + 1;	
      break;
    case ODAMRING:  
      env->cdude[WCLASS] += i + 1;	
      break;
    case OBELT:     
      env->cdude[WCLASS] += ((i<<1)) + 2;	
      break;
      
    case OREGENRING:	
      env->cdude[REGEN]  += i + 1;	
      break;
    case ORINGOFEXTRA:	
      env->cdude[REGEN]  += 5 * (i+1); 
      break;
    case OENERGYRING:	
      env->cdude[ENERGY] += i + 1;	
      break;
    }
  }

  if ((old_AC != env->cdude[AC]) || (old_WCLASS != env->cdude[WCLASS])) {
    if (FrmGetActiveFormID() == MainForm)
      print_stats();
    else
      recalc_changed_stats = true;
  }
}

/*
 * create a gem on a square near the player
 */
void creategem()
{
  Short i,j;
  switch(rnd(4))
  {
  case 1:	 
    i = ODIAMOND;	
    j = 50;	
    break;
  case 2:	 
    i = ORUBY;		
    j = 40;	
    break;
  case 3:	 
    i = OEMERALD;	
    j = 30;	
    break;
  default: 
    i = OSAPPHIRE;	
    j = 20;	
    break;
  };
  createitem(i, rnd(j) + j/10);
}

/*
 *  function to change character levels as needed when dropping an object
 *  that affects these characteristics
 */
void adjustcvalues(Short itm, Short arg, Boolean draw)
{
  Short flag,i;
  
  flag = 0;
  switch(itm) {
  case ODEXRING:	
    env->cdude[DEXTERITY] -= arg+1;  
    flag=1; 
    break;
  case OSTRRING:	
    env->cdude[STREXTRA]  -= arg+1;  
    flag=1; 
    break;
  case OCLEVERRING: 
    env->cdude[INTELLIGENCE] -= arg+1;  
    flag=1; 
    break;
  case OHAMMER:	
    env->cdude[DEXTERITY] -= 10;	
    env->cdude[STREXTRA] -= 10;
    env->cdude[INTELLIGENCE] += 10; 
    flag=1; 
    break;
  case OORB:	
    env->cdude[ORB] -= 1;
    env->cdude[AWARENESS] -= 1;
    // fall through
  case OSWORDofSLASHING:		
    env->cdude[DEXTERITY] -= 5;
    flag=1; 
    break;
  case OSLAYER:		
    env->cdude[INTELLIGENCE] -= 10; 
    flag=1; 
    break;
  case OPSTAFF:		
    env->cdude[WISDOM] -= 10; 
    flag=1;
    break;
  case OORBOFDRAGON:		
    env->cdude[SLAYING] -= 1;
    return;
  case OSPIRITSCARAB:		
    env->cdude[NEGATESPIRIT] -= 1;
    return;
  case OCUBEofUNDEAD:		
    env->cdude[CUBEofUNDEAD] -= 1;
    return;
  case ONOTHEFT:			
    env->cdude[NOTHEFT] -= 1;
    return;
  case OLANCE:			
    env->cdude[LANCEDEATH] = 0;
    return;
  case OLARNEYE: {
    ungot_larneye(); // redraw?
    return;
  }
  case OPOTION:	
  case OSCROLL:	
    return;
    
  default:	
    flag = 1;
  };

  for (i = 0 ; i < 6 ; i++)
    if (env->cdude[i] < 3)
      env->cdude[i] = 3;

  if (flag && draw)
    print_stats();
}

/*
 *  Returns the index of the object, if you have it, else -1 if you don't.
 *      just to make invisibility spell spiffier.
 */
Short has_object_at(Short what)
{
  Short i, res;
  
  i = 0;
  res = -1;
  while( !((i >= NINVT) || (res >= 0)) ) {
    if( env->iven[i] == what ) res = i;
    i++;
  }
  return res;
}
// a more sensible plain vanilla "do you gots it or dontcha"
Boolean has_object(Short what)
{
  Short i;
  for (i = 0 ; i < NINVT ; i++)
    if (env->iven[i] == what)
      return true;
  return false;
}

/*
 *  function to put something in the players inventory
 *  returns true if success, false if a failure
 *  it sort of does the opposite of adjustcvalues.
 */
Boolean take(Short itm, Short arg, Boolean noisy)
{
  Short i,limit;
  Long *c = env->cdude;
  Boolean stats_changed = false;
  
  if ((limit = 15+(c[LEVEL]>>1)) > NINVT)  limit=NINVT;
  for (i=0; i<limit; i++)
    if (env->iven[i]==0) {
      env->iven[i] = itm;  
      env->ivenarg[i] = arg;  
      limit=0;
      // now figure out what this has done to abilities..
      switch(itm) {
      case OPROTRING:	
      case ODAMRING: 
      case OBELT: 	
	stats_changed = true;  
	break;
      case ODEXRING:	
	c[DEXTERITY] += env->ivenarg[i]+1; 
	stats_changed = true;	
	break;
      case OSTRRING:	
	c[STREXTRA]  += env->ivenarg[i]+1;
	stats_changed = true; 
	break;
      case OCLEVERRING:
	c[INTELLIGENCE] += env->ivenarg[i]+1;
	stats_changed = true; 
	break;
      case OHAMMER:	
	c[DEXTERITY] += 10;	
	c[STREXTRA]+=10;
	c[INTELLIGENCE]-=10;	
	stats_changed = true;	 
	break;
      case OORB:	
	c[ORB]++;
	c[AWARENESS]++;
	break;
      case OORBOFDRAGON:
	c[SLAYING]++;		
	break;
      case OSPIRITSCARAB: 
	c[NEGATESPIRIT]++;	
	break;
      case OCUBEofUNDEAD: 
	c[CUBEofUNDEAD]++;	
	break;
      case ONOTHEFT:	
	c[NOTHEFT]++;		
	break;
      case OSWORDofSLASHING:	
	c[DEXTERITY] +=5;	
	stats_changed = true; 
	break;
      case OSLAYER:
	c[INTELLIGENCE]+=10;
	break;
      case OPSTAFF:
	c[WISDOM]+=10;
	break;
      case OLARNEYE:
	got_larneye();
	break;
      };

      if (noisy) {
	message("You pick up:"); 
	//      srcount=0;  
	show3(i);
	if (stats_changed)
	  recalc();
      }
      return true;
    }
  if (noisy) message("You can't carry anything else");  
  return false;
}
