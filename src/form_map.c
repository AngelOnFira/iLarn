#include "palm.h"
#include "iLarnRsc.h"
#include "Globals.h"
#include "color.h"

#ifdef I_AM_COLOR
extern LarnPreferenceType my_prefs;
IndexedColorType peek_color(Short x, Short y); // in display.c!!!
#endif
/***********************************************************************
 * This form is the "display entire dungeon level, really tiny" form.
 * You tap anywhere on the map form to return to the main form.
 ***********************************************************************/

static void show_dungeon() SEC_4;
Boolean Map_Form_HandleEvent(EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    /* this must be AFTER draw form */
    show_dungeon();
    handled = true;
    break;

  case penDownEvent:
    LeaveForm(); // LeaveForm(true);
    //    i_used_too_much_heap();
    handled = true;
    break;

  default:
    break;
  }

  return handled;
}



/**********************************************************************
                       SHOW_DUNGEON
 IN:
 sotu = various globals (dungeon, rooms, wizard, cur_level, rogue)
 OUT:
 nothing
 PURPOSE:
 The "real" dungeon is 24 wide x 80 high, and doesn't all fit on the
            ... now:   17,       67. 
 (156^2 pixel) screen at once when it's "full size".  So I have a
 form to display a tiny version of the map (just rooms/mazes/tunnels,
 the rogue, and the stairs), where each square is 2 x 3 pixels.
 Maybe I could put flyspecks for monsters/objects too.  Maybe.
 **********************************************************************/
