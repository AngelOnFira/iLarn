#ifndef __ILARN_PROTOTYPES_H__
#define __ILARN_PROTOTYPES_H__
#include "sections.h"


/*  alloc_env.c  L */
void free_env(struct everything *env) SEC_5;
void alloc_env(struct everything *env) SEC_5;
void clear_env(struct everything *env) SEC_5;

/*  cast.c  1 */
void draw_cast_slices() SEC_1;
Boolean isconfuse() SEC_1;
void genmonst(Short j) SEC_1;
void makewall(Short x, Short y) SEC_1;
void tdirect(Short x, Short y) SEC_1;
void omnidirect(Short spnum, Short dam, Char *str) SEC_1;
void dirpoly(Short x, Short y) SEC_1;
// static Boolean nospell(Short x, Short monst) SEC_1;
void direct(Short spnum, Short dam, Char *str, Short arg) SEC_1;
void direct_helper(Short x, Short y) SEC_1;
void godirect(Short spnum, Short dam, Char *str, Short delay,Char cshow) SEC_1;
void godirect_helper(Short dx, Short dy) SEC_1;
void cast(Short i) SEC_1;
void speldamage(Short x) SEC_1;
void dir_task_dispatch(Short click_dir) SEC_1;


/*  create.c  2 */
void makeplayer(Short selected_class) SEC_2;
void newcavelevel(Short x); //  SEC_2; // JUST FOR DEBUGGING.
// static Boolean cannedlevel(Short k, struct everything * env) SEC_2;
// static void makemaze(Short k, struct everything *env) SEC_2;
// static Boolean eat_this_way_p(Short x, Short y, Short dir,
//                               struct everything *env) SEC_2;
void eat(Short x0, Short y0) SEC_2;
// static void makeobject(Short j, struct everything *env) SEC_2;
// static void fillmroom(Short n, Char what, Short arg,
//                       struct everything *env) SEC_2;
// static void froom(Short n,Char itm,Short arg, struct everything *env) SEC_2;
// static void fillroom(Char what, Short arg, struct everything *env) SEC_2;
// static void treasureroom(Short lv, struct everything *env) SEC_2;
// static void troom(Short lv, Short xsize, Short ysize,
// 		  Short tx, Short ty, Short glyph,
// 		  struct everything *env) SEC_2;
Boolean fillmonst(Char what) SEC_2;
void sethp(Boolean flg) SEC_2;
// static void checkgen(struct everything *env) SEC_2;


/*  data.c  L */
Short m_hitpoints(Short i) SEC_5;
const Char *m_name(Short i) SEC_5;
Short m_intelligence(Short i) SEC_5;

/* dead.c  1 */
void get_default_username() SEC_1;
Short do_sum(Char *name); // SEC_3;
Short get_suid(); // SEC_3;
void get_taxes() SEC_1;
Boolean set_taxes(Long tax, Long dmg) SEC_4;
Boolean life_protected(Short x) SEC_1;
void newscore(Short cod) SEC_1;
void died(Short cod, Boolean show_scores) SEC_1;
void init_death(FormPtr frm) SEC_3;
void scroll_scores(Boolean up, FormPtr frm) SEC_3;
void which_scores(Boolean winners, FormPtr frm) SEC_3;
void topten_fwdcompatibl() SEC_4;
void topten_move_db();// SEC_4;

/*  display.c  L */
void level0_itsy_fix(Boolean yes) SEC_5;
void am_i_on_level_0(Boolean yes) SEC_5;
Boolean toggle_itsy() SEC_5;
void clear_visible()  SEC_5;
void setscreen(Short col, Short row, Short ch) SEC_5;
Char get_monstnamelist(Short i) SEC_3;
void see_invisible() SEC_5;
void got_larneye() SEC_5;
void unsee_invisible() SEC_5;
void ungot_larneye() SEC_5;
void recalc_screen() SEC_5;
// static void put_char_at(Short row, Short col, Char ch, Boolean bold) SEC_5;
void refresh() SEC_5;
void check_player_position(Short playerx, Short playery, Boolean runng) SEC_5;
void move_visible_window(Short left_x, Short top_y, Boolean centered) SEC_5;
void update_screen_cell(Short j, Short i) SEC_5;
void showcell(Short x, Short y) SEC_5;
void rogue_relativize(Short *x, Short *y, Short px, Short py) SEC_5;

