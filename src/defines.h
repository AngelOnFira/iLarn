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
#ifndef __ILARN_HEADER_H__
#define __ILARN_HEADER_H__


/* decide what include files I need */

/* these might already be defined */
#define TRUE 1
#define FALSE 0

#define MAXLEVEL 16 /*	max # levels + 1 in the dungeon	*/
#define MAXVLEVEL 5 /*	max # of levels in the temple of the luran (volcano) */

#define NINVT     (26)         /* maximum parts in inventory */
#define ANYINVT   (-1)         /* matches anything in inventory except gold */ 

#define MAXX 67
#define MAXY 17

#define MAXX_0 20
#define MAXY_0 11


#define MAXPLEVEL 100 	/* maximum player level allowed		*/

/* 56 monsters, 7 demon lords, 1 demon prince, 1 God of Hellfire*/
#define MAXMONST 57 	/* maximum # monsters in the dungeon	*/

                        /* 3 funny numbers below, watch it ! (ws, 10.08.93) */
#define SPNUM     39  	/* maximum number of spells in existance	*/
#define MAXSCROLL 28    /* maximum number of scrolls that are possible	*/
#define MAXPOTION 35 	/* maximum number of potions that are possible	*/

#define TIMELIMIT 40000L /* maximum number of moves before the game is called*/
//#define TIMELIMIT 40960 // trying to fudge 'Long' division problems.
/*  was TAXRATE 1/20 */
#define TAXRATEinv 20 	/* the tax rate for the LRS */
#define MAXOBJ     93 	/* the maximum number of objects   n < MAXOBJ */

/***************************************************************************/
/***************************************************************************/
/* Defines for the character attribute array "long c[100];" in data.c      */
/* Some of these are timers and some are booleans and stuff, I guess.      */
/***************************************************************************/
/***************************************************************************/

/****** Str, Int, Wis, Con, Dex, Cha
	These each have a minimum of 3, and no maximum. */
#define STRENGTH 0	/* characters physical strength not due to objects */
#define INTELLIGENCE 1
#define WISDOM 2
#define CONSTITUTION 3
#define DEXTERITY 4
#define CHARISMA 5
/****** Hpmax has a minimum of 1 and maximum of 999, and defines HP min/max.
	Gold and Experience don't seem to have a maximum; minimum of 0. 
        The maximum Level is MAXPLEVEL. */
#define HPMAX 6
#define HP 7
#define GOLD 8
#define EXPERIENCE 9
#define LEVEL 10
/****** Regen is a timer to add back to HP, minimum is 1. WCLASS min is 0 */
#define REGEN 11
#define WCLASS 12
#define AC 13
/****** BackAccount interest limit is 1 million.
	SpellMax maximum is 125 and it defines the maximum of Spells; min 0 */
#define BANKACCOUNT 14
#define SPELLMAX 15
#define SPELLS 16
/****** Energy is calculated from rings, min 0, used by ecounter.
        Ecounter is the timer for when to regenerate spell points
        MoreDenfenses is also calculated from objects */
#define ENERGY 17
#define ECOUNTER 18
#define MOREDEFENSES 19
/****** Wear is the index in inventory of what you're wearing; or -1.
	Same goes for Wield.  By the way, "AMULET" is NOT USED, nor c[23].
        ProtectionTime is the duration timer for a protection spell. 
	RegenCounter is the countdown for when to regenerate next. */
#define WEAR 20
#define PROTECTIONTIME 21
#define WIELD 22
#define AMULET 23		/* if have amulet of invisibility */
#define REGENCOUNTER 24
/****** hello, MoreDam doesn't seem to be used either. 
	DexCount is the duration timer for extra dexterity,
	ditto StrCount and BlindCount... see regen.c for many other timers */
#define MOREDAM 25
#define DEXCOUNT 26
#define STRCOUNT 27
#define BLINDCOUNT 28		/* if blind */
/****** I guess this is what dungeon level you are on now. */
#define CAVELEVEL 29
/****** Confuse is an eponymous timer.
	AltPro is a timer for a temporary increase of MoreDefenses.
	Hero is an eponymous timer. */
