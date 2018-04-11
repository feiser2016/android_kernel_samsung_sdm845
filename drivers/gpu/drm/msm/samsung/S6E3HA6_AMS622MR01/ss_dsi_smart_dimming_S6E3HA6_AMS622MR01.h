/*
 * =================================================================
 *
 *       Filename:  smart_mtp_s6e3.h
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_DSI_SMART_DIMMING_S6E3HA6_AMS622MR01_H_
#define _SS_DSI_SMART_DIMMING_S6E3HA6_AMS622MR01_H_

#include "ss_dsi_panel_common.h"
#include "ss_dsi_smart_dimming_common.h"
#include "ss_dsi_panel_S6E3HA6_AMS622MR01.h"

static int id1, id2, id3;

#define LUMINANCE_MAX 74
#define HMT_LUMINANCE_MAX 37
#define GAMMA_SET_MAX 35
#define GRAY_SCALE_MAX 256
#define RGB_COMPENSATION 27

/* BIT_SHIFT is used for right bit shfit */
#define BIT_SHIFT 22
#define BIT_SHFIT_MUL 4194304 // pow(2,BIT_SHIFT)

#define VREG0_REF_6P8 28521267	/* 6.8*4194304 */
#define VREG0_REF_6P5 27262976	/* 6.5*4194304 */
#define VREG0_REF_6P4 26843546	/* 6.4*4194304 */
#define VREG0_REF_6P3 26424115	/* 6.3*4194304 */
#define VREG0_REF_6P2 26004685	/* 6.2*4194304 */

enum {
	GAMMA_CURVE_1P9 = 0,
	GAMMA_CURVE_2P15,
	GAMMA_CURVE_2P2,
};

/* TP */
enum {
	VT = 0,
	V1 = 1,
	V7 = 2,
	V11 = 3,
	V23 = 4,
	V35 = 5,
	V51 = 6,
	V87 = 7,
	V151 = 8,
	V203 = 9,
	V255 = 10,
	V_MAX,
};

/* RGB */
enum {
	R = 0,
	G = 1,
	B = 2,
	RGB_MAX,
};

static char V_LIST[V_MAX][5] = {
	"VT",
	"V1",
	"V7",
	"V11",
	"V23",
	"V35",
	"V51",
	"V87",
	"V151",
	"V203",
	"V255",
};

struct illuminance_table {
	int lux;
	char gamma_setting[GAMMA_SET_MAX];
} __packed;

struct SMART_DIM {
	/* read C8h 1st ~ 35th */
	char MTP_ORIGN[GAMMA_SET_MAX];

	/* copy MTP_ORIGN -> MTP */
	int MTP[V_MAX][RGB_MAX];

	/* TP's gamma voltage */
	int RGB_OUTPUT[V_MAX][RGB_MAX];

	/* GRAY (0~255) */
	int GRAY[GRAY_SCALE_MAX][RGB_MAX];

	/* Because of AID funtion, below members are added*/
	int lux_table_max;
	int *plux_table;
	struct illuminance_table gen_table[LUMINANCE_MAX];

	int brightness_level;
	int ldi_revision;
	int vregout_voltage;
	char panel_revision;

	/* HBM interpolation */
	struct illuminance_table hbm_interpolation_table[HBM_INTERPOLATION_STEP+1];
	char *hbm_payload;
	int hbm_brightness_level;

	struct illuminance_table hmt_gen_table[HMT_LUMINANCE_MAX];
} __packed;

/* V0,V1,V7,V11,V23,V35,V51,V87,V151,V203,V255 */
static int INFLECTION_VOLTAGE_ARRAY[V_MAX] = {0, 1, 7, 11, 23, 35, 51, 87, 151, 203, 255};

static int vt_coefficient[] = {
	0,  12,  24,  36,
	48,  60,  72,  84,
	96, 108, 138, 148,
	158, 168, 178, 186,
};

// center (max) gamma value (Hex)
static int center_gamma[V_MAX][RGB_MAX] = {
	{0x0, 0x0, 0x0},		/* VT */
	{0x80, 0x80, 0x80},		/* V1 */
	{0x80, 0x80, 0x80},		/* V7 */
	{0x80, 0x80, 0x80},		/* V11 */
	{0x80, 0x80, 0x80},		/* V23 */
	{0x80, 0x80, 0x80},		/* V35 */
	{0x80, 0x80, 0x80},		/* V51 */
	{0x80, 0x80, 0x80},		/* V87 */
	{0x80, 0x80, 0x80},		/* V151 */
	{0x80, 0x80, 0x80},		/* V203 */
	{0x100, 0x100, 0x100},	/* V255 */
};

/* fraction for gamma code */
static int fraction[V_MAX][2] = {
/* {numerator, denominator} */
	{0,  860},		/* VT */
	{0,  256},		/* V1 */
	{64, 320},		/* V7 */
	{64, 320},		/* V11 */
	{64, 320},		/* V23 */
	{64, 320},		/* V35 */
	{64, 320},		/* V51 */
	{64, 320},		/* V87 */
	{64, 320},		/* V151 */
	{64, 320},		/* V203 */
	{129, 860},		/* V255 */
};

/* 443 is made but not used */
static int hbm_interpolation_candela_table[HBM_INTERPOLATION_STEP] = {443, 465, 488, 510, 533, 555, 578, 600};

/* DREAM2 rev.A ~ H */
static unsigned int base_luminance_dream_revA[LUMINANCE_MAX][2] = {
	{2,		109},
	{3,		109},
	{4,		109},
	{5,		109},
	{6,		109},
	{7,		109},
	{8,		109},
	{9,		109},
	{10,	109},
	{11,	109},
	{12,	109},
	{13,	109},
	{14,	109},
	{15,	109},
	{16,	109},
	{17,	109},
	{19,	109},
	{20,	109},
	{21,	109},
	{22,	109},
	{24,	109},
	{25,	109},
	{27,	109},
	{29,	109},
	{30,	109},
	{32,	109},
	{34,	109},
	{37,	109},
	{39,	109},
	{41,	109},
	{44,	109},
	{47,	109},
	{50,	109},
	{53,	109},
	{56,	109},
	{60,	112},
	{64,	119},
	{68,	125},
	{72,	133},
	{77,	142},
	{82,	151},
	{87,	161},
	{93,	170},
	{98,	183},
	{105,	193},
	{111,	205},
	{119,	217},
	{126,	230},
	{134,	241},
	{143,	254},
	{152,	265},
	{162,	280},
	{172,	294},
	{183,	300},
	{195,	300},
	{207,	300},
	{220,	300},
	{234,	300},
	{249,	304},
	{265,	319},
	{282,	335},
	{300,	348},
	{316,	364},
	{333,	381},
	{350,	392},
	{357,	402},
	{365,	406},
	{372,	406},
	{380,	406},
	{387,	406},
	{395,	406},
	{403,	409},
	{412,	416},
	{420,	420},
};

