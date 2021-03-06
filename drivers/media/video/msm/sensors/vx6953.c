/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
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

#include "msm_sensor.h"
#include "msm.h"
#include "vx6953.h"
#include "vx6953_reg.h"
#define SENSOR_NAME "vx6953"
#define PLATFORM_DRIVER_NAME "msm_camera_vx6953"
#define vx6953_obj vx6953_##obj

DEFINE_MUTEX(vx6953_mut);

#undef CDBG
#define CDBG printk
#define REG_GROUPED_PARAMETER_HOLD			0x0104
#define GROUPED_PARAMETER_HOLD_OFF			0x00
#define GROUPED_PARAMETER_HOLD				0x01
#define REG_MODE_SELECT					0x0100
#define MODE_SELECT_STANDBY_MODE			0x00
#define MODE_SELECT_STREAM				0x01
/* Integration Time */
#define REG_COARSE_INTEGRATION_TIME_HI			0x0202
#define REG_COARSE_INTEGRATION_TIME_LO			0x0203
/* Gain */
#define REG_ANALOGUE_GAIN_CODE_GLOBAL_HI		0x0204
#define REG_ANALOGUE_GAIN_CODE_GLOBAL_LO		0x0205
/* Digital Gain */
#define REG_DIGITAL_GAIN_GREEN_R_HI			0x020E
#define REG_DIGITAL_GAIN_GREEN_R_LO			0x020F
#define REG_DIGITAL_GAIN_RED_HI				0x0210
#define REG_DIGITAL_GAIN_RED_LO				0x0211
#define REG_DIGITAL_GAIN_BLUE_HI			0x0212
#define REG_DIGITAL_GAIN_BLUE_LO			0x0213
#define REG_DIGITAL_GAIN_GREEN_B_HI			0x0214
#define REG_DIGITAL_GAIN_GREEN_B_LO			0x0215
/* output bits setting */
#define REG_0x0112					0x0112
#define REG_0x0113					0x0113
/* PLL registers */
#define REG_VT_PIX_CLK_DIV				0x0301
#define REG_PRE_PLL_CLK_DIV				0x0305
#define REG_PLL_MULTIPLIER				0x0307
#define REG_OP_PIX_CLK_DIV				0x0309
#define REG_0x034c					0x034c
#define REG_0x034d					0x034d
#define REG_0x034e					0x034e
#define REG_0x034f					0x034f
#define REG_0x0387					0x0387
#define REG_0x0383					0x0383
#define REG_FRAME_LENGTH_LINES_HI			0x0340
#define REG_FRAME_LENGTH_LINES_LO			0x0341
#define REG_LINE_LENGTH_PCK_HI				0x0342
#define REG_LINE_LENGTH_PCK_LO				0x0343
#define REG_0x3030					0x3030
#define REG_0x0111					0x0111
#define REG_0x0136					0x0136
#define REG_0x0137					0x0137
#define REG_0x0b00					0x0b00
#define REG_0x3001					0x3001
#define REG_0x3004					0x3004
#define REG_0x3007					0x3007
#define REG_0x301a					0x301a
#define REG_0x3101					0x3101
#define REG_0x3364					0x3364
#define REG_0x3365					0x3365
#define REG_0x0b83					0x0b83
#define REG_0x0b84					0x0b84
#define REG_0x0b85					0x0b85
#define REG_0x0b88					0x0b88
#define REG_0x0b89					0x0b89
#define REG_0x0b8a					0x0b8a
#define REG_0x3005					0x3005
#define REG_0x3010					0x3010
#define REG_0x3036					0x3036
#define REG_0x3041					0x3041
#define REG_0x0b80					0x0b80
#define REG_0x0900					0x0900
#define REG_0x0901					0x0901
#define REG_0x0902					0x0902
#define REG_0x3016					0x3016
#define REG_0x301d					0x301d
#define REG_0x317e					0x317e
#define REG_0x317f					0x317f
#define REG_0x3400					0x3400
#define REG_0x303a					0x303a
#define REG_0x1716					0x1716
#define REG_0x1717					0x1717
#define REG_0x1718					0x1718
#define REG_0x1719					0x1719
#define REG_0x3006					0x3006
#define REG_0x301b					0x301b
#define REG_0x3098					0x3098
#define REG_0x309d					0x309d
#define REG_0x3011					0x3011
#define REG_0x3035					0x3035
#define REG_0x3045					0x3045
#define REG_0x3210					0x3210
#define	REG_0x0111					0x0111
#define REG_0x3410					0x3410
#define REG_0x0b06					0x0b06
#define REG_0x0b07					0x0b07
#define REG_0x0b08					0x0b08
#define REG_0x0b09					0x0b09
#define REG_0x3640					0x3640
/* Test Pattern */
#define REG_TEST_PATTERN_MODE				0x0601
/* 16bit address - 8 bit context register structure */
#define	VX6953_STM5M0EDOF_OFFSET	9
#define	Q8		0x00000100
#define	Q10		0x00000400
#define	VX6953_STM5M0EDOF_MAX_SNAPSHOT_EXPOSURE_LINE_COUNT	2922
#define	VX6953_STM5M0EDOF_DEFAULT_MASTER_CLK_RATE	24000000
#define	VX6953_STM5M0EDOF_OP_PIXEL_CLOCK_RATE	79800000
#define	VX6953_STM5M0EDOF_VT_PIXEL_CLOCK_RATE	88670000
/* Full	Size */
#define	VX6953_FULL_SIZE_WIDTH	2608
#define	VX6953_FULL_SIZE_HEIGHT		1960
#define	VX6953_FULL_SIZE_DUMMY_PIXELS	1
#define	VX6953_FULL_SIZE_DUMMY_LINES	0
/* Quarter Size	*/
#define	VX6953_QTR_SIZE_WIDTH	1304
#define	VX6953_QTR_SIZE_HEIGHT		980
#define	VX6953_QTR_SIZE_DUMMY_PIXELS	1
#define	VX6953_QTR_SIZE_DUMMY_LINES		0
/* Blanking	as measured	on the scope */
/* Full	Size */
#define	VX6953_HRZ_FULL_BLK_PIXELS	348
#define	VX6953_VER_FULL_BLK_LINES	40
/* Quarter Size	*/
#define	VX6953_HRZ_QTR_BLK_PIXELS	1628
#define	VX6953_VER_QTR_BLK_LINES	28
#define	MAX_LINE_LENGTH_PCK		8190
#define	MAX_FRAME_LENGTH_LINES	16383
#define	VX6953_REVISION_NUMBER_CUT2	0x10/*revision number	for	Cut2.0*/
#define	VX6953_REVISION_NUMBER_CUT3	0x20/*revision number	for	Cut3.0*/
static struct msm_sensor_ctrl_t vx6953_s_ctrl;
static uint32_t fps_divider;/* init to 1 * 0x00000400 */
static uint16_t fps;
static uint8_t vx6953_stm5m0edof_delay_msecs_stdby;
static struct msm_camera_i2c_reg_conf vx6953_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf vx6953_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf vx6953_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf vx6953_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf vx6953_prev_settings[] = {
	{0x0202, 0x03},/*REG = 0x0202 coarse integration_time_hi*/
	{0x0203, 0xD0},/*REG = 0x0203 coarse_integration_time_lo*/
	{0x0205, 0xC0},/*REG = 0x0205 analogue_gain_code_global*/
	{0x0340, 0x03},/*REG = 0x0340 frame_length_lines_hi*/
	{0x0341, 0xf0},/*REG = 0x0341 frame_length_lines_lo*/
	{0x0342, 0x0b},/*REG = 0x0342  line_length_pck_hi*/
	{0x0343, 0x74},/*REG = 0x0343  line_length_pck_lo*/
	{0x3005, 0x03},/*REG = 0x3005*/
	{0x3010, 0x00},/*REG = 0x3010*/
	{0x3011, 0x01},/*REG = 0x3011*/
	{0x301a, 0x6a},/*REG = 0x301a*/
	{0x3035, 0x03},/*REG = 0x3035*/
	{0x3036, 0x2c},/*REG = 0x3036*/
	{0x3041, 0x00},/*REG = 0x3041*/
	{0x3042, 0x24},/*REG = 0x3042*/
	{0x3045, 0x81},/*REG = 0x3045*/
	{0x0b80, 0x02},/*REG = 0x0b80 edof estimate*/
	{0x0900, 0x01},/*REG = 0x0900*/
	{0x0901, 0x22},/*REG = 0x0901*/
	{0x0902, 0x04},/*REG = 0x0902*/
	{0x0383, 0x03},/*REG = 0x0383*/
	{0x0387, 0x03},/*REG = 0x0387*/
	{0x034c, 0x05},/*REG = 0x034c*/
	{0x034d, 0x18},/*REG = 0x034d*/
	{0x034e, 0x03},/*REG = 0x034e*/
	{0x034f, 0xd4},/*REG = 0x034f*/
	{0x1716, 0x02},/*0x1716*/
	{0x1717, 0x04},/*0x1717*/
	{0x1718, 0x08},/*0x1718*/
	{0x1719, 0x2c},/*0x1719*/
};

