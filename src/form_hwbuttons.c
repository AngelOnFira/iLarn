#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form allows the player to bind/unbind actions to hardware buttons
 * It is full of popup lists and stuff.
 * There is an ok button and a cancel button.
 **********************************************************************/
extern LarnPreferenceType my_prefs;
Boolean HwButtons_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  ListPtr lst;    // popup list
  ControlPtr ctl; // popup trigger
  CharPtr label;
  Short i;
    
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    // my stuff
    ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_hwb));
    CtlSetValue(ctl, (my_prefs.use_hardware ? 1 : 0));
    ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_hwb_keyb));
    CtlSetValue(ctl, (my_prefs.use_keyboard ? 1 : 0));
    for (i = 0 ; i < 8 ; i++) {
      lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_hwb_1 + i));
      ctl = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, popup_hwb_1 + i));
      LstSetSelection(lst, my_prefs.hardware[i]);
      label = LstGetSelectionText(lst, my_prefs.hardware[i]);
      CtlSetLabel(ctl, label);
    }
    // end my stuff
    FrmDrawForm(frm);
    handled = true;
    break;

    // XXX - I need it to redraw stuff.
  case ctlSelectEvent:
    switch(e->data.ctlSelect.controlID) {
    case btn_hwb_ok:
      frm = FrmGetActiveForm();
      ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_hwb));
      my_prefs.use_hardware = (CtlGetValue(ctl) != 0);	
      ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, check_hwb_keyb));
      my_prefs.use_keyboard = (CtlGetValue(ctl) != 0);	
      for (i = 0 ; i < 8 ; i++) {
	lst = FrmGetObjectPtr (frm, FrmGetObjectIndex(frm, list_hwb_1 + i));
	my_prefs.hardware[i] = LstGetSelection(lst);
      }
      writeLarnPrefs();
      LeaveForm(); // LeaveForm(true);
      handled = true;
      break;
    case btn_hwb_cancel:
      LeaveForm(); // LeaveForm(true);
      handled = true;
      break;
    }
    break;
  default:
    break;
  }

  return handled;
}