static int gradation_offset_dream_revA[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	4,	5,	11,	14,	17,	21,	24,	25},
	{0,	4,	5,	11,	13,	15,	19,	23,	24},
	{0,	4,	5,	9,	10,	13,	16,	21,	22},
	{0,	4,	3,	7,	8,	11,	14,	17,	19},
	{0,	4,	3,	7,	8,	11,	14,	17,	19},
	{0,	4,	3,	6,	7,	10,	12,	16,	18},
	{0,	4,	3,	5,	6,	9,	11,	15,	17},
	{0,	4,	3,	5,	6,	8,	11,	15,	16},
	{0,	4,	3,	5,	6,	8,	11,	15,	16},
	{0,	4,	3,	5,	6,	8,	11,	15,	16},
	{0,	4,	3,	5,	6,	8,	11,	15,	16},
	{0,	4,	3,	5,	6,	9,	11,	15,	16},
	{0,	4,	3,	5,	7,	9,	11,	15,	16},
	{0,	4,	3,	5,	7,	9,	11,	15,	16},
	{0,	4,	3,	5,	7,	9,	11,	15,	16},
	{0,	4,	3,	5,	7,	9,	11,	15,	16},
	{0,	2,	2,	4,	5,	8,	10,	13,	15},
	{0,	2,	1,	3,	5,	7,	9,	13,	15},
	{0,	2,	2,	4,	5,	7,	9,	13,	14},
	{0,	2,	2,	4,	5,	6,	9,	12,	13},
	{0,	2,	2,	3,	4,	6,	8,	11,	12},
	{0,	2,	2,	3,	4,	6,	8,	11,	12},
	{0,	2,	2,	3,	4,	5,	7,	10,	11},
	{0,	2,	2,	3,	4,	5,	7,	10,	11},
	{0,	2,	1,	2,	4,	4,	6,	10,	11},
	{0,	2,	1,	2,	3,	4,	6,	9,	10},
	{0,	2,	1,	2,	3,	4,	6,	9,	10},
	{0,	2,	1,	2,	2,	3,	5,	8,	9},
	{0,	2,	1,	2,	2,	3,	5,	8,	9},
	{0,	1,	0,	1,	1,	2,	4,	7,	7},
	{0,	1,	0,	1,	1,	2,	4,	7,	7},
	{0,	1,	0,	1,	1,	2,	4,	6,	7},
	{0,	1,	0,	1,	1,	2,	3,	6,	6},
	{0,	1,	0,	1,	1,	2,	3,	5,	6},
	{0,	1,	0,	1,	1,	2,	3,	5,	6},
	{0,	1,	0,	1,	1,	2,	3,	5,	6},
	{0,	1,	0,	1,	1,	1,	2,	4,	6},
	{0,	1,	1,	1,	1,	1,	3,	4,	6},
	{0,	1,	1,	1,	1,	1,	3,	4,	6},
	{0,	1,	1,	1,	1,	2,	2,	4,	5},
	{0,	1,	0,	1,	1,	1,	2,	4,	5},
	{0,	2,	1,	1,	1,	1,	2,	4,	5},
	{0,	2,	3,	2,	1,	2,	2,	4,	5},
	{0,	2,	3,	3,	3,	3,	3,	4,	6},
	{0,	1,	4,	3,	2,	2,	2,	5,	6},
	{0,	1,	4,	2,	2,	2,	2,	4,	6},
	{0,	1,	4,	2,	2,	2,	3,	4,	5},
	{0,	1,	3,	2,	1,	2,	3,	4,	5},
	{0,	1,	3,	2,	1,	2,	2,	4,	5},
	{0,	1,	2,	2,	1,	2,	2,	4,	5},
	{0,	2,	5,	3,	2,	2,	2,	4,	5},
	{0,	2,	5,	3,	3,	2,	2,	4,	5},
	{0,	2,	5,	3,	3,	2,	1,	4,	5},
	{0,	3,	5,	4,	3,	2,	2,	3,	3},
	{0,	1,	4,	1,	2,	2,	1,	3,	3},
	{0,	1,	3,	1,	2,	2,	1,	2,	2},
	{0,	1,	3,	1,	1,	1,	1,	2,	2},
	{0,	0,	2,	0,	1,	1,	0,	1,	2},
	{0,	0,	2,	0,	0,	0,	0,	1,	2},
	{0,	1,	2,	0,	0,	0,	0,	1,	1},
	{0,	1,	2,	0,	0,	0,	0,	0,	1},
	{0,	1,	2,	0,	0,	-1,	0,	0,	1},
	{0,	1,	2,	0,	0,	0,	0,	0,	1},
	{0,	1,	1,	1,	0,	0,	0,	0,	1},
	{0,	1,	1,	0,	0,	-1,	-1,	0,	0},
	{0,	1,	1,	-1,	-1,	-1,	-1,	-1,	0},
	{0,	0,	1,	-1,	-1,	0,	-1,	-1,	0},
	{0,	0,	1,	-1,	-1,	0,	-1,	-1,	0},
	{0,	0,	1,	-1,	-1,	0,	0,	-1,	0},
	{0,	0,	0,	-1,	-1,	0,	0,	-1,	0},
	{0,	0,	0,	-1,	-1,	0,	0,	-1,	0},
	{0,	0,	0,	-1,	-1,	0,	0,	0,	1},
	{0,	0,	0,	-1,	-1,	0,	0,	0,	0},
	{0,	0,	0,	0,	0,	0,	0,	0,	0},
};

