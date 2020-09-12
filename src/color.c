// Borrowed copiously from iRogue

#include "palm.h"
#include "sections.h"
#include "defines.h"
#include "types.h"
#include "color.h"
//#include "rogue.h" // for my EMDASH and FOODCHAR
Boolean have_read_color = false;

#ifdef I_AM_COLOR
extern LarnPreferenceType my_prefs;
DmOpenRef MemoDB;
// this seems weird to me, but what the heck.
typedef struct {
  Char note;           // null terminated
} MemoDBRecordType;
typedef MemoDBRecordType * MemoDBRecordPtr;
#define memoDBType 'DATA'
#define Is_Hex(x) ((('0' <= (x)) && ((x) <= '9')) || (('A' <= (x)) && ((x) <= 'F')) || (('a' <= (x)) && ((x) <= 'f')))

// So.  what are the symbols.  From explain_it():
/*
 * These are the monsters:
 * "lGHJKOScjtAELNQRZabhiCTYdegmvzFWflorXV pqsyUkMwDDPxnDDuD1234567890";
 *  i.e. 1 each of 0-9A-Za-z with these exceptions:
 *  0 of B, 5 of D, 0 of I (that's i as in India), 2 of l (that's L as in Lima)
 *
 * These are the objects:
 * "AToP%^F&^+M=%^$$f*OD#~][[)))(((||||||||{?!BC.o...<<<<EVV))([[]]](^ [H*** ^^ S tsTLcu...//))]:::::@
 *  i.e. !#$%&()*+./:<=?@ABCDEFHLMOPSTV[]^cfostu{|~
 *
 * Got some serious overlap on the letters, there.
 * Ok, suppose we had:
 * an array for regular ascii ! thru ~ (33 thru 126)    i = (Short) (c - 33);
 * plus "[A-Za-z]_" e.g. T_ is a Throne, say an extra 65 thru..um..117.
 *   i = (Short) (c - 65) + (126-33);
 * total space-wasting array is 126-33 + 117-65 = 145 * sizeof(UInt).  290 B.
 *
 * put_char_at will do:
 *    if (!bold) i = c - 33; else i = c + 28;    // i is index into my array.
 * Sounds ok to me.  put_char_at doesn't know what kind of dragon you're seeing
 * so, sorry, all dragons are the same dam' color For Now.
 */

// have not iLarnified, from here down.


// These are color symbols that get special cased in some way:
//#define NUM_SYMBOLS 145
#define NUM_SYMBOLS 150
#define BOLD_SYM '_'
#define SUBTRACT_BANG 33
#define SKIP_TO_BOLD 28
#define SKIP_TO_DRAGONS 145
IndexedColorType symbol_colors[NUM_SYMBOLS];
IndexedColorType black, white;
static Short grok_hex(Char *buf, Short len) SEC_4;
#define NUM_DRAGONS 5
const Char dragon_color_syms[NUM_DRAGONS] = { 'b', 'g', 's', 'p', 'r' };
/* Need to fix it so that I accept "d " (default for dragon), "dw", "db", "dg",
   "ds", "dp", "dr" */
Boolean dragon_colors[NUM_DRAGONS];
#endif // I_AM_COLOR


/* 
 * This is ONLY called if the OS version is at least 3.5.
 * If a memo entitled "iLarn-RGB" exists, read from it into symbol_colors
 * and set IsColor = true.  (Any symbol missing from the memo will be black.)
 * Otherwise IsColor is false (and symbol_colors remains black).
 */