static struct msm_camera_i2c_reg_conf vx6953_snap_settings[] = {
	{0x0202, 0x07},/*REG = 0x0202 coarse_integration_time_hi*/
	{0x0203, 0x00},/*REG = 0x0203 coarse_integration_time_lo*/
	{0x0205, 0xc0},/*REG = 0x0205 analogue_gain_code_global*/
	{0x0340, 0x07},/*REG = 0x0340 frame_length_lines_hi*/
	{0x0341, 0xd0},/*REG = 0x0341 frame_length_lines_lo*/
	{0x0342, 0x0b},/*REG = 0x0342 line_length_pck_hi*/
	{0x0343, 0x8c},/*REG = 0x0343 line_length_pck_lo*/
	{0x3005, 0x01},/*REG = 0x3005*/
	{0x3010, 0x00},/*REG = 0x3010*/
	{0x3011, 0x00},/*REG = 0x3011*/
	{0x301a, 0x55},/*REG = 0x301a*/
	{0x3035, 0x01},/*REG = 0x3035*/
	{0x3036, 0x23},/*REG = 0x3036*/
	{0x3041, 0x00},/*REG = 0x3041*/
	{0x3042, 0x24},/*REG = 0x3042*/
	{0x3045, 0xb7},/*REG = 0x3045*/
	{0x0b80, 0x01},/*REG = 0x0b80 edof application*/
	{0x0900, 0x00},/*REG = 0x0900*/
	{0x0901, 0x00},/*REG = 0x0901*/
	{0x0902, 0x00},/*REG = 0x0902*/
	{0x0383, 0x01},/*REG = 0x0383*/
	{0x0387, 0x01},/*REG = 0x0387*/
	{0x034c, 0x0A},/*REG = 0x034c*/
	{0x034d, 0x30},/*REG = 0x034d*/
	{0x034e, 0x07},/*REG = 0x034e*/
	{0x034f, 0xA8},/*REG = 0x034f*/
	{0x1716, 0x02},/*0x1716*/
	{0x1717, 0x0d},/*0x1717*/
	{0x1718, 0x07},/*0x1718*/
	{0x1719, 0x7d},/*0x1719*/
};