#define CONFUSE 30		/* if confused */
#define ALTPRO 31
#define HERO 32			/* if hero  */
/****** CharmCount through HasteSelf are spell/effect timers */
#define CHARMCOUNT 33
#define INVISIBILITY 34		/* if invisible */
#define CANCELLATION 35		/* if cancel cast */
#define HASTESELF 36		/* if hasted */
/****** I don't see EyeOfLarn used anywhere either */
#define EYEOFLARN 37		/* if have eye */
/****** Aggravate and Globe are spell/effect timers
        TeleFlag is just a boolean.
	Slaying and NegateSpirit are the quantity you have of that orb. */
#define AGGRAVATE 38
#define GLOBE 39
#define TELEFLAG 40		/* if been teleported */
#define SLAYING 41		/* if have orb of dragon slaying */
#define NEGATESPIRIT 42		/* if negate spirit */
/****** ScareMonst through HasteMonst are spell/effect timers */
#define SCAREMONST 43		/* if scare cast */
#define AWARENESS 44		/* if awareness cast */
#define HOLDMONST 45
#define TIMESTOP 46
#define HASTEMONST 47
/****** CubeOfUndead is the quantity you have of that cube. */
#define CUBEofUNDEAD 48		/* if have cube */
/****** GiantStr is a timer for giant strength */
#define GIANTSTR 49		/* if giant strength */
/* ok, I'm bored of this.  figure out the rest yourself. */
#define FIRERESISTANCE 50
#define BESSMANN 51		/* flag for hammer */
#define NOTHEFT 52
#define HARDGAME 53
#define BYTESIN 54		/* used for checkpointing in tok.c */
		/* 55 to 59 are open */
#define LANCEDEATH 60		/* if have LoD */
#define SPIRITPRO 61
#define UNDEADPRO 62
#define SHIELD 63
#define STEALTH 64
#define ITCHING 65
#define LAUGHING 66		/* not implemented */
#define DRAINSTRENGTH 67
#define CLUMSINESS 68
#define INFEEBLEMENT 69
#define HALFDAM 70
#define SEEINVISIBLE 71
#define FILLROOM 72
	/* 73 is open */
#define SPHCAST 74	/* nz if an active sphere of annihilation */
#define WTW 75		/* walk through walls */
#define STREXTRA 76	/* character strength due to objects or enchantments */
#define TMP 77		/* misc scratch space */
#define LIFEPROT 78 	/* life protection counter */
			/* FLAGS : non-zero if object has been made */
#define ORB 79		/* orb - 1 if created, 2 if held */
#define ELVUP 80	/* elevator up */
#define ELVDOWN 81	/* elevator down */
#define HAND 82		/* Hand of Fear */
#define CUBEUNDEAD 83	/* cube of undead control */
#define DRAGSLAY 84	/* orb of dragon slaying */
#define NEGATE 85	/* scarab of negate spirit */
#define URN 86		/* golden urn */
#define LAMP 87		/* brass lamp */
#define TALISMAN 88	/* Talisman of the Sphere */
#define WAND 89		/* wand of wonder */
#define STAFF 90	/* staff of power */
#define DEVICE 91	/* anti-theft */
#define SLASH 92 	/* sword of slashing */
#define ELVEN 93	/* elven chain */
#define VORP  94	/* Vorpal */
#define SLAY  95	/* Slayer */
#define PAD   96	/* Dealer McDopes */
/****** Coked is an eponymous timer */
#define COKED 97	/* timer for being coked out */
/* used up to 97 of 100 */
// Excellent!  I shall use another one.
#define BIRTHDATE 98




/***************************************************************************/
/***************************************************************************/
/*	defines for the objects in the game                                */
/***************************************************************************/
/***************************************************************************/


#define NO_OBJ  (0)
#define OALTAR 1
#define OTHRONE 2
#define OORB 3		/* orb of enlightement - gives expanded awareness
			  	as long as held */
#define OPIT 4
#define OSTAIRSUP 5
#define OELEVATORUP 6
#define OFOUNTAIN 7
#define OSTATUE 8
#define OTELEPORTER 9
#define OSCHOOL 10
#define OMIRROR 11
#define ODNDSTORE 12
#define OSTAIRSDOWN 13
#define OELEVATORDOWN 14
#define OBANK2 15
#define OBANK 16
#define ODEADFOUNTAIN 17
#define OMAXGOLD 70
#define OGOLDPILE 18
#define OOPENDOOR 19
#define OCLOSEDDOOR 20
#define OWALL 21
#define OTRAPARROW 66
#define OTRAPARROWIV 67

