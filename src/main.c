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
#include <Graffiti.h> /* just for GrfGetState */
#include <CharAttr.h> /* for IsDigit */ // why is this not working for 3.1??
//#include "callback.h"
#include "iLarnRsc.h"
#include "Globals.h"

#ifndef I_AM_COLOR
#include <System/Globals.h> // for font stuff
#else /* I_AM_COLOR */
// hm, do I need something for font stuff?  guess not
#include "color.h" // for finding color memo
void beam_iLarn();
#endif /* I_AM_COLOR */


void    i_used_too_much_heap();
// xxx Let's try moving THIS son: ok, no crashes in emulator, must be ok
static Boolean main_help_handle_cmd(Char c, Boolean *took_timep) SEC_4;
static Boolean OpenDatabase(void);
static Boolean ApplicationHandleEvent(EventPtr event);
static void    readLarnPrefs();
//static void    writeLarnPrefs();
static Word    StartApplication(void);
static void    StopApplication(void);
static void    EventLoop(void);

DmOpenRef       iLarnDB;
DmOpenRef       iLarnSaveDB;

Boolean newlevel_p = true;
extern Short in_store;

// see types.h for the everything-struct.
struct everything *env;

//#define WIZARD true
#define WIZARD false

LarnPreferenceType my_prefs = {
  6, // run_walk_border
  2, // walk_center_border
  true, // male
  true, // run_on ... this incorporates dave mode
  false, // stay centered
  true, // sound_on
  false, // use hardware
  true, // use large font by default
  false, // -L18
  false, // use keyboard hjklyubnHJKLYUBN
  false, // rogue-relative
  false, // coward_on
  {0, 0, 0, 0, // the hardware buttons
   0, 0, 0, 0}, // the hardware buttons
  "",
  false, // color
  false  // black_bg
};


Boolean init_game()
{
  //  seed_random();
  SysRandom(TimGetSeconds());
  env = (struct everything *) md_malloc(sizeof(struct everything));
  //  message("Welcome to iLarn");
  alloc_message_log(); // must come BEFORE clear_env_globals is called. heh.
  alloc_env(env);
  if (!restore_all(env))
    return false;
  if (has_object(OLARNEYE))
    got_larneye(); // make demons visible!
  // try to restore level.. if we can't.. do something funky
  // this stuff is pulled from makeplayer in create.c
  // perhaps i also need recalc();
  if (!getlevel(env)) {
    env->beenhere[(Short) env->level] = false; // don't save nonexistent level!
    newcavelevel(env->level); // no worries, playerx/y is made consistent.
  } else 
    am_i_on_level_0(env->level==0);
  return true;
}

/*	main.c		*/
/* This game is bad for you. It is evil. It will rot your brain. */
// ok, if you say so.



// This is the redraw-absolutely-everything routine
// used in form_prefs
void i_used_too_much_heap()
{
  //  WinEraseWindow(); // wonder if that works
  clear_visible();
  recalc_screen();
  showcell(env->playerx, env->playery);
  showplayer(env->playerx, env->playery);
  print_stats();
  refresh();
}

// this is the new thing I have learned today, copying to offscreen window.
//WinHandle OffscreenCopyWinH = 0;


Boolean is_male() { return my_prefs.gender_male; }
// Hey! When I have prefs, remember to CHANGE that!


//#define abs(a)                                  (((a) >= 0) ? (a) : (-(a)))
//#define sgn(x) ( (x) >= 0 ? 1 : -1 )

///* The screen can have 15 rows, but I took 4 for messages and stats */
//#define visible_h 11
///* You can put about 19.9 m's on a memo line.. so.. we'll have 20 cols */
//#define visible_w 20

Boolean first_time_through = true;
Boolean save_me = true;
Boolean i_am_dead = false;
Short count = 0;

Boolean casting_a_spell = false;
Boolean fighting_not_casting = false;
Short cast_form_use; // decide what I'm using the list selection form for
extern Short diroffx[];
extern Short diroffy[];

// PREFS: need a hardware-button-handler
// also need to decide what actions to offer there

