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
#include "iLarnRsc.h"

static void obottomless() SEC_1;
extern Short cast_form_use;
extern LarnPreferenceType my_prefs;
Char **cast_select_items; // [SPNUM][27];
Short cast_qty;

/*
 *  Enter the volcano, possibly slip and fall.
 */
void ovoldown()
{
  Short wt = packweight();
  if (env->level!=0) { 
    message("The shaft only extends 5 feet downward!"); 
    return; 
  }
  // Hey!  If the player prefers small font, make it small.
  if (!my_prefs.font_large)
    toggle_itsy();
  if (wt > 45 + 3 * (env->cdude[STRENGTH] + env->cdude[STREXTRA])) { 
    message("You slip and fall down the shaft"); 
    //    beep();
    env->lastnum=275;  
    losehp(30+rnd(20)); 
    print_stats();
  } else {
    ;  //      lprcat("climb down");
  };  
  //  nap(3000); 
  newcavelevel(MAXLEVEL);
  env->playerx = rnd(MAXX-2);
  env->playery = rnd(MAXY-2);
  positionplayer(); // let's not start out in a wall, shall we?
  o_draws();
}


/*
 *  Exit the volcano, possibly slip and fall back in
 */
void ovolup()
{
  Short i,j,wt = packweight();
  //  Short i,j,wt = 0;
  if (wt > 40 + 5 * (env->cdude[DEXTERITY] +
		     env->cdude[STRENGTH] + env->cdude[STREXTRA])) { 
    message("You slip and fall down the shaft"); 
    //    beep();
    env->lastnum=275; 
    losehp(15+rnd(20)); 
    print_stats();
  }else{
    //    nap(3000); 
    newcavelevel(0);
    // xxx check moveplayer.c and do what it does
    for (i=0; i<MAXY; i++)
      for (j=0; j<MAXX; j++) 
	/* put player near volcano shaft */
	if (env->item[j][i]==OVOLDOWN) { 
	  env->playerx=j; 
	  env->playery=i; 
	  j=MAXX; 
	  i=MAXY; 
	  /* deleted, ws 08.08.93		positionplayer(); */ 
	}
    o_draws();
  }
}
/////////////////////////////////////////////////////////////////////
/*
 *  Enter dungeon
 */
void oentrance()
{
  // Hey!  If the player prefers small font, make it small.
  if (!my_prefs.font_large)
    toggle_itsy();
  newcavelevel(1); 
  env->playerx=33; 
  env->playery=MAXY-2;
  env->item[33][MAXY-1]=env->know[33][MAXY-1]=env->mitem[33][MAXY-1]=0;
  o_draws(); 
}
/////////////////////////////////////////////////////////////////////
#define forget() (env->item[px][py]=env->know[px][py]=0)
/*
 * Close an opened door
 */
void oopendoor()
{
  Short px = env->playerx;
  Short py = env->playery;
  forget();
  env->item[px][py]=OCLOSEDDOOR;
  env->iarg[px][py]=0;
  unmoveplayer();
  message("closed");
}
/*
 * Open a closed door, maybe
 */
void oclosedoor()
{
  Short px = env->playerx;
  Short py = env->playery;
  Boolean no = false;
  if (rnd(11)<7) {
    switch(env->iarg[px][py]) {
    case 6: 
      env->cdude[AGGRAVATE] += rnd(400);	
      break;

    case 7:	
      message("You are jolted by an electric shock!"); 
      no = true;
      env->lastnum=274; 
      losehp(rnd(20));  
      print_stats();
      break;

    case 8:	
      loselevel();  
      break;

    case 9:	
      message("You suddenly feel weaker!");
      no = true;
      if (env->cdude[STRENGTH]>3) env->cdude[STRENGTH]--;
      print_stats();
      break;

    default:	
      break;
    }
    if (!no) message("hm... it's stuck.");
    unmoveplayer();
  } else {
    forget();  
    env->item[px][py] = OOPENDOOR;
    message("open");
  }
}
#undef forget
/////////////////////////////////////////////////////////////////////
/*
 * Ride an elevator up or down a random amount
 */
