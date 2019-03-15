#include "config.h"
#ifdef FALL_V2
#pragma once

//=================================
//author wangzhibin_x@foxmail.com
//time 2018-12-14
//=================================

#define DATA_ABS(x) ((x)>0? (x):(-x))
//xyz2 means |xyz|

#define LINE_TO_TIME_MS(x) (20*x)
typedef unsigned char uint8_t;

typedef enum {
	ACC_PEAK_XYZ = 0x0,
	ACC_PEAK_XYZ2 = 0x1,
	ACC_PEAK_Y = 0x2,
	ACC_PEAK_Y2 = 0x3,
}detect_peak_acc_mode_t;

typedef struct {
	float acc_xyz_old;
	float acc_xyz2_old;
	float acc_y_old;
	float acc_y2_old;
}acc_value_mode_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
	int acc_y;
	int acc_y2;

}last_status_is_up_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
	int acc_y;
	int acc_y2;

}direction_is_up_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
	int acc_y;
	int acc_y2;
} continue_up_count_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
	int acc_y;
	int acc_y2;
} continue_up_former_count_t;

typedef struct {
	float acc_xyz;
	float acc_xyz2;
	float acc_y;
	float acc_y2;

} peak_wave_t;

typedef struct {
	float acc_xyz;
	float acc_xyz2;
	float acc_y;
	float acc_y2;
}valley_wave_t;

typedef struct {
	uint8_t rel;
	long time;
}last_fall_t;

typedef struct {
	uint8_t rel;
	uint8_t xyz_flag;
	float xyz_neg_peak_value;
	long xyz_neg_peak_time;
	float xyz_1_peak_value;
	long xyz_1_peak_time;
	float xyz_2_peak_value;
	long xyz_2_peak_time;

}fall_xyz_t;

typedef struct {
	uint8_t y_flag;
	uint8_t rel;
	long time;
	float y_fir_peak_value;
	long y_fir_peak_time;
	float y_sec_peak_value;
	long y_sec_peak_time;

}fall_y_t;

typedef struct {
	uint8_t xyz_flag;
	uint8_t y_flag;
	uint8_t rel;
	long time;
	float xyz_neg_peak_value;
	long xyz_neg_peak_time;
	float xyz_1_peak_value;
	long xyz_1_peak_time;
	float xyz_2_peak_value;
	long xyz_2_peak_time;
	float y_fir_peak_value;
	long y_fir_peak_time;
	float y_sec_peak_value;
	long y_sec_peak_time;
}temp_fall_t;

typedef struct {
	uint8_t flag;
	uint8_t rel;
	uint8_t detect_peak_mode;
	last_status_is_up_t last_status_is_up;
	direction_is_up_t direction_is_up;
	continue_up_former_count_t continue_up_former_count;
	continue_up_count_t continue_up_count;

	acc_value_mode_t acc_value_mode;

	float min_acc_value;
	float max_acc_value;

	peak_wave_t peak_wave;
	valley_wave_t valley_wave;

	long eff_time_of_last_peak_acc_xyz;
	long eff_time_of_last_peak;
	long eff_time_of_this_peak;
	long time_of_now;

	last_fall_t last_fall;
	temp_fall_t temp_fall;

	fall_y_t fall_y;
	fall_xyz_t fall_xyz;

	int test_fall_count;
	int xyz_zero_num;
}fall_env_t;


fall_env_t fall_env;
void init_fall_env();
void detect_new_fall_v2(float acc_y, float acc_xyz, int line);
void print_fall_result();

#endif