static int gradation_offset_dream_revS[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0, 7, 8, 14, 16, 19, 22, 24, 14},
	{0, 7, 7, 12, 13, 15, 19, 21, 14},
	{0, 5, 5,  8, 10, 12, 15, 17, 14},
	{0, 5, 4,  7,  8, 10, 12, 15, 14},
	{0, 5, 3,  6,  7,  9, 11, 14, 14},
	{0, 5, 3,  6,  6,  8, 10, 13, 13},
	{0, 5, 4,  6,  6,  8,  9, 12, 13},
	{0, 5, 4,  6,  7,  8,  9, 12, 13},
	{0, 4, 4,  6,  7,  8,  9, 12, 13},
	{0, 4, 4,  6,  7,  8,  9, 12, 13},
	{0, 4, 4,  7,  7,  8,  9, 12, 13},
	{0, 4, 4,  7,  8,  9, 10, 13, 14},
	{0, 4, 4,  7,  8, 10, 11, 14, 15},
	{0, 4, 4,  7,  8, 10, 11, 14, 15},
	{0, 4, 3,  6,  7,  9, 10, 14, 14},
	{0, 4, 3,  5,  6,  8, 10, 13, 14},
	{0, 4, 3,  5,  6,  8,  9, 12, 13},
	{0, 4, 3,  5,  6,  7,  9, 12, 13},
	{0, 2, 2,  4,  5,  7,  9, 13, 14},
	{0, 2, 2,  4,  5,  6,  9, 12, 13},
	{0, 2, 2,  3,  4,  6,  8, 11, 12},
	{0, 2, 2,  3,  4,  6,  8, 11, 12},
	{0, 2, 2,  3,  4,  5,  7, 10, 11},
	{0, 2, 2,  3,  4,  5,  7, 10, 11},
	{0, 2, 1,  2,  4,  4,  6, 10, 11},
	{0, 2, 1,  2,  3,  4,  6,  9, 10},
	{0, 2, 1,  2,  3,  4,  6,  9, 10},
	{0, 2, 1,  2,  2,  3,  5,  8,  9},
	{0, 2, 1,  2,  2,  3,  5,  8,  9},
	{0, 1, 0,  1,  1,  2,  4,  7,  7},
	{0, 1, 0,  1,  1,  2,  4,  7,  7},
	{0, 1, 0,  1,  1,  2,  4,  6,  7},
	{0, 1, 0,  1,  1,  2,  3,  6,  6},
	{0, 1, 0,  1,  1,  2,  3,  5,  6},
	{0, 1, 0,  1,  1,  2,  3,  5,  6},
	{0, 1, 0,  1,  1,  2,  3,  5,  6},
	{0, 1, 0,  1,  1,  1,  2,  4,  6},
	{0, 1, 1,  1,  1,  1,  3,  4,  6},
	{0, 1, 1,  1,  1,  1,  3,  4,  6},
	{0, 1, 1,  1,  1,  2,  2,  4,  5},
	{0, 1, 0,  1,  1,  1,  2,  4,  5},
	{0, 2, 1,  1,  1,  1,  2,  4,  5},
	{0, 2, 3,  2,  1,  2,  2,  4,  5},
	{0, 2, 3,  3,  3,  3,  3,  4,  6},
	{0, 1, 4,  3,  2,  2,  2,  5,  6},
	{0, 1, 4,  2,  2,  2,  2,  4,  6},
	{0, 1, 4,  2,  2,  2,  3,  4,  5},
	{0, 1, 3,  2,  1,  2,  3,  4,  5},
	{0, 1, 3,  2,  1,  2,  2,  4,  5},
	{0, 1, 2,  2,  1,  2,  2,  4,  5},
	{0, 2, 5,  3,  2,  2,  2,  4,  5},
	{0, 2, 5,  3,  3,  2,  2,  4,  5},
	{0, 2, 5,  3,  3,  2,  1,  4,  5},
	{0, 3, 5,  4,  3,  2,  2,  3,  3},
	{0, 1, 4,  1,  2,  2,  1,  3,  3},
	{0, 1, 3,  1,  2,  2,  1,  2,  2},
	{0, 1, 3,  1,  1,  1,  1,  2,  2},
	{0, 0, 2,  0,  1,  1,  0,  1,  2},
	{0, 0, 2,  0,  0,  0,  0,  1,  2},
	{0, 1, 2,  0,  0,  0,  0,  1,  1},
	{0, 1, 2,  0,  0,  0,  0,  0,  1},
	{0, 1, 2,  0,  0, -1,  0,  0,  1},
	{0, 1, 2,  0,  0,  0,  0,  0,  1},
	{0, 1, 1,  1,  0,  0,  0,  0,  1},
	{0, 1, 1,  0,  0, -1, -1,  0,  0},
	{0, 1, 1, -1, -1, -1, -1, -1,  0},
	{0, 0, 1, -1, -1,  0, -1, -1,  0},
	{0, 0, 1, -1, -1,  0, -1, -1,  0},
	{0, 0, 1, -1, -1,  0,  0, -1,  0},
	{0, 0, 0, -1, -1,  0,  0, -1,  0},
	{0, 0, 0, -1, -1,  0,  0, -1,  0},
	{0, 0, 0, -1, -1,  0,  0,  0,  1},
	{0, 0, 0, -1, -1,  0,  0,  0,  0},
	{0, 0, 0,  0,  0,  0,  0,  0,  0},
};