static struct msm_camera_i2c_reg_conf vx6953_recommend_settings[] = {
	{0x0103, 0x01}, /* standby */
	{0x0100, 0x00}, /* stop streaming */
	/* patch cut 2*/
	{0xFB94, 0},	/*intialise Data Xfer Status reg*/
	{0xFB95, 0},	/*gain 1	  (0x00)*/
	{0xFB96, 0},	/*gain 1.07   (0x10)*/
	{0xFB97, 0},	/*gain 1.14   (0x20)*/
	{0xFB98, 0},	/*gain 1.23   (0x30)*/
	{0xFB99, 0},	/*gain 1.33   (0x40)*/
	{0xFB9A, 0},	/*gain 1.45   (0x50)*/
	{0xFB9B, 0},	/*gain 1.6    (0x60)*/
	{0xFB9C, 0},	/*gain 1.78   (0x70)*/
	{0xFB9D, 2},	/*gain 2	  (0x80)*/
	{0xFB9E, 2},	/*gain 2.29   (0x90)*/
	{0xFB9F, 3},	/*gain 2.67   (0xA0)*/
	{0xFBA0, 3},	/*gain 3.2    (0xB0)*/
	{0xFBA1, 4},	/*gain 4	  (0xC0)*/
	{0xFBA2, 7},	/*gain 5.33   (0xD0)*/
	{0xFBA3, 10},	/*gain 8	  (0xE0)*/
	{0xFBA4, 11},	/*gain 9.14   (0xE4)*/
	{0xFBA5, 13},	/*gain 10.67  (0xE8)*/
	{0xFBA6, 15},	/*gain 12.8   (0xEC)*/
	{0xFBA7, 19},	/*gain 16     (0xF0)*/
	{0xF800, 0x12},
	{0xF801, 0x06},
	{0xF802, 0xf7},
	{0xF803, 0x90},
	{0xF804, 0x02},
	{0xF805, 0x05},
	{0xF806, 0xe0},
	{0xF807, 0xff},
	{0xF808, 0x65},
	{0xF809, 0x7d},
	{0xF80A, 0x70},
	{0xF80B, 0x03},
	{0xF80C, 0x02},
	{0xF80D, 0xf9},
	{0xF80E, 0x1c},
	{0xF80F, 0x8f},
	{0xF810, 0x7d},
	{0xF811, 0xe4},
	{0xF812, 0xf5},
	{0xF813, 0x7a},
	{0xF814, 0x75},
	{0xF815, 0x78},
	{0xF816, 0x30},
	{0xF817, 0x75},
	{0xF818, 0x79},
	{0xF819, 0x53},
	{0xF81A, 0x85},
	{0xF81B, 0x79},
	{0xF81C, 0x82},
	{0xF81D, 0x85},
	{0xF81E, 0x78},
	{0xF81F, 0x83},
	{0xF820, 0xe0},
	{0xF821, 0xc3},
	{0xF822, 0x95},
	{0xF823, 0x7b},
	{0xF824, 0xf0},
	{0xF825, 0x74},
	{0xF826, 0x02},
	{0xF827, 0x25},
	{0xF828, 0x79},
	{0xF829, 0xf5},
	{0xF82A, 0x79},
	{0xF82B, 0xe4},
	{0xF82C, 0x35},
	{0xF82D, 0x78},
	{0xF82E, 0xf5},
	{0xF82F, 0x78},
	{0xF830, 0x05},
	{0xF831, 0x7a},
	{0xF832, 0xe5},
	{0xF833, 0x7a},
	{0xF834, 0xb4},
	{0xF835, 0x08},
	{0xF836, 0xe3},
	{0xF837, 0xe5},
	{0xF838, 0x7d},
	{0xF839, 0x70},
	{0xF83A, 0x04},
	{0xF83B, 0xff},
	{0xF83C, 0x02},
	{0xF83D, 0xf8},
	{0xF83E, 0xe4},
	{0xF83F, 0xe5},
	{0xF840, 0x7d},
	{0xF841, 0xb4},
	{0xF842, 0x10},
	{0xF843, 0x05},
	{0xF844, 0x7f},
	{0xF845, 0x01},
	{0xF846, 0x02},
	{0xF847, 0xf8},
	{0xF848, 0xe4},
	{0xF849, 0xe5},
	{0xF84A, 0x7d},
	{0xF84B, 0xb4},
	{0xF84C, 0x20},
	{0xF84D, 0x05},
	{0xF84E, 0x7f},
	{0xF84F, 0x02},
	{0xF850, 0x02},
	{0xF851, 0xf8},
	{0xF852, 0xe4},
	{0xF853, 0xe5},
	{0xF854, 0x7d},
	{0xF855, 0xb4},
	{0xF856, 0x30},
	{0xF857, 0x05},
	{0xF858, 0x7f},
	{0xF859, 0x03},
	{0xF85A, 0x02},
	{0xF85B, 0xf8},
	{0xF85C, 0xe4},
	{0xF85D, 0xe5},
	{0xF85E, 0x7d},
	{0xF85F, 0xb4},
	{0xF860, 0x40},
	{0xF861, 0x04},
	{0xF862, 0x7f},
	{0xF863, 0x04},
	{0xF864, 0x80},
	{0xF865, 0x7e},
	{0xF866, 0xe5},
	{0xF867, 0x7d},
	{0xF868, 0xb4},
	{0xF869, 0x50},
	{0xF86A, 0x04},
	{0xF86B, 0x7f},
	{0xF86C, 0x05},
	{0xF86D, 0x80},
	{0xF86E, 0x75},
	{0xF86F, 0xe5},
	{0xF870, 0x7d},
	{0xF871, 0xb4},
	{0xF872, 0x60},
	{0xF873, 0x04},
	{0xF874, 0x7f},
	{0xF875, 0x06},
	{0xF876, 0x80},
	{0xF877, 0x6c},
	{0xF878, 0xe5},
	{0xF879, 0x7d},
	{0xF87A, 0xb4},
	{0xF87B, 0x70},
	{0xF87C, 0x04},
	{0xF87D, 0x7f},
	{0xF87E, 0x07},
	{0xF87F, 0x80},
	{0xF880, 0x63},
	{0xF881, 0xe5},
	{0xF882, 0x7d},
	{0xF883, 0xb4},
	{0xF884, 0x80},
	{0xF885, 0x04},
	{0xF886, 0x7f},
	{0xF887, 0x08},
	{0xF888, 0x80},
	{0xF889, 0x5a},
	{0xF88A, 0xe5},
	{0xF88B, 0x7d},
	{0xF88C, 0xb4},
	{0xF88D, 0x90},
	{0xF88E, 0x04},
	{0xF88F, 0x7f},
	{0xF890, 0x09},
	{0xF891, 0x80},
	{0xF892, 0x51},
	{0xF893, 0xe5},
	{0xF894, 0x7d},
	{0xF895, 0xb4},
	{0xF896, 0xa0},
	{0xF897, 0x04},
	{0xF898, 0x7f},
	{0xF899, 0x0a},
	{0xF89A, 0x80},
	{0xF89B, 0x48},
	{0xF89C, 0xe5},
	{0xF89D, 0x7d},
	{0xF89E, 0xb4},
	{0xF89F, 0xb0},
	{0xF8A0, 0x04},
	{0xF8A1, 0x7f},
	{0xF8A2, 0x0b},
	{0xF8A3, 0x80},
	{0xF8A4, 0x3f},
	{0xF8A5, 0xe5},
	{0xF8A6, 0x7d},
	{0xF8A7, 0xb4},
	{0xF8A8, 0xc0},
	{0xF8A9, 0x04},
	{0xF8AA, 0x7f},
	{0xF8AB, 0x0c},
	{0xF8AC, 0x80},
	{0xF8AD, 0x36},
	{0xF8AE, 0xe5},
	{0xF8AF, 0x7d},
	{0xF8B0, 0xb4},
	{0xF8B1, 0xd0},
	{0xF8B2, 0x04},
	{0xF8B3, 0x7f},
	{0xF8B4, 0x0d},
	{0xF8B5, 0x80},
	{0xF8B6, 0x2d},
	{0xF8B7, 0xe5},
	{0xF8B8, 0x7d},
	{0xF8B9, 0xb4},
	{0xF8BA, 0xe0},
	{0xF8BB, 0x04},
	{0xF8BC, 0x7f},
	{0xF8BD, 0x0e},
	{0xF8BE, 0x80},
	{0xF8BF, 0x24},
	{0xF8C0, 0xe5},
	{0xF8C1, 0x7d},
	{0xF8C2, 0xb4},
	{0xF8C3, 0xe4},
	{0xF8C4, 0x04},
	{0xF8C5, 0x7f},
	{0xF8C6, 0x0f},
	{0xF8C7, 0x80},
	{0xF8C8, 0x1b},
	{0xF8C9, 0xe5},
	{0xF8CA, 0x7d},
	{0xF8CB, 0xb4},
	{0xF8CC, 0xe8},
	{0xF8CD, 0x04},
	{0xF8CE, 0x7f},
	{0xF8CF, 0x10},
	{0xF8D0, 0x80},
	{0xF8D1, 0x12},
	{0xF8D2, 0xe5},
	{0xF8D3, 0x7d},
	{0xF8D4, 0xb4},
	{0xF8D5, 0xec},
	{0xF8D6, 0x04},
	{0xF8D7, 0x7f},
	{0xF8D8, 0x11},
	{0xF8D9, 0x80},
	{0xF8DA, 0x09},
	{0xF8DB, 0xe5},
	{0xF8DC, 0x7d},
	{0xF8DD, 0x7f},
	{0xF8DE, 0x00},
	{0xF8DF, 0xb4},
	{0xF8E0, 0xf0},
	{0xF8E1, 0x02},
	{0xF8E2, 0x7f},
	{0xF8E3, 0x12},
	{0xF8E4, 0x8f},
	{0xF8E5, 0x7c},
	{0xF8E6, 0xef},
	{0xF8E7, 0x24},
	{0xF8E8, 0x95},
	{0xF8E9, 0xff},
	{0xF8EA, 0xe4},
	{0xF8EB, 0x34},
	{0xF8EC, 0xfb},
	{0xF8ED, 0x8f},
	{0xF8EE, 0x82},
	{0xF8EF, 0xf5},
	{0xF8F0, 0x83},
	{0xF8F1, 0xe4},
	{0xF8F2, 0x93},
	{0xF8F3, 0xf5},
	{0xF8F4, 0x7c},
	{0xF8F5, 0xf5},
	{0xF8F6, 0x7b},
	{0xF8F7, 0xe4},
	{0xF8F8, 0xf5},
	{0xF8F9, 0x7a},
	{0xF8FA, 0x75},
	{0xF8FB, 0x78},
	{0xF8FC, 0x30},
	{0xF8FD, 0x75},
	{0xF8FE, 0x79},
	{0xF8FF, 0x53},
	{0xF900, 0x85},
	{0xF901, 0x79},
	{0xF902, 0x82},
	{0xF903, 0x85},
	{0xF904, 0x78},
	{0xF905, 0x83},
	{0xF906, 0xe0},
	{0xF907, 0x25},
	{0xF908, 0x7c},
	{0xF909, 0xf0},
	{0xF90A, 0x74},
	{0xF90B, 0x02},
	{0xF90C, 0x25},
	{0xF90D, 0x79},
	{0xF90E, 0xf5},
	{0xF90F, 0x79},
	{0xF910, 0xe4},
	{0xF911, 0x35},
	{0xF912, 0x78},
	{0xF913, 0xf5},
	{0xF914, 0x78},
	{0xF915, 0x05},
	{0xF916, 0x7a},
	{0xF917, 0xe5},
	{0xF918, 0x7a},
	{0xF919, 0xb4},
	{0xF91A, 0x08},
	{0xF91B, 0xe4},
	{0xF91C, 0x02},
	{0xF91D, 0x18},
	{0xF91E, 0x32},
	{0xF91F, 0x22},
	{0xF920, 0xf0},
	{0xF921, 0x90},
	{0xF922, 0xa0},
	{0xF923, 0xf8},
	{0xF924, 0xe0},
	{0xF925, 0x70},
	{0xF926, 0x02},
	{0xF927, 0xa3},
	{0xF928, 0xe0},
	{0xF929, 0x70},
	{0xF92A, 0x0a},
	{0xF92B, 0x90},
	{0xF92C, 0xa1},
	{0xF92D, 0x10},
	{0xF92E, 0xe0},
	{0xF92F, 0xfe},
	{0xF930, 0xa3},
	{0xF931, 0xe0},
	{0xF932, 0xff},
	{0xF933, 0x80},
	{0xF934, 0x04},
	{0xF935, 0x7e},
	{0xF936, 0x00},
	{0xF937, 0x7f},
	{0xF938, 0x00},
	{0xF939, 0x8e},
	{0xF93A, 0x7e},
	{0xF93B, 0x8f},
	{0xF93C, 0x7f},
	{0xF93D, 0x90},
	{0xF93E, 0x36},
	{0xF93F, 0x0d},
	{0xF940, 0xe0},
	{0xF941, 0x44},
	{0xF942, 0x02},
	{0xF943, 0xf0},
	{0xF944, 0x90},
	{0xF945, 0x36},
	{0xF946, 0x0e},
	{0xF947, 0xe5},
	{0xF948, 0x7e},
	{0xF949, 0xf0},
	{0xF94A, 0xa3},
	{0xF94B, 0xe5},
	{0xF94C, 0x7f},
	{0xF94D, 0xf0},
	{0xF94E, 0xe5},
	{0xF94F, 0x3a},
	{0xF950, 0x60},
	{0xF951, 0x0c},
	{0xF952, 0x90},
	{0xF953, 0x36},
	{0xF954, 0x09},
	{0xF955, 0xe0},
	{0xF956, 0x70},
	{0xF957, 0x06},
	{0xF958, 0x90},
	{0xF959, 0x36},
	{0xF95A, 0x08},
	{0xF95B, 0xf0},
	{0xF95C, 0xf5},
	{0xF95D, 0x3a},
	{0xF95E, 0x02},
	{0xF95F, 0x03},
	{0xF960, 0x94},
	{0xF961, 0x22},
	{0xF962, 0x78},
	{0xF963, 0x07},
	{0xF964, 0xe6},
	{0xF965, 0xd3},
	{0xF966, 0x94},
	{0xF967, 0x00},
	{0xF968, 0x40},
	{0xF969, 0x16},
	{0xF96A, 0x16},
	{0xF96B, 0xe6},
	{0xF96C, 0x90},
	{0xF96D, 0x30},
	{0xF96E, 0xa1},
	{0xF96F, 0xf0},
	{0xF970, 0x90},
	{0xF971, 0x43},
	{0xF972, 0x83},
	{0xF973, 0xe0},
	{0xF974, 0xb4},
	{0xF975, 0x01},
	{0xF976, 0x0f},
	{0xF977, 0x90},
	{0xF978, 0x43},
	{0xF979, 0x87},
	{0xF97A, 0xe0},
	{0xF97B, 0xb4},
	{0xF97C, 0x01},
	{0xF97D, 0x08},
	{0xF97E, 0x80},
	{0xF97F, 0x00},
	{0xF980, 0x90},
	{0xF981, 0x30},
	{0xF982, 0xa0},
	{0xF983, 0x74},
	{0xF984, 0x01},
	{0xF985, 0xf0},
	{0xF986, 0x22},
	{0xF987, 0xf0},
	{0xF988, 0x90},
	{0xF989, 0x35},
	{0xF98A, 0xba},
	{0xF98B, 0xe0},
	{0xF98C, 0xb4},
	{0xF98D, 0x0a},
	{0xF98E, 0x0d},
	{0xF98F, 0xa3},
	{0xF990, 0xe0},
	{0xF991, 0xb4},
	{0xF992, 0x01},
	{0xF993, 0x08},
	{0xF994, 0x90},
	{0xF995, 0xfb},
	{0xF996, 0x94},
	{0xF997, 0xe0},
	{0xF998, 0x90},
	{0xF999, 0x35},
	{0xF99A, 0xb8},
	{0xF99B, 0xf0},
	{0xF99C, 0xd0},
	{0xF99D, 0xd0},
	{0xF99E, 0xd0},
	{0xF99F, 0x82},
	{0xF9A0, 0xd0},
	{0xF9A1, 0x83},
	{0xF9A2, 0xd0},
	{0xF9A3, 0xe0},
	{0xF9A4, 0x32},
	{0xF9A5, 0x22},
	{0xF9A6, 0xe5},
	{0xF9A7, 0x7f},
	{0xF9A8, 0x45},
	{0xF9A9, 0x7e},
	{0xF9AA, 0x60},
	{0xF9AB, 0x15},
	{0xF9AC, 0x90},
	{0xF9AD, 0x01},
	{0xF9AE, 0x00},
	{0xF9AF, 0xe0},
	{0xF9B0, 0x70},
	{0xF9B1, 0x0f},
	{0xF9B2, 0x90},
	{0xF9B3, 0xa0},
	{0xF9B4, 0xf8},
	{0xF9B5, 0xe5},
	{0xF9B6, 0x7e},
	{0xF9B7, 0xf0},
	{0xF9B8, 0xa3},
	{0xF9B9, 0xe5},
	{0xF9BA, 0x7f},
	{0xF9BB, 0xf0},
	{0xF9BC, 0xe4},
	{0xF9BD, 0xf5},
	{0xF9BE, 0x7e},
	{0xF9BF, 0xf5},
	{0xF9C0, 0x7f},
	{0xF9C1, 0x22},
	{0xF9C2, 0x02},
	{0xF9C3, 0x0e},
	{0xF9C4, 0x79},
	{0xF9C5, 0x22},
	/* Offsets:*/
	{0x35C6, 0x00},/* FIDDLEDARKCAL*/
	{0x35C7, 0x00},
	{0x35C8, 0x01},/*STOREDISTANCEATSTOPSTREAMING*/
	{0x35C9, 0x20},
	{0x35CA, 0x01},/*BRUCEFIX*/
	{0x35CB, 0x62},
	{0x35CC, 0x01},/*FIXDATAXFERSTATUSREG*/
	{0x35CD, 0x87},
	{0x35CE, 0x01},/*FOCUSDISTANCEUPDATE*/
	{0x35CF, 0xA6},
	{0x35D0, 0x01},/*SKIPEDOFRESET*/
	{0x35D1, 0xC2},
	{0x35D2, 0x00},
	{0x35D3, 0xFB},
	{0x35D4, 0x00},
	{0x35D5, 0x94},
	{0x35D6, 0x00},
	{0x35D7, 0xFB},
	{0x35D8, 0x00},
	{0x35D9, 0x94},
	{0x35DA, 0x00},
	{0x35DB, 0xFB},
	{0x35DC, 0x00},
	{0x35DD, 0x94},
	{0x35DE, 0x00},
	{0x35DF, 0xFB},
	{0x35E0, 0x00},
	{0x35E1, 0x94},
	{0x35E6, 0x18},/* FIDDLEDARKCAL*/
	{0x35E7, 0x2F},
	{0x35E8, 0x03},/* STOREDISTANCEATSTOPSTREAMING*/
	{0x35E9, 0x93},
	{0x35EA, 0x18},/* BRUCEFIX*/
	{0x35EB, 0x99},
	{0x35EC, 0x00},/* FIXDATAXFERSTATUSREG*/
	{0x35ED, 0xA3},
	{0x35EE, 0x21},/* FOCUSDISTANCEUPDATE*/
	{0x35EF, 0x5B},
	{0x35F0, 0x0E},/* SKIPEDOFRESET*/
	{0x35F1, 0x74},
	{0x35F2, 0x04},
	{0x35F3, 0x64},
	{0x35F4, 0x04},
	{0x35F5, 0x65},
	{0x35F6, 0x04},
	{0x35F7, 0x7B},
	{0x35F8, 0x04},
	{0x35F9, 0x7C},
	{0x35FA, 0x04},
	{0x35FB, 0xDD},
	{0x35FC, 0x04},
	{0x35FD, 0xDE},
	{0x35FE, 0x04},
	{0x35FF, 0xEF},
	{0x3600, 0x04},
	{0x3601, 0xF0},
	/*Jump/Data:*/
	{0x35C2, 0x3F},/* Jump Reg*/
	{0x35C3, 0xFF},/* Jump Reg*/
	{0x35C4, 0x3F},/* Data Reg*/
	{0x35C5, 0xC0},/* Data Reg*/
	{0x35C0, 0x01},/* Enable*/
	/* end of patch cut 2 */
	/* common settings */
	{0x0112, 10},/*REG = 0x0112 , 10 bit */
	{0x0113, 10},/*REG = 0x0113*/
	{0x0301, 9},/*REG = 0x0301 vt_pix_clk_div*/
	{0x0305, 4},/*REG = 0x0305 pre_pll_clk_div*/
	{0x0307, 133},/*REG = 0x0307 pll_multiplier*/
	{0x0309, 10},/*REG = 0x0309 op_pix_clk_div*/
	{0x3030, 0x08},/*REG = 0x3030*/
	{0x0111, 0x02},/*REG = 0x0111*/
	{0x0b00, 0x01},/*REG = 0x0b00 ,lens shading off */
	{0x3001, 0x30},/*REG = 0x3001*/
	{0x3004, 0x33},/*REG = 0x3004*/
	{0x3007, 0x09},/*REG = 0x3007*/
	{0x3016, 0x1F},/*REG = 0x3016*/
	{0x301d, 0x03},/*REG = 0x301d*/
	{0x317E, 0x11},/*REG = 0x317E*/
	{0x317F, 0x09},/*REG = 0x317F*/
	{0x3400, 0x38},/*REG = 0x3400*/
	{0x0b06, 0x00},/*REG_0x0b06*/
	{0x0b07, 0x80},/*REG_0x0b07*/
	{0x0b08, 0x01},/*REG_0x0b08*/
	{0x0b09, 0x4F},/*REG_0x0b09*/
	{0x0136, 0x18},/*REG_0x0136*/
	{0x0137, 0x00},/*/REG_0x0137*/
	{0x0b83, 0x20},/*REG = 0x0b83*/
	{0x0b84, 0x90},/*REG = 0x0b84*/
	{0x0b85, 0x20},/*REG = 0x0b85*/
	{0x0b88, 0x80},/*REG = 0x0b88*/
	{0x0b89, 0x00},/*REG = 0x0b89*/
	{0x0b8a, 0x00},/*REG = 0x0b8a*/
	/* end of common settings */
};

