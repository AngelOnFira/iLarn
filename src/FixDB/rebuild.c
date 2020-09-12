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
#include <Pilot.h>
//#include <Globals.h>
#include "fix_db.h"

// Here is a little routine for creating the database record for dndstore

DmOpenRef iLarnDB;
static Boolean OpenDatabase();
static void make_spheres();

/*****************************************************************************
 *                                                                           *
 *                      RebuildDatabase                                      *
 *                                                                           *
 *****************************************************************************/
// returns false on error, true otherwise.
Boolean RebuildDatabase()
{
  // open the database
  if (OpenDatabase())
    return false;

  // write the dndstore records
  make_dndstore_rec(iLarnDB);

  // write any other records that I need
  // (such as, canned levels)
  make_canned_levels(iLarnDB);

  make_spheres();
  
  // close database
  if (iLarnDB != NULL) {
    DmCloseDatabase(iLarnDB);
    iLarnDB = NULL;
  }
  return true;
}

static void make_spheres()
{
  VoidHand fooRec;
  Ptr p;
  Int i, size;
  UInt index;
  ULong uniqueID = REC_SPHERES;

  index = DmNumRecords(iLarnDB);
  size = sizeof(struct sphere) * MAXSPHERE;

  fooRec = DmNewRecord(iLarnDB, &index, size);
  p = MemHandleLock(fooRec);
  DmSet(p, 0, size, 0);  // set it all to 0.  i think that will work.
  MemPtrUnlock(p);
  DmReleaseRecord(iLarnDB, index, true);
  DmSetRecordInfo(iLarnDB, index, NULL, &uniqueID); // set its uid!
}


/*****************************************************************************
 *                                                                           *
 *                      OpenDatabase                                         *
 *                                                                           *
 *****************************************************************************/
static Boolean OpenDatabase()
{
  LocalID dbID;

  // DELETE database, if any.
  dbID = DmFindDatabase(0, iLarnDBName); // card#, name
  if (dbID)
    if (DmDeleteDatabase(0, dbID)) // card#, id
      return 1;

  // create database
  if (DmCreateDatabase(0, iLarnDBName, iLarnAppID, iLarnDBType, false))
    return 1;

  // open database
  dbID = DmFindDatabase(0, iLarnDBName); // card#, name
  if (!dbID) return 1;
  iLarnDB = DmOpenDatabase(0, dbID, dmModeReadWrite);
  // we assume that opening a newly created db will work
  return 0;
}