void oelevator(Boolean up)
{
  env->playerx = rnd(MAXX-2);	
  env->playery = rnd(MAXY-2);

  if (up) 
    newcavelevel(rund(env->level));
  else 
    newcavelevel(env->level + rnd(MAXLEVEL-1 - env->level));
  positionplayer();
  o_draws();
}
/////////////////////////////////////////////////////////////////////

/*
 * Climb stairs, up or down
 */
void ostairs(Boolean up)
{
  Short x, y;

  if (up) {
    if (env->level>=2 && env->level!=MAXLEVEL) {
      newcavelevel(env->level-1);
      for (x=0;x<MAXX;x++)
	for (y=0;y<MAXY;y++)
	  if (env->item[x][y] == OSTAIRSDOWN) {
	    env->playerx=x;
	    env->playery=y;
	  }
      o_draws();
    }
    else message("The stairs lead to a dead end!");
    return;
  } else {
    if ((env->level!=0) && (env->level!= (MAXLEVEL-1)) && 
	(env->level!=(MAXLEVEL-1 + MAXVLEVEL))) {
      newcavelevel(env->level+1);
      for (x=0;x<MAXX;x++)
	for (y=0;y<MAXY;y++)
	  if (env->item[x][y] == OSTAIRSUP) {
	    env->playerx=x;
	    env->playery=y;
	  }
      o_draws();
    }
    else message("The stairs lead to a dead end!");
    return;
  }
}

/////////////////////////////////////////////////////////////////////

/*
 * Fall into a bottomless pit
 */
static void obottomless()
{
  if (env->cdude[HARDGAME] == 0 && (rnd(100) > 5)) {
    // I will try to be nice to 'inexperienced' players.
    message("You ALMOST fell into a bottomless pit!");
    return;
  }
  message("You fell into a bottomless pit!!!");  
  //  beep(); 
  nap(3000);  
  env->cdude[HP] = -1;// just to be sure
  died(262, true);
}

/*
 *  Maybe fall into a pit, possibly bottomless
 */
void opit()
{
  Short i;
  Short wt = packweight();
  if (rnd(101) < 81)
    if (rnd(70) > 9 * env->cdude[DEXTERITY] - wt || rnd(101)<5) {
      if ( has_object(OWWAND) ) {
	message("You float right over the pit");
	return;
      }
      if (env->level==MAXLEVEL-1) obottomless(); 
      else if (env->level==MAXLEVEL+MAXVLEVEL-1) obottomless(); 
      else {
	if (rnd(101)<20) {
	  i=0;
	  message("You fall in a pit and land on a monster!");
	} else {
	  i = rnd(env->level*3+3);
	  message("You fell into a pit!  ow!");
	  env->lastnum=261; 
	  /* if he dies scoreboard will say so */
	}
	losehp(i); 
	//	nap(2000);  
	newcavelevel(env->level+1);  
	o_draws();
      }
    }
}

/*
 *  Why is this here?  No good reason.
 */
void show_packweight()
{
  Char buf[40];
  Short wt = packweight();
  StrPrintF(buf, "Pack weight: %d", wt);
  message(buf);
  return;
}


/////////////////////////////////////////////////////////////////////
/*
 *  You get a spell if you read a book.
 */
static Char splev[] =
{  1, 4, 7,11,15,
  20,24,28,30,32,
  33,34,35,36,37,
  38,38,38,38,38,
  38};
/*
 *  Since spelname is here, we might as well init the forms that need it here.
 */
static Char spelname[SPNUM+1][24]={
  "protection",	/* 0 */
  "magic missile",
  "dexterity",
  "sleep",
  "charm monster",
  "sonic spear",	/* 5 */
  "web",
  "strength",
  "enlightenment",
  "healing",
  "cure blindness",	/* 10 */
  "create monster",
  "phantasmal forces",
  "invisibility",
  "fireball",
  "cold",			/* 15 */
  "polymorph",
  "cancellation",	
  "haste self",
  "cloud kill",
  "vaporize rock",	/* 20 */
  "dehydration",
  "lightning",
  "drain life",
  "invulnerability",
  "flood",		/* 25 */
  "finger of death",
  "scare monster",
  "hold monster",	
  "time stop",
  "teleport away",	/* 30 */
  "magic fire",
  "make a wall",
  "sphere of annihilation",
  "genocide",		/* 34 */
  "summon demon",
  "walk through walls",
  "alter reality",
  "permanence",		/* 38 */
  ""			/* 39 */
};
/*
 *  You get to see this when you learn the spell
 */