extern Short stats_area_dirty;
Boolean recalc_changed_stats;
void start_of_Play_Level_loop()
{
  Boolean skip_for_popup = false;
  if (env->cdude[HP] <= 0) return; // dude, you are dead
  // "M A I N  L O O P"
  //  env->pending_splash = false;

  // if dropflag is true, we're on the way into the store, not on the way out
  if (in_store == STORE_DND && !env->dropflag)
    in_store = STORE_NONE;

  if (!env->dropflag) {
    if (lookforobject(0)) {
      showplayer(env->playerx, env->playery); // this is necessary if you '.'d.
      FrmPopupForm(QuestionForm);
      skip_for_popup = true;
    }
  } else
    env->dropflag = false;

  if (!env->hitflag && !first_time_through && !skip_for_popup) {
    if (env->cdude[HASTEMONST]) 
      movemonst(); 
    movemonst(); 
    if (env->cdude[HP] <= 0) return; // dude, you are dead
  }	// move the monsters

  /*
  if (viewflag==0) 
    showcell(playerx,playery); 
  else viewflag=0;	// show stuff around player
  
  if (hit3flag) 
    flushall();
//  hitflag = hit3flag = 0;	
  nomove=1;
  bot_linex();	// update bottom line
    
  while (nomove)//	get commands and make moves
    {
      if (hit3flag) 
	flushall();
      nomove=0; 
      parse(&auto_pray); // (HERE WE GET AN EVENT)
    }	
  */
  if (!first_time_through && !skip_for_popup)
    regen(true);	//	regenerate hp and spells

  env->hitflag = false;
  first_time_through = false;
  /*
  if (c[TIMESTOP]==0)
    if (--rmst <= 0) { 	
      rmst = 120-(level<<2); 
      fillmonst(makemonst(level)); 
    }
  */
  refresh();
  //  print_stats();
  /* XXXX We do want to print_stats if hit pts changed.. maybe it would
     be better to do this where they change, like regen does */
  if (env->pending_splash) {
    show_messages(); //  FrmPopupForm(SplashForm);
    //    WinDrawChars("b", 1, 155, 160-11);
  }
  else if (stats_area_dirty || recalc_changed_stats)
    print_stats(); // This might be too eager.
}

Short get_click_dir(Short tmp_x, Short tmp_y)
{
  if ((abs(tmp_x)) > 2 * (abs(tmp_y))) {
    // mostly x, move in the x direction
    if (tmp_x > 0)     return EAST;
    else               return WEST;
  } else if ((abs(tmp_y)) > 2 * (abs(tmp_x))) {
    // mostly y
    if (tmp_y > 0)     return SOUTH;
    else               return NORTH;
  } else {
    // closer to diagonal
    if (tmp_y > 0) {
      // southish
      if (tmp_x > 0)   return SOUTHEAST;
      else             return SOUTHWEST;
    } else {
      // northish
      if (tmp_x > 0)   return NORTHEAST;
      else             return NORTHWEST;	
    }
  }
}

// used by tapping, hardware buttons, and hjklyubn.
static Boolean common_move(Short click_dir)
{
  Short i;
  i = moveplayer(click_dir);
  if (i==1 || i==2 || i==3) {
    // xxx i==3 DOES take time.
    return false; // there's a wall in the way, or something
  } else {
    // identical to what happens in penDownEvent
    check_player_position(env->playerx, env->playery, false);
    recalc_screen();
    showcell(env->playerx, env->playery);
    showplayer(env->playerx, env->playery);
    do_feep(200,9);
    refresh();
    return true;
  }
}

// I could be more clever.  Probably should.
static Boolean main_keyb_handle_cmd(Char c, Boolean *took_timep)
{
  // These are keys that do stuff IFF you have keyboard enabled.
  Short click_dir = 0;
  Char lc_c = (c < 'a') ? c + ('a'-'A') : c; // lc_c is the lower-case of c.
  switch(lc_c) {
  case 'h': // w
    click_dir = WEST;
    break;
  case 'j': // s
    click_dir = SOUTH;
    break;
  case 'k': // n
    click_dir = NORTH;
    break;
  case 'l': // e
    click_dir = EAST;
    break;
  case 'y': // nw
    click_dir = NORTHWEST;
    break;
  case 'u': // ne
    click_dir = NORTHEAST;
    break;
  case 'b': // sw
    click_dir = SOUTHWEST;
    break;
  case 'n': // se
    click_dir = SOUTHEAST;
    break;
  default: 
    *took_timep = false;
    return false;
  }
  if (!casting_a_spell) {
    // Erase message, and deal with "--more--" if any
    if (message_clear()) {
	show_messages();
	*took_timep = false;
	return true;
    }
    // Ok now move.
    if (c == lc_c)
      *took_timep = common_move(click_dir);
    else {
      a_better_run(click_dir);
      first_time_through = *took_timep = true;
    }
  } else {
    draw_cast_slices(); //re-invert! works 'cause display otherwise unchanged
    casting_a_spell = false;
    if (fighting_not_casting) {
      // we're really Fighting in direction, we just borrowed cast_slices.
      fighting_not_casting = false;
      fight(click_dir);
    } else {
      print_stats();//maybe needed.  must be after the draw_cast_slices.
      dir_task_dispatch(click_dir);
    }
  }
  return true;
}