static int rgb_offset_dream_revA[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-3,	0,	-2,	-2,	0,	-1,	-2,	0,	-1,	-1,	0,	-3,	-4,	0,	-5,	-4,	0,	-5,	-3,	0,	-6,	-12,	0,	-5,	-6,	0,	-1},
	{-2,	0,	-2,	-1,	0,	-1,	-2,	0,	0,	-1,	0,	-3,	-3,	0,	-6,	-4,	0,	-6,	-4,	0,	-5,	-11,	0,	-7,	-6,	0,	-2},
	{-2,	0,	-2,	0,	0,	0,	-2,	0,	-1,	-2,	0,	-3,	-2,	0,	-5,	-4,	0,	-6,	-4,	0,	-7,	-9,	0,	-6,	-4,	0,	-2},
	{-1,	0,	-1,	-1,	0,	0,	-2,	0,	-1,	-1,	0,	-2,	-2,	0,	-7,	-3,	0,	-5,	-3,	0,	-5,	-7,	0,	-8,	-3,	0,	-3},
	{-1,	0,	-1,	-1,	0,	-1,	-1,	0,	1,	-2,	0,	-2,	-2,	0,	-7,	-3,	0,	-6,	-4,	0,	-6,	-6,	0,	-10,	-3,	0,	-2},
	{0,	0,	-1,	-2,	0,	-1,	0,	0,	1,	-3,	0,	-2,	-2,	0,	-6,	-2,	0,	-4,	-7,	0,	-10,	-7,	0,	-9,	-3,	0,	-2},
	{0,	0,	-1,	-2,	0,	-1,	0,	0,	1,	-1,	0,	-1,	-3,	0,	-6,	-2,	0,	-5,	-7,	0,	-9,	-7,	0,	-9,	-2,	0,	-1},
	{0,	0,	-1,	-1,	0,	0,	-1,	0,	0,	0,	0,	-1,	-3,	0,	-5,	-3,	0,	-7,	-3,	0,	-6,	-5,	0,	-9,	-2,	0,	-2},
	{0,	0,	-1,	-1,	0,	0,	-1,	0,	0,	0,	0,	-1,	-2,	0,	-5,	-3,	0,	-7,	-3,	0,	-7,	-5,	0,	-9,	-2,	0,	-2},
	{0,	0,	-1,	-1,	0,	0,	0,	0,	1,	-1,	0,	-2,	-2,	0,	-5,	-2,	0,	-7,	-2,	0,	-6,	-4,	0,	-9,	-1,	0,	-2},
	{1,	0,	0,	-1,	0,	-1,	-1,	0,	1,	0,	0,	-1,	-2,	0,	-5,	-1,	0,	-7,	-2,	0,	-6,	-2,	0,	-7,	0,	0,	-3},
	{1,	0,	0,	-1,	0,	-1,	-1,	0,	1,	0,	0,	-1,	-2,	0,	-6,	0,	0,	-6,	-2,	0,	-8,	0,	0,	-7,	1,	0,	-3},
	{1,	0,	0,	-1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-6,	0,	0,	-7,	-1,	0,	-9,	1,	0,	-8,	1,	0,	-2},
	{1,	0,	0,	-1,	0,	0,	0,	0,	1,	0,	0,	-2,	-2,	0,	-6,	-1,	0,	-9,	1,	0,	-10,	2,	0,	-8,	1,	0,	-2},
	{1,	0,	1,	-1,	0,	0,	0,	0,	0,	0,	0,	-1,	-3,	0,	-6,	1,	0,	-8,	-1,	0,	-9,	2,	0,	-8,	1,	0,	-2},
	{1,	0,	1,	0,	0,	0,	0,	0,	1,	-1,	0,	-2,	-2,	0,	-5,	0,	0,	-8,	-1,	0,	-9,	2,	0,	-8,	1,	0,	-2},
	{1,	0,	1,	0,	0,	1,	-1,	0,	-1,	-2,	0,	-2,	0,	0,	-5,	-1,	0,	-8,	-1,	0,	-9,	2,	0,	-12,	3,	0,	-1},
	{1,	0,	2,	0,	0,	1,	0,	0,	-1,	-1,	0,	-4,	-4,	0,	-5,	-1,	0,	-8,	-5,	0,	-17,	3,	0,	-20,	0,	0,	-3},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	-1,	0,	-2,	-1,	0,	-5,	-1,	0,	-9,	0,	0,	-8,	1,	0,	-8,	1,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	-1,	0,	-2,	-1,	0,	-5,	-1,	0,	-9,	0,	0,	-8,	1,	0,	-12,	1,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	0,	0,	-1,	-2,	0,	-5,	0,	0,	-8,	0,	0,	-10,	1,	0,	-12,	1,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	0,	0,	0,	-1,	-2,	0,	-4,	0,	0,	-7,	0,	0,	-10,	1,	0,	-11,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	0,	0,	-1,	-2,	0,	-4,	0,	0,	-7,	1,	0,	-11,	0,	0,	-14,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	1,	0,	0,	-2,	0,	-4,	-1,	0,	-8,	2,	0,	-11,	0,	0,	-13,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	1,	0,	0,	-2,	0,	-4,	-1,	0,	-8,	2,	0,	-11,	-1,	0,	-10,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	1,	0,	0,	-2,	0,	-4,	-2,	0,	-7,	2,	0,	-10,	0,	0,	-12,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	0,	0,	1,	1,	0,	0,	-1,	0,	-3,	-2,	0,	-6,	2,	0,	-10,	0,	0,	-11,	2,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	0,	0,	-1,	-1,	0,	-2,	-1,	0,	-5,	3,	0,	-11,	0,	0,	-13,	3,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	0,	0,	-1,	-1,	0,	-2,	-1,	0,	-5,	3,	0,	-11,	-1,	0,	-12,	3,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	0,	-2,	0,	-3,	0,	0,	-5,	3,	0,	-11,	-2,	0,	-12,	3,	0,	-9},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	0,	-2,	0,	-3,	1,	0,	-4,	3,	0,	-9,	-2,	0,	-12,	3,	0,	-9},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	0,	-1,	0,	-2,	0,	0,	-4,	3,	0,	-9,	-1,	0,	-14,	3,	0,	-4},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	1,	-1,	0,	-3,	0,	0,	-4,	3,	0,	-10,	-2,	0,	-12,	4,	0,	-8},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	1,	-1,	0,	-3,	0,	0,	-3,	3,	0,	-10,	-2,	0,	-15,	5,	0,	-2},
	{1,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	1,	0,	0,	-2,	0,	0,	-3,	3,	0,	-9,	-3,	0,	-14,	5,	0,	-2},
	{1,	0,	2,	-1,	0,	0,	0,	0,	2,	1,	0,	1,	0,	0,	-2,	0,	0,	-3,	3,	0,	-8,	-3,	0,	-14,	5,	0,	-2},
	{2,	0,	2,	-1,	0,	0,	1,	0,	1,	0,	0,	0,	1,	0,	0,	0,	0,	-4,	3,	0,	-8,	-2,	0,	-13,	4,	0,	-2},
	{2,	0,	2,	-1,	0,	0,	1,	0,	2,	0,	0,	0,	1,	0,	-1,	0,	0,	-4,	4,	0,	-5,	-2,	0,	-13,	4,	0,	-3},
	{2,	0,	2,	-2,	0,	0,	1,	0,	1,	0,	0,	0,	1,	0,	-1,	0,	0,	-3,	4,	0,	-5,	-2,	0,	-13,	3,	0,	-3},
	{2,	0,	2,	-1,	0,	0,	1,	0,	2,	1,	0,	1,	1,	0,	0,	0,	0,	-3,	3,	0,	-6,	-2,	0,	-13,	3,	0,	-3},
	{1,	0,	2,	0,	0,	0,	0,	0,	2,	0,	0,	0,	1,	0,	-1,	0,	0,	-2,	3,	0,	-7,	-2,	0,	-13,	3,	0,	-3},
	{2,	0,	2,	-1,	0,	0,	0,	0,	1,	0,	0,	0,	2,	0,	0,	0,	0,	-2,	3,	0,	-4,	-1,	0,	-10,	4,	0,	-6},
	{1,	0,	2,	-1,	0,	-1,	0,	0,	1,	1,	0,	1,	2,	0,	0,	-1,	0,	-2,	3,	0,	-5,	-1,	0,	-12,	4,	0,	-6},
	{2,	0,	3,	0,	0,	0,	-2,	0,	0,	1,	0,	1,	0,	0,	-1,	1,	0,	-2,	2,	0,	-2,	4,	0,	-12,	2,	0,	-5},
	{2,	0,	3,	0,	0,	1,	-1,	0,	-1,	1,	0,	1,	2,	0,	0,	-1,	0,	-1,	1,	0,	-3,	-2,	0,	-10,	2,	0,	-9},
	{3,	0,	3,	-1,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	-1,	1,	0,	-1,	0,	0,	-3,	-1,	0,	-10,	2,	0,	-9},
	{3,	0,	3,	-1,	0,	0,	-1,	0,	-1,	2,	0,	2,	-1,	0,	-1,	1,	0,	-1,	-1,	0,	-3,	0,	0,	-11,	1,	0,	-10},
	{3,	0,	3,	1,	0,	1,	-1,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	-1,	-1,	0,	-3,	0,	0,	-11,	1,	0,	-9},
	{3,	0,	4,	-1,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	-1,	0,	0,	-3,	0,	0,	-11,	1,	0,	-9},
	{3,	0,	4,	0,	0,	1,	0,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	-1,	0,	0,	-1,	0,	0,	-9,	1,	0,	-8},
	{3,	0,	4,	0,	0,	0,	-1,	0,	0,	2,	0,	1,	0,	0,	0,	0,	0,	-1,	-1,	0,	-2,	-1,	0,	-7,	2,	0,	-11},
	{2,	0,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	1,	1,	0,	1,	-1,	0,	-1,	0,	0,	-2,	-1,	0,	-6,	2,	0,	-10},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	-1,	0,	-1,	0,	0,	-2,	-1,	0,	-6,	2,	0,	-10},
	{2,	0,	3,	-1,	0,	0,	0,	0,	0,	-1,	0,	1,	2,	0,	0,	-1,	0,	-2,	1,	0,	-1,	0,	0,	-9,	1,	0,	-6},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-5,	3,	0,	-7},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-5,	5,	0,	-6},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	1,	1,	0,	0,	0,	0,	-1,	0,	0,	-4,	5,	0,	-4},
	{2,	0,	3,	-1,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	1,	1,	0,	0,	0,	0,	-1,	-1,	0,	-6,	4,	0,	-3},
	{2,	0,	3,	-1,	0,	0,	-1,	0,	0,	2,	0,	1,	-1,	0,	0,	1,	0,	0,	0,	0,	-1,	-1,	0,	-5,	4,	0,	-2},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	2,	0,	2,	-1,	0,	0,	1,	0,	0,	0,	0,	-1,	-1,	0,	-4,	4,	0,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	2,	0,	1,	1,	0,	2,	0,	0,	0,	0,	0,	-1,	1,	0,	-3,	4,	0,	-2},
	{1,	0,	2,	-1,	0,	-1,	0,	0,	0,	1,	0,	1,	0,	0,	1,	2,	0,	0,	1,	0,	-1,	1,	0,	-2,	3,	0,	-2},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	2,	0,	1,	0,	0,	1,	0,	0,	-1,	0,	0,	0,	1,	0,	-2,	3,	0,	0},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	1,	1,	0,	0,	0,	0,	-1,	1,	0,	-1,	4,	0,	1},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	1,	2,	0,	0,	-1,	0,	-1,	1,	0,	0,	5,	0,	1},
	{0,	0,	1,	0,	0,	0,	1,	0,	0,	1,	0,	1,	-1,	0,	0,	1,	0,	0,	0,	0,	0,	1,	0,	0,	5,	0,	1},
	{0,	0,	1,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	1,	0,	1,	2,	0,	1,	5,	0,	3},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	1,	0,	0,	-1,	1,	0,	1,	2,	0,	1,	5,	0,	3},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	1,	0,	0,	-1,	-1,	0,	0,	2,	0,	1,	5,	0,	3},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	1,	0,	0,	-1,	-1,	0,	0,	2,	0,	1,	3,	0,	3},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	0,	1,	0,	0,	-1,	-1,	0,	0,	2,	0,	1,	3,	0,	2},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	1,	0,	0,	-1,	0,	0,	0,	0,	-1,	0,	0,	-1},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	1,	0,	0,	-1,	0,	0,	0,	0,	0,	0,	0,	-1},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
};