#ifdef I_AM_COLOR
void look_for_memo()
{
  UInt recordNum;
  VoidHand recH;
  UInt mode = dmModeReadOnly;
  Char magic_title[20];
  Short title_len, memo_len, i, j, symbol;
  Char *memo, c;
  RGBColorType rgbcolor;
  IndexedColorType icolor;
  //  Boolean no_foreground = true;
  //  Boolean symbol_set[NUM_SYMBOLS];

  // initialize the symbol_colors array
  have_read_color = false;
  rgbcolor.r = rgbcolor.g = rgbcolor.b = 255; // white
  white = WinRGBToIndex(&rgbcolor);
  rgbcolor.r = rgbcolor.g = rgbcolor.b = 0; // black
  black = WinRGBToIndex(&rgbcolor);
  for (i = 0 ; i < NUM_SYMBOLS ; i++) {
    symbol_colors[i] = black;
  }
  for (i = 0 ; i < NUM_DRAGONS ; i++) {
    dragon_colors[i] = false;
  }
  //  symbol_colors[BG_COL_IX] = white;
  //  symbol_set[BG_COL_IX] = true;

  // FIND KITTEN^H^H^H^H^H^HMEMO
  // first, what is our magic title:
  StrPrintF(magic_title, "iLarn-RGB%c", linefeedChr);
  title_len = StrLen(magic_title);
  // try to open the memo database
  if (!(MemoDB = DmOpenDatabaseByTypeCreator(memoDBType, sysFileCMemo, mode))){
    have_read_color = false;
    return;
  }
  recordNum = 0;
  // get a record (stop if we run out)
  while (!have_read_color &&
	 (recH = DmQueryNextInCategory(MemoDB, &recordNum, dmAllCategories))) {
    MemoDBRecordPtr memoRecP = MemHandleLock (recH);
    memo = &(memoRecP->note);
    // Does the title match our magic title?
    if (0 == StrNCompare(memo, magic_title, title_len)) {
      have_read_color = true;
      memo_len = StrLen(memo);
      i = title_len; // skip past the title!
      while (i < memo_len - 1) {
	symbol = memo[i]; // this is the symbol whose color we'll define
	if (memo[i+1] == BOLD_SYM) {
	  i++;
	  if ((symbol >= 'a' && symbol <= 'z') ||
	      (symbol >= 'A' && symbol <= 'Z'))
	    symbol += SKIP_TO_BOLD; // distinguish "bold" letters
	  else
	    symbol -= SUBTRACT_BANG; // non-letters only represent one thing
	} else {
	  symbol -= SUBTRACT_BANG;
	  // Sneakily allow multiple colors for different kinds of dragons.
	  if (memo[i] == DRAGON_CHAR) {
	    Char dchar = memo[i+1];
	    Short dindex;
	    for (dindex = 0 ; (dindex < NUM_DRAGONS) &&
		   (symbol < SKIP_TO_DRAGONS) ; dindex++) {
	      if (dchar == dragon_color_syms[dindex]) {
		i++;
		symbol = SKIP_TO_DRAGONS + dindex;
	      }
	    }
	  } // end sneakiness
	} // end else-not-bold
	// ok, now 'symbol' is an index into symbol_colors[]
	if (symbol >= 0 && symbol < NUM_SYMBOLS) {
	  // ok now skip some (probably) whitespace to get to the rrggbb
	  Boolean found = false;
	  for ( i = i+1 ; i < memo_len - 1 ; i++) {
	    c = memo[i];
	    if (Is_Hex(memo[i])) { found = true; break; }
	    if (memo[i] == linefeedChr) { i++; break; }
	  }
	  if (!found) continue; // we've hit end of line or, maybe, EOF
	  if (memo_len < i + 6) break; // we'll hit EOF before end of rrggbb
	  // Read rrggbb color value.  First make sure they're all hexy.
	  for (j = 0 ; j < 6 ; j++)
	    if (!Is_Hex(memo[i+j]))
	      found = false;

	  // Ok if they're all hexy then understand it as a color and set it.
	  if (found) {
	    rgbcolor.r = grok_hex(&memo[i], 2);
	    rgbcolor.g = grok_hex(&memo[i+2], 2);
	    rgbcolor.b = grok_hex(&memo[i+4], 2);
	    icolor = WinRGBToIndex(&rgbcolor);
	    symbol_colors[symbol] = icolor;  // HOORAY!

	    if (symbol >= SKIP_TO_DRAGONS)
	      dragon_colors[symbol-SKIP_TO_DRAGONS] = true; // remember it.

	    i += 6;
	  } // else the rrggbb is garbage and we skip to "Skip to the newline".
	} // else it's some freakish character we don't care about
	// We've sucked all the juice out of this line.  Skip to the newline
	while ((i < memo_len - 1) && (memo[i] != linefeedChr))
	  i++;
	if (memo[i] == linefeedChr) i++;
      } // end while(not EOF)
    } // else memo does not match, try the next memo
    MemHandleUnlock(recH);
    recordNum++;
  } // end while(records)
  DmCloseDatabase(MemoDB);
  /*
   * DONE READING MEMO
   * At this point either IsColor is true, meaning we found the memo and,
   * unless user is taunting us, set some entries of symbol_colors[];
   * or it is false, meaning we did not find a color definition memo.
   */

}
#endif // I_AM_COLOR


#ifdef I_AM_COLOR
// lacking a scanf function, and being uncertain whether StrAToI likes hex,..
// Requires:  There are at least len characters in buf, and those len
//            characters satisfy Is_Hex (that is they are 0-9a-fA-F)
static Short grok_hex(Char *buf, Short len)
{
  Short i, val;
  Short total = 0;
  for (i = 0 ; i < len ; i++) {
    total *= 16;
    if (buf[i] >= '0' && buf[i] <= '9')
      val = buf[i] - '0';
    else if (buf[i] >= 'a' && buf[i] <= 'f')
      val = 10 + (buf[i] - 'a');
    else if (buf[i] >= 'A' && buf[i] <= 'F')
      val = 10 + (buf[i] - 'A');
    else // silly rabbit, this is not hex
      val = 0;
    total += val;
  }
  return total;
}
#endif // I_AM_COLOR
// This might be pretty slow.  Maybe should inline it,
// Also compare how long it takes w/o bounds check.
// If inlined this would go in put_char_at() in lib_curses.c
// put_char_at is called only by refresh() so the Push and Pop should go there.
#ifdef I_AM_COLOR
IndexedColorType get_color(Char c, Boolean bold)
{
  IndexedColorType ict = black;
  Short i;
  if (bold &&  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))  )
    i = c + SKIP_TO_BOLD;
  else i = c - SUBTRACT_BANG;
  // Let's have some bounds checking here.
  if (i >= 0 && i < NUM_SYMBOLS)
    ict = symbol_colors[i];
  if (my_prefs.black_bg && ict == black)
    return white;
  else
    return ict;
}
#endif // I_AM_COLOR

#ifdef I_AM_COLOR
IndexedColorType get_dragon_color(Short mon)
{
  IndexedColorType ict = black;
  Short i = DRAGON_CHAR - SUBTRACT_BANG;
  Short j = -1;
  switch(mon){
    //  case WHITEDRAGON:    break; // White dragons are 'd', not 'D'. ha!
  case BRONZEDRAGON:   j = 0; break;
  case GREENDRAGON:    j = 1; break;
  case SILVERDRAGON:   j = 2; break;
  case PLATINUMDRAGON: j = 3; break;
  case REDDRAGON:      j = 4; break;
  default:
    break; // hmmm there's a bug if you hit default.
  }
  if (j >= 0 && j < NUM_DRAGONS)
    if (dragon_colors[j])
      i = SKIP_TO_DRAGONS + j; // else i remains index of default 'D' color.

  ict = symbol_colors[i];
  if (my_prefs.black_bg && ict == black)
    return white;
  else
    return ict;
}
#endif // I_AM_COLOR
