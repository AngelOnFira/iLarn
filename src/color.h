#ifdef I_AM_COLOR

void look_for_memo();// SEC_4; // putting these in sec 4 makes pose angry
IndexedColorType get_color(Char c, Boolean bold); // SEC_4;
IndexedColorType get_dragon_color(Short mon); // SEC_4;

extern Boolean have_read_color;
#define IsColor (have_read_color && my_prefs.color)
extern IndexedColorType black, white;

#define DRAGON_CHAR 'D'

#endif /* I_AM_COLOR */
