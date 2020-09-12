#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"

/***********************************************************************
 * This is the list of top scores.  It is the form you go to when you die.
 * You can view the top 25 scores, or view the top n "winners".
 * There is also a button to start a new game that will put you
 * back in character selection form.
 ***********************************************************************/
extern Boolean i_am_dead; 

Boolean Score_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    //    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, field_score));
    /* ...init form... */
    FrmDrawForm(frm);
    init_death(frm); // xxx
    handled = true;
    break;

  case ctlRepeatEvent:
    /*     "Repeating controls don't repeat if handled is set true." */
    frm = FrmGetActiveForm();
    switch(e->data.ctlRepeat.controlID) {
    case repeat_score_up:
      scroll_scores(true, frm);
      break;
    case repeat_score_down:
      scroll_scores(false, frm);
      break;
    }
    break;

  case keyDownEvent:
    // hardware button -- or else graffiti.
    frm = FrmGetActiveForm();
    switch(e->data.keyDown.chr) {
    case pageUpChr:
      scroll_scores(true, frm);
      handled = true;
      break;
    case pageDownChr:
      scroll_scores(false, frm);
      handled = true;
      break;
    }
    break;

  case ctlSelectEvent:
    frm = FrmGetActiveForm();
    switch(e->data.ctlSelect.controlID) {
    case pbtn_score_all:
      which_scores(false, frm);
      handled = true;
      break;
    case pbtn_score_win:
      which_scores(true, frm);
      handled = true;
      break;
    case btn_score_done:
      if (i_am_dead)
	FrmGotoForm(PickChForm);
      else
	LeaveForm();
      handled = true;
      break;
    }
    break;

  default:
    break;
  }
  return handled;
}