static int rgb_offset_dream_revS[LUMINANCE_MAX][RGB_COMPENSATION] = {
	{-3,  0, -3, -1,  0, -1, -2,  0, -2, -2,  0, -2, -4,  0, -6, -5,  0, -7, -6,  0, -6, -3,  0,  0,  3,  0,  3},
	{-3,  0, -3, -2,  0, -1, -2,  0, -2, -2,  0, -4, -3,  0, -5, -7,  0, -8, -7,  0, -7, -3,  0, -1,  3,  0,  3},
	{-2,  0, -3, -2,  0,  0, -2,  0, -2, -2,  0, -3, -3,  0, -6, -6,  0, -6, -7,  0, -8, -3,  0, -2,  3,  0,  3},
	{-1,  0, -2, -2,  0,  0, -2,  0, -2, -2,  0, -4, -1,  0, -4, -5,  0, -5, -7,  0, -8, -2,  0, -3,  3,  0,  3},
	{-1,  0, -2, -1,  0,  0, -2,  0, -1, -3,  0, -3, -1,  0, -5, -3,  0, -4, -6,  0, -8, -1,  0, -5,  3,  0,  3},
	{-1,  0, -2, -1,  0,  0, -2,  0,  0, -1,  0, -3, -1,  0, -5, -2,  0, -3, -6,  0, -7,  0,  0, -6,  3,  0,  3},
	{-1,  0, -2, -1,  0,  0, -1,  0,  0, -2,  0, -3, -2,  0, -5, -2,  0, -4, -6,  0, -6,  0,  0, -8,  3,  0,  2},
	{-1,  0, -2, -1,  0,  0, -1,  0,  0, -2,  0, -3, -2,  0, -4, -1,  0, -4, -5,  0, -7,  1,  0, -8,  3,  0,  1},
	{ 0,  0, -1, -2,  0, -1, -1,  0, -1, -2,  0, -3, -2,  0, -4, -1,  0, -5, -3,  0, -7,  1,  0, -8,  4,  0,  1},
	{ 0,  0, -1, -2,  0, -1, -1,  0, -1, -2,  0, -3, -2,  0, -4, -1,  0, -6, -2,  0, -7,  2,  0, -8,  4,  0,  1},
	{ 0,  0, -1, -1,  0, -1, -1,  0,  0, -2,  0, -4, -2,  0, -5, -1,  0, -6, -2,  0, -8,  4,  0, -9,  4,  0,  1},
	{ 0,  0, -1, -1,  0, -1, -1,  0,  0, -1,  0, -4, -3,  0, -5, -1,  0, -8, -2,  0, -8,  3,  0, -11,  4,  0,  1},
	{ 1,  0,  0, -1,  0, -1, -2,  0, -1, -1,  0, -4, -3,  0, -6, -2,  0, -8,  0,  0, -9,  3,  0, -12,  3,  0,  0},
	{ 1,  0,  0, -1,  0, -1, -2,  0, -1, -1,  0, -4, -3,  0, -6, -2,  0, -9,  0,  0, -11,  4,  0, -13,  4,  0,  1},
	{ 1,  0,  0, -1,  0, -1, -1,  0,  0, -2,  0, -3, -2,  0, -6, -2,  0, -8,  0,  0, -10,  4,  0, -12,  4,  0,  0},
	{ 1,  0,  0, -1,  0, -1, -1,  0,  0, -1,  0, -3, -2,  0, -6, -3,  0, -9,  0,  0, -8,  5,  0, -12,  4,  0,  0},
	{ 1,  0,  0, -1,  0,  0, -1,  0, -1, -1,  0, -3, -1,  0, -4, -3,  0, -8,  0,  0, -9,  5,  0, -13,  4,  0, -1},
	{ 1,  0,  1,  0,  0,  1,  0,  0,  0, -1,  0, -4, -3,  0, -6, -3,  0, -9,  0,  0, -8,  5,  0, -12,  5,  0, -1},
	{ 1,  0,  0, -1,  0,  0,  0,  0,  1, -1,  0, -3, -2,  0, -5, -2,  0, -9,  0,  0, -7,  0,  0, -8,  3,  0, -3},
	{ 1,  0,  0, -1,  0,  0,  0,  0,  1, -1,  0, -2, -2,  0, -5, -2,  0, -9,  0,  0, -6,  2,  0, -8,  3,  0, -3},
	{ 1,  0,  0, -1,  0,  0,  0,  0,  1,  0,  0, -1, -2,  0, -5, -1,  0, -8,  0,  0, -6,  3,  0, -7,  3,  0, -3},
	{ 1,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0, -1, -2,  0, -5, -1,  0, -7,  0,  0, -5,  3,  0, -6,  3,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  0,  0,  1,  0,  0, -1, -2,  0, -5, -1,  0, -7,  1,  0, -7,  4,  0, -7,  3,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  0,  0,  1,  1,  0,  0, -2,  0, -5, -2,  0, -8,  1,  0, -6,  2,  0, -4,  2,  0, -4},
	{ 1,  0,  1, -1,  0,  0,  0,  0,  1,  1,  0,  0, -2,  0, -5, -3,  0, -8,  1,  0, -7,  1,  0, -4,  2,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  0,  0,  1,  1,  0,  0, -2,  0, -5, -3,  0, -7,  1,  0, -6,  3,  0, -6,  2,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  0,  0,  1,  1,  0,  0, -1,  0, -4, -3,  0, -7,  0,  0, -6,  2,  0, -5,  2,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  1,  0,  2,  0,  0, -1, -1,  0, -3, -2,  0, -6,  1,  0, -4,  2,  0, -3,  2,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  1,  0,  2,  0,  0, -1, -1,  0, -3, -2,  0, -6,  1,  0, -3,  2,  0, -1,  2,  0, -3},
	{ 1,  0,  1, -1,  0,  0,  1,  0,  2,  1,  0, -1, -2,  0, -3, -1,  0, -5,  2,  0, -5,  3,  0, -2,  4,  0, -6},
	{ 1,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0, -2, -2,  0, -3,  0,  0, -5,  2,  0, -2,  2,  0, -3,  4,  0, -6},
	{ 1,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0, -2, -1,  0, -2,  0,  0, -4,  1,  0, -2,  2,  0, -4,  3,  0, -3},
	{ 1,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0, -1, -1,  0, -3,  0,  0, -4,  1,  0, -1,  2,  0,  0,  6,  0, -4},
	{ 1,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0, -1, -1,  0, -3,  0,  0, -3,  3,  0, -1,  3,  0, -2,  5,  0, -1},
	{ 1,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0, -1,  0,  0, -2,  0,  0, -3,  3,  0,  0,  2,  0, -2,  5,  0,  0},
	{ 1,  0,  2, -1,  0,  0,  0,  0,  2,  1,  0,  1,  0,  0, -2, -1,  0, -3,  5,  0,  0,  2,  0, -1,  5,  0,  0},
	{ 2,  0,  2, -1,  0,  0,  1,  0,  1,  0,  0,  0,  1,  0, -2, -1,  0, -4,  2,  0,  2,  0,  0, -1,  5,  0,  0},
	{ 2,  0,  2, -1,  0,  0,  1,  0,  2,  0,  0,  0,  1,  0, -2, -1,  0, -4,  2,  0,  3,  0,  0, -1,  5,  0,  0},
	{ 2,  0,  2, -2,  0,  0,  1,  0,  1,  0,  0,  0,  1,  0, -2, -1,  0, -3,  1,  0,  4, -1,  0,  0,  5,  0,  0},
	{ 2,  0,  2, -1,  0,  0,  1,  0,  2,  1,  0,  1,  1,  0, -2, -1,  0, -3,  1,  0,  0,  0,  0, -4,  5,  0, -1},
	{ 1,  0,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  1,  0, -2, -1,  0, -2,  2,  0,  0,  0,  0, -2,  5,  0, -1},
	{ 2,  0,  2, -1,  0,  0,  0,  0,  1,  0,  0,  0,  1,  0, -1, -1,  0, -2,  2,  0, -1, -2,  0, -6,  5,  0, -2},
	{ 1,  0,  2, -1,  0, -1,  0,  0,  1,  1,  0,  1,  1,  0, -1, -1,  0, -2,  2,  0, -1,  0,  0, -4,  5,  0, -3},
	{ 2,  0,  3,  0,  0,  0, -2,  0,  0,  1,  0,  1,  0,  0, -1,  1,  0, -2,  2,  0, -2,  2,  0, -6,  2,  0, -3},
	{ 2,  0,  3,  0,  0,  1, -1,  0, -1,  1,  0,  1,  2,  0, -1, -1,  0, -1,  1,  0, -3,  1,  0, -6,  2,  0, -5},
	{ 3,  0,  3, -1,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0, -1,  1,  0, -1,  0,  0, -3,  2,  0, -4,  2,  0, -5},
	{ 3,  0,  3, -1,  0,  0, -1,  0, -1,  2,  0,  2, -1,  0, -1,  1,  0, -1, -1,  0, -3,  4,  0, -4,  2,  0, -5},
	{ 3,  0,  3,  1,  0,  1, -1,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0, -1, -1,  0, -3,  1,  0, -8,  2,  0, -5},
	{ 3,  0,  4, -1,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0, -1,  0,  0, -3,  2,  0, -6,  2,  0, -5},
	{ 3,  0,  4,  0,  0,  1,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0, -1,  0,  0, -1,  0,  0, -7,  2,  0, -4},
	{ 3,  0,  4,  0,  0,  0, -1,  0,  0,  2,  0,  1,  0,  0,  0,  0,  0, -1, -1,  0, -2,  2,  0, -4,  3,  0, -5},
	{ 2,  0,  3,  0,  0,  0, -1,  0,  0,  0,  0,  1,  1,  0,  1, -1,  0, -1,  0,  0, -2,  5,  0, -4,  2,  0, -5},
	{ 2,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0, -1,  0, -1,  0,  0, -2,  3,  0, -2,  1,  0, -6},
	{ 2,  0,  3, -1,  0,  0,  0,  0,  0, -1,  0,  1,  2,  0,  0, -1,  0, -2,  1,  0, -1,  3,  0, -2,  5,  0, -2},
	{ 2,  0,  3,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0, -1,  3,  0,  1,  4,  0, -2},
	{ 2,  0,  3,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0, -1,  4,  0,  1,  7,  0,  0},
	{ 2,  0,  3,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  1,  1,  0,  0,  0,  0, -1,  4,  0,  1,  7,  0,  0},
	{ 2,  0,  3, -1,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  1,  1,  0,  0,  0,  0, -1,  4,  0, -1,  6,  0,  2},
	{ 2,  0,  3, -1,  0,  0, -1,  0,  0,  2,  0,  1, -1,  0,  0,  1,  0,  0,  0,  0, -1,  4,  0,  1,  6,  0,  2},
	{ 1,  0,  2,  0,  0,  0,  0,  0,  0,  2,  0,  2, -1,  0,  0,  1,  0,  0,  0,  0, -1,  5,  0,  2, 10,  0,  5},
	{ 1,  0,  2,  0,  0,  0,  0,  0,  0,  2,  0,  1,  1,  0,  2,  0,  0,  0,  0,  0, -1,  4,  0,  2,  9,  0,  5},
	{ 1,  0,  2, -1,  0, -1,  0,  0,  0,  1,  0,  1,  0,  0,  1,  2,  0,  0,  1,  0, -1,  3,  0,  4, 10,  0,  6},
	{ 1,  0,  2,  0,  0,  0,  0,  0,  0,  2,  0,  1,  0,  0,  1,  0,  0, -1,  0,  0,  0,  6,  0,  4,  9,  0,  6},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  1,  1,  0,  0,  0,  0, -1,  6,  0,  5,  9,  0,  7},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  2,  0,  0, -1,  0, -1,  7,  0,  6, 12,  0, 10},
	{ 0,  0,  1,  0,  0,  0,  1,  0,  0,  1,  0,  1, -1,  0,  0,  1,  0,  0,  0,  0,  0,  7,  0,  6, 11,  0, 10},
	{ 0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,  1,  0,  1,  7,  0,  7, 12,  0, 11},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0, -1,  1,  0,  1,  7,  0,  7, 12,  0, 11},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0, -1, -1,  0,  0,  8,  0,  7, 11,  0, 11},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0, -1, -1,  0,  0,  7,  0,  7, 11,  0, 11},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0, -1, -1,  0,  0,  1,  0,  0, -2,  0,  1},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,  0,  0,  1,  0,  0, -1,  0,  0,  2,  0,  2,  0,  0,  2},
	{ 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,  0,  0,  1,  0,  0, -1,  0,  0,  2,  0,  3, -1,  0,  2},
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
};

