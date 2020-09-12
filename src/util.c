#include "palm.h"
#include "defines.h"
#include "types.h"

extern LarnPreferenceType my_prefs;

/**********************************************************************
                       DO_FEEP
 IN:
 frq = frequency
 dur = duration
 OUT:
 nothing
 PURPOSE:
 This will produce a beep of the requested frequency and duration,
 if the pilot owner has "game sound" turned on in the Preferences app.
 (Try to keep it down to chirps...)
 **********************************************************************/
/* SoundLevelType: slOn, slOff
   gameSoundLevel found in SystemPreferencesType
 */
void do_feep(Long frq, UInt dur)
{
  SystemPreferencesChoice allgamesound;

  if (!my_prefs.sound_on)
    return;

#ifdef I_AM_OS_2
  allgamesound = prefGameSoundLevel;
#else
  allgamesound = prefGameSoundLevelV20;
#endif

  if (PrefGetPreference(allgamesound) != slOff) {
    /* click: 200, 9
       confirmation: 500, 70  */
    SndCommandType sndCmd;
    sndCmd.cmd = sndCmdFreqDurationAmp; /* "play a sound" */
    sndCmd.param1 = frq; /* frequency in Hz */
    sndCmd.param2 = dur; /* duration in milliseconds */
    sndCmd.param3 = sndDefaultAmp; /* amplitude (0 to sndMaxAmp) */
    SndDoCmd(0, &sndCmd, true);
  }
}


/**********************************************************************
                       DRAW_CIRCLE
 IN:
 x,y = center (pixels)
 radius = radius (pixels)
 b = whether to draw it in black (or erase it in "white")
 various globals
 OUT:
 nothing
 PURPOSE:
 Draw a circle on the screen...
 **********************************************************************/
void draw_circle(Short x, Short y, Short radius, Boolean b)
{
  RectangleType r;
  RctSetRectangle(&r, x-radius, y-radius, 2*radius, 2*radius);
  if (b)
    WinDrawRectangle(&r, radius);
  else 
    WinEraseRectangle(&r, radius);
}


/**********************************************************************
                       UPDATE_FIELD_SCROLLERS
 IN:
 frm, fld, up_scroller, down_scroller = various UI doodads
 OUT:
 nothing
 PURPOSE:
 Update the given scroller widgets (for the given field 
 (in the given form)), according to whether the field is scrollable
 in the "up" and "down" directions.
 **********************************************************************/
void update_field_scrollers(FormPtr frm, FieldPtr fld,
			    Word up_scroller, Word down_scroller) 
{
  Boolean u, d;
  u = FldScrollable(fld, winUp);
  d = FldScrollable(fld, winDown);
  FrmUpdateScrollers(frm, 
		     FrmGetObjectIndex(frm, up_scroller),
		     FrmGetObjectIndex(frm, down_scroller),
		     u, d);
  return;
}


/**********************************************************************
                       PAGE_SCROLL_FIELD
 IN:
 frm, fld = various UI doodads
 dir = whether to scroll 'fld' up or down
 OUT:
 PURPOSE:
 Call this to scroll the field 'fld' up/down by one "page".
 (The caller should call update_field_scrollers immediately afterward.)
 **********************************************************************/
#ifndef I_AM_COLOR
void page_scroll_field(FormPtr frm, FieldPtr fld, DirectionType dir)
#else /* I_AM_COLOR */
void page_scroll_field(FormPtr frm, FieldPtr fld, WinDirectionType dir)
#endif /* I_AM_COLOR */
{
  Word linesToScroll;

  /* how many lines can we scroll? */
  if (FldScrollable(fld, dir)) {
    linesToScroll = FldGetVisibleLines(fld) - 1;
    FldScrollField(fld, linesToScroll, dir);
  }

  return;
}


/*
 *	routine to take a nap for n milliseconds  - not very exciting.
 */
void nap(Short x)
{
  Short delay = SysTicksPerSecond(); /* we want ticks * n / 1000 */
  //return; // for debugging
  if (x <= 0) return;
  /* also, "make sure the output buffer is flushed" */
  if (x < 1000) {
    delay *= x;
    delay /= 1000;
  } else {
    x /= 1000; /* this will truncate but who cares */
    delay *= x;
  }
  SysTaskDelay(delay);
}