static Char speldescript[SPNUM+1][42]={
  /* 1 */
  "generates a +2 protection field",
  "equivalent to a + 1 magic arrow",
  "adds +2 to the casters dexterity",
  "causes some monsters to go to sleep",
  "some monsters may be awed",
  "causes your hands to emit screeches",
  /* 7 */
  "sticky thread to entangle an enemy",
  "adds +2 to casters strength briefly",
  "caster becomes aware of surroundings",
  "restores some hp to the caster",
  "restores sight to a blinded caster",
  "creates a monster near the caster",
  "creates lethal illusions",
  "the caster becomes invisible",
  /* 15 */
  "a ball of fire that burns what it hits",
  "a cone of cold that freezes things",
  "find out yourself what this",
  "negates monster's special abilities",
  "speeds up the casters movements",
  "creates a fog of poisonous gas",
  "this changes rock to air",
  /* 22 */
  "dries up water in the immediate vicinity",
  "your finger will emit a lightning bolt",
  "you and a monster both lose hit points",
  "protect caster from physical attack",
  "avalanche of water to flood the room",
  "calls upon your god to back you up",
  /* 28 */
  "terrify the monster",
  "freeze the monster in his tracks",
  "all movement here ceases for a time",
  "...hopefully away from you",
  "a curtain of fire around the caster",
  /* 33 */
  "makes a wall in the place you specify",
  "anything within it is killed -- dangerous",
  "eliminate a species -- use sparingly",
  "summon a demon to (hopefully) help",
  "lets you walk through walls for a while",
  "God only knows what this will do",
  "make a spell permanent, e.g. strength",
  ""
};


/*
 *  The cast-select-form uses the data above, so it is freed-up here
 */
//Short inventory_select_numItems;
// note spelname is in object_actions.c
//extern spelname[SPNUM+1][24];
#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
void free_cast_select(FormPtr frm)
{
  Short i;
  ListPtr lst;
  VoidHand h;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
  LstSetListChoices(lst, NULL, 0);
  for (i = 0 ; i < cast_qty ; i++) {
    free_me(cast_select_items[i]);
  }
  if (cast_qty > 0) {
    free_me(cast_select_items);
  }
}
#undef free_me
/*
 *  The known-items-form uses the data above, so it is initialized here
 *  Actually it doesn't.  It only does scrolls and potions, not spells.
 */
void init_known_items(FormPtr frm)
{
  // initialize the list
  ListPtr lst;
  Short i, k, tot;
  Char *objname;
  ControlPtr btn;
  tot = MAXPOTION + MAXSCROLL;
  FrmCopyTitle(frm, "Known Items");

  cast_select_items = (Char **) md_malloc(sizeof(Char *) * (2+tot));

  k = 0;
  cast_select_items[k] = md_malloc(sizeof(Char) * 16);
  StrCopy(cast_select_items[k], "Known Potions");
  k++;
  for (i = 0 ; i < MAXPOTION ; i++) {
    if (env->potion_known[i]) {
      objname = get_Of_potion(i, true);
      cast_select_items[k] = md_malloc(sizeof(Char) * (StrLen(objname)));
      StrCopy(cast_select_items[k], objname+4); // skip " of "
      k++;
    }
  }
  cast_select_items[k] = md_malloc(sizeof(Char) * 16);
  StrCopy(cast_select_items[k], "Known Scrolls");
  k++;
  for (i = 0 ; i < MAXSCROLL ; i++) {
    if (env->scroll_known[i]) {
      objname = get_Of_scroll(i, true);
      cast_select_items[k] = md_malloc(sizeof(Char) * (StrLen(objname)));
      StrCopy(cast_select_items[k], objname+4); // skip " of "
      k++;
    }
  }
  cast_qty = k; // remember how many to free, later
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
  LstSetListChoices(lst, cast_select_items, k);
  LstSetSelection(lst, -1);
  // also, disable the cancel button.
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_ca_cancel));
  CtlHideControl(btn);
}

