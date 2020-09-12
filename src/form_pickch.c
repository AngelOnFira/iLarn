#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form is the form you start in when you start a new game.
 * It allows you to select your character class and gender.
 * After that you go to the main form.
 ***********************************************************************/

extern LarnPreferenceType my_prefs;
extern Boolean newlevel_p;
extern Boolean i_am_dead;
extern Boolean first_time_through;
Short difficulty_items = 0;
Char ** difficulty;

static void init_pickch_form(FormPtr frm) SEC_4;
static void clean_stuff_up(FormPtr frm) SEC_4;
static void enable_difficulty(FormPtr frm) SEC_4;
static void reset_difficulty(FormPtr frm) SEC_4;
static void free_difficulty(FormPtr frm) SEC_4;
static void snapshot_or_no() SEC_4; // initially & when dif selected

Boolean sketch_frobify = false;

Boolean PickCh_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  ControlPtr pushbtn;
  Short i, selected_class;
  Boolean gender;
  ListPtr lst;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    init_pickch_form(frm);
    sketch_frobify = sketch_exists();
    snapshot_or_no();
    handled = true;
    break;

  case popSelectEvent:
    // user selected a new difficulty level..
    if (sketch_frobify)
      snapshot_or_no();
    //    handled = true; // NO.
    break;

  case ctlSelectEvent:
    switch(e->data.ctlSelect.controlID) {
    case btn_pickch_dif:
      reset_difficulty(FrmGetActiveForm());
      if (sketch_frobify)
	snapshot_or_no();
      handled = true;
      break; 
    case btn_pickch_snap:
      if (load_sketch()) {
	clean_stuff_up(FrmGetActiveForm());
	// LOAD IN THE SNAPSHOT
	recalc(); // calculate armor class, etc, of player
	newcavelevel(0);
	env->playerx = rnd(MAXX_0);
	env->playery = rnd(MAXY_0);
	FrmGotoForm(MainForm);
      } else {
	// else need a FrmAlert with "Error, unable to load clone".
	// also, need to disable this button.
      }
      handled = true;
      break; 
    case btn_pickch_gen:
    case btn_pickch_ok:
      frm = FrmGetActiveForm();
      for (i = pbtn_pickch_0, selected_class = -1 ; i <= pbtn_pickch_7 ; i++) {
	pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, i));
	if (CtlGetValue(pushbtn))
	  selected_class = i - pbtn_pickch_0;
      }
      // male or female
      pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_pickch_m));
      gender = (CtlGetValue(pushbtn) != 0);
      /* if (selected_class == -1) selected_class = 8; */
      if (selected_class != -1) {
	get_default_username(); // write prefs..
	writeLarnPrefs();
	makeplayer(selected_class);
	env->courses_taken = env->warez_taken = 0; // reset college + pad
	if (my_prefs.gender_male != gender) {
	  my_prefs.gender_male = gender;
	  writeLarnPrefs();
	}
	if (difficulty_items > 0) {
	  lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_pickch));
	  i = LstGetSelection(lst);
	  if (i != -1) env->cdude[HARDGAME] = i;
	}
	clean_stuff_up(frm);
	FrmGotoForm(MainForm);
      }
      // else say something snide like "hello, pick one"
      handled = true;
      break;
    case pbtn_pickch_0: case pbtn_pickch_1: case pbtn_pickch_2:
    case pbtn_pickch_3: case pbtn_pickch_4: case pbtn_pickch_5:
    case pbtn_pickch_6: case pbtn_pickch_7:
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}

static void init_pickch_form(FormPtr frm)
{
  ControlPtr pushbtn;

  // Select a random class
  pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_pickch_0
						   /* + rund(8) */ ));
  CtlSetValue(pushbtn, 1);
  // Select most recently used gender
  if (my_prefs.gender_male)
    pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_pickch_m));
  else
    pushbtn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, pbtn_pickch_f));
  CtlSetValue(pushbtn, 1);
  // Also, allow former-winners to set the game difficulty.
  if (env->cdude[HARDGAME] > 0)
    enable_difficulty(frm);
  else
    difficulty_items = 0;
}