static struct v4l2_subdev_info vx6953_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SGRBG10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array vx6953_init_conf[] = {
	{&vx6953_recommend_settings[0],
	ARRAY_SIZE(vx6953_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array vx6953_confs[] = {
	{&vx6953_snap_settings[0],
	ARRAY_SIZE(vx6953_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&vx6953_prev_settings[0],
	ARRAY_SIZE(vx6953_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t vx6953_dimensions[] = {
	{
		.x_output = 0xA30,
		.y_output = 0x7A8,
		.line_length_pclk = 0xB8C,
		.frame_length_lines = 0x7D0,
		.vt_pixel_clk = 88666666,
		.op_pixel_clk = 192000000,
		.binning_factor = 1,
	},
	{
		.x_output = 0x518,
		.y_output = 0x3D4,
		.line_length_pclk = 0xB74,
		.frame_length_lines = 0x3F0,
		.vt_pixel_clk = 88666666,
		.op_pixel_clk = 192000000,
		.binning_factor = 1,
	},
};

static struct msm_sensor_output_reg_addr_t vx6953_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_id_info_t vx6953_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x03B9,
};

static struct msm_sensor_exp_gain_info_t vx6953_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0204,
	.digital_gain_gr = 0x020E,
	.digital_gain_r = 0x0210,
	.digital_gain_b = 0x0212,
	.digital_gain_gb = 0x0214,
	.vert_offset = 9,
};

static const struct i2c_device_id vx6953_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&vx6953_s_ctrl},
	{ }
};

static struct i2c_driver vx6953_i2c_driver = {
	.id_table = vx6953_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client vx6953_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&vx6953_i2c_driver);
}

static int32_t vx6953_set_fps(struct msm_sensor_ctrl_t *s_ctrl,
	struct fps_cfg *fps) {
	uint16_t total_lines_per_frame;
	int32_t rc = 0;

