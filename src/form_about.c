#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form is the 'About' screen that you get to from the Options menu.
 * It displays some static text and bitmaps.
 *  press the ok button to exit
 *  press the more button to view a string with copyleft information
 **********************************************************************/
Boolean About_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    handled = true;
    break;

  case ctlSelectEvent:
    switch(e->data.ctlSelect.controlID) {
    case btn_about_ok:
      LeaveForm(); // LeaveForm(true);
      handled = true;
      break;
    case btn_about_more:
      FrmHelp(AboutStr);
      handled = true;
      break;
    case btn_about_credits:
      FrmHelp(CreditStr);
      handled = true;
      break;
    }
    break;

  default:
    break;
  }
  return handled;
}
