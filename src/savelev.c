/*********************************************************************
 * iLarn - Larn adapted for the PalmPilot.                           *
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
#include "Globals.h"
#include "bit.h"

// the 'deepest' floor at which it is 'ok' for me to assume "OWALL-->!monster"
// (this is used in saving and restoring monsters, obviously both must match)
#define WTWMON_LEV 10

void remove_save_records()
{
  ULong uniqueID;
  UInt obsolete;
  // remove all records that were created by a 'save'
  // delete any old CHARACTER save records
  for (uniqueID = REC_ENV ; uniqueID <= REC_CDUDE ; uniqueID++)
    if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
      DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !
  // delete any old LEVEL save records
  for (uniqueID = 100 ; uniqueID <= 100+(10*NLEVELS) ; uniqueID++)
    if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
      DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !
  // leave REC_SPHERES and those other guys alone!
}


// save_all is UNCHANGED
// I think I don't need to save 'moved'
// also, I've made char_class into a number instead of string, so there
void save_all(struct everything *env)
{
  Short index;
  UInt obsolete;
  VoidHand vh;
  VoidPtr p;
  ULong uniqueID;
  Short x, i;

  savelevel(env);
  // NOTE: it's possible for savelevel to fail and return false.  xxxxxxx
  // It would be polite for me to detect this and warn the user  xxxxxxx
  // that he/she is, presumably, low on memory.   xxxxxxxxxxxxxxx

  // delete any old save records
  for (uniqueID = REC_GENOCIDED ; uniqueID <= REC_CDUDE ; uniqueID++)
    if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
      DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !
  // save cdude
  index = DmNumRecords(iLarnSaveDB);
  uniqueID = REC_CDUDE;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Long) * 100);
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env->cdude, sizeof(Long) * 100);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save ivenarg
  uniqueID = REC_IVENARG;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Short) * NINVT);
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env->ivenarg, sizeof(Short) * NINVT);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save iven
  uniqueID = REC_IVEN;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Char) * NINVT);
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env->iven, sizeof(Char) * NINVT);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save spelknow
  uniqueID = REC_SPELKNOW;
  vh = DmNewRecord(iLarnSaveDB,&index,sizeof(Boolean)*(SPNUM+MAXPOTION+MAXSCROLL));
  p = MemHandleLock(vh);
  if (p) {
    i = 0;
    DmWrite(p, i, env->spelknow, sizeof(Boolean) * SPNUM);
    i += sizeof(Boolean) * SPNUM;
    DmWrite(p, i, env->potion_known, sizeof(Boolean) * MAXPOTION);
    i += sizeof(Boolean) * MAXPOTION;
    DmWrite(p, i, env->scroll_known, sizeof(Boolean) * MAXSCROLL);
  }
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save beenhere
  uniqueID = REC_BEENHERE;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Boolean)*(MAXLEVEL+MAXVLEVEL));
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env->beenhere, sizeof(Boolean) * (MAXLEVEL+MAXVLEVEL));
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save genocided
  uniqueID = REC_GENOCIDED;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Boolean) * 66);
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env->genocided, sizeof(Boolean) * 66);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save stealth
  uniqueID = REC_STEALTH;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(Boolean) * MAXX * MAXY);
  p = MemHandleLock(vh);
  if (p) {
    for (x = 0, i = 0 ; x < MAXX ; x++) {
      DmWrite(p, i, env->stealth[x], MAXY * sizeof(Boolean));
      i += MAXY * sizeof(Boolean);
    }
  }
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  // save env
  uniqueID = REC_ENV;
  vh = DmNewRecord(iLarnSaveDB, &index, sizeof(struct everything));
  p = MemHandleLock(vh);
  if (p)
    DmWrite(p, 0, env, sizeof(struct everything)); // careful when you read it
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
}


// restore_all will NOT try to restore the level.
// instead you should do that AFTER calling restore_all.
// (thus, like save_all, this is unchanged by new level format)
Boolean restore_all(struct everything *env)
{
  // Go through the same thing in reverse, eh..
  // except load scalar members of 'env' one at a time
  // well actually I could just copy it xxxxxxxxx
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID;
  //  Short x,y,i;
  Short x,i;
  struct everything *oldenv;

  //
  uniqueID = REC_CDUDE;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false; // didn't find it
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;   // we're in some kind of trouble
  err = MemMove(env->cdude, p, sizeof(Long) * 100);
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_IVENARG;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  err = MemMove(env->ivenarg, p, sizeof(Short) * NINVT);
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_IVEN;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  err = MemMove(env->iven, p, sizeof(Char) * NINVT);
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_SPELKNOW;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  i = 0;
  err = MemMove(env->spelknow, p+i, sizeof(Boolean) * SPNUM);
  i += sizeof(Boolean) * SPNUM;
  err = MemMove(env->potion_known, p+i, sizeof(Boolean) * MAXPOTION);
  i += sizeof(Boolean) * MAXPOTION;
  err = MemMove(env->scroll_known, p+i, sizeof(Boolean) * MAXSCROLL);
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_BEENHERE;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  err = MemMove(env->beenhere, p, sizeof(Boolean) * (MAXLEVEL+MAXVLEVEL));
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_GENOCIDED;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  err = MemMove(env->genocided, p, sizeof(Boolean) * 66);
  if (err) { MemHandleUnlock(vh); return false; }
  MemHandleUnlock(vh);
  //
  uniqueID = REC_STEALTH;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  for (x = 0, i = 0 ; x < MAXX ; x++) {
    err = MemMove(env->stealth[x], p + i, MAXY * sizeof(Boolean));
    if (err) { MemHandleUnlock(vh); return false; }
    i += MAXY * sizeof(Boolean);
  }
  MemHandleUnlock(vh);
  //
  // finally env, which will be a special case.
  uniqueID = REC_ENV;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;
  oldenv = (struct everything *) p;
  env->gtime = oldenv->gtime;
  env->lasttime = oldenv->lasttime;
  env->playerx = oldenv->playerx;
  env->playery = oldenv->playery;
  env->lastnum = oldenv->lastnum;
  env->lasthx = oldenv->lasthx;
  env->lasthy = oldenv->lasthy;
  env->ko_object = oldenv->ko_object;
  env->level = oldenv->level;
  env->char_class = oldenv->char_class;
  env->courses_taken = oldenv->courses_taken;
  env->warez_taken = oldenv->warez_taken;
  env->outstanding_taxes = oldenv->outstanding_taxes;
  env->damage_taxes = oldenv->damage_taxes;
  // skipping: dropflag, hitflag, pending_splash.  these should be false.
  MemHandleUnlock(vh);
  return true; // success!  wow.  now try to restore level.
}


////////////////////////////////////////////////////////////
// The following routines (savelevel and getlevel)
// have been REVISED to use a more COMPACT, I trust, format.
// They are probably full of MANY-LEGGED BUGS now.



// Ok, need to save:
// item, iarg, mitem, hitp, know
// I have a plan, see savelev.txt
#define ROW_BYTES 9
// Record offsets from 100 + 10*n
#define RECOFF_WALLP 0
#define RECOFF_OBJP  1
#define RECOFF_MONP  2
#define RECOFF_ITEM  3
#define RECOFF_IARG  4
#define RECOFF_MITEM 5
#define RECOFF_HITP  6
#define RECOFF_KNOW  7
#define RECOFF_KNOW2 8

Boolean savelevel(struct everything *env)
{
  // Maybe I should open the SAVEDLEVELS_INDEX_RECORD
  // and read the i'th entry; if it's 0, don't believe it.
  Short this_lev = env->level;
  Short index, index2;
  UInt obsolete;
  VoidHand vh, vh2;
  VoidPtr p, p2;
  ULong uniqueID;
  Short x,y,i,i2, nowall_ctr, obj_ctr, mon_ctr;
  UChar bit_array[ROW_BYTES*17]; // 67 bits per dungeon row = 9 bytes
  Char tmp_ch;
  Short tmp_sh;
  Long tmp_lo;

  // Get the set of unique IDs to record this level at.
  uniqueID = 100 + this_lev * 10; // + 0-8
  // Delete old version of this level, if any
  for (i = 0 ; i < 10 ; i++, uniqueID++)
    if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
      DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !

  // Create a record for each thing I am saving:

  ///////////////
  // RECORD n+0: The bitfield of 0="no wall" / 1="wall"
  uniqueID = 100 + this_lev * 10 + RECOFF_WALLP;
  index = DmNumRecords(iLarnSaveDB);
  for (i = 0 ; i < ROW_BYTES*17 ; i++)
    bit_array[i] = 0;
  for (y = 0, i = 0, nowall_ctr = 0 ; y < MAXY ; y++) { // for each wall
    for (x = 0 ; x < MAXX ; x++, i++) { // for each tile
      if (env->item[x][y] == OWALL)
	BITSET( bit_array, i ); // if wall, set bit ON
      else
	nowall_ctr++; // keep track of the number of NON-walls!
    }
  } // ok, there is one bit for every tile in the dungeon..
  //  vh = DmNewRecord(iLarnSaveDB, &index, ROW_BYTES * 17);
  if (i > 0) { // should always be true
    vh = DmNewRecord(iLarnSaveDB, &index, (i+7)/8);
    p = MemHandleLock(vh);
    if (!p) return false;
    DmWrite(p, 0, bit_array, (i+7)/8);
    //  DmWrite(p, i, nowall_ctr, sizeof(Short)); // write the # of non-walls
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
    index++;
  }
  // Done with the wall/nowall record

  ///////////////
  // RECORD n+1: The bitfield of object/noobject
  uniqueID = 100 + this_lev * 10 + RECOFF_OBJP;
  for (i = 0 ; i < ROW_BYTES*17 ; i++)
    bit_array[i] = 0;
  for (y = 0, i = 0, obj_ctr = 0 ; y < MAXY ; y++) {
    // For each row of the dungeon, collect wall information
    for (x = 0 ; x < MAXX ; x++) {
      if (env->item[x][y] != OWALL) {
	if (env->item[x][y] != NO_OBJ) {
	  BITSET( bit_array, i );
	  obj_ctr++; // counts the number of (non-wall) objects in level
	}
	i++; // counts the number of non-wall tiles, we use one bit for each
      } // else it's a wall, and we ignore it.
    }
  }
  if (i > 0) { // should always be true
    vh = DmNewRecord(iLarnSaveDB, &index, (i+7)/8);
    p = MemHandleLock(vh);
    if (!p) return false;
    DmWrite(p, 0, bit_array, (i+7)/8);
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
    index++;
  }
  // Done with the object/noobject record

  ///////////////
  // RECORD n+2: The bitfield of monster/nomonster
  uniqueID = 100 + this_lev * 10 + RECOFF_MONP;
  for (i = 0 ; i < ROW_BYTES*17 ; i++)
    bit_array[i] = 0;
  for (y = 0, i = 0, mon_ctr = 0 ; y < MAXY ; y++) {
    // For each row of the dungeon, collect wall information
    for (x = 0 ; x < MAXX ; x++) {
      // Skip walls IFF this level is too early to have demons an'at.
      if (this_lev > WTWMON_LEV || env->item[x][y] != OWALL) { // this is ITEM,
	if (env->mitem[x][y] != NO_MON) { // this is MITEM.  got it?
	  BITSET( bit_array, i );
	  mon_ctr++; // counts the number of monsters in level
	}
	i++; // # non-wall tiles
      } // else it's a wall, and we ignore it.
    }
  }
  if (i > 0) { // should always be true
    vh = DmNewRecord(iLarnSaveDB, &index, (i+7)/8);
    p = MemHandleLock(vh);
    if (!p) return false;
    DmWrite(p, 0, bit_array, (i+7)/8);
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
    index++;
  }
  // Done with the monster/nomonster record

  //////////////////
  // RECORD n+3,n+4: Compact the object arrays: remove OWALLs and NO_OBJs.
  index2 = index + 1;
  if (obj_ctr > 0) { // might not be true!
    vh = DmNewRecord(iLarnSaveDB, &index, obj_ctr * sizeof(Char));
    vh2 = DmNewRecord(iLarnSaveDB, &index2, obj_ctr * sizeof(Long));
    p = MemHandleLock(vh);
    p2 = MemHandleLock(vh2);
    if (!p || !p2) return false;
    for (y = 0, i = 0, i2 = 0 ; y < MAXY ; y++)  // for each row
      for (x = 0 ; x < MAXX ; x++)  // for each tile
	if (env->item[x][y] != OWALL && env->item[x][y] != NO_OBJ) {
	  tmp_ch = env->item[x][y];
	  DmWrite(p, i, &tmp_ch, sizeof(Char));
	  tmp_lo = env->iarg[x][y];
	  DmWrite(p2, i2, &tmp_lo, sizeof(Long));
	  i += sizeof(Char);
	  i2 += sizeof(Long);
	}  // else it's a wall or nothing, and we ignore it!
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    DmReleaseRecord(iLarnSaveDB, index2, true);
    uniqueID = 100 + this_lev * 10 + RECOFF_ITEM;
    DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
    uniqueID = 100 + this_lev * 10 + RECOFF_IARG;
    DmSetRecordInfo(iLarnSaveDB, index2, NULL, &uniqueID);
    index+=2;
  }
  // Done with the compacted-item and compacted-iarg arrays

  //////////////////
  // RECORD n+5,n+6: Compact the monster arrays: remove OWALLs and NO_MONs.
  index2 = index + 1;
  if (mon_ctr > 0) { // might not be true!
    vh = DmNewRecord(iLarnSaveDB, &index, mon_ctr * sizeof(Char));
    vh2 = DmNewRecord(iLarnSaveDB, &index2, mon_ctr * sizeof(Short));
    p = MemHandleLock(vh);
    p2 = MemHandleLock(vh2);
    if (!p || !p2) return false;
    for (y = 0, i = 0, i2 = 0 ; y < MAXY ; y++)  // for each row
      for (x = 0 ; x < MAXX ; x++)  // for each tile
	if (env->item[x][y] != OWALL && env->mitem[x][y] != NO_MON) {
	  // *item* is not wall and *mitem* is not no-monster
	  tmp_ch = env->mitem[x][y];
	  DmWrite(p, i, &tmp_ch, sizeof(Char));
	  tmp_sh = env->hitp[x][y];
	  DmWrite(p2, i2, &tmp_sh, sizeof(Short));
	  i += sizeof(Char);
	  i2 += sizeof(Short);
	}  // else it's a wall or nothing, and we ignore it!
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    DmReleaseRecord(iLarnSaveDB, index2, true);
    uniqueID = 100 + this_lev * 10 + RECOFF_MITEM;
    DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
    uniqueID = 100 + this_lev * 10 + RECOFF_HITP;
    DmSetRecordInfo(iLarnSaveDB, index2, NULL, &uniqueID);
    index+=2;
  }
  // Done with the compacted-mitem and compacted-hitp arrays

  ///////////////
  // RECORD n+7: The bitfield of 0="know==0" / 1="know>0"
  // Suppose that we make all know>0 into know==2; is that too evil?
  uniqueID = 100 + this_lev * 10 + RECOFF_KNOW;
  for (i = 0 ; i < ROW_BYTES*17 ; i++)
    bit_array[i] = 0;
  for (y = 0, i = 0 ; y < MAXY ; y++) {
    for (x = 0 ; x < MAXX ; x++, i++) {
      if (env->know[x][y] > 0) { // set bit ON if know==1 or know==2
	BITSET( bit_array, i );
      }
    }
  }
  vh = DmNewRecord(iLarnSaveDB, &index, (i+7)/8);
  p = MemHandleLock(vh);
  if (!p) return false;
  DmWrite(p, 0, bit_array, (i+7)/8);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  uniqueID++;
  ///////////////
  // RECORD n+8: The bitfield of 0="know<2" / 1="know==2"
  uniqueID = 100 + this_lev * 10 + RECOFF_KNOW2;
  for (i = 0 ; i < ROW_BYTES*17 ; i++)
    bit_array[i] = 0;
  for (y = 0, i = 0 ; y < MAXY ; y++) {
    for (x = 0 ; x < MAXX ; x++, i++) {
      if (env->know[x][y] == 2) { // set bit ON if know==1 or know==2
	BITSET( bit_array, i );
      }
    }
  }
  vh = DmNewRecord(iLarnSaveDB, &index, (i+7)/8);
  p = MemHandleLock(vh);
  if (!p) return false;
  DmWrite(p, 0, bit_array, (i+7)/8);
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  index++;
  uniqueID++;
  // Done with the wall/nowall record
  return true; // we made it!
}

// You should ONLY call this if you know that you previously SAVED the level.
// returns true on success
// note that if it returns false, those arrays could be full of cruft.
Boolean getlevel(struct everything *env)
{
  Short this_lev = env->level;
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err, err2;
  ULong uniqueID;
  Short x,y,i, nowall_ctr, obj_ctr, mon_ctr;
  UChar *iswall_pack;
  UChar *isobj_pack;
  UChar *ismon_pack;
  UChar *know_pack;
  Char *item_pack=0;
  Char *mitem_pack=0;
  Long *iarg_pack=0;
  Short *hitp_pack=0;

  //  return false; // for now.

  ///////////////
  // VARIOUS records: Will I regret opening them all at once?  Probably
  // I'll try opening only half of them at once.
  // FIRST we'll write item,iarg, SECOND we'll write mitem,hitp.
  //
  // first get the n+0 record of wall/no-wall (must exist)
  uniqueID = 100 + this_lev * 10 + RECOFF_WALLP;
  if ((err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex))) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  if ((p = MemHandleLock(vh))==0) return false;
  iswall_pack = (UChar *) p;
  // next get the n+1 record of obj/no-obj (must exist)
  uniqueID = 100 + this_lev * 10 + RECOFF_OBJP;
  if ((err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex))) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  if ((p = MemHandleLock(vh))==0) return false;
  isobj_pack = (UChar *) p;
  // next get the n+3 and n+4 compact records of item and iarg
  // these might not exist if there are no objects on this level!!!
  // (but either 0 or BOTH should exist.)
  uniqueID = 100 + this_lev * 10 + RECOFF_ITEM;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (!err) {
    vh = DmQueryRecord(iLarnSaveDB, recindex);
    if ((p = MemHandleLock(vh))==0) return false;
    item_pack = (Char *) p;
  }
  uniqueID = 100 + this_lev * 10 + RECOFF_IARG;
  err2 = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err || err2) {
    if (!(err && err2)) {
      if (item_pack) MemHandleUnlock(MemPtrRecoverHandle(item_pack));
      if (iarg_pack) MemHandleUnlock(MemPtrRecoverHandle(iarg_pack));
      return false; // 1 but not both exist.
    }
  } else {
    vh = DmQueryRecord(iLarnSaveDB, recindex);
    if ((p = MemHandleLock(vh))==0) return false;
    iarg_pack = (Long *) p;
  }
  // read bits, write env->item and env->iarg
  nowall_ctr = obj_ctr = 0;
  for (y = 0, i = 0 ; y < MAXY ; y++)  // for each wall
    for (x = 0 ; x < MAXX ; x++, i++)  // for each tile
      if (BITTEST( iswall_pack, i)) {
	env->item[x][y] = OWALL; // it's a wall, how boring.
	env->iarg[x][y] = 0;
      } else {
	if (BITTEST( isobj_pack, nowall_ctr ) && item_pack && iarg_pack) {
	  // cool! it's a (non-wall) object.
	  env->item[x][y] = item_pack[obj_ctr];
	  env->iarg[x][y] = iarg_pack[obj_ctr];
	  obj_ctr++;
	} else {
	  env->item[x][y] = NO_OBJ; // it's nothing, how boring.
	  env->iarg[x][y] = 0;
	}
	nowall_ctr++;
      }
  MemHandleUnlock(MemPtrRecoverHandle(iswall_pack));
  MemHandleUnlock(MemPtrRecoverHandle(isobj_pack));
  if (!err) {
    MemHandleUnlock(MemPtrRecoverHandle(item_pack));
    MemHandleUnlock(MemPtrRecoverHandle(iarg_pack));
  }
  //  return false; // for now.

  ///////////////
  // OK NOW try for mitem and hitp
  //
  // first get the n+2 record of mon/no-mon (must exist)
  uniqueID = 100 + this_lev * 10 + RECOFF_MONP;
  if ((err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex))) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  if ((p = MemHandleLock(vh))==0) return false;
  ismon_pack = (UChar *) p;
  // next get the n+5 and n+6 compact records of mitem and hitp
  // these might not exist if there are no objects on this level!!!
  // (but either 0 or BOTH should exist.)
  uniqueID = 100 + this_lev * 10 + RECOFF_MITEM;
  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (!err) {
    vh = DmQueryRecord(iLarnSaveDB, recindex);
    if ((p = MemHandleLock(vh))==0) return false;
    mitem_pack = (Char *) p;
  }
  uniqueID = 100 + this_lev * 10 + RECOFF_HITP;
  err2 = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err || err2) {
    if (!(err && err2)) {
      if (mitem_pack) MemHandleUnlock(MemPtrRecoverHandle(mitem_pack));
      if (hitp_pack) MemHandleUnlock(MemPtrRecoverHandle(hitp_pack));
      return false; // 1 but not both exist.
    }
  } else {
    vh = DmQueryRecord(iLarnSaveDB, recindex);
    if ((p = MemHandleLock(vh))==0) return false;
    hitp_pack = (Short *) p;
  }
  // read bits, write env->mitem and env->hitp
  nowall_ctr = mon_ctr = 0;
  for (y = 0 ; y < MAXY ; y++)  // for each wall
    for (x = 0 ; x < MAXX ; x++)  // for each tile
      // skip walls IFF this_lev is too early for demons an'at
      if (this_lev > WTWMON_LEV || env->item[x][y] != OWALL) {
	if (BITTEST( ismon_pack, nowall_ctr ) && mitem_pack && hitp_pack) {
	  // cool! it's a monster.
	  env->mitem[x][y] = mitem_pack[mon_ctr];
	  env->hitp[x][y] = hitp_pack[mon_ctr];
	  mon_ctr++;
	} else {
	  env->mitem[x][y] = NO_MON; // it's nothing, how boring.
	  env->hitp[x][y] = 0;
	}
	nowall_ctr++;
      } else {
	env->mitem[x][y] = NO_MON; // it's nothing, how boring.
	env->hitp[x][y] = 0;
      }

  MemHandleUnlock(MemPtrRecoverHandle(ismon_pack));
  if (!err) {
    MemHandleUnlock(MemPtrRecoverHandle(mitem_pack));
    MemHandleUnlock(MemPtrRecoverHandle(hitp_pack));
  }

  // I think that that just about takes care of it....
  // ok except for the know array.
  // get the n+7 record of know (must exist)
  uniqueID = 100 + this_lev * 10 + RECOFF_KNOW;
  if ((err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex))) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  if ((p = MemHandleLock(vh))==0) return false;
  know_pack = (UChar *) p;
  for (y = 0, i = 0 ; y < MAXY ; y++)  // for each wall
    for (x = 0 ; x < MAXX ; x++, i++)  // for each tile
      if (BITTEST( know_pack, i))
	env->know[x][y] = 1;
      else
	env->know[x][y] = 0;
  MemHandleUnlock(MemPtrRecoverHandle(know_pack));
  // get the n+8 record of know (must exist)
  uniqueID = 100 + this_lev * 10 + RECOFF_KNOW2;
  if ((err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex))) return false;
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  if ((p = MemHandleLock(vh))==0) return false;
  know_pack = (UChar *) p;
  for (y = 0, i = 0 ; y < MAXY ; y++)  // for each wall
    for (x = 0 ; x < MAXX ; x++, i++)  // for each tile
      if (BITTEST( know_pack, i))
	env->know[x][y] = 2;
  // else it was already set when we read RECOFF_KNOW.
  MemHandleUnlock(MemPtrRecoverHandle(know_pack));

  // Ok, NOW i am done.  This will probably be a good deal slower
  // but take up somewhat less space.
  return true;
}


/**************************************************************************
 *
 *
 *  Save a snapshot of the current character.  Can be accessed in chargen
 *
 *
 *************************************************************************/