static Boolean main_help_handle_cmd(Char c, Boolean *took_timep)
{
  Short i, numItems;
  *took_timep = false;
  // ...
  switch(c) {
    // XXXXX Debugging!!!! Remove these!!!!
    // START OF DEBUGGING
    /*
  case 'U':
    ostairs(true);
    return true;
  case 'D':
    ostairs(false);
    return true;
  case 'X':
    for (i = 1 ; i < MAXLEVEL+MAXVLEVEL ; i++)
      if (i != env->level)
	env->beenhere[i] = false;
    return true;
  case 'Y':
    env->cdude[HP] = env->cdude[HPMAX] = 999;
    return true;
    */
    // END OF DEBUGGING
  case 'c':
    if (env->cdude[SPELLS] <= 0)
      message("You're out of spells!");
    else {
      for (i = 0, numItems = 0 ; i < SPNUM ; i++)
	if (env->spelknow[i])
	  numItems++;
      if (numItems) {
	cast_form_use = CAST_SPELL;
	FrmPopupForm(CastForm);
	// I think that 'cast' takes care of took-time-ing.
      } else
	message("but you don't know any spells!");
    }
    return true;
  case 'i':
    for (i = 0, numItems = 0 ; i < NINVT ; i++)
      if (env->iven[i])
	numItems++;
    if (numItems) {
      in_store = STORE_NONE;
      FrmPopupForm(InvSelectForm);
      // Inventory will have to take care of took-time-ing
      // bcause you might hit 'cancel'
    } else
      message("your pack is empty!");
    return true;
  case 't': //  case 'T':
    *took_timep = takeoff();
    return true;
  case 'Q':
    if (0 == FrmAlert(QuitP)) {
      // kill character... test create character...
      //      clear_env();
      //      FrmGotoForm(PickChForm);
      died(256, true);
    }
    return true;
  case '$':
    // drop gold.. pop up a transaction form.
    // "How much gold do you drop?"  add a button for 'all'
    if (env->item[env->playerx][env->playery])
      message("There's something here already"); 
    else if (env->cdude[GOLD] <= 0)
      message("You don't have any!"); 
    else {
      //      i_used_too_much_heap();
      in_store = STORE_NONE;
      FrmPopupForm(TransForm);
      *took_timep = true; // I think
    }
    return true;
  case '.':
    // rest
    //    start_of_Play_Level_loop();
    message("resting");
    *took_timep = true;
    return true;
  case 'd': //  case 'D':
    cast_form_use = CAST_DISPLAY;
    FrmPopupForm(CastForm);
    return true;
  case '^':
    id_trap();
    return true;
  case 'w': //  case 'g':
    show_packweight();
    // This is just a test:
    if (WIZARD)
      for (i = 0 ; i < 10 ; i++)
	raiselevel();
    return true;
  case 's': //  case 'P':
    print_tax();
    // This is just a test:
    //    if (WIZARD) {
    //      raiselevel();
      // JUST FOR TESTING,
      // env->spelknow[S_GEN] = 1;
      // JUST FOR TESTING,
      //      env->gtime += TIMELIMIT;
    //    }
    return true;
    /*
  case 'f':
    if (env->level > 0) {
      toggle_itsy();
      move_visible_window(env->playerx, env->playery, true);
      i_used_too_much_heap();
    }
    return true;
    */
  case 'Z':
    if (env->cdude[LEVEL] > 9) {
      // I am not covering the case of "teleport from H, change font"
      message_clear();
      oteleport(env->cdude[HARDGAME]>0); // if 'true', chance of dying
      *took_timep = true; // I think
    } else
      message("you don't know how.");
    return true;
  case 'F':
    fighting_not_casting = true; // borrow the spell-casting slices! a kludge
    draw_cast_slices(); // we'll call fight(dir) when we have a dir.
    //    message("Fight in which direction?");
    return true;    
  default:
    if (my_prefs.use_keyboard)
      return main_keyb_handle_cmd(c, took_timep);
    break;
  }
  return false;
}


