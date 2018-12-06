#pragma once
//=================================
//author wangzhibin_x@foxmail.com
//time 2018-12-06
//=================================

#define DATA_ABS(x) ((x)>0? (x):(-x))
//xyz2 means |xyz|

#define LINE_TO_TIME_MS (20.0f)
typedef unsigned char uint8_t;

typedef enum {
	ACC_PEAK_XYZ=0x0,
	ACC_PEAK_XYZ2=0x1,
}detect_peak_acc_mode_t;

typedef struct {
	float acc_xyz_old;
	float acc_xyz2_old;
}acc_value_mode_t;

typedef struct {
	uint8_t acc_xyz;
	uint8_t acc_xyz2;

}last_status_is_up_t;

typedef struct {
	uint8_t acc_xyz;
	uint8_t acc_xyz2;

}direction_is_up_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
} continue_up_count_t;

typedef struct {
	int acc_xyz;
	int acc_xyz2;
} continue_up_former_count_t;

typedef struct{
	float acc_xyz;
	float acc_xyz2;

} peak_wave_t;

typedef struct {
	float acc_xyz;
	float acc_xyz2;
}valley_wave_t;

typedef struct {
	uint8_t rel;
	long time;
}last_fall_t;

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

}fall_env_t;


fall_env_t fall_env;
void init_fall_env();
void detect_new_fall_v1(float acc_xyz,int line);




