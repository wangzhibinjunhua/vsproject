#include "config.h"
#ifdef FALL_V2
//=================================
//author wangzhibin_x@foxmail.com
//time 2018-12-06
//=================================
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fall_v2.h"

static void reset_temp_y_flag();
static void reset_temp_all_flag();
static void reset_temp_xyz_flag();
static int detect_peak(float new_value, float old_value);
static float data_abs(float value);

static void copy_fall_y_to_temp();
static void reset_fall_y();
static void copy_fall_xyz_to_temp();
static void reset_fall_xyz();

static float data_abs(float value)
{
	return value > 0 ? value : -value;
}

static int detect_peak(float new_value, float old_value)
{
	if (fall_env.detect_peak_mode == ACC_PEAK_XYZ2) {
		fall_env.last_status_is_up.acc_xyz2 = fall_env.direction_is_up.acc_xyz2;
		if (new_value > old_value) {
			fall_env.direction_is_up.acc_xyz2 = 1;
			fall_env.continue_up_count.acc_xyz2++;
		}
		else {
			fall_env.continue_up_former_count.acc_xyz2 = fall_env.continue_up_count.acc_xyz2;
			fall_env.continue_up_count.acc_xyz2 = 0;

			fall_env.direction_is_up.acc_xyz2 = 0;
		}
		//printf("wzb detect xyz peak old v=%f,new v=%f\r\n",old_value,new_value);
		if ((fall_env.direction_is_up.acc_xyz2 == 0) && (fall_env.last_status_is_up.acc_xyz2 == 1) && (fall_env.continue_up_former_count.acc_xyz2 >= 2/*2*/ && (old_value >= fall_env.min_acc_value))) {
			fall_env.peak_wave.acc_xyz2 = old_value;
			printf("wzb detect xyz peak 11 old value=%f\r\n", old_value);
			return 1;
		}
		else if ((fall_env.last_status_is_up.acc_xyz2 == 0) && (fall_env.direction_is_up.acc_xyz2 == 1)) {
			fall_env.valley_wave.acc_xyz2 = old_value;
			return 0;
		}
		else {
			return 0;
		}
	}
	else if (fall_env.detect_peak_mode == ACC_PEAK_Y2) {
		fall_env.last_status_is_up.acc_y2 = fall_env.direction_is_up.acc_y2;
		if (new_value > old_value) {
			fall_env.direction_is_up.acc_y2 = 1;
			fall_env.continue_up_count.acc_y2++;
		}
		else {
			fall_env.continue_up_former_count.acc_y2 = fall_env.continue_up_count.acc_y2;
			fall_env.continue_up_count.acc_y2 = 0;
			fall_env.direction_is_up.acc_y2 = 0;
		}
		//printf("wzb detect peak y old v=%f,new v=%f\r\n",old_value,new_value);
		if ((fall_env.direction_is_up.acc_y2 == 0) && (fall_env.last_status_is_up.acc_y2 == 1) && (fall_env.continue_up_former_count.acc_y2 >= 1/*2*/ && (old_value >= fall_env.min_acc_value)) && (old_value <= fall_env.max_acc_value)) {
			fall_env.peak_wave.acc_y2 = old_value;
			//printf("wzb detect peak y 11\r\n");
			return 1;
		}
		else if ((fall_env.last_status_is_up.acc_y2 == 0) && (fall_env.direction_is_up.acc_y2 == 1)) {
			fall_env.valley_wave.acc_y2 = old_value;
			return 0;
		}
		else {
			return 0;
		}

	}
	return 0;

}