	total_lines_per_frame = (uint16_t)((VX6953_QTR_SIZE_HEIGHT +
			VX6953_VER_QTR_BLK_LINES) * s_ctrl->fps_divider/0x400);
			s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines,
			((total_lines_per_frame & 0xFF00) >> 8),
			MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines + 1,
			(total_lines_per_frame & 0xFF00),
			MSM_CAMERA_I2C_BYTE_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return rc;
}

int32_t vx6953_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t gain, uint32_t line) {
	uint16_t line_length_pck, frame_length_lines;
	uint8_t gain_hi, gain_lo;
	uint8_t intg_time_hi, intg_time_lo;
	uint8_t frame_length_lines_hi = 0, frame_length_lines_lo = 0;
	int32_t rc = 0;

	frame_length_lines = VX6953_QTR_SIZE_HEIGHT +
		VX6953_VER_QTR_BLK_LINES;
	line_length_pck = VX6953_QTR_SIZE_WIDTH +
		VX6953_HRZ_QTR_BLK_PIXELS;
	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	if ((line + VX6953_STM5M0EDOF_OFFSET) > MAX_FRAME_LENGTH_LINES) {
		frame_length_lines = MAX_FRAME_LENGTH_LINES;
		line = MAX_FRAME_LENGTH_LINES - VX6953_STM5M0EDOF_OFFSET;
	} else if ((line + VX6953_STM5M0EDOF_OFFSET) > frame_length_lines) {
			frame_length_lines = line + VX6953_STM5M0EDOF_OFFSET;
			line = frame_length_lines;
			}
	frame_length_lines_hi = (uint8_t) ((frame_length_lines &
		0xFF00) >> 8);
	frame_length_lines_lo = (uint8_t) (frame_length_lines &
		0x00FF);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines,
		(uint8_t)(frame_length_lines_hi),
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines + 1,
		(uint8_t)(frame_length_lines_lo),
		MSM_CAMERA_I2C_BYTE_DATA);
	/* update analogue gain registers */
	gain_hi = (uint8_t) ((gain & 0xFF00) >> 8);
	gain_lo = (uint8_t) (gain & 0x00FF);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr + 1,
		gain_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr,
		gain_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
		/*Update GR Comopnent*/
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_gr,
		gain_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_gr + 1,
		gain_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		/*Update R Comopnent*/
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_r,
		gain_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_r + 1,
		gain_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		/*Update B Comopnent*/
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_b,
		gain_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_b + 1,
		gain_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		/*Update GB Comopnent*/
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_gb,
		gain_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->digital_gain_gb + 1,
		gain_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		/* update line count registers */
		intg_time_hi = (uint8_t) (((uint16_t)line & 0xFF00) >> 8);
		intg_time_lo = (uint8_t) ((uint16_t)line & 0x00FF);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
		intg_time_hi,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr + 1,
		intg_time_lo,
		MSM_CAMERA_I2C_BYTE_DATA);
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
		return rc;
}

static struct v4l2_subdev_core_ops vx6953_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops vx6953_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops vx6953_subdev_ops = {
	.core = &vx6953_subdev_core_ops,
	.video  = &vx6953_subdev_video_ops,
};

static struct msm_camera_i2c_reg_conf vx6953_edof_estimation[] = {
	{REG_0x0b80, 0x02},
};

static struct msm_camera_i2c_reg_conf vx6953_edof_application[] = {
	{REG_0x0b80, 0x01},
};

static struct msm_camera_i2c_reg_conf vx6953_edof_default[] = {
	{REG_0x0b80, 0x00},
};

static int vx6953_enable_edof(enum edof_mode_t edof_mode)
{
	int rc = 0;
	if (edof_mode == VX6953_EDOF_ESTIMATION) {
		/* EDof Estimation mode for preview */
		msm_camera_i2c_write_tbl(
			vx6953_s_ctrl.sensor_i2c_client,
			vx6953_edof_estimation,
			ARRAY_SIZE(vx6953_edof_estimation),
			vx6953_s_ctrl.msm_sensor_reg->default_data_type);
		CDBG("VX6953_EDOF_ESTIMATION");
	} else if (edof_mode == VX6953_EDOF_APPLICATION) {
		/* EDof Application mode for Capture */
		msm_camera_i2c_write_tbl(
			vx6953_s_ctrl.sensor_i2c_client,
			vx6953_edof_application,
			ARRAY_SIZE(vx6953_edof_application),
			vx6953_s_ctrl.msm_sensor_reg->default_data_type);
		CDBG("VX6953_EDOF_APPLICATION");
	} else {
		/* EDOF disabled */
		msm_camera_i2c_write_tbl(
			vx6953_s_ctrl.sensor_i2c_client,
			vx6953_edof_default,
			ARRAY_SIZE(vx6953_edof_default),
			vx6953_s_ctrl.msm_sensor_reg->default_data_type);
		CDBG("VX6953_EDOF_DISABLE");
	}
	return rc;
}