static Boolean buttonsHandleEvent(EventPtr e)
{
  Boolean handled = false;
  Boolean took_time = true;
  Short dispatch_type = 0;
  //  Word which_form;

  // I've decided that "if (!(ChrIsHardKey(e->data.keyDown.chr)) ..)" is 
  // not so good, since that includes hardPowerChr, hardCradleChr etc.!
  if ( ((e->data.keyDown.chr < hard1Chr) || (e->data.keyDown.chr > hard4Chr))
       && (e->data.keyDown.chr != calcChr)
       && (e->data.keyDown.chr != findChr)
       && (e->data.keyDown.chr != pageUpChr)
       && (e->data.keyDown.chr != pageDownChr))
    return false; // it's NOT a hardware button.. probably graffiti.

  // Ok, we are dealing now ONLY with the 8 keys that we might rebind.
  // IF we are in main form, then rebind what we're rebinding.
  // ELSE if it is rebound in main form, just MASK it.

  // <incs>/UI/Chars.h is useful....   ChrIsHardKey(c)
  // hard[1-4]Chr, calcChr, findChr.
  switch (e->data.keyDown.chr) {
  case hard1Chr:      // datebook
    dispatch_type = my_prefs.hardware[0];
    break;
  case hard2Chr:      // address
    dispatch_type = my_prefs.hardware[1];
    break;
  case hard3Chr:      // todo
    dispatch_type = my_prefs.hardware[2];
    break;
  case hard4Chr:      // memos
    dispatch_type = my_prefs.hardware[3];
    break;
  case pageUpChr:
    dispatch_type = my_prefs.hardware[4];
    break;
  case pageDownChr:
    dispatch_type = my_prefs.hardware[5];
    break;
  case calcChr:
    dispatch_type = my_prefs.hardware[6];
    break;
  case findChr:
    dispatch_type = my_prefs.hardware[7];
    break;
  default:
    return false;
  }

  if (FrmGetActiveFormID() != MainForm)
    // IF the key is not bound, return false, else return TRUE to MASK it.
    // unless it is up/down.
    return ( dispatch_type != HWB_NOOP &&
	     e->data.keyDown.chr != pageUpChr &&
	     e->data.keyDown.chr != pageDownChr );

  // OK, we ARE in the main form.  
  switch (dispatch_type) {
  case HWB_NOOP:
    return false;
    // n = 1 e = 3 s = 5 w = 7
    // x = {1 2 3 4} -> x * 2 - 1
  case HWB_N:
  case HWB_S:
  case HWB_E:
  case HWB_W:
    // because we were careful in defines.h, HWB_N == NORTH, etc.
    took_time = common_move(dispatch_type);
    handled = true;
    break;
  case HWB_CAST:
    handled = main_help_handle_cmd('c', &took_time);
    break;
  case HWB_FONT:
    if (env->level > 0 && toggle_itsy()) {
      move_visible_window(env->playerx, env->playery, true);
      i_used_too_much_heap();
    }
    took_time = false;
    handled = true;
    break;
  case HWB_INV:
    handled = main_help_handle_cmd('i', &took_time);
    break;
  case HWB_KNOWN:
    handled = main_help_handle_cmd('d', &took_time);
    break;
  case HWB_MAP:
    FrmPopupForm(MapForm);
    handled = true;
    took_time = false;
    break;
  case HWB_REST:
    handled = true;
    break;
  case HWB_TELE:
    handled = main_help_handle_cmd('Z', &took_time);
    break;
  }
  // do that loop thing too.  yeah.
  if (handled && took_time) {
    start_of_Play_Level_loop();
  }
  return handled;
}