extern LarnPreferenceType my_prefs; // must save character name too!
Boolean save_sketch()
{
  UInt obsolete;
  VoidHand vh;
  VoidPtr p;
  ULong uniqueID = REC_SKETCH;
  Int size;
  Short index, x, i, offset;
  //
  Short tmps, pots[4] = { P_R_LVL, P_INC_A, P_LEARN, P_INSTH };
  Char tmpc;
  Boolean potion;
  sketch_misc smisc;

  smisc.gtime = env->gtime;

  // delete any old sketch record
  if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
    DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !

  // How much space will this take:
  size = (sizeof(Long) * 100 // cdude
	  + sizeof(Short) * NINVT // ivenarg
	  + sizeof(Char) * NINVT // iven
	  + sizeof(Boolean)*(SPNUM+MAXPOTION+MAXSCROLL) // which known
	  + sizeof(Boolean) * 66 // genocided
	  + sizeof(sketch_misc)); // tax, dmg, gtime, class, courses, warez
  
  index = DmNumRecords(iLarnSaveDB);
  vh = DmNewRecord(iLarnSaveDB, &index, size);
  p = MemHandleLock(vh);
  if (!p) {
    MemPtrUnlock(p);
    DmReleaseRecord(iLarnSaveDB, index, true);
    return false;
  }

  offset = 0;
  // Write Longs     cdude
  DmWrite(p, offset, env->cdude, sizeof(Long) * 100);
  offset += sizeof(Long) * 100;
  // Write Shorts    ivenarg 
  DmWrite(p, offset, env->ivenarg, sizeof(Short) * NINVT);
  offset += sizeof(Short) * NINVT;
  // Write Chars     iven
  DmWrite(p, offset, env->iven, sizeof(Char) * NINVT);
  offset += sizeof(Char) * NINVT;
  //      ... but not the OLARNEYE or the P_CURED...
  potion = false;
  for (i = 0 ; i < NINVT ; i++) {
    if (env->iven[i] == OPOTION && env->ivenarg[i] == P_CURED) {
      tmps = pots[rund(4)]; // potion becomes a different potion
      DmWrite(p, sizeof(Long)*100 + sizeof(Short)*i, &tmps, sizeof(Short));
      smisc.gtime /= 2;
      potion = true;
    }
  }
  if (!potion) // ok, if you had the potion you can keep the eye of larn...
    for (i = 0 ; i < NINVT ; i++) {
      if (env->iven[i] == OLARNEYE) {
	tmpc = ODIAMOND + rund(4); // eye of larn becomes a gem
	x = sizeof(Long)*100 + sizeof(Short)*NINVT;
	DmWrite(p, x + sizeof(Char)*i, &tmpc, sizeof(Char));
	tmps = 255; // set value to maximum
	DmWrite(p, sizeof(Long)*100 + sizeof(Short)*i, &tmps, sizeof(Short));
	smisc.gtime /= 2;
      }
    }
  // ok, done voiding plot tokens
  // Write Booleans  spelknow, potion_known, scroll_known, genocided
  DmWrite(p, offset, env->spelknow, sizeof(Boolean) * SPNUM);
  offset += sizeof(Boolean) * SPNUM;
  DmWrite(p, offset, env->potion_known, sizeof(Boolean) * MAXPOTION);
  offset += sizeof(Boolean) * MAXPOTION;
  DmWrite(p, offset, env->scroll_known, sizeof(Boolean) * MAXSCROLL);
  offset += sizeof(Boolean) * MAXSCROLL;
  DmWrite(p, offset, env->genocided, sizeof(Boolean) * 66);
  offset += sizeof(Boolean) * 66;
  // All ok up to here......
  // Write tax, dmg, gtime
  // Write char_class, courses_taken, warez_taken
  smisc.taxes = env->outstanding_taxes;
  smisc.dmg = env->damage_taxes;
  smisc.char_class = env->char_class;
  smisc.courses_taken = env->courses_taken;
  smisc.warez_taken = env->warez_taken;
  StrNCopy(smisc.who, my_prefs.name, NAMELEN);  
  DmWrite(p, offset, &smisc, sizeof(sketch_misc));

  MemPtrUnlock(p);
  DmReleaseRecord(iLarnSaveDB, index, true);
  DmSetRecordInfo(iLarnSaveDB, index, NULL, &uniqueID);
  return true;
}
Boolean load_sketch()
{
  UInt recindex;
  VoidHand vh;
  VoidPtr p;
  Err err;
  ULong uniqueID = REC_SKETCH;
  Short i=0;
  sketch_misc *smiscp;

  err = DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex);
  if (err) return false; // didn't find it
  vh = DmQueryRecord(iLarnSaveDB, recindex);
  p = MemHandleLock(vh);
  if (!p) return false;   // we're in some kind of trouble
  err = false;
  if (!err) err = MemMove(env->cdude, p, sizeof(Long) * 100);
  i += sizeof(Long) * 100;
  if (!err) err = MemMove(env->ivenarg, p+i, sizeof(Short) * NINVT);
  i += sizeof(Short) * NINVT;
  if (!err) err = MemMove(env->iven, p+i, sizeof(Char) * NINVT);
  i += sizeof(Char) * NINVT;
  if (!err) err = MemMove(env->spelknow, p+i, sizeof(Boolean) * SPNUM);
  i += sizeof(Boolean) * SPNUM;
  if (!err) err = MemMove(env->potion_known, p+i, sizeof(Boolean) * MAXPOTION);
  i += sizeof(Boolean) * MAXPOTION;
  if (!err) err = MemMove(env->scroll_known, p+i, sizeof(Boolean) * MAXSCROLL);
  i += sizeof(Boolean) * MAXSCROLL;
  if (!err) err = MemMove(env->genocided, p+i, sizeof(Boolean) * 66);
  i += sizeof(Boolean) * 66;
  // Read tax, dmg, gtime, etc etc
  if (!err) {
    smiscp = (sketch_misc *)(p+i);
    env->gtime = smiscp->gtime;
    env->outstanding_taxes = smiscp->taxes;
    env->damage_taxes = smiscp->dmg;
    env->char_class = smiscp->char_class;
    env->courses_taken = smiscp->courses_taken;
    env->warez_taken = smiscp->warez_taken;
    env->cdude[BANKACCOUNT] = 0;
    //    env->cdude[GOLD] = 0; // nah, too cruel
    StrNCopy(my_prefs.name, smiscp->who, NAMELEN);  
  }
  MemHandleUnlock(vh);

  if (!err)
    DmRemoveRecord(iLarnSaveDB, recindex);      // EX TERM IN ATE !

  return !err;
}

void destroy_sketch_if_any()
{
  UInt obsolete;
  ULong uniqueID = REC_SKETCH;
  // delete any old sketch record
  if (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &obsolete))
    DmRemoveRecord(iLarnSaveDB, obsolete);      // EX TERM IN ATE !
}

Boolean sketch_exists()
{
  UInt recindex;
  ULong uniqueID = REC_SKETCH;
  return (0 == DmFindRecordByID(iLarnSaveDB, uniqueID, &recindex));
}