/************
 *	  HMT   *
 ************/
static unsigned int base_luminance_reverse_hmt_single_revA[][2] = {
	{10,	43},
	{11,	47},
	{12,	51},
	{13,	56},
	{14,	61},
	{15,	65},
	{16,	70},
	{17,	73},
	{19,	81},
	{20,	88},
	{21,	91},
	{22,	95},
	{23,	98},
	{25,	107},
	{27,	113},
	{29,	122},
	{31,	129},
	{33,	137},
	{35,	142},
	{37,	151},
	{39,	158},
	{41,	166},
	{44,	176},
	{47,	188},
	{50,	197},
	{53,	207},
	{56,	218},
	{60,	229},
	{64,	246},
	{68,	260},
	{72,	267},
	{77,	207},
	{82,	221},
	{87,	232},
	{93,	246},
	{99,	259},
	{105,	271},
};

static unsigned int base_luminance_reverse_hmt_single_revS[][2] = {
	{10,	41},
	{11,	46},
	{12,	50},
	{13,	55},
	{14,	60},
	{15,	64},
	{16,	67},
	{17,	71},
	{19,	82},
	{20,	84},
	{21,	89},
	{22,	92},
	{23,	96},
	{25,	105},
	{27,	113},
	{29,	120},
	{31,	126},
	{33,	134},
	{35,	142},
	{37,	150},
	{39,	156},
	{41,	165},
	{44,	176},
	{47,	188},
	{50,	201},
	{53,	211},
	{56,	221},
	{60,	236},
	{64,	251},
	{68,	263},
	{72,	274},
	{77,	211},
	{82,	222},
	{87,	234},
	{93,	248},
	{99,	262},
	{105,	273},
};