void detect_new_fall_v2(float acc_y, float acc_xyz, int line)
{
	float acc_xyz2;
	float acc_y2;
	acc_xyz2 = data_abs(acc_xyz);
	acc_y2 = data_abs(acc_y);
	fall_env.time_of_now = LINE_TO_TIME_MS(line);

	if (fall_env.time_of_now - fall_env.temp_fall.xyz_2_peak_time > 5000) {
		//printf("reset_temp_xyz_flag time  out of 5s line=%d\n", line);
		reset_temp_xyz_flag();
	}
	if (fall_env.time_of_now - fall_env.temp_fall.y_sec_peak_time > 3000) {
		//printf("reset_temp_y_flag time  out of 3s line=%d\n", line);
		reset_temp_y_flag();
	}

	if (fall_env.acc_value_mode.acc_y2_old == 0) {
		fall_env.acc_value_mode.acc_y2_old = acc_y2;
	}
	else {
		fall_env.detect_peak_mode = ACC_PEAK_Y2;
		fall_env.min_acc_value = 3.0f;

		fall_env.max_acc_value = 25.0f;
		if (detect_peak(acc_y2, fall_env.acc_value_mode.acc_y2_old)) {
			//printf("fall_env.last_fall.time=%ld\n", fall_env.last_fall.time);
			//printf("now-last=%ld\n", fall_env.time_of_now-fall_env.last_fall.time);
			printf("y flag=%d\n", fall_env.fall_y.y_flag);
			printf("y peak value=%f\n", fall_env.acc_value_mode.acc_y_old);
			if (fall_env.time_of_now - fall_env.last_fall.time > 3000) {
				if (fall_env.fall_y.y_flag == 0) {
					fall_env.fall_y.y_flag = 1;
					fall_env.fall_y.y_fir_peak_time = fall_env.time_of_now;
					fall_env.fall_y.y_fir_peak_value = fall_env.acc_value_mode.acc_y_old;
					printf("y flag=1,,, line=%d\n", line);
				}
				else if (fall_env.fall_y.y_flag == 1) {
					if (fall_env.acc_value_mode.acc_y_old*fall_env.fall_y.y_fir_peak_value < 0) {
						if (data_abs(fall_env.acc_value_mode.acc_y_old - fall_env.fall_y.y_fir_peak_value) > 10) {
							fall_env.fall_y.y_flag = 2;
							fall_env.fall_y.y_sec_peak_time = fall_env.time_of_now;
							fall_env.fall_y.y_sec_peak_value = fall_env.acc_value_mode.acc_y_old;
							printf("y flag=2,,, line=%d\n", line);
							copy_fall_y_to_temp();
							reset_fall_y();
				
						}
						else {
							fall_env.fall_y.y_flag = 1;
							fall_env.fall_y.y_fir_peak_time = fall_env.time_of_now;
							fall_env.fall_y.y_fir_peak_value = fall_env.acc_value_mode.acc_y_old;
							printf("y flag=1 reset line=%d\n", line);
						}
					}
					else {
						fall_env.fall_y.y_flag = 1;
						fall_env.fall_y.y_fir_peak_time = fall_env.time_of_now;
						fall_env.fall_y.y_fir_peak_value = fall_env.acc_value_mode.acc_y_old;
						printf("y flag=1 reset line=%d\n", line);
					}
				}
				
			}

		}
		fall_env.acc_value_mode.acc_y2_old = acc_y2;
		fall_env.acc_value_mode.acc_y_old = acc_y;
	}

	if (fall_env.acc_value_mode.acc_xyz2_old == 0) {
		fall_env.acc_value_mode.acc_xyz2_old = acc_xyz2;
	}
	else {
		fall_env.detect_peak_mode = ACC_PEAK_XYZ2;
		fall_env.min_acc_value = 0.6f;
		fall_env.max_acc_value = 50.0f;
		if (detect_peak(acc_xyz2, fall_env.acc_value_mode.acc_xyz2_old)) {
			printf("detect_peak xyz line=%d,xyz flag=%d,value=%f\n", line, fall_env.fall_xyz.xyz_flag, fall_env.acc_value_mode.acc_xyz_old);
			if (fall_env.time_of_now - fall_env.last_fall.time > 3000) {
				if (fall_env.acc_value_mode.acc_xyz_old < -1.0) {

					reset_fall_xyz();
				}
				if (fall_env.fall_xyz.xyz_flag == 0) {
					if (fall_env.acc_value_mode.acc_xyz_old < -0.6) {
						fall_env.fall_xyz.xyz_flag = 1;
						printf("xyz flag=1,line=%d\n", line);
						fall_env.fall_xyz.xyz_neg_peak_time = fall_env.time_of_now;
						fall_env.fall_xyz.xyz_neg_peak_value = fall_env.acc_value_mode.acc_xyz_old;
					}
				}
				else if (fall_env.fall_xyz.xyz_flag == 1) {
					if (fall_env.time_of_now - fall_env.fall_xyz.xyz_neg_peak_time > 1500) {
						reset_fall_xyz();
						printf("reset_temp_xyz_flag,line=%d\n", line);
					}
					else {
						if (fall_env.acc_value_mode.acc_xyz_old > 5 && fall_env.acc_value_mode.acc_xyz_old < 20) {
							fall_env.fall_xyz.xyz_flag = 2;
							printf("xyz_flag=2,line=%d\n", line);
							fall_env.fall_xyz.xyz_1_peak_time = fall_env.time_of_now;
							fall_env.fall_xyz.xyz_1_peak_value = fall_env.acc_value_mode.acc_xyz_old;
						}
					}

				}
				else if (fall_env.fall_xyz.xyz_flag == 2) {
					if (fall_env.time_of_now - fall_env.fall_xyz.xyz_neg_peak_time > 2000) {
						reset_fall_xyz();
						printf("reset_temp_xyz_flag 222 ,line=%d\n", line);
					}
					else {
						if (data_abs(fall_env.acc_value_mode.acc_xyz_old) > 5 && fall_env.acc_value_mode.acc_xyz_old < 20) {
							fall_env.fall_xyz.xyz_flag = 3;
							printf("xyz_flag=3,line=%d\n", line);
							fall_env.fall_xyz.xyz_2_peak_time = fall_env.time_of_now;
							fall_env.fall_xyz.xyz_2_peak_value = fall_env.acc_value_mode.acc_xyz_old;

							copy_fall_xyz_to_temp();
							reset_fall_xyz();
						}
					}
				}
			}
		}
		fall_env.acc_value_mode.acc_xyz2_old = acc_xyz2;
		fall_env.acc_value_mode.acc_xyz_old = acc_xyz;
	}

	//¼ÆËã
	/*if (fall_env.temp_fall.y_flag == 2 && fall_env.temp_fall.xyz_flag == 3) {
		printf("temp y_flag=2,temp xyz_flag=3,line=%d\n", line);
		if (data_abs(fall_env.temp_fall.xyz_2_peak_time - fall_env.temp_fall.y_sec_peak_time) < 2500) {
			printf("#####################################\n");
			printf("###########FALL FALL FALL ##############\n");
			fall_env.test_fall_count++;
			fall_env.last_fall.time = fall_env.time_of_now + 200;
			reset_temp_all_flag();
		}
		else {
			reset_temp_xyz_flag();
		}
	}*/
	if (fall_env.temp_fall.y_flag == 2 && fall_env.temp_fall.xyz_flag == 3) {
		printf("temp y_flag=2,temp xyz_flag=3,line=%d\n", line);
		if (fall_env.time_of_now - fall_env.temp_fall.time < 500) {
			fall_env.xyz_zero_num = 0;
		}
		if (fall_env.time_of_now - fall_env.temp_fall.time > 400) {
			printf("temp y_flag=2,temp xyz_flag=3,xyz_zero_num=%d,line=%d\n", fall_env.xyz_zero_num, line);
			if (acc_xyz2 < 1.0) {
				fall_env.xyz_zero_num++;


			}
#if 1
			if (acc_xyz2 > 2.9) fall_env.xyz_zero_num--;
#endif
			if (fall_env.xyz_zero_num > 20) {
				printf("#####################################\n");
				printf("###########FALL FALL FALL ##############\n");
				fall_env.test_fall_count++;
				fall_env.last_fall.time = fall_env.time_of_now;
				reset_temp_all_flag();
			}
		}
	}

}