// Cool, this actually works (code reuse is good)
// I just need to fix it to be centered since it's a different size now.
// (Sadly can't make it any larger unless I try for fractional pixels :)
//static CustomPatternType yourPattern = { 0xaaaa, 0x5555, 0xaaaa, 0x5555 };
// The passage pattern got messed up so I moved it inside.
static void show_dungeon()
{
  RectangleType r;
  Word col, row;
  Short r_width, max_r_width=3, r_left = 0, x;
  //  CustomPatternType origPattern;
  //  CustomPatternType yourPattern = { 0xaaaa, 0x5555, 0xaaaa, 0x5555 };
#ifdef I_AM_COLOR
  RGBColorType rgbcolor;
  IndexedColorType icolor;
#endif

  RctSetRectangle(&r, 0,0, 160, 160);
  WinDrawRectangle(&r, 6);

  /* This is just some fluff to put in the extra space above and below. 
     I could probably think of some more-useful fluff to put there. */
  WinDrawInvertedChars("You Can't Get There From Here", 29, 15, 1);
  WinDrawInvertedChars("(Tap  Anywhere  When  Done)", 27, 22, 147);
  /* This is just some decoration. */
  WinEraseLine(8, 34, 148, 34);
  WinEraseLine(6, 36, 150, 36);
  WinEraseLine(4, 38, 152, 38);
  /* Each tile gets 2 pixels/col and 3 pixels/row. */
  // RctSetRectangle(&r, 0,42, 160,72); /* left,top, width,height */
  RctSetRectangle(&r, 0,42, 160,51); /* left,top, width,height */
  //    NOTE - There's an extra 3 pixels at the end, I think.
  //  WinEraseRectangle(&r, 0);
  // WinDrawRectangleFrame(simpleFrame, &r); /* simple,round,popup,boldRound */

#ifdef I_AM_COLOR
  if (IsColor) {
    WinPushDrawState();
    rgbcolor.r = rgbcolor.g = rgbcolor.b = (my_prefs.black_bg?102:153); // gray
    icolor = WinRGBToIndex(&rgbcolor);
    WinSetBackColor(icolor);
    WinEraseRectangle(&r, 0);
    WinSetBackColor(white);
  } else {
    for (col = 1 ; col < 160 ; col+=2)
      WinEraseLine(col, 42, col, 92);
  }
#else
  for (col = 1 ; col < 160 ; col+=2)
    WinEraseLine(col, 42, col, 92);
  // there's an extra 3 pixels (white, dark, white) at the end.  sigh
#endif

  /* Draw each cell in the dungeon.  Could perhaps be more clever */
  for (col = 0; col < MAXX; col++) {
    r_width = (col % 3) ? max_r_width-1 : max_r_width;//was always 2 for iRogue
    for (row = 0; row < MAXY; row++) {
      Short r_top = row*3 + 42, r_height = 3;
      Short xtmp = (r_left>0 && r_width<max_r_width) ? r_left-1 : r_left;
      //      RctSetRectangle(&r, r_left, row*3 + 42, r_width, 3);
      RctSetRectangle(&r, r_left, r_top, r_width, r_height);
      //      if (wizard_p || env->know[col][row]) {
      if (env->know[col][row]) {
	switch (env->item[col][row]) {
	case OWALL:
	  if (col == env->playerx && row == env->playery)
	    WinEraseRectangle(&r, 0); // in case you're wall walking
	  else {
#ifdef I_AM_COLOR
	    if (IsColor)
	      WinSetForeColor(get_color('#', false));
#endif
	    WinDrawRectangle(&r, 0);
	  }
	  break;
	case OSTAIRSUP:
	case OVOLUP:
	case OSTAIRSDOWN:
	  WinEraseRectangle(&r, 0);
	  x = r_left - 2;
	  x = max(0,min(x,160-2)); // make sure what's drawn'llbe within bounds
	  if (env->item[col][row] == OSTAIRSDOWN)
	    WinDrawInvertedChars("d", 1, x, 101);
	  else 
	    WinDrawInvertedChars("u", 1, x, 93);
#ifdef I_AM_COLOR
	  if (IsColor) {
	    WinSetForeColor(get_color('%', false));
	    WinDrawRectangle(&r, 0);
	  } else {
	    // not sure why the -1 is necessary, but, it is.
	    WinDrawLine(xtmp+max_r_width, (r_top-1), xtmp, (r_top-1)+r_height);
	  }
#else
	  //	  WinSetPattern(yourPattern);
	  //	  WinFillRectangle(&r, 0);
	  WinDrawLine(xtmp+max_r_width, (r_top-1), xtmp, (r_top-1)+r_height);
#endif
	  break;
	default:
#ifdef I_AM_COLOR
	  // Could...
	  //   Fetch the thingy drawn in this square
	  //   Look up its color and if it's neither black nor white,
	  //     then set fore-color and draw a solid blob.  Yay!!
	  if (IsColor) {
	    WinSetForeColor(my_prefs.black_bg?black:white);
	    icolor = peek_color(row, col);
	    if (icolor != white && icolor != black)
	      WinSetForeColor(icolor);
	    WinDrawRectangle(&r, 0);
	  } else
	    WinEraseRectangle(&r, 0);
#else
	  WinEraseRectangle(&r, 0);
#endif
	  break;
	}
	if (col == env->playerx && row == env->playery) {
	  WinEraseRectangle(&r, 0);
#ifdef I_AM_COLOR
	  if (IsColor) {
	    WinSetForeColor(get_color('@', false));
	    WinDrawRectangle(&r, 0);
	  } else {
	    WinDrawLine(xtmp, r_top, xtmp+max_r_width, r_top+r_height);
	  }
#else
	  //	  WinSetPattern(yourPattern);
	  //	  WinFillRectangle(&r, 0);
	  WinDrawLine(xtmp, r_top, xtmp+max_r_width, r_top+r_height);
#endif
	  // This is what draws the arrow:
	  RctSetRectangle(&r, r_left, 114, 3, 20);
	  WinEraseRectangle(&r,0);
	  RctSetRectangle(&r, r_left-1, 117, 5, 3);
	  WinEraseRectangle(&r,0);
	  RctSetRectangle(&r, r_left+1, 111, 1, 3);
	  WinEraseRectangle(&r,0);
	}
      } else {
	//	WinSetPattern(origPattern);
	//	WinFillRectangle(&r, 0);
	// Leave it like it is.
      }
    } // end for row
    r_left += r_width;
  }
  //  WinSetPattern(origPattern);
  /* don't forget to put the player somewhere */

#ifdef I_AM_COLOR
  if (IsColor) {
    WinPopDrawState();
  }
#endif // I_AM_COLOR

  return;
}
