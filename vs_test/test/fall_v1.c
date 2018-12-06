//=================================
//author wangzhibin_x@foxmail.com
//time 2018-12-06
//=================================
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fall_v1.h"

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
		//printf("wzb detect peak old v=%d,new v=%d\r\n",(int)old_value,(int)new_value);
		if ((fall_env.direction_is_up.acc_xyz2 == 0) && (fall_env.last_status_is_up.acc_xyz2 == 1) && (fall_env.continue_up_former_count.acc_xyz2 >= 1/*2*/ && (old_value >= fall_env.min_acc_value))) {
			fall_env.peak_wave.acc_xyz2 = old_value;
			//printf("wzb detect peak 11\r\n");
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

}

void init_fall_env()
{
	memset(&fall_env, 0, sizeof(fall_env));
	fall_env.max_acc_value = 50.0f;
	fall_env.min_acc_value = 0.0f;
}