#define OLARNEYE 22

#define OPLATE 23
#define OCHAIN 24
#define OLEATHER 25
#define ORING 60
#define OSTUDLEATHER 61
#define OSPLINT 62
#define OPLATEARMOR 63
#define OSSPLATE 64
#define OSHIELD 68

#define OSWORDofSLASHING 26	/* impervious to rust, light, strong */
#define OHAMMER 27		/* Bessman's Flailing Hammer */
#define OSWORD 28
#define O2SWORD 29		/* 2 handed sword */
#define OSPEAR 30
#define ODAGGER 31
#define OBATTLEAXE 57
#define OLONGSWORD 58
#define OFLAIL 59
#define OLANCE 65

#define ORINGOFEXTRA 32
#define OREGENRING 33
#define OPROTRING 34
#define OENERGYRING 35
#define ODEXRING 36
#define OSTRRING 37
#define OCLEVERRING 38
#define ODAMRING 39

#define OBELT 40

#define OSCROLL 41
#define OPOTION 42
#define OBOOK 43
#define OCHEST 44

#define OAMULET 45
#define OORBOFDRAGON 46
#define OSPIRITSCARAB 47
#define OCUBEofUNDEAD 48
#define ONOTHEFT 49

#define ODIAMOND 50
#define ORUBY 51
#define OEMERALD 52
#define OSAPPHIRE 53

#define OENTRANCE 54
#define OVOLDOWN 55
#define OVOLUP 56
#define OHOME 69

#define OKGOLD 71
#define ODGOLD 72
#define OIVDARTRAP 73
#define ODARTRAP 74
#define OTRAPDOOR 75
#define OIVTRAPDOOR 76
#define OTRADEPOST 77
#define OIVTELETRAP 78
#define ODEADTHRONE 79
#define OANNIHILATION 80		/* sphere of annihilation */
#define OTHRONE2 81
#define OLRS 82				/* Larn Revenue Service */
#define OCOOKIE 83
#define OURN 84			/* golden urn - not implemented */
#define OBRASSLAMP 85	/* brass lamp - genie pops up and offers spell */
#define OHANDofFEAR 86		/* hand of fear - scare monster spell lasts
				   twice as long if have this */
#define OSPHTALISMAN 87		/* talisman of the sphere */
#define OWWAND 88	/* wand of wonder - cant fall in trap doors/pits */
#define OPSTAFF 89		/* staff of power - cancels drain life attack*/

#define OVORPAL 90	/* ? - not implemented */
#define OSLAYER 91	/* magical sword - increases intelligence by 10,
			   halves damage caused by demons, demon prince 
			   and lucifer - strong as lance of death against them*/

#define OELVENCHAIN 92		/* elven chain - strong and light, 
				   impervious to rust */
#define OSPEED 93		
#define OACID 94
#define OHASH 95
#define OSHROOMS 96
#define OCOKE 97
#define OPAD 98		/* Dealer McDope's Pad */
/* used up to 98 */





/***************************************************************************/
/***************************************************************************/
/*	defines for the monsters as objects		                   */
/***************************************************************************/
/***************************************************************************/

#define NO_MON  (0)