Boolean Main_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  Short tmp_x, tmp_y, click_dir;
  Boolean do_run = true, took_time = false;
  //  Char buf[40];

    // CALLBACK_PROLOGUE

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    /* mine */
    /** place any form-initialization stuff here **/
    /** init_game(); should go somewhere ELSE. */
    // use the right default font
    // XXXXXXXXXX  Um maybe here you should be checking OS
    if (!my_prefs.font_large && env->level > 0)
      toggle_itsy();

    /* b/c this stuff will be called when we return from a popup. ? */
    /* end mine */	
    FrmDrawForm(frm);

    // Make new level if needed, display level, display stats, yadda.
    //    move_visible_window(env->playerx, env->playery, true);
    if (newlevel_p) {
      check_player_position(env->playerx, env->playery, false);
      recalc_screen(); // replacing above loop
      showcell(env->playerx, env->playery);
      showplayer(env->playerx, env->playery);
      print_stats();
      env->dropflag = true; // so we ignore what we start out standing on!...
      start_of_Play_Level_loop();
      newlevel_p = false;
      env->pending_splash = false;
    }

    handled = true;
    goto SKIP_THE_REST;
    break;

  case menuEvent:
    MenuEraseStatus(NULL); /* clear menu status from display */
    switch(e->data.menu.itemID) {
    case menu_main_wiz1:
      // make like a scroll of enlightenment
      createitem(OSCROLL, 2L);
      env->cdude[TELEFLAG] = 0;  // now you know what level you're on, too
      goto SKIP_THE_REST;      
      break;
    case menu_main_wiz2:
      // gain experience
      { Short i;
      for (i = 0 ; i < 20 ; i++)
	raiselevel();
      }
      env->cdude[WTW] = 99999L;
      env->cdude[AWARENESS] = 99999L;
      env->cdude[HASTESELF] = 99999L;
      env->cdude[CANCELLATION] = 99999L;
      goto SKIP_THE_REST;      
      break;
      /*
    case menu_main_wiz3:
      // do like stairs up
      ostairs(true);
      goto SKIP_THE_REST;      
      break;
    case menu_main_wiz4:
      // do like stairs down
      ostairs(false);
      goto SKIP_THE_REST;      
      break;
      */
    case menu_mainCenter:
      if (env->level > 0)
	move_visible_window(env->playerx, env->playery, true);
      i_used_too_much_heap();
      goto SKIP_THE_REST;
      break;
    case menu_main_test:
      if (env->level > 0) {
	if (toggle_itsy()) {
	  move_visible_window(env->playerx, env->playery, true);
	  i_used_too_much_heap();
	} else {
	  FrmAlert(FontAlert);
	}
      }
      goto SKIP_THE_REST;      
      break;
    case menu_mainAbout:
      FrmPopupForm(AboutForm);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainSettings:
      FrmPopupForm(PrefsForm);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainButtons:
      FrmPopupForm(HwButtonsForm);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainMoveInstruct:
      FrmHelp(IntroStr);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainGraffiti:
      SysGraffitiReferenceDialog(referenceDefault);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainBeam:
      // XXX Make sure to test this, in the old xcopilot.
#ifdef I_AM_COLOR
      {
	DWord version;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
	if (version < 0x03000000l)  message("can't beam in OS2");
	else  beam_iLarn();
      }
#else
      message("can't beam in OS2 version");
#endif
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainScores:
      FrmPopupForm(ScoreForm);
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainSketch:
      if (env->cdude[HARDGAME] > 0) {
	Char d[10];
	StrPrintF(d, "%d", (Short) env->cdude[HARDGAME]);
	FrmCustomAlert(NoCloneP, d, NULL, NULL);
      } else {
	Word bn; // which button will user press
	bn = FrmCustomAlert(CloneP, (sketch_exists() ?
				     "overwrite your existing clone." :
				     "create a clone of yourself."),
			    NULL, NULL);
	if (bn == 0)
	  if (save_sketch()) // I should check the return value...
	    message("clone saved.");
      }
      handled = true;
      goto SKIP_THE_REST;
      break;
    case menu_mainMap:
      if (env->level > 0)
	FrmPopupForm(MapForm);
      else
	message("You don't need a map for this level.");
      handled = true;
      goto SKIP_THE_REST;
    case menu_mainMsgs:
      FrmPopupForm(MsgLogForm);
      handled = true;
      goto SKIP_THE_REST;
    case menu_mainWiz:
      //      wizard_p = !wizard_p;
      //      init_scroll_potion(wizard_p); // JUST FOR TESTING!
      handled = true;
      goto SKIP_THE_REST;
      //    case menu_cmd_c:
      //    case menu_cmd_i:
      //    case menu_cmd_T:
      //    case menu_cmd_quit:
    default:
      tmp_x = e->data.menu.itemID - MAGIC_MENU_NUMBER;
      if (tmp_x > 0 && tmp_x < 128) {
	handled = main_help_handle_cmd((Char) tmp_x, &took_time);
	if (handled && !took_time)
	  goto SKIP_THE_REST; // hm, except I don't want to for 'T' etc...
      }
      break;
    }
    break; // end case menuEvent

  case keyDownEvent:
    /* graffiti or up/down hardware button. */
    handled = main_help_handle_cmd(e->data.keyDown.chr, &took_time);
    // I'm not doing a numeric count (yet?) so ththththat's all folks
    if (handled && !took_time)
      goto SKIP_THE_REST;
    break;

  case penDownEvent:
    if (!casting_a_spell) {
      if (message_clear()) // hello, why is this not working?
	{ // "--more--"
	  handled = true;
	  show_messages();
	  return handled;
	  //	  goto SKIP_THE_REST; // skip to the end and print messages!
	}
      do_run = my_prefs.run_on;
      tmp_x = e->screenX - 80;
      tmp_y = e->screenY - 80;
      // Next, decide whether to be screen-relative or rogue-relative.
      if (my_prefs.rogue_relative)
	rogue_relativize(&tmp_x, &tmp_y, env->playerx, env->playery);
      if ((abs(tmp_x) < my_prefs.walk_center_border*10) && 
	  (abs(tmp_y) < my_prefs.walk_center_border*10)) {
	handled = true;   // this is a noop, for now.
	print_stats(); // it just refreshes the 'stats' window.
	goto SKIP_THE_REST;
      } else {
	if ((abs(tmp_x) < my_prefs.run_walk_border*10) &&
	    (abs(tmp_y) < my_prefs.run_walk_border*10)) {
	  do_run = false;
	}
	click_dir = get_click_dir(tmp_x, tmp_y);
	if (do_run) {
	  a_better_run(click_dir);
	  first_time_through = true; // a_better_run does all the movemonst!
	  // xxx I should make sure it doesn't err on the generous side now!
	  // I would say that it definitely is erring thus.
	  // but it's my bedtime so i"ll have to re-fix it some time!
	} else {
	  took_time = common_move(click_dir);
	  if (!took_time) goto SKIP_THE_REST;
	}
      }
    } else {
      draw_cast_slices(); //re-invert! works 'cause display otherwise unchanged
      casting_a_spell = false;
      tmp_x = e->screenX - 80;
      tmp_y = e->screenY - 80;
      if ((abs(tmp_x) < 10) &&
	  (abs(tmp_y) < 10)) {
	if (!fighting_not_casting) {
	  env->cdude[SPELLS] += 1; // I'll be nice about it
	  print_stats();
	}
	message("aborted");
      } else {
	click_dir = get_click_dir(tmp_x, tmp_y);
	if (fighting_not_casting) {
	  // we're really Fighting in direction, we just borrowed cast_slices.
	  fighting_not_casting = false;
	  fight(click_dir);
	} else {
	  print_stats();//maybe needed.  must be after the draw_cast_slices.
	  dir_task_dispatch(click_dir);
	  // does dir_task_dispatch call print_stats for me?
	  // maybe it should
	}
      }
      handled = true;
      goto SKIP_THE_REST; // do not move monsters right now
    }

    handled = true;
    count = 0;
    break; // end penDownEvent

  //  case anotherEvent:
  default:
    goto SKIP_THE_REST;
    break;
  } // end switch event type!

  /* make monsters move... */

  start_of_Play_Level_loop();
  return handled;

