#ifndef __ILARN_TYPES_H__
#define __ILARN_TYPES_H__


/*
struct monst {
  Char	name[20];
  Short	level;
  Short	armorclass;
  Short	damage;
  Short	attack;
  Short	defense;
  Short	genocided;
  Short 	intelligence; // used to choose movement
  Short	gold;
  Short	hitpoints;
  ULong experience;
};
*/
struct monst {
  Char	name[20];
  Char	level;
  Short	armorclass;
  Char	damage;
  Char	attack;
  Char	defense;
  //  Char	genocided;
  Char 	intelligence; // used to choose movement
  Short	gold;
  Short	hitpoints;
  ULong experience;
};

struct everything {
  Long *cdude; // lots of character status. timers.
  Long (*iarg)[MAXY]; // arg for the item array.. must be L to allow large gold vals
  Short (*hitp)[MAXY]; // monster hp on level
  Short *ivenarg;	// inventory args for player
  Char (*item)[MAXY]; // objects in maze if any
  Char (*know)[MAXY]; // 1 or 0 if here before; 2 if we know there's floor but not what's on it
  Char (*mitem)[MAXY]; // monster item array
  Char *iven; // inventory for player
  Boolean (*moved)[MAXY]; // monster moved-p array
  Boolean (*stealth)[MAXY]; //  0=sleeping 1=awake monst  
  Boolean *spelknow; // do you know spell i
  Boolean *potion_known; // do you know potion i
  Boolean *scroll_known; // do you know scroll i
  Boolean *beenhere;  //  1 if have been on this level
  Boolean *genocided;  // monsters that don't exist now
  Long gtime;
  Long lasttime; // time since last bank visit used to calc. interest
  Long outstanding_taxes;
  Long damage_taxes;
  Short playerx;
  Short playery; // the room on the present level of the player
  Short lastnum; // number of monster that last hit (maybe killed) the player
  Short lasthx;
  Short lasthy; // location of monster last hit BY player
  Short ko_object; // used by object.c forms
  Char level;	//  cavelevel player is on ~= c[CAVELEVEL]
  Char char_class; // ENUM of your character class
  Boolean dropflag;	//  if true, DON'T prompt "you're on an object"
  Boolean hitflag;	//  was player hit while running
  Boolean pending_splash;	//  queued messages
  //  struct monst *monster;       // NO.
  UChar courses_taken; // this is a bitfield.. 8 bits, 8 courses, eh.
  UChar warez_taken; // this is a bitfield.. 5 bits, 5 warez, eh.
};

// Ok, I need to add monstlevel and monster




// Item for DnD Store:

// Ok basically 8 bytes * 92 =  will fit in 1 record.
struct _itm
{
  Short   price;
  //  Char    **mem; // this was used ONLY to make potions/scrolls 'KNOWN'
  // these were Char
  Short    obj;
  Short    arg;
  Short    qty;
};

// sphere of annihilation // let's make them an array, shall we?
/* the structure for maintaining & moving the spheres of annihilation */
struct sphere
{
  // struct sphere *p;       /* pointer to next structure */
  Char x,y,lev;           /* location of the sphere */
  Char dir;               /* direction the sphere is going in */
  Char lifetime;          /* duration of this sphere */
};
//struct sphere *spheres=0; /*pointer to linked list for spheres of annihilation*/

// monster.c  newsphere, rmsphere, sphboom
// speldamage
// movem.c  move spehre

/* whats AbtShowAbout ? */
struct LarnPreferenceType_s {
  /* ... */
  Short run_walk_border;
  Short walk_center_border;
  Boolean gender_male;
  Boolean run_on;
  Boolean stay_centered;
  Boolean sound_on;
  Boolean use_hardware;
  Boolean font_large;
  Boolean L18;
  Boolean use_keyboard;
  Boolean rogue_relative;
  Boolean coward_on;
  Short hardware[8];
  Char name[NAMELEN]; // NAMELEN == 20 so alignment should be a-ok...
  Boolean color; // new in whatever version this is.  0.24.
  Boolean black_bg; // new in 0.24.
};
typedef struct LarnPreferenceType_s LarnPreferenceType;


// I will be EEEVIL and waste some space so that I can use the same
// struct for Winner and Non-winner scoreboards.
// Originally scoreformat used:
// score, suid, hardlev, cod, level, is_mail, char_class_who
// Originally *win*scoreformat used:
// score, suid, hardlev, timeused, taxes, dmg, bank, char_class_who
typedef struct scoreformat /*This is the structure for the scoreboard 	*/
{
  Long  score;		/* the score of the player 	*/
  Long	timeused;	/* the time used in mobuls to win the game*/
  Long	taxes;		/* taxes he owes to LRS 	*/
  Long  dmg;            /* "taxes" due to inflicted damages */
  Long  bank;           /* money deposited in the bank */
  ULong date;           /* date (in seconds) of this entry */
  ULong birthdate;      /* date (in seconds) the Character was created */
  Short suid;		/* the user id number of the PLAYER (Palm-owner) */
  Short hardlev;	/* the level of difficulty player played at*/
  Short cod;		/* the cause of death */
  Short level;		/* the level player was on when he died */
  Short char_class;     /* the character class */
  Boolean is_male;      /* 0 == female */
  Char  who[NAMELEN];	/* the name of the CHARACTER */
} scf;

typedef struct old_scoreformat
{
  Long  score;		/* the score of the player 	*/
  Long	timeused;	/* the time used in mobuls to win the game*/
  Long	taxes;		/* taxes he owes to LRS 	*/
  Long  dmg;            /* "taxes" due to inflicted damages */
  Long  bank;           /* money deposited in the bank */
  ULong date;           /* date (in seconds) of this entry */
  Short suid;		/* the user id number of the PLAYER (Palm-owner) */
  Short hardlev;	/* the level of difficulty player played at*/
  Short cod;		/* the cause of death */
  Short level;		/* the level player was on when he died */
  Short char_class;     /* the character class */
  Boolean is_male;      /* 0 == female */
  Char  who[NAMELEN];	/* the name of the CHARACTER */
} scf_vx01;

typedef struct sketch_misc_s
{
  Long  gtime;
  Long	taxes;
  Long  dmg;
  Char  char_class;
  UChar courses_taken;
  UChar warez_taken;
  Char  who[NAMELEN];	/* the name of the CHARACTER */
} sketch_misc;


#endif