static struct msm_camera_i2c_reg_conf vx6953_standby[] = {
	{0x103, 0x01},
};

struct msm_camera_i2c_reg_conf init_mode_tbl[59];
struct msm_camera_i2c_reg_conf init_tbl[] = {
	{0x0112, 10},
	{0x0113, 10},
	{0x0301, 9},
	{0x0305, 4},
	{0x0307, 133},
	{0x0309, 10},
	{0x0202, 0x03},
	{0x0203, 0xd0},
	{0x0205, 0xc0},
	{0x3030, 0x08},
	{0x0111, 0x02},
	{0x0b00, 0x01},
	{0x3001, 0x30},
	{0x3004, 0x33},
	{0x3007, 0x09},
	{0x3016, 0x1F},
	{0x301d, 0x03},
	{0x317e, 0x11},
	{0x317f, 0x09},
	{0x3400, 0x38},
	{0x0b06, 0x00},
	{0x0b07, 0x80},
	{0x0b08, 0x01},
	{0x0b09, 0x4F},
	{0x0136, 0x18},
	{0x0137, 0x00},
	{0x0b83, 0x20},
	{0x0b84, 0x90},
	{0x0b85, 0x20},
	{0x0b88, 0x80},
	{0x0b89, 0x00},
	{0x0b8a, 0x00},
	{0x0340, 0x03},   /*REG = 0x0340 frame_length_lines_hi*/
	{0x0341, 0xf0},   /*REG = 0x0341 frame_length_lines_lo*/
	{0x0342, 0x0b},   /*REG = 0x0342  line_length_pck_hi*/
	{0x0343, 0x74},   /*REG = 0x0343  line_length_pck_lo*/
	{0x3005, 0x03},   /*REG = 0x3005*/
	{0x3010, 0x00},   /*REG = 0x3010*/
	{0x3011, 0x01},   /*REG = 0x3011*/
	{0x301a, 0x6a},   /*REG = 0x301a*/
	{0x3035, 0x03},   /*REG = 0x3035*/
	{0x3036, 0x2c},   /*REG = 0x3036*/
	{0x3041, 0x00},   /*REG = 0x3041*/
	{0x3042, 0x24},   /*REG = 0x3042*/
	{0x3045, 0x81},   /*REG = 0x3045*/
	{0x0b80, 0x02},   /*REG = 0x0b80 edof estimate*/
	{0x0900, 0x01},   /*REG = 0x0900*/
	{0x0901, 0x22},   /*REG = 0x0901*/
	{0x0902, 0x04},   /*REG = 0x0902*/
	{0x0383, 0x03},   /*REG = 0x0383*/
	{0x0387, 0x03},   /*REG = 0x0387*/
	{0x034c, 0x05},   /*REG = 0x034c*/
	{0x034d, 0x18},   /*REG = 0x034d*/
	{0x034e, 0x03},   /*REG = 0x034e*/
	{0x034f, 0xd4},   /*REG = 0x034f*/
	{0x1716, 0x02},   /*0x1716*/
	{0x1717, 0x04},   /*0x1717*/
	{0x1718, 0x08},   /*0x1718*/
	{0x1719, 0x2c},   /*0x1719*/
};

struct msm_camera_i2c_reg_conf mode_tbl1[] = {
	{REG_0x0112, 10},/*REG = 0x0112 , 10 bit */
	{REG_0x0113, 10},/*REG = 0x0113*/
	{REG_VT_PIX_CLK_DIV, 9},/*REG = 0x0301 vt_pix_clk_div*/
	{REG_PRE_PLL_CLK_DIV, 4},/*REG = 0x0305 pre_pll_clk_div*/
	{REG_PLL_MULTIPLIER, 133},/*REG = 0x0307 pll_multiplier*/
	{REG_OP_PIX_CLK_DIV, 10},/*REG = 0x0309 op_pix_clk_div*/
	{REG_FRAME_LENGTH_LINES_HI, 0x03},/*REG = 0x0340 frame_length_lines_hi*/
	{REG_FRAME_LENGTH_LINES_LO, 0xf0},/*REG = 0x0341 frame_length_lines_lo*/
	{REG_LINE_LENGTH_PCK_HI, 0x0b},   /*REG = 0x0342  line_length_pck_hi*/
	{REG_LINE_LENGTH_PCK_LO, 0x74},   /*REG = 0x0343  line_length_pck_lo*/
	{REG_0x3005, 0x03},   /*REG = 0x3005*/
	{0x3010, 0x00},   /*REG = 0x3010*/
	{REG_0x3011, 0x01},   /*REG = 0x3011*/
	{REG_0x301a, 0x6a},   /*REG = 0x301a*/
	{REG_0x3035, 0x03},   /*REG = 0x3035*/
	{REG_0x3036, 0x2c},   /*REG = 0x3036*/
	{REG_0x3041, 0x00},   /*REG = 0x3041*/
	{0x3042, 0x24},   /*REG = 0x3042*/
	{REG_0x3045, 0x81},   /*REG = 0x3045*/
	{REG_0x0b80, 0x02},   /*REG = 0x0b80 edof estimate*/
	{REG_0x0900, 0x01},   /*REG = 0x0900*/
	{REG_0x0901, 0x22},   /*REG = 0x0901*/
	{REG_0x0902, 0x04},   /*REG = 0x0902*/
	{REG_0x0383, 0x03},   /*REG = 0x0383*/
	{REG_0x0387, 0x03},   /*REG = 0x0387*/
	{REG_0x034c, 0x05},   /*REG = 0x034c*/
	{REG_0x034d, 0x18},   /*REG = 0x034d*/
	{REG_0x034e, 0x03},   /*REG = 0x034e*/
	{REG_0x034f, 0xd4},   /*REG = 0x034f*/
	{REG_0x1716, 0x02},   /*0x1716*/
	{REG_0x1717, 0x04},   /*0x1717*/
	{REG_0x1718, 0x08},   /*0x1718*/
	{REG_0x1719, 0x2c},   /*0x1719*/
};

struct msm_camera_i2c_reg_conf mode_tbl2[] = {
	{REG_0x0112, 10},/*REG = 0x0112 , 10 bit */
	{REG_0x0113, 10},/*REG = 0x0113*/
	{REG_VT_PIX_CLK_DIV, 9},/*REG = 0x0301 vt_pix_clk_div*/
	{REG_PRE_PLL_CLK_DIV, 4},/*REG = 0x0305 pre_pll_clk_div*/
	{REG_PLL_MULTIPLIER, 133},/*REG = 0x0307 pll_multiplier*/
	{REG_OP_PIX_CLK_DIV, 10},/*REG = 0x0309 op_pix_clk_div*/
	{REG_FRAME_LENGTH_LINES_HI, 0x07},/*REG = 0x0340 frame_length_lines_hi*/
	{REG_FRAME_LENGTH_LINES_LO, 0xd0},/*REG = 0x0341 frame_length_lines_lo*/
	{REG_LINE_LENGTH_PCK_HI, 0x0b},/*REG = 0x0342 line_length_pck_hi*/
	{REG_LINE_LENGTH_PCK_LO, 0x8c},/*REG = 0x0343 line_length_pck_lo*/
	{REG_0x3005, 0x01},/*REG = 0x3005*/
	{0x3010, 0x00},/*REG = 0x3010*/
	{REG_0x3011, 0x00},/*REG = 0x3011*/
	{REG_0x301a, 0x55},/*REG = 0x301a*/
	{REG_0x3035, 0x01},/*REG = 0x3035*/
	{REG_0x3036, 0x23},/*REG = 0x3036*/
	{REG_0x3041, 0x00},/*REG = 0x3041*/
	{0x3042, 0x24},/*REG = 0x3042*/
	{REG_0x3045, 0xb7},/*REG = 0x3045*/
	{REG_0x0b80, 0x01},/*REG = 0x0b80 edof application*/
	{REG_0x0900, 0x00},/*REG = 0x0900*/
	{REG_0x0901, 0x00},/*REG = 0x0901*/
	{REG_0x0902, 0x00},/*REG = 0x0902*/
	{REG_0x0383, 0x01},/*REG = 0x0383*/
	{REG_0x0387, 0x01},/*REG = 0x0387*/
	{REG_0x034c, 0x0A},/*REG = 0x034c*/
	{REG_0x034d, 0x30},/*REG = 0x034d*/
	{REG_0x034e, 0x07},/*REG = 0x034e*/
	{REG_0x034f, 0xA8},/*REG = 0x034f*/
	{REG_0x1716, 0x02},/*0x1716*/
	{REG_0x1717, 0x0d},/*0x1717*/
	{REG_0x1718, 0x07},/*0x1718*/
	{REG_0x1719, 0x7d},/*0x1719*/
};