// enable clone revival, or not.
static void snapshot_or_no()
{
  FormPtr frm = FrmGetActiveForm();
  ControlPtr btn;
  Short dif = env->cdude[HARDGAME];
  if (dif > 0) {
    // what's the current selected difficulty
    ListPtr lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_pickch));
    dif = LstGetSelection(lst);
    if (dif == -1) dif = env->cdude[HARDGAME];
  }
  if (sketch_frobify && dif == 0) { // Hide must come before Show - overlapping
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_ok));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_gen));
    CtlShowControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_snap));
    CtlShowControl(btn);
  } else {
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_gen));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_snap));
    CtlHideControl(btn);
    btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_ok));
    CtlShowControl(btn);
  }
}

static void clean_stuff_up(FormPtr frm)
{
  i_am_dead = false;
  first_time_through = true;
  newlevel_p = true;
  dndstore_restock(); // store starts over
  clearspheres();
  if (difficulty_items > 0)
    free_difficulty(frm);
}

// Note - I allow people to set game difficulty now (if their default
// difficulty would be d > 0, they can set it to 0..d+1).
// However: default difficulty (and inherited bank account and tax duty)
// are drawn from the MOST RECENT winner data.  You could win level N,
// then set the difficulty to 0 and if you win at 0, your next default
// difficulty is *1*.  I don't really care enough to be smart about that.
// Probably a particular player either always likes to play at level 0,
// or likes to climb up in difficulty, either case it does not matter.

static void enable_difficulty(FormPtr frm)
{
  ListPtr lst;    // popup list
  ControlPtr ctl, btn; // popup trigger, button
  CharPtr label;
  Short i;
  // allocate list
  difficulty_items = 1 + (env->cdude[HARDGAME] + 1); // 0 to default+1
  // If someone wins this game more than 999 times, I will be deeply,
  // DEEPLY disturbed.  Better to be paranoid than run out of string tho'
  if (difficulty_items > 999) difficulty_items = 999;
  difficulty = (Char **) md_malloc(sizeof(Char *) * difficulty_items);
  for (i = 0 ; i < difficulty_items ; i++) {
    difficulty[i] = (Char *) md_malloc(sizeof(Char) * 4);
    StrPrintF(difficulty[i], "%d", i);
  }
  lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_pickch));
  LstSetListChoices(lst, difficulty, difficulty_items);
  LstSetHeight(lst, difficulty_items);
  LstSetSelection(lst, env->cdude[HARDGAME]); // list is 0-based.
  label = LstGetSelectionText(lst, env->cdude[HARDGAME]);
  ctl = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, popup_pickch));
  CtlSetLabel(ctl, label);
  FrmDrawForm(frm);
  // Enable the list.  Enable the button.  All that jive.
  btn = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btn_pickch_dif));
  CtlShowControl(btn);
  CtlShowControl(ctl);
}

static void reset_difficulty(FormPtr frm)
{
  ListPtr lst;    // popup list
  ControlPtr ctl; // popup trigger
  CharPtr label;
  lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_pickch));
  LstSetSelection(lst, env->cdude[HARDGAME]); // list is 0-based.
  label = LstGetSelectionText(lst, env->cdude[HARDGAME]);
  ctl = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, popup_pickch));
  CtlSetLabel(ctl, label);
}

#define free_me(a)  h = MemPtrRecoverHandle((a)); if (h) MemHandleFree(h);
static void free_difficulty(FormPtr frm)
{
  ListPtr lst;
  Short i;
  VoidHand h;
  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_pickch));
  LstSetListChoices(lst, NULL, 0);
  for (i = 0 ; i < difficulty_items ; i++) {
    free_me(difficulty[i]);
  }
  if (difficulty_items > 0) {
    free_me(difficulty);
  }
}
#undef free_me