/*
 *  The cast-select-form uses the data above, so it is initialized here
 */
//Boolean cast_not_genie = true;
void init_cast_select(FormPtr frm)
{
  // initialize the list
  ListPtr lst;
  Short i, k;
  Char c = 'a';
  //  Char my_buf[40];
  Boolean do_known = true;

  if (cast_form_use == CAST_GENIE) {
    FrmCopyTitle(frm, "Learn Which Spell?");
    do_known = false;
  }
  cast_select_items = (Char **) md_malloc(sizeof(Char *) * SPNUM);
  //  for (i = 0 ; i < SPNUM ; i++) {
  //    cast_select_items[i] = md_malloc(sizeof(Char) * 27);
  //  }

  for (i = 0, k = 0 ; i < SPNUM ; i++, c++) {
    if (c > 'z') c = 'A';
    if (env->spelknow[i] == do_known) {
      cast_select_items[k] = md_malloc(sizeof(Char) * (StrLen(spelname[i])+4));
      StrPrintF(cast_select_items[k], "%c) %s", c, spelname[i]);
      k++;
    }
  }
  cast_qty = k;

  // Yay.
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_ca));
  LstSetListChoices(lst, cast_select_items, k);
  LstSetSelection(lst, -1);
}

/*
 *  function to read a book (real reason the spell names are in this file)
 */
Boolean readbook(Short arg)
{
  Short i,tmp;
  Char buf[80];

  if (env->cdude[BLINDCOUNT]) {
    message("But you can't see!");
    return false;
  }

  if (arg<=3) 
    i = rund(((tmp=splev[arg])!=0)?tmp:1); 
  else {
    if (arg >= 21) 
      arg = 20;	/* # entries in splev = 21 */
    i = rnd( ((tmp=splev[arg]-9)!=0) ?tmp:1 ) + 9;
  }
  env->spelknow[i]=1;
  StrPrintF(buf, "You learn: %s", spelname[i]);
  message(buf);
  message(speldescript[i]);
  if (rnd(10)==4) {
    message("You feel more educated!"); 
    env->cdude[INTELLIGENCE]++; 
    print_stats();
  }
  return true;
}
/*
 *  function to rub a brass lamp and irritate a djinn
 * xxxx spell selection is not implemented
 */
#define forget() (env->item[px][py]=env->know[px][py]=0)
extern Char speldescript[SPNUM+1][42];
extern Char spelname[SPNUM+1][24];
void obrasslamp()
{
  Short px = env->playerx;
  Short py = env->playery;
  Short i;

  i=rnd(100);
  if (i>90) {
    message("The genie hates being disturbed!");
    losehp(env->cdude[HP]/2+1);
    //    beep();
  }
  /* higher level, better chance of spell */
  else if ( (rnd(100)+env->cdude[LEVEL]/2) > 80) {
    message("A magic genie appears!");
    //    message("  What spell would you like? : ");
    //    while ((a=getcharacter())=='D') { 	
    //      seemagic(99);
    //      cursors();  
    //      message("  What spell would you like? : ");
    //    }
    cast_form_use = CAST_GENIE;
    // can I nap here?
    FrmPopupForm(CastForm);
    return;
  }
  else message("nothing happened");
  if (rnd(100) < 15) {
    message("The genie prefers not to be disturbed!");
    forget();
    env->cdude[LAMP]=0;  /* chance of finding lamp again */
  }
  print_stats();
}
void obrasslamp_learn(Short i)
{
  Short px = env->playerx;
  Short py = env->playery;
  Char buf[80];
  env->spelknow[i]=1;
  StrPrintF(buf, "You learn: %s", spelname[i]);
  message(buf);
  message(speldescript[i]);
  //  message("The genie prefers not to be disturbed again");
  message("The genie vanishes!");
  forget();
  print_stats();
}
#undef forget

/////////////////////////////////////////////////////////////////////
/*
 * subroutine to handle a teleport trap +/- 1 level maximum
 */