static int32_t vx6953_sensor_setting(int update_type, int rt)
{

	int32_t rc = 0;
	uint16_t frame_cnt = 0;
		CDBG("%s update type = %d, rt = %d\n",
			__func__, update_type, rt);

		switch (update_type) {
		case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
						/* reset fps_divider */
			fps = 30 * Q8;
			/* stop streaming */

			/* Reset everything first */
			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				vx6953_standby,
				ARRAY_SIZE(vx6953_standby),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);

			msleep(20);

			CDBG("Init vx6953_sensor_setting standby\n");
			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				vx6953_stop_settings,
				ARRAY_SIZE(vx6953_stop_settings),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);
				/*vx6953_stm5m0edof_delay_msecs_stdby*/
			msleep(vx6953_stm5m0edof_delay_msecs_stdby);
			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				init_tbl,
				ARRAY_SIZE(init_tbl),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);
			msleep(vx6953_stm5m0edof_delay_msecs_stdby);
		}
		return rc;
		case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			init_mode_tbl[0].reg_addr = REG_0x0112;
			init_mode_tbl[0].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0112;
			init_mode_tbl[1].reg_addr = REG_0x0113;
			init_mode_tbl[1].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0113;
			init_mode_tbl[2].reg_addr = REG_VT_PIX_CLK_DIV;
			init_mode_tbl[2].reg_data =
			vx6953_regs.reg_pat_init[0].vt_pix_clk_div;
			init_mode_tbl[3].reg_addr = REG_PRE_PLL_CLK_DIV;
			init_mode_tbl[3].reg_data =
			vx6953_regs.reg_pat_init[0].pre_pll_clk_div;
			init_mode_tbl[4].reg_addr = REG_PLL_MULTIPLIER;
			init_mode_tbl[4].reg_data =
			vx6953_regs.reg_pat_init[0].pll_multiplier;
			init_mode_tbl[5].reg_addr = REG_OP_PIX_CLK_DIV;
			init_mode_tbl[5].reg_data =
			vx6953_regs.reg_pat_init[0].op_pix_clk_div;
			init_mode_tbl[6].reg_addr =
			REG_COARSE_INTEGRATION_TIME_HI;
			init_mode_tbl[6].reg_data =
			vx6953_regs.reg_pat[rt].coarse_integration_time_hi;
			init_mode_tbl[7].reg_addr =
			REG_COARSE_INTEGRATION_TIME_LO;
			init_mode_tbl[7].reg_data =
			vx6953_regs.reg_pat[rt].coarse_integration_time_lo;
			init_mode_tbl[8].reg_addr =
			REG_ANALOGUE_GAIN_CODE_GLOBAL_LO;
			init_mode_tbl[8].reg_data =
			vx6953_regs.reg_pat[rt].analogue_gain_code_global;
			init_mode_tbl[9].reg_addr = REG_0x3030;
			init_mode_tbl[9].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3030;
			init_mode_tbl[10].reg_addr = REG_0x0111;
			init_mode_tbl[10].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0111;
			init_mode_tbl[11].reg_addr = REG_0x0b00;
			init_mode_tbl[11].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b00;
			init_mode_tbl[12].reg_addr = REG_0x3001;
			init_mode_tbl[12].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3001;
			init_mode_tbl[13].reg_addr = REG_0x3004;
			init_mode_tbl[13].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3004;
			init_mode_tbl[14].reg_addr = REG_0x3007;
			init_mode_tbl[14].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3007;
			init_mode_tbl[15].reg_addr = REG_0x3016;
			init_mode_tbl[15].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3016;
			init_mode_tbl[16].reg_addr = REG_0x301d;
			init_mode_tbl[16].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x301d;
			init_mode_tbl[17].reg_addr = REG_0x317e;
			init_mode_tbl[17].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x317e;
			init_mode_tbl[18].reg_addr = REG_0x317f;
			init_mode_tbl[18].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x317f;
			init_mode_tbl[19].reg_addr = REG_0x3400;
			init_mode_tbl[19].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x3400;
			init_mode_tbl[20].reg_addr = 0x0b06;
			init_mode_tbl[20].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b06;
			init_mode_tbl[21].reg_addr = 0x0b07;
			init_mode_tbl[21].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b07;
			init_mode_tbl[22].reg_addr = 0x0b08;
			init_mode_tbl[22].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b08;
			init_mode_tbl[23].reg_addr = 0x0b09;
			init_mode_tbl[23].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b09;
			init_mode_tbl[24].reg_addr = 0x0136;
			init_mode_tbl[24].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0136;
			init_mode_tbl[25].reg_addr = 0x0137;
			init_mode_tbl[25].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0137;
			init_mode_tbl[26].reg_addr = REG_0x0b83;
			init_mode_tbl[26].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b83;
			init_mode_tbl[27].reg_addr = REG_0x0b84;
			init_mode_tbl[27].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b84;
			init_mode_tbl[28].reg_addr = 0x0b85;
			init_mode_tbl[28].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b85;
			init_mode_tbl[29].reg_addr = 0x0b88;
			init_mode_tbl[29].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b88;
			init_mode_tbl[30].reg_addr = 0x0b89;
			init_mode_tbl[30].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b89;
			init_mode_tbl[31].reg_addr = REG_0x0b8a;
			init_mode_tbl[31].reg_data =
			vx6953_regs.reg_pat_init[0].reg_0x0b8a;
			init_mode_tbl[32].reg_addr = REG_FRAME_LENGTH_LINES_HI;
			init_mode_tbl[32].reg_data =
			vx6953_regs.reg_pat[rt].frame_length_lines_hi;
			init_mode_tbl[33].reg_addr = REG_FRAME_LENGTH_LINES_LO;
			init_mode_tbl[33].reg_data =
			vx6953_regs.reg_pat[rt].frame_length_lines_lo;
			init_mode_tbl[34].reg_addr = REG_LINE_LENGTH_PCK_HI;
			init_mode_tbl[34].reg_data =
			vx6953_regs.reg_pat[rt].line_length_pck_hi;
			init_mode_tbl[35].reg_addr = REG_LINE_LENGTH_PCK_LO;
			init_mode_tbl[35].reg_data =
			vx6953_regs.reg_pat[rt].line_length_pck_lo;
			init_mode_tbl[36].reg_addr = REG_0x3005;
			init_mode_tbl[36].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3005;
			init_mode_tbl[37].reg_addr = 0x3010;
			init_mode_tbl[37].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3010;
			init_mode_tbl[38].reg_addr = REG_0x3011;
			init_mode_tbl[38].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3011;
			init_mode_tbl[39].reg_addr = REG_0x301a;
			init_mode_tbl[39].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x301a;
			init_mode_tbl[40].reg_addr = REG_0x3035;
			init_mode_tbl[40].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3035;
			init_mode_tbl[41].reg_addr = REG_0x3036;
			init_mode_tbl[41].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3036;
			init_mode_tbl[42].reg_addr = REG_0x3041;
			init_mode_tbl[42].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3041;
			init_mode_tbl[43].reg_addr = 0x3042;
			init_mode_tbl[43].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3042;
			init_mode_tbl[44].reg_addr = REG_0x3045;
			init_mode_tbl[44].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x3045;
			init_mode_tbl[45].reg_addr = REG_0x0b80;
			init_mode_tbl[45].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0b80;
			init_mode_tbl[46].reg_addr = REG_0x0900;
			init_mode_tbl[46].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0900;
			init_mode_tbl[47].reg_addr = REG_0x0901;
			init_mode_tbl[47].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0901;
			init_mode_tbl[48].reg_addr = REG_0x0902;
			init_mode_tbl[48].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0902;
			init_mode_tbl[49].reg_addr = REG_0x0383;
			init_mode_tbl[49].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0383;
			init_mode_tbl[50].reg_addr = REG_0x0387;
			init_mode_tbl[50].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x0387;
			init_mode_tbl[51].reg_addr = REG_0x034c;
			init_mode_tbl[51].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x034c;
			init_mode_tbl[52].reg_addr = REG_0x034d;
			init_mode_tbl[52].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x034d;
			init_mode_tbl[53].reg_addr = REG_0x034e;
			init_mode_tbl[53].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x034e;
			init_mode_tbl[54].reg_addr = REG_0x034f;
			init_mode_tbl[54].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x034f;
			init_mode_tbl[55].reg_addr = REG_0x1716;
			init_mode_tbl[55].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x1716;
			init_mode_tbl[56].reg_addr = REG_0x1717;
			init_mode_tbl[56].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x1717;
			init_mode_tbl[57].reg_addr = REG_0x1718;
			init_mode_tbl[57].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x1718;
			init_mode_tbl[58].reg_addr = REG_0x1719;
			init_mode_tbl[58].reg_data =
			vx6953_regs.reg_pat[rt].reg_0x1719;
			/* stop streaming */
			msleep(20);

			/* Reset everything first */
			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				vx6953_standby,
				ARRAY_SIZE(vx6953_standby),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);


			msleep(20);

			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				vx6953_stop_settings,
				ARRAY_SIZE(vx6953_stop_settings),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);
			/*vx6953_stm5m0edof_delay_msecs_stdby*/
			msleep(vx6953_stm5m0edof_delay_msecs_stdby);

			msleep(vx6953_stm5m0edof_delay_msecs_stdby);

			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				init_mode_tbl,
				ARRAY_SIZE(init_mode_tbl),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);

			msleep(vx6953_stm5m0edof_delay_msecs_stdby);
			if (rt == RES_PREVIEW) {
				CDBG("%s write mode_tbl for preview\n",
					__func__);
				msm_camera_i2c_write_tbl(
					vx6953_s_ctrl.sensor_i2c_client,
					mode_tbl1,
					ARRAY_SIZE(mode_tbl1),
					vx6953_s_ctrl.msm_sensor_reg->
					default_data_type);
			} else if (rt == RES_CAPTURE) {
				CDBG("%s write mode_tbl for capture\n",
					__func__);
				msm_camera_i2c_write_tbl(
					vx6953_s_ctrl.sensor_i2c_client,
					mode_tbl2,
					ARRAY_SIZE(mode_tbl2),
					vx6953_s_ctrl.msm_sensor_reg->
					default_data_type);
			}
			msleep(vx6953_stm5m0edof_delay_msecs_stdby);

			/* Start sensor streaming */
			msm_camera_i2c_write_tbl(
				vx6953_s_ctrl.sensor_i2c_client,
				vx6953_start_settings,
				ARRAY_SIZE(vx6953_start_settings),
				vx6953_s_ctrl.msm_sensor_reg->
				default_data_type);
			msleep(20);

			msm_camera_i2c_read(
				vx6953_s_ctrl.sensor_i2c_client,
				0x0005,
				&frame_cnt,
				MSM_CAMERA_I2C_BYTE_ADDR);
			while (frame_cnt == 0xFF) {
				msm_camera_i2c_read(
					vx6953_s_ctrl.sensor_i2c_client,
					0x0005,
					&frame_cnt,
					MSM_CAMERA_I2C_BYTE_ADDR);
				CDBG("%s frame_cnt = %d\n",
					__func__, frame_cnt);
				usleep_range(5000, 10000);
			}
		}
		return rc;
		default:
		return rc;
	}
	return rc;
}