static int gradation_offset_reverse_hmt_single_revA[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	0,	0,	0,	3,	4,	4,	6,	6},
	{0,	0,	2,	0,	3,	4,	4,	6,	6},
	{0,	0,	0,	2,	4,	4,	5,	7,	6},
	{0,	0,	0,	0,	3,	4,	4,	5,	6},
	{0,	-1,	0,	0,	3,	4,	5,	6,	6},
	{0,	-1,	0,	0,	2,	4,	4,	6,	6},
	{0,	0,	0,	2,	3,	4,	4,	6,	6},
	{0,	0,	0,	0,	2,	3,	4,	6,	6},
	{0,	0,	0,	0,	3,	4,	5,	5,	5},
	{0,	1,	0,	0,	3,	4,	4,	4,	5},
	{0,	0,	0,	2,	3,	4,	4,	5,	6},
	{0,	0,	0,	2,	3,	3,	4,	6,	6},
	{0,	2,	0,	0,	3,	4,	4,	6,	6},
	{0,	0,	0,	0,	3,	3,	4,	6,	6},
	{0,	2,	0,	0,	3,	3,	4,	6,	6},
	{0,	3,	0,	0,	3,	3,	4,	5,	6},
	{0,	3,	2,	2,	3,	4,	4,	5,	6},
	{0,	3,	3,	0,	2,	4,	4,	6,	5},
	{0,	3,	3,	3,	3,	4,	4,	6,	5},
	{0,	3,	3,	3,	3,	3,	4,	6,	5},
	{0,	3,	4,	4,	3,	4,	4,	5,	5},
	{0,	2,	3,	3,	3,	4,	4,	5,	5},
	{0,	4,	4,	4,	3,	4,	4,	5,	5},
	{0,	3,	5,	4,	3,	4,	4,	6,	6},
	{0,	3,	5,	4,	4,	4,	4,	6,	6},
	{0,	4,	6,	4,	4,	3,	4,	5,	6},
	{0,	3,	6,	4,	4,	4,	4,	5,	5},
	{0,	4,	6,	4,	4,	4,	4,	5,	5},
	{0,	4,	6,	5,	4,	4,	4,	5,	5},
	{0,	4,	7,	4,	4,	4,	4,	6,	5},
	{0,	7,	9,	6,	5,	4,	4,	4,	5},
	{0,	2,	3,	2,	0,	0,	0,	2,	2},
	{0,	3,	4,	2,	0,	0,	0,	2,	2},
	{0,	3,	4,	3,	0,	0,	0,	2,	2},
	{0,	3,	5,	3,	2,	0,	0,	2,	2},
	{0,	3,	5,	3,	2,	0,	2,	3,	2},
	{0,	4,	6,	4,	2,	0,	0,	2,	2},
};

static int gradation_offset_reverse_hmt_single_revS[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0, 2, 0, 2, 4, 4, 4, 9, 6},
	{0, 0, 0, 0, 3, 4, 4, 5, 6},
	{0, 0, 0, 0, 3, 4, 4, 6, 6},
	{0, 0, 0, 0, 3, 3, 4, 4, 6},
	{0, -1, 0, 0, 2, 4, 4, 4, 6},
	{0, -1, 0, 0, 3, 3, 4, 4, 6},
	{0, -1, 0, 0, 2, 4, 4, 5, 6},
	{0, -1, 0, 0, 2, 4, 4, 5, 6},
	{0, 0, 0, 0, 3, 4, 3, 4, 5},
	{0, 0, 0, 0, 3, 3, 4, 4, 5},
	{0, 0, 0, 0, 2, 3, 4, 4, 5},
	{0, 0, 0, 0, 0, 3, 4, 4, 5},
	{0, 0, 0, 0, 2, 3, 4, 4, 5},
	{0, 0, 0, 0, 2, 3, 4, 4, 5},
	{0, 0, 0, 0, 2, 3, 4, 4, 5},
	{0, 2, 0, 0, 0, 2, 4, 4, 6},
	{0, 3, 0, 0, 0, 3, 4, 4, 6},
	{0, 3, 0, 0, 2, 3, 4, 4, 6},
	{0, 2, 2, 2, 2, 3, 3, 4, 5},
	{0, 3, 0, 0, 0, 3, 4, 4, 5},
	{0, 3, 3, 0, 2, 4, 4, 4, 5},
	{0, 3, 3, 2, 0, 3, 3, 4, 5},
	{0, 3, 3, 2, 0, 3, 4, 4, 5},
	{0, 0, 3, 2, 0, 3, 3, 4, 5},
	{0, 0, 3, 0, 0, 3, 3, 4, 5},
	{0, 0, 3, 3, 0, 2, 3, 4, 4},
	{0, 0, 4, 2, 0, 3, 3, 4, 5},
	{0, 2, 4, 2, 0, 3, 3, 4, 5},
	{0, 3, 5, 3, 3, 3, 3, 4, 5},
	{0, 3, 4, 4, 2, 3, 3, 4, 5},
	{0, 4, 6, 4, 2, 3, 3, 4, 5},
	{0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 3, 0, 0, 0, 0, 0, 1},
	{0, 2, 3, 0, 0, 0, 0, 0, 1},
	{0, 0, 3, 0, 0, 0, 0, 2, 2},
	{0, 2, 4, 0, 0, 0, 0, 0, 2},
	{0, 4, 5, 3, 0, 0, 0, 0, 2},
};

static int rgb_offset_reverse_hmt_single_revA[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	-1,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	-2,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	-2,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	-2,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	0,	0,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	0,	0,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	1,	2,	-2},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	1,	2,	-2},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	0,	3,	-1},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	0,	3,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-4,	-3,	-7},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-4,	-2,	-7},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-4,	-2,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-3,	0,	-5},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-3,	0,	-5},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-2,	0,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-2,	0,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-2,	2,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-2,	2,	-4},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-2,	2,	-3},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-3,	2,	-5},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-3,	2,	-5},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-4,	0,	-5},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-7},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-7},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-7},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-6},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-8},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-5,	0,	-6},
	{1,	-1,	4,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-2,	1,	-2,	0,	1,	-2,	-3,	2,	-4,	-4,	4,	-8,	-4,	0,	-5},
	{2,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-1,	0,	-2,	-2,	1,	-4,	-2,	2,	-6,	10,	2,	6},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	-2,	-1,	0,	-1,	-1,	1,	-4,	-2,	3,	-6,	5,	1,	4},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-2,	0,	-1,	-1,	0,	-2,	-2,	1,	-4,	-1,	3,	-6,	5,	1,	3},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-2,	0,	-2,	0,	0,	-2,	-2,	1,	-3,	-2,	3,	-7,	5,	1,	7},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-1,	0,	-1,	-1,	0,	-2,	-2,	1,	-3,	-1,	3,	-6,	4,	1,	1},
	{4,	0,	4,	0,	0,	-1,	-1,	0,	0,	0,	0,	0,	-1,	0,	-2,	-1,	1,	-2,	-1,	1,	-2,	-1,	2,	-5,	4,	1,	2},
};