SKIP_THE_REST:

    // CALLBACK_EPILOGUE

  // JUST in case for all the dudes who have called 'message' to report stuff.
  if (env && env->pending_splash)
    show_messages();


  return handled;
}


      

/* how to exit a popup form that you can enter from more than one place */
void LeaveForm()
{
   FormPtr frm;
   frm = FrmGetActiveForm();
   FrmEraseForm (frm);
   FrmDeleteForm (frm);
   FrmSetActiveForm (FrmGetFirstForm ());
   //   if (false)
   //     i_used_too_much_heap();
}

    

/*
Boolean Splash_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
    
    // CALLBACK_PROLOGUE

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    // ...init form...
    FrmDrawForm(frm);
    // this must be AFTER draw form
    print_splash();
    handled = true;
    break;

  case penDownEvent:
    LeaveForm(); // LeaveForm(false);
    if (FrmGetActiveFormID() == MainForm)
      print_stats(); // sometimes a message turns up in the wrong form
    handled = true;
    break;

  default:
    break;
  }

    // CALLBACK_EPILOGUE

  return handled;
}
*/


Boolean Sad_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  switch (e->eType) {
  case frmOpenEvent:
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    handled = true;
    break;
  default:
    break;
  }
  return handled;
}

//Boolean Direction_Form_HandleEvent(EventPtr e)
// Was here but that is not going to be used anymore.
// Instead, call draw_cast_slices in cast.c


/*****************************************************************************
 *                                                                           *
 *   The following stufff actually belongs in this file.                     *
 *                                                                           *
 *****************************************************************************/
/*****************************************************************************
 *                                                                           *
 *                      OpenDatabase                                         *
 *                                                                           *
 *****************************************************************************/
static Boolean OpenDatabase(void)
{
  // The '0' arguments are all 'card number'.
  // Note I have two databases of the same type now, so
  //  DmOpenDatabaseByTypeCreator is not going to work.
  LocalID dbID;

  // We cannot run if we cannot find the iLarnDB, so return error
  if (0 == (dbID = DmFindDatabase(0, iLarnDBName))) return 1;
  if (0 == (iLarnDB = DmOpenDatabase(0, dbID, dmModeReadWrite))) return 1;

  // We can run if we cannot find the iLarnSaveDB.  Just create it.
  if (0 == (dbID = DmFindDatabase(0, iLarnSaveDBName))) {
    if (DmCreateDatabase(0, iLarnSaveDBName, iLarnAppID, iLarnSaveDBType,
			 false))
      return 1;
    if (0 == (dbID = DmFindDatabase(0, iLarnSaveDBName))) return 1;
  }
  if (0 == (iLarnSaveDB = DmOpenDatabase(0, dbID, dmModeReadWrite))) return 1;
  return 0;
}