static void copy_fall_xyz_to_temp()
{
	printf("copy_fall_xyz_to_temp \n");
	if (fall_env.fall_xyz.xyz_neg_peak_time < fall_env.last_fall.time) {
		reset_fall_xyz();
		printf("wzb copy_fall_xyz_to_temp  reset \n");
		return;
	}
	fall_env.temp_fall.xyz_flag = fall_env.fall_xyz.xyz_flag;
	fall_env.temp_fall.xyz_neg_peak_time = fall_env.fall_xyz.xyz_neg_peak_time;
	fall_env.temp_fall.xyz_neg_peak_value = fall_env.fall_xyz.xyz_neg_peak_value;
	fall_env.temp_fall.xyz_1_peak_time = fall_env.fall_xyz.xyz_1_peak_time;
	fall_env.temp_fall.xyz_1_peak_value = fall_env.fall_xyz.xyz_1_peak_value;
	fall_env.temp_fall.xyz_2_peak_time = fall_env.fall_xyz.xyz_2_peak_time;
	fall_env.temp_fall.xyz_2_peak_value = fall_env.fall_xyz.xyz_2_peak_value;
	fall_env.temp_fall.time= fall_env.fall_xyz.xyz_2_peak_time;
}

static void reset_fall_xyz()
{
	fall_env.fall_xyz.xyz_flag = 0;
}


static void copy_fall_y_to_temp()
{
	printf("copy_fall_y_to_temp \n");
	if (fall_env.fall_y.y_fir_peak_time < fall_env.last_fall.time) {
		reset_fall_y();
		printf("wzb copy_fall_y_to_temp  reset \n");
		return;
	}
	fall_env.temp_fall.y_flag = fall_env.fall_y.y_flag;
	fall_env.temp_fall.y_fir_peak_time = fall_env.fall_y.y_fir_peak_time;
	fall_env.temp_fall.y_fir_peak_value = fall_env.fall_y.y_fir_peak_value;
	fall_env.temp_fall.y_sec_peak_time = fall_env.fall_y.y_sec_peak_time;
	fall_env.temp_fall.y_sec_peak_value = fall_env.fall_y.y_sec_peak_value;
	fall_env.temp_fall.time= fall_env.fall_y.y_sec_peak_time;
}

static void reset_fall_y()
{
	fall_env.fall_y.y_flag = 0;
}

static void reset_temp_y_flag()
{
	fall_env.temp_fall.y_flag = 0;
}

static void reset_temp_all_flag()
{
	fall_env.temp_fall.xyz_flag = 0;
	fall_env.temp_fall.y_flag = 0;
}

static void reset_temp_xyz_flag()
{
	fall_env.temp_fall.xyz_flag = 0;
}

void init_fall_env()
{
	memset(&fall_env, 0, sizeof(fall_env));
	fall_env.max_acc_value = 50.0f;
	fall_env.min_acc_value = 0.0f;
}

void print_fall_result()
{
	printf("============================\n");
	printf("fall count=%d\n", fall_env.test_fall_count);
	printf("============================\n");
}
#endif