static int rgb_offset_reverse_hmt_single_revS[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{0,  0,  0,  0,  0,  0, -2,  0, -3, -3,  1, -3, -5,  2, -6,  0,  2, -5,  0,  1, -3,  0,  0,  0, 11, -8, -3},
	{0,  0,  0,  0,  0,  0, -1,  0, -2, -2,  1, -2, -3,  2, -4, -2,  2, -6,  0,  1, -3,  7,  0,  0, 11, -7, -3},
	{0,  0,  1,  0,  0,  0, -2,  0, -3, -4,  1, -3, -4,  2, -5, -1,  2, -5,  0,  1, -4,  0,  1, -2, 11, -7, -3},
	{0,  0,  1,  0,  0,  0,  0,  0, -1, -3,  1, -3, -3,  2, -4, -2,  2, -5,  0,  1, -2,  7,  1, -3, 11, -7, -3},
	{0,  0,  1,  0,  0,  0, -1,  0, -1, -3,  1, -2, -4,  2, -5, -2,  2, -4,  0,  1, -3, 10,  0,  0,  9, -5,  2},
	{0,  0,  0, -1,  0,  0,  0,  0,  0, -2,  1, -2, -3,  2, -4, -2,  2, -4, -1,  1, -3,  4,  1, -3,  9, -5,  2},
	{0,  0,  0,  0,  0,  0, -1,  0, -2, -2,  1, -2, -4,  2, -5, -3,  1, -4, -1,  1, -4,  2,  2, -4,  8, -3,  2},
	{0,  0,  1,  0,  0,  0,  0,  0, -2, -3,  1, -3, -4,  1, -4, -1,  2, -4, -2,  1, -4,  4,  1, -4,  6, -2,  1},
	{0,  0,  1,  0,  0,  0, -1,  0, -2, -2,  1, -3, -3,  1, -3, -2,  2, -4, -1,  2, -5,  3,  1, -3,  6, -2,  2},
	{0,  0,  1,  0,  0,  0,  0,  0, -2, -1,  1, -2, -3,  1, -3, -3,  2, -5, -2,  1, -4,  4,  1, -4,  5, -1,  4},
	{1,  0,  2,  0,  0,  0,  0,  0, -1, -1,  1, -2, -4,  1, -4, -2,  2, -4, -1,  2, -4,  4,  1, -4,  5, -1,  2},
	{0,  0,  2,  0,  0,  0,  0,  0,  0, -1,  1, -2, -4,  2, -4, -2,  2, -4, -2,  1, -4,  4,  2, -5,  7, -1,  2},
	{0,  0,  2,  0,  0,  0,  0,  0, -1, -2,  0, -2, -3,  1, -3, -3,  2, -5,  0,  2, -4,  3,  2, -4,  7,  0,  4},
	{0,  0,  2,  0,  0,  0,  0,  0, -1, -1,  1, -2, -5,  1, -4, -2,  2, -4, -2,  1, -4,  6,  2, -5,  7,  0,  3},
	{0,  0,  2,  0,  0,  0,  1,  0,  0,  0,  1, -2, -4,  1, -3, -2,  2, -4, -2,  2, -5,  5,  2, -4,  7,  0,  2},
	{1,  0,  3,  0,  0,  0, -1,  0, -1, -1,  1, -2, -4,  1, -3, -3,  1, -4, -2,  2, -4,  1,  2, -6,  0, -7, -7},
	{1,  0,  1,  0,  0,  0,  0,  0, -1, -1,  1, -2, -2,  1, -2, -4,  2, -5, -2,  1, -3,  0,  2, -6,  0, -5, -4},
	{1,  0,  2,  0,  0,  0, -1,  0, -1, -1,  0, -2, -3,  1, -2, -2,  2, -4, -2,  1, -4,  3,  2, -5, -2, -5, -6},
	{2,  0,  3,  0,  0,  0, -1,  0, -1, -1,  0, -2, -2,  1, -2, -3,  2, -5, -3,  1, -4,  1,  2, -6,  2, -5, -4},
	{1,  0,  2,  0,  0,  0,  0,  0, -1,  0,  0, -1, -4,  1, -3, -2,  1, -4, -3,  1, -4,  2,  3, -6,  1, -4, -3},
	{1,  0,  2,  0,  0,  0, -1,  0, -1, -2,  0, -2, -3,  0, -2, -2,  2, -4, -3,  2, -4,  1,  2, -6,  4, -1, -2},
	{2,  0,  3,  0,  0,  0,  0,  0, -1, -1,  0, -1, -3,  1, -3, -1,  1, -3, -3,  2, -4,  1,  2, -5,  2, -1, -4},
	{2,  0,  3,  0,  0,  0,  0,  0, -2, -1,  0, -1, -3,  1, -2, -3,  2, -4, -3,  2, -4,  0,  3, -6,  6, -1,  0},
	{1,  0,  2,  0,  0,  0,  0,  0, -1,  0,  0, -1, -4,  1, -3, -2,  1, -3, -4,  2, -5,  0,  2, -6,  6,  1,  0},
	{2,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0, -1, -3,  1, -2, -2,  1, -3, -3,  2, -4,  0,  3, -6,  5,  1,  0},
	{1,  0,  2,  0,  0,  1,  0,  0, -1,  0,  0, -1, -2,  0, -2, -2,  1, -4, -3,  1, -4,  0,  3, -6,  5,  2,  2},
	{2,  0,  3,  0,  0,  0,  0,  0,  0, -2,  0, -2, -2,  1, -2, -3,  1, -3, -2,  2, -4, -1,  3, -7, -4, -7, -9},
	{2,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0, -1, -4,  1, -3, -1,  1, -2, -4,  1, -4,  0,  3, -7, -5, -5, -9},
	{2,  0,  3,  0,  0,  0, -1,  0, -1, -1,  0, -1, -2,  1, -2, -3,  1, -3, -3,  2, -4,  0,  3, -7, -3, -5, -7},
	{2,  0,  3,  0,  0,  0,  0,  0,  0, -2,  0, -2, -2,  1, -2,  0,  1, -2, -3,  2, -4, -1,  3, -6, -1, -3, -6},
	{3,  0,  3,  0,  0,  0,  0,  0,  0, -2,  0, -2, -2,  1, -3, -3,  0, -2, -2,  2, -4,  0,  3, -6,  0, -2, -4},
	{3,  0,  3,  0,  0,  0,  0,  0,  0, -1,  0, -1, -1,  0, -1, -3,  1, -3, -1,  0, -2,  6,  0, -2, 10, -4,  3},
	{2,  0,  2,  0,  0,  1,  0,  0,  0,  0,  0, -1, -1,  0, -1, -2,  1, -2, -2,  1, -2,  6,  0, -2, 11, -4,  3},
	{3,  0,  3,  0,  0,  1,  0,  0, -1,  0,  0,  0, -2,  0, -2, -2,  1, -2, -2,  0, -2,  6,  1, -2, 13, -1,  4},
	{4,  0,  4,  0,  0,  0,  0,  0,  0, -1,  0, -1, -1,  0, -1, -2,  0, -2, -2,  1, -2,  0,  2, -5,  9, -6, -1},
	{4,  0,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0, -2,  0, -2, -2,  0, -2, -2,  0, -2,  2,  2, -4,  6, -6, -3},
	{3,  0,  3,  0,  0,  0,  0,  0, -1,  0,  0,  0, -1,  0, -1, -2,  1, -2, -2,  1, -2,  1,  1, -4,  7, -6, -3},
};
#endif