void oteleport(Boolean err)
{
  Short tmp;

  if (err)
    if (rnd(151)<3) {
      env->cdude[HP] = -1;// just to be sure
      died(264, true);  // stuck in a rock
      // xxx Hey, does it make sense if you have WTW?
    }

  /* show ?? on bottomline if been teleported	*/
  env->cdude[TELEFLAG] = true;

  if (env->level==0) 
    tmp=0;
  else if (env->level < MAXLEVEL) { 	/* in dungeon */
    tmp=rnd(5)+env->level-3; 
    if (tmp>=MAXLEVEL) 
      tmp=MAXLEVEL-1;
    if (tmp<0) 
      tmp=0; 
  }else{ 				/* in volcano */
    tmp=rnd(4)+env->level-2; 
    if (tmp>=MAXLEVEL+MAXVLEVEL) 
      tmp=MAXLEVEL+MAXVLEVEL-1;
    if (tmp<MAXLEVEL) 
      tmp=0; 
  }
  env->playerx = rnd(MAXX-2);
  env->playery = rnd(MAXY-2);
  if (env->level != tmp) {
    newcavelevel(tmp);
    // actually it might call positionplayer already.  hm.
  }
  positionplayer(); // seems like a good idea anyway
  o_draws();
}

/////////////////////////////////////////////////////////////////////

/*
  function to enchant armor player is currently wearing
  */
Boolean enchantarmor()
{
  Short tmp;
  Long *c = env->cdude;
  if (c[WEAR] < 0) { 
    // no armor, try for shield
    if (c[SHIELD] < 0) { 
      //      beep(); 
      message("You feel a sense of loss"); 
      return false; 
    } else { 
      tmp = env->iven[c[SHIELD]]; 
      if (tmp != OSCROLL && tmp != OPOTION) { 
	// you can use a scroll or potion as a shield??
	env->ivenarg[c[SHIELD]] += 1; 
	recalc();
	//	print_stats();
      } 
    } 
  } else {
    // ok enchant the armor
    tmp = env->iven[c[WEAR]];
    if (tmp != OSCROLL && tmp != OPOTION)  { 
      env->ivenarg[c[WEAR]] += 1;
      recalc();
      //      print_stats();
    }
  }
  return true;
}

/*
  function to enchant a weapon presently being wielded
  */
Boolean enchweapon()
{
  Short tmp;
  Long *c = env->cdude;  
  if (c[WIELD]<0) { 
    // beep(); 
    message("You feel depressed"); 
    return false; 
  }
  tmp = env->iven[c[WIELD]];
  if (tmp != OSCROLL && tmp != OPOTION) { 
    env->ivenarg[c[WIELD]] += 1;
    if (tmp == OCLEVERRING)
      c[INTELLIGENCE]++;  
    else if (tmp == OSTRRING)
      c[STREXTRA]++;  
    else if (tmp == ODEXRING)
      c[DEXTERITY]++;		  
    recalc();
    //    print_stats();
  }
  return true;
}

/*
 *	function to adjust time when time warping and taking courses in school
 */
Char time_change[26] = { 
  HASTESELF, HERO,       ALTPRO,         PROTECTIONTIME, DEXCOUNT,
  STRCOUNT,  GIANTSTR,   CHARMCOUNT,     INVISIBILITY,   CANCELLATION,
  HASTESELF, AGGRAVATE,  SCAREMONST,     STEALTH,        AWARENESS,
  HOLDMONST, HASTEMONST, FIRERESISTANCE, GLOBE,          SPIRITPRO,
  UNDEADPRO, HALFDAM,    SEEINVISIBLE,   ITCHING,        CLUMSINESS,
  WTW
};

// Note that this is also called with a large negative argument
// to create "permanence" of effects.  FREAKY.
void adjtime(Long time, Boolean show_stats)
{
  Short i;
  Long tmp;
  for (i = 0 ; i < 26 ; i++) {
    tmp = env->cdude[(Short) time_change[i]];
    if ( tmp ) {
      // subtract time, going no lower than 1.
      env->cdude[(Short) time_change[i]] = (tmp > time) ? (tmp - time) : 1;
    }
  }
  regen(show_stats); 
  // regen will fix all the counters that are at 1 now :-)
}


