/*********************************************************************
 * iLarn - Ularn adapted for the PalmPilot.                          *
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
/* 
 * Here find a file full of "extern" declarations 
 * These are all defined in iLarn.c (currently)`
 */
#ifndef __ILARN_GLOBALS_H__
#define __ILARN_GLOBALS_H__

#define iLarnAppType 'Larn'
#define iLarnDBType  'Data'
#define iLarnSaveDBType  'Data'
#define iLarnAppID   'Larn'
extern DmOpenRef       iLarnDB;
extern DmOpenRef       iLarnSaveDB;
#define iLarnDBName "iLarnDB"
#define iLarnSaveDBName "iLarnSaveDB"
//#define iLarnLibraryName "iLarn_Library"
//#define iLarn2LibraryName "iLarn2_Library"

#define iLarnAppPrefID 0x00
#define PrefVersion1 0x01 // Had a different top ten format
#define PrefVersion2 0x02 // Did not have 'color' and 'black_bg'
#define PrefVersion3 0x03 // Still had top ten in DB
#define iLarnAppPrefVersion 0x04 // [We move top ten to SaveDB.]


#include "header.h"
#include "types.h"
//#include "prototypes.h"
#include "larn_prototypes.h"

extern struct everything *env;


#endif