/*  global.c  L */
void raiselevel() SEC_5;
void loselevel() SEC_5;
void raiseexperience(Long x) SEC_5;
void loseexperience(Long x) SEC_5;
void losehp(Short x) SEC_5;
void losemhp(Short x) SEC_5;
void raisehp(Short x) SEC_5;
void raisemhp(Short x) SEC_5;
// static void raisespells(Short x) SEC_5;
void raisemspells(Short x) SEC_5;
// static void losespells(Short x) SEC_5;
void losemspells(Short x) SEC_5;
void positionplayer() SEC_5;
Short makemonst(Short lev) SEC_5;
void recalc() SEC_5;
void creategem() SEC_5;
void adjustcvalues(Short itm, Short arg, Boolean draw) SEC_5;
Boolean has_object(Short what) SEC_5;
Short has_object_at(Short what) SEC_5;
Boolean take(Short itm, Short arg, Boolean noisy) SEC_5;


// ignore iLarnstub.c


/*  inv.c  2 */
// static void you_dont_have(Char x) SEC_2;
void drop_object(Short k) SEC_2;
Boolean takeoff() SEC_2;
// static void make_it_fit(CharPtr buf,Short avail_space,Short maxbuflen)SEC_2;
void free_inventory_select(FormPtr frm) SEC_2;
void init_inventory_select(FormPtr frm, Boolean t) SEC_2;
void remove1_inventory_select(FormPtr frm, Short k) SEC_2;
void specialize_labels(FormPtr frm, Short k, Boolean t) SEC_2;
Boolean deduce_task(Short k, Short btn012) SEC_2;






/*  main.c  (0) */
Boolean init_game();
// static void i_used_too_much_heap();
Boolean is_male();
void start_of_Play_Level_loop();
Short get_click_dir(Short tmp_x, Short tmp_y);
// static Boolean main_help_handle_cmd(Char c);
Boolean Main_Form_HandleEvent(EventPtr e);
void LeaveForm(); //void LeaveForm(Boolean redraw);
// these are in various form_*.c files
Boolean About_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Cast_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Evader_Form_HandleEvent(EventPtr e) SEC_4;
Boolean HwButtons_Form_HandleEvent(EventPtr e) SEC_4;
Boolean InvSelect_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Lrs_Form_HandleEvent(EventPtr e) SEC_4;
//void show_dungeon(struct everything * env);
Boolean Map_Form_HandleEvent(EventPtr e) SEC_4;
Boolean MsgLog_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Pad_Form_HandleEvent(EventPtr e) SEC_4;
Boolean PickCh_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Prefs_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Question_Form_HandleEvent(EventPtr e) SEC_4;
//Boolean Splash_Form_HandleEvent(EventPtr e);
Boolean Score_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Store_Form_HandleEvent(EventPtr e) SEC_4;
Boolean Trans_Form_HandleEvent(EventPtr e) SEC_4;
// static Boolean OpenDatabase(void);
// static Boolean ApplicationHandleEvent(EventPtr e);
void writeLarnPrefs();
// static Word StartApplication(void);
// static void StopApplication(void);
// static void EventLoop(void);
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags);

/*  md_malloc.c  L */
Char * md_malloc(Int n);

/*  monster.c  L */
void heal_monsters() SEC_3;
Boolean cgood(Short x, Short y, Short itm, Short monst) SEC_5;
void createmonster(Short mon) SEC_5;
void createitem(Short it, Long arg) SEC_5;
Short fullhit(Short xx) SEC_5;
void ifblind(Short x, Short y) SEC_5;
Boolean vxy(Short *x, Short *y) SEC_5;
void hitmonster(Short x, Short y, Boolean silent) SEC_5;
Short hitm(Short x, Short y, Short amt) SEC_5;
void clobberm(Short monst) SEC_3;
void hitplayer(Short x, Short y) SEC_5;
// static void dropsomething(Short monst) SEC_5;
void dropgold(Short amount) SEC_5;
void something(Short lev) SEC_5;
Short newobject(Short lev, Short *i) SEC_5;
void checkloss(Short x) SEC_5;
// static Boolean emptyhanded(struct everything *env) SEC_5;
// static Boolean stealsomething(struct everything *env) SEC_5;
// static Boolean spattack(Short attack, Short xx, Short yy,
// 			struct everything *env) SEC_5;
void munge_lastmonst(Char * mname) SEC_5;
Char * read_lastmonst() SEC_5;
void init_doomed_monsters(FormPtr frm) SEC_3;
void fight(Short dir) SEC_3;

