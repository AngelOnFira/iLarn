#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"
#include "color.h"

/***********************************************************************
 * This form lets you view and modify your preferences.
 * There are a bunch of checkboxes and things in it,
 * and an ok and a cancel button, as you might expect.
 * (There are also buttons to draw the donut and redraw this form.)
 ***********************************************************************/
extern LarnPreferenceType my_prefs;
static void usr_form_init(FormPtr frm);
static void usr_form_update(FormPtr frm);
void i_used_too_much_heap(); // in main.c

#define DifficultyY 14

Boolean Prefs_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  ListPtr lst;
  ControlPtr checkbox;
  static Short rw = 0;
  static Short ws = 0;
  Boolean redraw = false;
    
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    rw = max(1, my_prefs.run_walk_border - my_prefs.walk_center_border);
    ws = max(1, my_prefs.walk_center_border);
    /* and checkboxes */
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_1));
    CtlSetValue(checkbox, (my_prefs.run_on ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_2));
    CtlSetValue(checkbox, (my_prefs.stay_centered ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_3));
    CtlSetValue(checkbox, (my_prefs.font_large ? 1 : 0));
    //    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_4));
    //    CtlSetValue(checkbox, (my_prefs.gender_male ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_4));
    CtlSetValue(checkbox, (my_prefs.rogue_relative ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_5));
    CtlSetValue(checkbox, (my_prefs.sound_on ? 1 : 0));
    //    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_6));
    //    CtlSetValue(checkbox, (my_prefs.L18 ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_7));
    CtlSetValue(checkbox, (my_prefs.coward_on ? 1 : 0));
    checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_8));
    CtlSetValue(checkbox, (my_prefs.black_bg ? 1 : 0));

    /* set player name */
    //get_default_username(); // (set to palmos user name)
    usr_form_init(frm);
    FrmDrawForm(frm);

#ifdef I_AM_COLOR
    {
      DWord version;
      FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
      if (version >= 0x03503000L) {
	checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_9));
	CtlSetValue(checkbox, (my_prefs.color) ? 1 : 0);
	CtlShowControl(checkbox);
      }
    }
#endif // COLOR

    /* set initial settings for lists */
    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_bul_1));
    LstSetSelection(lst, ws-1);
    lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, list_bul_2));
    LstSetSelection(lst, rw-1);

    // print current game difficulty
    {
      Char buf[10];
      StrPrintF(buf, "%d", (Short) env->cdude[HARDGAME]);
      //      WinDrawChars(buf, StrLen(buf), 156-11, 78);
      WinDrawChars(buf, StrLen(buf), 156-11, DifficultyY);
    }

    handled = true;
    break;

  case lstSelectEvent:
    switch (e->data.lstSelect.listID) {
    case list_bul_1:
      ws = max(1, 1 + e->data.lstSelect.selection);      /* "hole" */
      break;
    case list_bul_2:
      rw = max(1, 1 + e->data.lstSelect.selection);      /* "donut" */
      break;
    }
    handled = true;
    break;
  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
      /*     case btn_bul_save: */
      /*       handled = true; */
      /*       break; */
    case btn_bul_ok:
      my_prefs.run_walk_border = rw+ws;
      my_prefs.walk_center_border = ws;
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_1));
      my_prefs.run_on = (CtlGetValue(checkbox) != 0);	
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_2));
      my_prefs.stay_centered = (CtlGetValue(checkbox) != 0);	
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_3));
      my_prefs.font_large = (CtlGetValue(checkbox) != 0);	
      //      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_4));
      //      my_prefs.gender_male = (CtlGetValue(checkbox) != 0);	
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_4));
      my_prefs.rogue_relative = (CtlGetValue(checkbox) != 0);	
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_5));
      my_prefs.sound_on = (CtlGetValue(checkbox) != 0);	
      //  checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_6));
      //  my_prefs.L18 = (CtlGetValue(checkbox) != 0);	
      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_bul_7));
      my_prefs.coward_on = (CtlGetValue(checkbox) != 0);	

      checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,check_bul_8));
      if ((CtlGetValue(checkbox) != 0) != my_prefs.black_bg)
	redraw = true;
      my_prefs.black_bg = (CtlGetValue(checkbox) != 0);
#ifdef I_AM_COLOR
      {
	/* read color checkbox... */
	DWord version;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
	if (version >= 0x03503000L) {
	  checkbox = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,check_bul_9));
	  if ((CtlGetValue(checkbox) != 0) != my_prefs.color)
	    redraw = true;
	  my_prefs.color = (CtlGetValue(checkbox) != 0);
	  if (!have_read_color && my_prefs.color)
	    // we'll need to read the memo.
	    look_for_memo(); // will set have_read_color true if found
	}
      }
#endif // COLOR


      /* read player name... */
      usr_form_update(frm);
      /* write bools and shorts to a PREFERENCES data thingy. */
      writeLarnPrefs();
      LeaveForm(); // LeaveForm(true);
      if (redraw) { // This might be overkill actually
	//move_visible_window(env->playerx, env->playery, true);
	i_used_too_much_heap();
      }
      /* It is important to do this after LeaveForm for msg to display */
      /* <anything with message> */
      handled = true;
      break;
    case btn_bul_cancel:
      LeaveForm(); // LeaveForm(true);
      handled = true;
      break;
      /*    case btn_bul_hw:
      FrmPopupForm(HwButtonsForm);
      handled = true;
      break; */
    case btn_bul_draw:
      draw_circle(80, 80, (rw+ws)*10, true);
      draw_circle(80, 80, ws*10, false);
      handled = true;
      break;
    case btn_bul_clear:
      draw_circle(80, 80, 160, false); // Is this necessary?  I forget.
      FrmDrawForm(frm);
      // print current game difficulty
      {
	Char buf[10];
	StrPrintF(buf, "%d", (Short) env->cdude[HARDGAME]);
	WinDrawChars(buf, StrLen(buf), 156-11, DifficultyY);
      }
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}



static void usr_form_init(FormPtr frm)
{
  FieldPtr fld;
  CharPtr p;
  VoidHand vh;
  fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_usr));
  vh = (VoidHand) FldGetTextHandle(fld);
  if (!vh) {
    vh = MemHandleNew((NAMELEN+1) * sizeof(Char));
  }
  FldSetTextHandle(fld, (Handle) 0);
  p = MemHandleLock(vh);
  StrNCopy(p, my_prefs.name, NAMELEN);
  MemHandleUnlock(vh);
  FldSetTextHandle(fld, (Handle) vh);
}

static void usr_form_update(FormPtr frm)
{
  FieldPtr fld;
  CharPtr p;
  VoidHand vh;
  fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_usr));
  vh = (VoidHand) FldGetTextHandle(fld);
  if (vh) {
    p = MemHandleLock(vh);
    StrNCopy(my_prefs.name, p, NAMELEN);
    MemHandleUnlock(vh);
  }
}