/*
	data for the function to read a scroll
 */
static Char curse[] =
{ 
  BLINDCOUNT, CONFUSE, AGGRAVATE, HASTEMONST, ITCHING,
  LAUGHING, DRAINSTRENGTH, CLUMSINESS, INFEEBLEMENT, HALFDAM };
static Char exten[] =
{ 
  PROTECTIONTIME, DEXCOUNT, STRCOUNT, CHARMCOUNT,
  INVISIBILITY, CANCELLATION, HASTESELF, GLOBE, SCAREMONST, HOLDMONST, TIMESTOP };

/*
	function to read a scroll
 */
Boolean read_scroll(Short typ)
{
  Short i,j;
  Short yh, xh, yl, xl;
  Long *c = env->cdude;
  Char buf[80];
  if (env->cdude[BLINDCOUNT]) {
    message("But you can't see!");
    return false;
  }
  if (typ<0 || typ>=MAXSCROLL) return false;  // be sure we are within bounds
  env->scroll_known[typ] = true;
  switch(typ) {
  case 0:	
    if (enchantarmor())
      message("Your armor glows for a moment"); // (overwritten if no armor)
    return true;

  case 1: 
    if (enchweapon())
      message("Your weapon glows for a moment"); 
    return true;  /* enchant weapon */

  case 2:	
    message("You have been granted enlightenment!");
    yh = min(env->playery+7,MAXY);	
    xh = min(env->playerx+25,MAXX);
    yl = max(env->playery-7,0);		
    xl = max(env->playerx-25,0);
    for (i=yl; i<yh; i++) 
      for (j=xl; j<xh; j++)  
	env->know[j][i]=1;
    nap(1500);	
    //    draws(xl,xh,yl,yh);	
    o_draws();
    return true;

  case 3:	
    message("This scroll seems to be blank");	
    return true;

  case 4:	
    createmonster(makemonst(env->level+1));  
    recalc_screen(); // I think I need this
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh(); // I think I need this
    return true;  /*  this one creates a monster  */

  case 5:	
    something(env->level);
    recalc_screen(); // I think I need this
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    refresh(); // I think I need this
    /* create artifact	*/  
    return true;

  case 6:	
    c[AGGRAVATE]+=800; 
    return true; /* aggravate monsters */

  case 7:	
    i = -rnd(1500) ;             /* time warp */
    if ( is_male() ) i += 300;     /* fast wie im richtigen Leben ;-*/
    env->gtime += i;                       // "just like in real life"--bas

    if (i >= 0)
      StrPrintF(buf, "You went ahead in time by %d mobuls",
		(i+99)/100);
    else
      StrPrintF(buf, "You went back in time by %d mobuls",
		-(i+99)/100);
    adjtime((Long) i, true); // make curr. effect-timers longer or shorter
    message(buf);
    return true;

  case 8:	
    oteleport(false);
    return true;	/*	teleportation */

  case 9:	
    c[AWARENESS] += 1800;  
    return true;	/* expanded awareness	*/

  case 10:	
    c[HASTEMONST] += rnd(55)+12; 
    message("You feel nervous");
    return true;	/* haste monster */

  case 11:	
    heal_monsters();
    message("You feel uneasy");
    return true;	/* monster healing */

  case 12:	
    c[SPIRITPRO] += 300 + rnd(200); 
    spell_stats(); 
    return true; /* spirit protection */

  case 13:	
    c[UNDEADPRO] += 300 + rnd(200); 
    spell_stats(); 
    return true; /* undead protection */

  case 14:	
    c[STEALTH] += 250 + rnd(250);  
    spell_stats(); 
    return true; /* stealth */

  case 15:	
    message("You have been granted enlightenment!"); 
    /* magic mapping */
    for (i=0; i<MAXY; i++) 
      for (j=0; j<MAXX; j++)  
	env->know[j][i]=1;
    nap(1500);	
    o_draws();
    return true;

  case 16:	
    c[HOLDMONST] += 30; 
    spell_stats(); 
    return true; /* hold monster */

  case 17:	
    for (i=0; i < NINVT; i++)	/* gem perfection */
      switch(env->iven[i]) {
      case ODIAMOND:	
      case ORUBY:
      case OEMERALD:	
      case OSAPPHIRE:
	j = env->ivenarg[i];  
	j &= 255;  
	j <<= 1;
	if (j > 255) 
	  j=255; /*double value */
	env->ivenarg[i] = j;	
	break;
      }
    break;

  case 18:	
    /* spell extension: double the time remaining */ 
    for (i=0; i<11; i++)
      c[(Short) exten[i]] <<= 1;
    spell_stats();
    break;

  case 19:	
    for (i=0; i < NINVT; i++) { /* identify */
      j = env->ivenarg[i];
      if (env->iven[i]==OPOTION && j >= 0 && j < MAXPOTION)
	env->potion_known[j] = true;
      if (env->iven[i]==OSCROLL && j >= 0 && j < MAXSCROLL)
	env->scroll_known[j] = true;
    }
    break;

  case 20:	
    for (i=0; i<10; i++)	/* remove curse */
      if (c[(Short) curse[i]])
	c[(Short) curse[i]] = 1; // reuse the normal curse-removal code
    break;

  case 21:	
    annihilate();
    break;	/* scroll of annihilation */

  case 22:	
    godirect(R_PULVR, 150, "The ray hits the %s", 0, ' ');
    /* pulverization */
    break;

  case 23:  
    c[LIFEPROT]++;  
    break; /* life protection */
  };
  return true;
}