/*  movem.c  2 */
void movemonst() SEC_2;
//static void movemt(Short i, Short j) SEC_2;
//static void mmove(Short aa, Short bb, Short cc, Short dd,
//		  struct everything *env) SEC_2;


/*  moveplayer.c  2 */
void unmoveplayer() SEC_2;
void showplayer(Short playerx, Short playery) SEC_2;
Short moveplayer(Short dir) SEC_2;
void a_better_run(Short dir) SEC_2;

/*  nap.c  1 */
void nap(Short x); // SEC_1;

/*  obj.c  L */
Char newpotion() SEC_5;
Char newscroll() SEC_5;
Char newleather(Boolean hardgame) SEC_5;
Char newchain() SEC_5;
Char newplate(Boolean hardgame) SEC_5;
Char newdagger() SEC_5;
Char newsword(Boolean hardgame) SEC_5;
void disappear(Short x, Short y) SEC_5;
Char * get_Of_scroll(Short j, Boolean known) SEC_5;
Char * get_Of_potion(Short j, Boolean known) SEC_5;
const Char * get_objectname(Short j) SEC_5;
void show3(Short index) SEC_5;
void show3_cp(Short index, Char *buf) SEC_5;

/*  object.c  2 */
void id_trap() SEC_1;
Boolean nearbymonst() SEC_5;
void ogold(Short arg) SEC_5;
void setupQuestionForm(FormPtr frm) SEC_5;
Boolean lookforobject(Short auto_pray) SEC_2;
// static void finditem_part1(Short item) SEC_2;
//Boolean dspchQuestionForm(Short button); // SEC_2; // used in form_question.c

/*  object_actions.c  1 */
void ovoldown();// SEC_1;
void ovolup() SEC_1;
void oentrance() SEC_1;
void oopendoor() SEC_1;
void oclosedoor() SEC_1;
void oelevator(Boolean up) SEC_1;
void ostairs(Boolean up) SEC_1;
// static void obottomless(struct everything *env) SEC_1;
void opit() SEC_1;
void show_packweight() SEC_1;
void free_cast_select(FormPtr frm) SEC_1;
void init_known_items(FormPtr frm) SEC_1;
void init_cast_select(FormPtr frm) SEC_1;
Boolean readbook(Short arg) SEC_1;
void obrasslamp() SEC_1;
void obrasslamp_learn(Short i) SEC_3;
void oteleport(Boolean err) SEC_1;
Boolean enchantarmor() SEC_1;
Boolean enchweapon() SEC_1;
void adjtime(Long time, Boolean show_stats) SEC_1;
Boolean read_scroll(Short typ) SEC_1;
void enlighten(Short dyl, Short dyh, Short dxl, Short dxh) SEC_1;
void spill() SEC_1;
void quaffpotion(Short pot) SEC_1; 
void o_draws() SEC_1;

/*  object_actions_cont.c  2 */
void ocookie() SEC_2;
Boolean odesecrate() SEC_2;
Boolean ojustpr() SEC_2;
Boolean omoney(ULong amnt) SEC_2;
// static void ohear(struct everything *env) SEC_2;
Boolean othrone_pry(Boolean two) SEC_2;
Boolean othrone_sit(Boolean two) SEC_2;
void odeadthrone() SEC_2;
void ochest() SEC_2;
void ofountain_drink() SEC_2;
Boolean ofountain_wash() SEC_2;
// static void fntchange(Boolean how) SEC_2;
void drug_speed() SEC_3;
void drug_shrooms() SEC_3;
void drug_acid() SEC_3;
void drug_hash() SEC_3;
void drug_coke() SEC_3;

// ignore old-monster.c


/*  pickchar.c  L */
//Char *class_name(Short i);
void pick_char(Short i) SEC_5;

/*  random.c  L */
//void seed_random() SEC_5;
Int rnd(Int y); // SEC_5;
Int rund(Int y); // SEC_5;

/*  regen.c  2 */
void regen(Boolean show_stats) SEC_2;