#define LEMMING 1 
#define GNOME 2
#define HOBGOBLIN 3
#define JACKAL 4 
#define KOBOLD 5 
#define ORC 6 
#define SNAKE 7
#define CENTIPEDE 8
#define JACULI 9 
#define TROGLODYTE 10 
#define ANT 11 
#define EYE 12 
#define LEPRECHAUN 13
#define NYMPH 14 
#define QUASIT 15 
#define RUSTMONSTER 16 
#define ZOMBIE 17 
#define ASSASSINBUG 18 
#define BUGBEAR 19 
#define HELLHOUND 20 
#define ICELIZARD 21 
#define CENTAUR 22 
#define TROLL 23 
#define YETI 24 
#define WHITEDRAGON 25 
#define ELF 26 
#define CUBE 27 
#define METAMORPH 28 
#define VORTEX 29 
#define ZILLER 30 
#define VIOLETFUNGI 31 
#define WRAITH 32 
#define FORVALAKA 33 
#define LAMANOBE 34 
#define OSEQUIP 35 
#define ROTHE 36 
#define XORN 37 
#define VAMPIRE 38 
#define INVISIBLESTALKER 39 
#define POLTERGEIST 40 
#define DISENCHANTRESS 41 
#define SHAMBLINGMOUND 42 
#define YELLOWMOLD 43
#define UMBERHULK 44
#define GNOMEKING 45
#define MIMIC 46
#define WATERLORD 47
#define BRONZEDRAGON 48
#define GREENDRAGON 49
#define PURPLEWORM 50
#define XVART 51
#define SPIRITNAGA 52
#define SILVERDRAGON 53
#define PLATINUMDRAGON 54
#define GREENURCHIN 55
#define REDDRAGON 56
#define DEMONLORD 57
#define DEMONPRINCE 64
#define LUCIFER 65




/***************************************************************************/
/***************************************************************************/
/*                              spells                                     */
/***************************************************************************/
/***************************************************************************/


                             /* SPELLS: */
#define S_NON (-1)  /* None */
#define S_PRO  (0)
#define S_MLE  (1)
#define S_DEX  (2)
#define S_SLE  (3)
#define S_CHM  (4)
#define S_SSP  (5)
#define S_WEB  (6)
#define S_STR  (7)
#define S_ENL  (8)
#define S_HEL  (9)
#define S_CBL (10)
#define S_CRE (11)
#define S_PHA (12)
#define S_INV (13)
#define S_BAL (14)
#define S_CLD (15)
#define S_PLY (16)
#define S_CAN (17)
#define S_HAS (18)
#define S_CKL (19)
#define S_VPR (20)
#define S_DRY (21)
#define S_LIT (22)
#define S_DRL (23)
#define S_GLO (24)
#define S_FLO (25)
#define S_FGR (26)
#define S_SCA (27)
#define S_HLD (28)
#define S_STP (29)
#define S_TEL (30)
#define S_MFI (31)
#define S_MKW (32)
#define S_SPH (33)
#define S_GEN (34)
#define S_SUM (35)
#define S_WTW (36)
#define S_ALT (37)
#define S_PER (38)


/***************************************************************************/
/***************************************************************************/
/*                              scrolls                                    */
/***************************************************************************/
/***************************************************************************/


                             /* SCrOLLS: */
#define R_ENC_A  (0)
#define R_ENC_W  (1)
#define R_ENL    (2)
#define R_BLANK  (3)
#define R_C_MON  (4)
#define R_C_ART  (5)
#define R_AGG_M  (6)
#define R_TIM_W  (7)
#define R_TELEP  (8)
#define R_XPD_A  (9)
#define R_HAS_M (10)
#define R_M_HEL (11)
#define R_SPIRT (12)
#define R_UNDED (13)
#define R_STEAL (14)
#define R_MAPPG (15)
#define R_HLD_M (16)
#define R_GEM_P (17)
#define R_SPL_X (18)
#define R_IDENT (19)
#define R_REM_C (20)
#define R_ANNIH (21)
#define R_PULVR (22)
#define R_LIFEP (23)

/***************************************************************************/
/***************************************************************************/
/*                              potions                                    */
/***************************************************************************/
/***************************************************************************/

                             /* POTIONS: */
#define P_SLEEP  (0)
#define P_HEALG  (1)
#define P_R_LVL  (2)
#define P_INC_A  (3)
#define P_WISDM  (4)
#define P_STRNG  (5)
#define P_R_CHA  (6)
#define P_DIZZY  (7)
#define P_LEARN  (8)
#define P_GLD_D  (9)
#define P_MNS_D (10)
#define P_FORGT (11)
#define P_WATER (12)
#define P_BLIND (13)
#define P_CONFN (14)
#define P_HEROS (15)
#define P_STURD (16)
#define P_GIANT (17)
#define P_FIRE_ (18)
#define P_TRS_F (19)
#define P_INSTH (20)
#define P_CURED (21)
#define P_POISN (22)
#define P_SEE_I (23)



#define LOGNAMESIZE 40			/* max size of the players name */