/*
 *     enlighten the player. dyl/dxl negative.
 */

void enlighten(Short dyl, Short dyh, Short dxl, Short dxh)
{
  Short yl, yh, xl, xh, i, j, px,py;
  px = env->playerx;
  py = env->playery;

  yl = py+dyl;     /* enlightenment */
  yh = py+dyh;   
  xl = px+dxl;   
  xh = px+dxh;
  vxy(&xl,&yl);   
  vxy(&xh,&yh); /* check bounds */
  for (i=yl; i<=yh; i++) /* enlightenment	*/
    for (j=xl; j<=xh; j++)
      env->know[j][i]=1;
  o_draws();
}

/////////////////////////////////////////////////////////////////////
/*
 *  Spill a potion.  even works on that volcano level
 */
void spill()
{
  Short had_wall, px,py,i,j;
  Char buf[80];
  px = env->playerx;
  py = env->playery;
  had_wall = 0;
  if (env->level == 0) {
    message("nothing happens");
    return;
  }
  for   ( i=max(px-1, 1); i <= min(px+1, MAXX-2); i++) {
    for ( j=max(py-1, 1); j <= min(py+1, MAXY-2); j++) {
      if( env->item[i][j] == OWALL ){
	env->item[i][j] = 0;
	env->know[i][j] = 0;
	had_wall++;
      } /* yes, this works on ALL levels */
    }
  }
  if (had_wall) {
    StrPrintF(buf, "The potion dissolves the wall%s",
	      (had_wall > 1) ? "s!" : "s");
    o_draws();
    message(buf);
  } else
    message("nothing happens");
}

/*
 *  Quaff a potion
 */