/*****************************************************************************
 *                      ApplicationHandleEvent                               *
 *****************************************************************************/
static Boolean ApplicationHandleEvent(EventPtr e)
{
    FormPtr frm;
    Word    formId;
    Boolean handled = false;

    if (e->eType == frmLoadEvent) {
	formId = e->data.frmLoad.formID;
	frm = FrmInitForm(formId);
	FrmSetActiveForm(frm);

	switch(formId) {
	case MainForm:
	    FrmSetEventHandler(frm, Main_Form_HandleEvent);
	    break;
	case AboutForm:
	    FrmSetEventHandler(frm, About_Form_HandleEvent);
	    break;
	case PickChForm:
	    FrmSetEventHandler(frm, PickCh_Form_HandleEvent);
	    break;
	case QuestionForm:
	    FrmSetEventHandler(frm, Question_Form_HandleEvent);
	    break;
	case MapForm:
	    FrmSetEventHandler(frm, Map_Form_HandleEvent);
	    break;
	    //	case SplashForm:
	    //	    FrmSetEventHandler(frm, Splash_Form_HandleEvent);
	    //	    break;
	case InvSelectForm:
	    FrmSetEventHandler(frm, InvSelect_Form_HandleEvent);
	    break;
	case CastForm:
	    FrmSetEventHandler(frm, Cast_Form_HandleEvent);
	    break;
	case StoreForm:
	    FrmSetEventHandler(frm, Store_Form_HandleEvent);
	    break;
	case TransForm:
	    FrmSetEventHandler(frm, Trans_Form_HandleEvent);
	    break;
	case LrsForm:
	    FrmSetEventHandler(frm, Lrs_Form_HandleEvent);
	    break;
	case EvaderForm:
	    FrmSetEventHandler(frm, Evader_Form_HandleEvent);
	    break;
	case PrefsForm:
	    FrmSetEventHandler(frm, Prefs_Form_HandleEvent);
	    break;
	case HwButtonsForm:
	    FrmSetEventHandler(frm, HwButtons_Form_HandleEvent);
	    break;
	case PadForm:
	    FrmSetEventHandler(frm, Pad_Form_HandleEvent);
	    break;
	case ScoreForm:
	    FrmSetEventHandler(frm, Score_Form_HandleEvent);
	    break;
	case SadForm:
	    FrmSetEventHandler(frm, Sad_Form_HandleEvent);
	    break;
	case MsgLogForm:
	    FrmSetEventHandler(frm, MsgLog_Form_HandleEvent);
	    break;
	}
	handled = true;
    }

    return handled;
}

/*****************************************************************************
 *                      Preferences                                          *
 *****************************************************************************/

static void readLarnPrefs()
{
  Word prefsSize;
  SWord prefsVersion;
  //  Short i;
  //  LarnPreferenceType prefs;
  prefsSize = sizeof(LarnPreferenceType);
  prefsVersion = PrefGetAppPreferences(iLarnAppID, iLarnAppPrefID, &my_prefs,
				       &prefsSize, true);
  if (prefsVersion != noPreferenceFound) {
    if (prefsVersion <= PrefVersion1) {
      topten_fwdcompatibl();
    }
    if (prefsVersion <= PrefVersion2) {
      my_prefs.color = false;
      my_prefs.black_bg = false;
    }
    if (prefsVersion <= PrefVersion3) {
      ULong foo;
      foo = OLD_REC_SCORE;
      topten_move_db(foo);
      foo = OLD_REC_WINSCORE;
      topten_move_db(foo);
    }
    if (prefsVersion < iLarnAppPrefVersion)
      writeLarnPrefs(); // update the version number to show that we are ok now
  }
}
// not static - it is used by stuff in three forms
void writeLarnPrefs()
{
  Word prefsSize;
  //  LarnPreferenceType prefs;
  prefsSize = sizeof(LarnPreferenceType);

  PrefSetAppPreferences(iLarnAppID, iLarnAppPrefID, iLarnAppPrefVersion,
			&my_prefs, prefsSize, true);
}


/*****************************************************************************
 *                      StartApplication                                     *
 *****************************************************************************/
