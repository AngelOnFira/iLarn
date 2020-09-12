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

#define MainForm              1000
#define ItsyFont              3456
#define MainFormMenu          1010
#define menu_mainMap          1011
#define menu_mainSettings     1012
#define menu_mainButtons      1013
#define menu_mainMoveInstruct 1014
#define menu_mainAbout        1015
#define menu_mainGraffiti     1016
#define menu_mainWiz     1017
#define menu_main_test 1018
#define menu_mainMsgs 1019
#define menu_mainScores 1020
#define menu_mainSketch 1021
#define menu_mainCenter 1022
#define menu_mainBeam   1023

#define IntroStr 1030
#define AboutStr 1031
#define QuitP    1032

#define MapForm 1033
#define SplashForm   1034
#define CloneP       1035
#define CloneStr     1036
#define NoCloneP     1037
#define CreditStr   1038
#define BeamNextP   1039

#define QuestionForm 1100
#define btn_qst_1    1101
#define btn_qst_2    1102
#define btn_qst_3    1103
#define btn_qst_ign  1104

#define InvSelectForm 1200
#define list_i        1201
// do NOT change these three numbers
#define btn_if_frob  1202
#define btn_if_drop  1203
#define btn_if_wield 1204
//#define btn_i_ok      1205
#define btn_i_cancel  1206

#define CastForm 1210
#define list_ca 1211
#define btn_ca_ok 1212
#define btn_ca_cancel 1213

//#define DirectionForm 1300
//#define btn_dir_here 1310
//#define btn_dir_s 1311
//#define btn_dir_e 1312
//#define btn_dir_n 1313
//#define btn_dir_w 1314
//#define btn_dir_ne 1315
//#define btn_dir_nw 1316
//#define btn_dir_se 1317
//#define btn_dir_sw 1318
//#define btn_dir_cancel 1319

#define MsgLogForm      1320
#define field_ml        1321
#define repeat_ml_up    1322
#define repeat_ml_down  1323
#define btn_ml_ok       1324

#define StoreForm 1400
#define btn_st_exit 1401
#define btn_st_sell 1402
#define list_st 1403
#define btn_st_prev 1404
#define btn_st_next 1405
#define btn_st_trans 1406

#define TransForm 1500
#define btn_trans_dp 1501
#define btn_trans_wd 1502
#define btn_trans_cancel 1503
#define fld_trans_amt 1504
#define btn_trans_alld 1505
#define btn_trans_allw 1506

#define LrsForm 1510
#define btn_lrs_dp 1511
#define btn_lrs_wd 1512
#define btn_lrs_cancel 1513
#define fld_lrs_amt 1514
#define EvaderForm 1515
#define btn_eva_exit 1516

#define PickChForm 2000
#define PickChStr 2002
#define pbtn_pickch_0 2100
#define pbtn_pickch_1 2101
#define pbtn_pickch_2 2102
#define pbtn_pickch_3 2103
#define pbtn_pickch_4 2104
#define pbtn_pickch_5 2105
#define pbtn_pickch_6 2106
#define pbtn_pickch_7 2107
#define pbtn_pickch_m 2108
#define pbtn_pickch_f 2109
// skip 8
#define btn_pickch_ok 2111
#define popup_pickch  2112
#define list_pickch   2113
#define btn_pickch_dif 2114
#define btn_pickch_gen 2115
#define btn_pickch_snap 2116

#define AboutForm            3020
#define btn_about_ok         3021
#define bitmap_iLarn         3022
#define bitmap_cat           3023
#define btn_about_more       3024
#define btn_about_credits    3025


#define MAGIC_MENU_NUMBER 4000

#define menu_cmd_gold    4036
#define menu_cmd_rest    4046
#define menu_cmd_D       4068
#define menu_cmd_F       4070
#define menu_cmd_P       4080
#define menu_cmd_quit    4081
#define menu_cmd_T       4084
#define menu_cmd_Z       4090
#define menu_cmd_trap    4094
#define menu_cmd_c       4099
#define menu_cmd_d       4100
#define menu_cmd_g       4103
#define menu_cmd_i       4105
#define menu_cmd_s       4115
#define menu_cmd_t       4116
#define menu_cmd_w       4119


#define PrefsForm         5050
#define btn_bul_ok        5051
#define btn_bul_cancel    5052
#define list_bul_1        5053
#define list_bul_2        5055
#define btn_bul_draw      5057
#define btn_bul_clear     5058
#define check_bul_1       5059
#define check_bul_2       5060
#define field_usr         5061
#define btn_bul_save      5062
#define check_bul_3       5063
#define check_bul_4       5064
#define btn_bul_hw        5065
#define check_bul_5       5066
#define check_bul_6       5067
#define check_bul_7       5068
#define check_bul_8       5069
#define check_bul_9       5070

#define check_hwb_keyb   5078
#define check_hwb        5079
#define HwButtonsForm    5080
#define btn_hwb_ok       5081
#define btn_hwb_cancel   5082
#define popup_hwb_1      5083
#define popup_hwb_2      5084
#define popup_hwb_3      5085
#define popup_hwb_4      5086
#define popup_hwb_5      5087
#define popup_hwb_6      5088
#define popup_hwb_7      5089
#define popup_hwb_8      5090
#define list_hwb_1       5091
#define list_hwb_2       5092
#define list_hwb_3       5093
#define list_hwb_4       5094
#define list_hwb_5       5095
#define list_hwb_6       5096
#define list_hwb_7       5097
#define list_hwb_8       5098



#define PadForm    5100
#define list_pad   5101
#define btn_pad_buy 5102
#define btn_pad_split 5103

#define ScoreForm         5200
#define pbtn_score_all    5208
#define pbtn_score_win    5209
#define btn_score_done    5210
#define field_score       5211
#define repeat_score_up   5212
#define repeat_score_down 5213
#define SadForm           5199
#define FontAlert         5198

#define ButtonsStr    5300

#define menu_main_wiz1 5301
#define menu_main_wiz2 5302
#define menu_main_wiz3 5303
#define menu_main_wiz4 5304

#define NAMELEN 20