static int32_t vx6953_init_config(void)
{
	int32_t rc = 0;
	int rt;
	/* change sensor resolution	if needed */
	CDBG("%s called\n", __func__);
	rt = RES_PREVIEW;
	vx6953_stm5m0edof_delay_msecs_stdby =
		((((2 * 1000 * fps_divider) /
		   fps) * Q8) / Q10) + 1;

	vx6953_sensor_setting(REG_INIT, rt);

	vx6953_enable_edof(VX6953_EDOF_ESTIMATION);
	return rc;
}

static int32_t vx6953_update_config(int rt)
{
	int32_t rc = 0;
	CDBG("%s rt = %d\n", __func__, rt);
	if (rt == MSM_SENSOR_RES_FULL)
		rt = RES_CAPTURE;
	else if (rt == MSM_SENSOR_RES_QTR)
		rt = RES_PREVIEW;

	vx6953_stm5m0edof_delay_msecs_stdby = 67;
	vx6953_sensor_setting(UPDATE_PERIODIC, rt);

	if (rt == RES_PREVIEW)
		vx6953_enable_edof(VX6953_EDOF_ESTIMATION);
	else if (rt == RES_CAPTURE)
		vx6953_enable_edof(VX6953_EDOF_APPLICATION);

	return rc;
} /*end of vx6953_snapshot_config*/

static int32_t vx6953_set_sensor_mode(struct msm_sensor_ctrl_t *s_ctrl,
	int update_type, int rt)
{
	int32_t rc = 0;

	fps_divider = 1 * 0x00000400;
	fps = 30*Q8;

	switch (update_type) {
	case MSM_SENSOR_REG_INIT:
		rc = vx6953_init_config();
		break;
	case MSM_SENSOR_UPDATE_PERIODIC:
		rc = vx6953_update_config(rt);
		break;
	default:
		rc = -EINVAL;
		break;
	}
	return rc;
}
static struct msm_sensor_fn_t vx6953_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = vx6953_set_fps,
	.sensor_write_exp_gain = vx6953_write_exp_gain,
	.sensor_write_snapshot_exp_gain = vx6953_write_exp_gain,
	.sensor_csi_setting = vx6953_set_sensor_mode,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
};

static struct msm_sensor_reg_t vx6953_data_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = vx6953_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(vx6953_start_settings),
	.stop_stream_conf = vx6953_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(vx6953_stop_settings),
	.group_hold_on_conf = vx6953_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(vx6953_groupon_settings),
	.group_hold_off_conf = vx6953_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(vx6953_groupoff_settings),
	.init_settings = &vx6953_init_conf[0],
	.init_size = ARRAY_SIZE(vx6953_init_conf),
	.mode_settings = &vx6953_confs[0],
	.output_settings = &vx6953_dimensions[0],
	.num_conf = ARRAY_SIZE(vx6953_confs),
};

static struct msm_sensor_ctrl_t vx6953_s_ctrl = {
	.msm_sensor_reg = &vx6953_data_regs,
	.sensor_i2c_client = &vx6953_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &vx6953_reg_addr,
	.sensor_id_info = &vx6953_id_info,
	.sensor_exp_gain_info = &vx6953_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &vx6953_mut,
	.sensor_i2c_driver = &vx6953_i2c_driver,
	.sensor_v4l2_subdev_info = vx6953_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(vx6953_subdev_info),
	.sensor_v4l2_subdev_ops = &vx6953_subdev_ops,
	.func_tbl = &vx6953_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sensor VX6953 (BAYER 5M)");
MODULE_LICENSE("GPL v2");