/*  savelev.c  1 */
void remove_save_records() SEC_1;
void save_all(struct everything *env) SEC_1;
Boolean restore_all(struct everything *env) SEC_1;
Boolean savelevel(struct everything *env); // SEC_3;  // JUST FOR DEBUGGING.
Boolean getlevel(struct everything *env); // SEC_3;  // JUST FOR DEBUGGING.
Boolean save_sketch() SEC_4;
Boolean load_sketch() SEC_4;
void destroy_sketch_if_any() SEC_4;
Boolean sketch_exists() SEC_4;


// ignore start.c

// ignore start2.c

/*  stats.c  L */
Boolean message_clear() SEC_5;
void message(Char *buf) SEC_5;
void alloc_message_log() SEC_5;
//void add_to_splash(Char *msg) SEC_5;
//void add_long_to_splash(Char *msg) SEC_3;
void preempt_messages() SEC_5;
//void print_splash(struct everything *env) SEC_5;
void show_messages() SEC_5; // replaces print_splash
void spell_stats() SEC_5;
void print_stats() SEC_5;

/* store.c  1 */
Short packweight() SEC_1;
Boolean pocketfull() SEC_1;
//void pay_tax(Long amt) SEC_1;
void add_tax(Long amt, Boolean damage) SEC_1;
void print_tax() SEC_1;
void lrs_init_or_refresh(FormPtr frm) SEC_1;
void lrs_pay_cash(FormPtr frm, Long amt) SEC_1;
void lrs_req_trans(FormPtr frm) SEC_1;
Boolean has_potion_cured() SEC_1;
void drop_dough(Long amt) SEC_3;
void evader_init(FormPtr frm, Short in_store) SEC_1;
void store_gold(Long gold) SEC_1;
void bank_gold(Long gold) SEC_1;
void bank_interest() SEC_5;
void bank_init(FormPtr frm) SEC_1;
void bank_init_buttons(FormPtr frm) SEC_1;
void bank_mumble() SEC_1;
void bank_sell(FormPtr frm, Char inv_i, Short list_j) SEC_1;
void bank_trans_init(FormPtr frm, Boolean bank) SEC_1;
Boolean bank_transact(Long amt, Boolean deposit) SEC_1;
void dndstore_buy(FormPtr frm, Short store_item) SEC_1;
void dndstore_restock() SEC_1;
void dndstore_init(FormPtr frm) SEC_1;
void dndstore_list_tens(FormPtr frm, Short plus) SEC_1;
void dndstore_btns(FormPtr frm) SEC_1;
void store_cleanup(FormPtr frm) SEC_1;
void college_init(FormPtr frm) SEC_1;
void college_mumble(Long gold) SEC_1;
// static void college_remark(Char *buf) SEC_1;
void college_enroll(FormPtr frm, Short i) SEC_1;
void tpost_print_value(Short k) SEC_1;
Boolean tpost_sell_item(Short k) SEC_1;
void pad_gold() SEC_3;
void pad_buy(FormPtr frm, Short store_item) SEC_3;
Long tpost_calc_val(Int itm_cost, Short izarg) SEC_2;
Long get_mobuls() SEC_2;
void add_winner_bonus() SEC_2;
Long get_mobuls_elapsed() SEC_1;

/*  util.c  2 */
void do_feep(Long frq, UInt dur); // SEC_2;
void draw_circle(Short x, Short y, Short radius, Boolean b); // SEC_2;
void update_field_scrollers(FormPtr frm, FieldPtr fld,
			    Word up_scroller, Word down_scroller);
void page_scroll_field(FormPtr frm, FieldPtr fld, 
#ifndef I_AM_COLOR
		       DirectionType
#else /* I_AM_COLOR */
		       WinDirectionType
#endif /* I_AM_COLOR */
		       dir); // SEC_3;


// note: sec_1 is pretty much full
/* sphere.c  1 */
void annihilate() SEC_2;
void newsphere(Short x, Short y, Short dir, Short life, Short optional_i) SEC_2;
void rmsphere(Short x, Short y) SEC_2;
void rmsp(Short i, Short x, Short y) SEC_2;
void sphboom(Short x, Short y) SEC_2;
void movsphere() SEC_2;
void clearspheres() SEC_2;


#endif

/*
  POSE gets angry if these are not in the default code section:
   get_suid
   do_sum
   rnd, rund
   do_feep
   nap
  I don't know why.  Sometime not a late night I should try to find out.
*/
