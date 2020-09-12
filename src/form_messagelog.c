#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This form allows player to view the last N message that went by..
 * useful if he/she acted too fast and missed one.
 * Note that the old_messages array is probably going to be combined with
 * the splash array and a position-in-array int to enable "--more--"
 ***********************************************************************/

extern Char *old_messages[SAVED_MSGS];
VoidHand msglog_TextHandle = NULL;
static void init_messagelog_view(FormPtr frm) SEC_3;
Boolean MsgLog_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  FieldPtr fld;
    
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    init_messagelog_view(frm);
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_ml));
    update_field_scrollers(frm, fld, repeat_ml_up, repeat_ml_down);
    FrmDrawForm(frm);
    handled = true;
    break;

  case ctlSelectEvent:
    switch(e->data.ctlSelect.controlID) {
    case btn_ml_ok:
      LeaveForm(); // LeaveForm(true);
      handled = true;
      break;
    }
    break;

  case ctlRepeatEvent:
    /*     "Repeating controls don't repeat if handled is set true." */
    frm = FrmGetActiveForm();
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_ml));
    switch(e->data.ctlRepeat.controlID) {
    case repeat_ml_up:
      FldScrollField(fld, 1, winUp);
      update_field_scrollers(frm, fld, repeat_ml_up, repeat_ml_down);
      break;
    case repeat_ml_down:
      FldScrollField(fld, 1, winDown);
      update_field_scrollers(frm, fld, repeat_ml_up, repeat_ml_down);
      break;
    }
    break;

  case keyDownEvent:
    /* hardware button -- or else graffiti. */
    frm = FrmGetActiveForm();
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_ml));
    switch(e->data.keyDown.chr) {
    case pageUpChr:
      page_scroll_field(frm, fld, winUp);
      update_field_scrollers(frm, fld, repeat_ml_up, repeat_ml_down);
      handled = true;
      break;
    case pageDownChr:
      page_scroll_field(frm, fld, winDown);
      update_field_scrollers(frm, fld, repeat_ml_up, repeat_ml_down);
      handled = true;
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}



/**********************************************************************
                       INIT_MESSAGELOG_VIEW
 IN: frm = a form that has a text-view widget
 PURPOSE:
 Initializes the displayed form 'frm' with a list of the last
 SAVED_MSGS messages.  Called when frm is being initialized.
 **********************************************************************/
static void init_messagelog_view(FormPtr frm)
{
  FieldPtr fld;
  CharPtr txtP;
  Short i;
  // Get the text field
  fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_ml));
  // Create a mem. handle and lock it
  msglog_TextHandle = MemHandleNew(BIG_NUMBER);
  txtP = MemHandleLock(msglog_TextHandle);
  // Use MemMove and/or MemSet to copy text to the mem. handle
  for (i = 0 ; i < SAVED_MSGS ; i++) {
    MemMove(txtP, old_messages[i], StrLen(old_messages[i])+1);
    txtP += StrLen(old_messages[i]);
  }
  // Unlock the handle.  Set the field to display the handle's text.
  MemHandleUnlock(msglog_TextHandle);
  FldSetTextHandle(fld, (Handle) msglog_TextHandle);
}