/* moved to the head of the class */
#ifdef I_AM_OS_2
FontPtr oldFontSix = 0;
#endif
// SDK 2:
// #define UIFontTable ((UIGlobalsPtr)GUIGlobalsP)->uiFontTable
// SDK 3.1:
// // UIFontTable no longer exists; use UISysFontPtr and UIAppFontPtr
// #define UISysFontTablePtr ((UIGlobalsPtr)GUIGlobalsP)->uiSysFontTablePtr
Boolean evil = false;
static Word StartApplication(void)
{
  Word error;
  Boolean found_my_character;
  DWord version;
#ifdef I_AM_OS_2
  void *font128 = 0;
#else
  VoidHand fontHandle;
  FontType *fontPtr;
#endif

  error = OpenDatabase();
  if (error) {
    FrmGotoForm(SadForm);
    evil = true;
  } else {
    
    /* load game and/or do all the ONE-TIME initialization */
    found_my_character = init_game(); // allocates env et al

    readLarnPrefs();

    
#ifdef I_AM_OS_2
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
    // evil font kludges!
    font128 = MemHandleLock(DmGetResource('NFNT', ItsyFont));
    if (version < 0x03000000L) {
      oldFontSix = ((UIGlobalsPtr)GUIGlobalsP)->uiFontTable[6];
      ((UIGlobalsPtr)GUIGlobalsP)->uiFontTable[6] = font128;
    }
#else
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
    if (version >= 0x03000000L) {
      fontHandle = DmGetResource('NFNT',ItsyFont);
      fontPtr = MemHandleLock(fontHandle);
      // "user defined fonts start from 129"
      FntDefineFont(129,fontPtr);
      // the custom font is uninstalled automatically when we leave iLarn;
      // however, fontHandle must remain locked until then.
    }  
  // set HasColor boolean to false
#ifdef I_AM_COLOR
  if ((version >= 0x03503000L) && my_prefs.color)
    // call a routine to find and grok 'iLarn-RGB' memo, if it exists.
    look_for_memo(); // may set "have_read_color" to true
#endif // COLOR
#endif // OS 2
    
    if (found_my_character)
      FrmGotoForm(MainForm);
    else
      FrmGotoForm(PickChForm);
    
  } // end "if error"

  return 0;
}


/*****************************************************************************
 *                      StopApplication                                      *
 *****************************************************************************/
#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h); a = NULL;
static void StopApplication(void)
{
  VoidHand h;
  DWord version;
#ifdef I_AM_OS_2
  //void *font128 = 0;
#else
  VoidHand fontHandle;
#endif
  if (evil) return;

  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
#ifdef I_AM_OS_2
  if (version < 0x03000000L) {
    if (oldFontSix) {
      ((UIGlobalsPtr)GUIGlobalsP)->uiFontTable[6] = oldFontSix;
      oldFontSix = 0;
      //      if ((font128 = ((UIGlobalsPtr)GUIGlobalsP)->uiFontTable[6]))
      //	MemHandleUnlock(font128);
    }
  }
#else
  if (version >= 0x03000000L) {
    fontHandle = DmGetResource('NFNT',ItsyFont);
    MemHandleUnlock(fontHandle);
  }  
#endif

  // try to save!  woohoo.
  if (env->cdude[HP] > 0)
    save_all(env);

  free_env(env);
  free_me(env);
  FrmSaveAllForms();
  FrmCloseAllForms();

  if (iLarnDB!=NULL) {
    DmCloseDatabase(iLarnDB);
    iLarnDB = NULL;
  }
  if (iLarnSaveDB!=NULL) {
    // debug rom says some records got left locked..
    DmCloseDatabase(iLarnSaveDB);
    iLarnSaveDB = NULL;
  }
  // free stuff

}


/*****************************************************************************
 *                      EventLoop                                            *
 *****************************************************************************/

/* The main event loop */


static void EventLoop(void)
{
    Word err;
    EventType e;
     
    do {
	EvtGetEvent(&e, evtWaitForever);
	// first see if it's a hardware button thing!!!
	// don't ask me what the poweredOnKeyMask is, though; cargo cult.
	// Do special hardware button things only if:
	// it's a hardware button event, [you're alive,] and in the main form.
	if ( (e.eType != keyDownEvent)
	     || !my_prefs.use_hardware 
	     || (e.data.keyDown.modifiers & poweredOnKeyMask)
	     // need to remove next line if I'm going to make popups ignore hw
	     //	     || (FrmGetActiveFormID() != MainForm)
	     || !buttonsHandleEvent(&e) )
	  // now proceed with usual handling
	  if (! SysHandleEvent (&e))
	    if (! MenuHandleEvent (NULL, &e, &err))
	      if (! ApplicationHandleEvent (&e))
		FrmDispatchEvent (&e);
    } while (e.eType != appStopEvent);
}


/* Main entry point; it is unlikely you will need to change this except to
   handle other launch command codes */
/*****************************************************************************
 *                      PilotMain                                            *
 *****************************************************************************/
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
    Word err;

    if (cmd == sysAppLaunchCmdNormalLaunch) {

      err = StartApplication();
      if (err) return err;

      EventLoop();
      StopApplication();

    } else {
      return sysErrParamErr;
    }

    return 0;
}