/***************************************************************************/
/***************************************************************************/
/*                indices of special records                               */
/***************************************************************************/
/***************************************************************************/

// these folks are in iLarnDB.pdb, which will no longer lookup by unique-id.
#define DNDSTORE_RECORD 0
#define CANNED_RECORD 1
#define SPHERES_RECORD 21
//
// Unique IDs
//
// The DNDSTORE Record has index 0 and unique id 1.
/*  #define REC_DNDSTORE   1 */
//
// There are 20 canned maps.  each gets one record.  uids 10 thru 29.
#define REC_CANNED    10
// These constants are used in FixDB to build the canned maps.
#define CAN_LEV 20 // 20 canned maps
#define CAN_ROW 15 // they have 15 rows
#define CAN_COL 33 // they have 33 columns
//
// 1 record used to clone the character on request
#define REC_SKETCH    50
//
#define REC_SCORE     86
#define REC_WINSCORE  87
#define REC_GENOCIDED 88
#define REC_SPHERES   89
#define OLD_REC_SCORE     90
#define OLD_REC_WINSCORE  91
#define REC_ENV       93
#define REC_STEALTH   94
#define REC_BEENHERE  95
#define REC_SPELKNOW  96
#define REC_IVEN      97
#define REC_IVENARG   98
#define REC_CDUDE     99
// Level-save records start at 100.
// sizes of these records, if you're curious:
#define MAXSPHERE 32
// sz score, winscore = ?
#define SCORESIZE 25
#define NAMELEN 20 // COPIED to iLarnRsc.h where form can see it too
// sz env = sizeof(struct everything)
// sz stealth = MAXX*MAXY Boolean
// sz beenhere = MAXL.+MAXVL. Boolean
// sz spelknow = SPNUM Boolean
// sz iven = NINVT Char
// sz ivenarg = NINVT Short
// sz cdude = 100 Long
#define NLEVELS (MAXLEVEL + MAXVLEVEL)
// For each level 'k' that is a saved level, the uids of its records are
//     100 + k*10 + x      where x = { 0 ... 8 }.
// e.g. level 2 is saved in records with uid 120-128 (nothing uses 129.)
// 
// The last set is 100 + NLEVELS*10 ..  modulo fenceposts .. it's in the 300s.



/***************************************************************************/
/***************************************************************************/
/*                              stores                                     */
/***************************************************************************/
/***************************************************************************/

#define STORE_DND 0
#define STORE_COLLEGE 1
#define STORE_BANK 2
#define STORE_TPOST 3
#define STORE_LRS 4 
#define STORE_HOME 5 
// for dropping money
#define STORE_ALTAR 6 
#define STORE_PAD 7 
#define STORE_NONE -1

// How to decide what to be in the cast form:

#define CAST_NONE     0
// cast a spell
#define CAST_SPELL    1
// get a spell from the genie
#define CAST_GENIE    2
// display known items
#define CAST_DISPLAY  3
// select a monster
#define CAST_GENOCIDE 4


/***************************************************************************/
/***************************************************************************/
/*      Actions that are assignable to hardware buttons                    */
/***************************************************************************/
/***************************************************************************/

// pg-u, pg-d, calc, find
// date, addr, todo, memo
#define HWB_NOOP   0
// n, e, s, w order is important: x*2-1 = direction as above
#define HWB_S      1
#define HWB_E      2
#define HWB_N      3
#define HWB_W      4
#define HWB_CAST   5
#define HWB_REST   6
#define HWB_MAP    7
#define HWB_FONT   8
#define HWB_KNOWN  9
#define HWB_TELE   10
#define HWB_INV    11
/* ...that's enough for now, surely. */


//
// These are as seen in ULarn main.c 'parse()'.
// directions that are assigned to click_dir.. these match HWB_<dir>!
// 
#define SOUTH 1
#define EAST 2
#define NORTH 3
#define WEST 4
#define NORTHEAST 5
#define NORTHWEST 6
#define SOUTHEAST 7
#define SOUTHWEST 8

#define OWES_TAXES (env->outstanding_taxes>0)

#define MAXGOLD ((Long)0x7FFFFFFF)

#define SAVED_MSGS 10 
#define SAVED_MSG_LEN 70
#define BIG_NUMBER 2048

#endif