void quaffpotion(Short pot) 
{
  Short i,j,k;
  Long *c = env->cdude;

  if (pot<0 || pot>=MAXPOTION) return; /* check for within bounds */
  env->potion_known[pot] = true;
  switch(pot) {
  case 9: 
    message("You feel greedy . . .");   
    nap(1500);
    for (i=0; i<MAXY; i++)
      for (j=0; j<MAXX; j++)
	if ((env->item[j][i]==OGOLDPILE) || (env->item[j][i]==OMAXGOLD)) {
	  env->know[j][i]=1; 
	  update_screen_cell(j,i);
	  refresh();
	}
    showplayer(env->playerx, env->playery);  
    return;

  case 19: 
    message("You feel greedy . . .");   
    nap(1500);
    for (i=0; i<MAXY; i++)
      for (j=0; j<MAXX; j++) {
	k = env->item[j][i];
	if ((k==ODIAMOND) || (k==ORUBY) || (k==OEMERALD) || (k==OMAXGOLD)
	    || (k==OSAPPHIRE) || (k==OLARNEYE) || (k==OGOLDPILE))
	  {
	    env->know[j][i]=1; 
	    update_screen_cell(j,i);
	    refresh();
	  }
      }
    showplayer(env->playerx, env->playery);  
    return;

  case 20: 
    c[HP] = c[HPMAX]; 
    break;	/* instant healing */

  case 1:	
    message("You feel better");
    if (c[HP] == c[HPMAX])  raisemhp(1);
    else if ((c[HP] += rnd(20)+20+c[LEVEL]) > c[HPMAX]) c[HP]=c[HPMAX];  
    break;

  case 2:	
    message("Suddenly, you feel much more skillful!");
    raiselevel();
    raisemhp(1); 
    return;

  case 3:	
    message("You feel strange for a moment"); 
    c[rund(6)]++;  
    break;

  case 4:	
    message("You feel more self confident!");
    c[WISDOM] += rnd(2);  
    break;

  case 5:	
    message("Wow!  You feel great!");
    if (c[STRENGTH]<12) c[STRENGTH]=12; 
    else c[STRENGTH]++;  
    break;

  case 6:	
    message("Aaaoooww!  You're looking good now!");  
    //    message("Your charm went up by one!");  
    c[CHARISMA]++;  
    break;

  case 8:	
    message("You feel smart!");
    //    message("Your intelligence went up by one!");
    c[INTELLIGENCE]++;  
    break;

  case 10: 
    for (i=0; i<MAXY; i++)
      for (j=0; j<MAXX; j++)
	if (env->mitem[j][i]) {
	  env->know[j][i]=1; 
	  update_screen_cell(j,i);
	  refresh();
	}
    /*	monster detection	*/
    return;

  case 12: 
    message("This potion has no taste to it");  
    return;

  case 15: 
    message("WOW!!!  You feel Super-fantastic!!!");
    if (c[HERO]==0)
      for (i=0; i<6; i++)
	c[i] += 11;
    c[HERO] += 250;  
    break;

  case 16: 
    message("You have greater intestinal fortitude!");
    //    message("You have a greater intestinal constitude!");
    c[CONSTITUTION]++;  
    break;

  case 17: 
    //    message("You now have incredibly bulging muscles!!!");
    message("Your muscles are bulging!!!");
    if (c[GIANTSTR]==0) c[STREXTRA] += 21;
    c[GIANTSTR] += 700;  
    spell_stats();
    break;

  case 18: 
    message("You feel a chill run up your spine!");
    c[FIRERESISTANCE] += 1000;  
    spell_stats();
    break;

  case 0:	
    message("You fall asleep. . .");
    i=rnd(11)-(c[CONSTITUTION]>>2)+2;  
    while(--i>0) { 
      //      parse();   // move monsters? xxx
      nap(100);
    }
    message("You woke up!");  
    return;

  case 7:	
    message("You become dizzy!");
    if (--c[STRENGTH] < 3) c[STRENGTH]=3;  
    break;

  case 11: 
    message("You stagger for a moment . .");
    for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++)
      env->know[j][i]=0;
    nap(1500);	
    /* potion of forgetfulness */
    o_draws();
    return;

  case 13: 
    message("You can't see anything!");  /* blindness */
    c[BLINDCOUNT]+=500;  
    return;

  case 14: 
    message("You feel confused"); 
    c[CONFUSE]+= 20+rnd(9); 
    return;

  case 21: 
    message("You don't seem to be affected");  
    return; /* cure dianthroritis */

  case 22: 
    message("You feel a sickness engulf you"); /* poison */
    c[HALFDAM] += 200 + rnd(200);  
    return;

  case 23: 
    message("You feel your vision sharpen");	/* see invisible */
    c[SEEINVISIBLE] += rnd(1000)+400;
    see_invisible(); // maybe should refresh the screen?
    return;
  };
  print_stats();
  return;
}

/////////////////////////////////////////////////////////////////////
// hm, nearly the same as i_used_too_much_heap in main.c
void o_draws()
{
  check_player_position(env->playerx, env->playery, false);
  recalc_screen();
  showcell(env->playerx, env->playery);
  showplayer(env->playerx, env->playery);
  refresh();
  print_stats();
}
