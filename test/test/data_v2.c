#ifdef BLE_USE_DATA_V2
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "data_v2.h"

#define WEIBIN_PARAMETER

static void reset_jump_state();
//static step_env_t step_env;

float jump_time_height[100][2] = { 0 };
int jump_row = 0;
int jump_col = 0;

///initialization parameter
#ifdef WEIBIN_PARAMETER
float acc_x_abs_min_normal = 15.0f;//25.2f;
float acc_y_abs_min_normal = 15.0f;//25.2f;
float acc_x_abs_min_run = 37.4f;
float acc_y_abs_min_run = 37.4f;
float acc_z_abs_min_jump = 56.0f;//15.0f;//40.0f;
int acc_y_min_interval_step = 500;//ms

int acc_y_min_interval_normal = 1260;//ms
int acc_y_min_interval_run = 720;//ms
int acc_y_min_interval_dash = 600;//ms
#else
float acc_x_abs_min_normal = 15.0f;//28.2f;
float acc_y_abs_min_normal = 15.0f;//28.2f;
float acc_x_abs_min_run = 37.4f;
float acc_y_abs_min_run = 37.4f;
float acc_z_abs_min_jump = 40.0f;//15.0f;//40.0f;
#endif


static int detect_peak(float new_value, float old_value)
{
	if (step_env.detect_peak_mode == ACC_PEAK_X2) {
		step_env.last_status_is_up.acc_x2 = step_env.direction_is_up.acc_x2;
		if (new_value > old_value) {
			step_env.direction_is_up.acc_x2 = 1;
			step_env.continue_up_count.acc_x2++;
		}
		else {
			step_env.continue_up_former_count.acc_x2 = step_env.continue_up_count.acc_x2;
			step_env.continue_up_count.acc_x2 = 0;
			step_env.direction_is_up.acc_x2 = 0;
		}
		//printf("wzb detect peak old v=%d,new v=%d\r\n",(int)old_value,(int)new_value);
		if ((step_env.direction_is_up.acc_x2 == 0) && (step_env.last_status_is_up.acc_x2 == 1) && (step_env.continue_up_former_count.acc_x2 >= 1/*2*/ && (old_value >= step_env.min_acc_value))) {
			step_env.peak_wave.acc_x2 = old_value;
			//printf("wzb detect peak 11\r\n");
			return 1;
		}
		else if ((step_env.last_status_is_up.acc_x2 == 0) && (step_env.direction_is_up.acc_x2 == 1)) {
			step_env.valley_wave.acc_x2 = old_value;
			return 0;
		}
		else {
			return 0;
		}
	}
	else if (step_env.detect_peak_mode == ACC_PEAK_Y2) {
		step_env.last_status_is_up.acc_y2 = step_env.direction_is_up.acc_y2;
		if (new_value > old_value) {
			step_env.direction_is_up.acc_y2 = 1;
			step_env.continue_up_count.acc_y2++;
		}
		else {
			step_env.continue_up_former_count.acc_y2 = step_env.continue_up_count.acc_y2;
			step_env.continue_up_count.acc_y2 = 0;
			step_env.direction_is_up.acc_y2 = 0;
		}
		//printf("wzb detect peak y old v=%f,new v=%f\r\n",old_value,new_value);
		if ((step_env.direction_is_up.acc_y2 == 0) && (step_env.last_status_is_up.acc_y2 == 1) && (step_env.continue_up_former_count.acc_y2 >= 1/*2*/ && (old_value >= step_env.min_acc_value))) {
			step_env.peak_wave.acc_y2 = old_value;
			// printf("wzb detect peak y 11\r\n");
			return 1;
		}
		else if ((step_env.last_status_is_up.acc_y2 == 0) && (step_env.direction_is_up.acc_y2 == 1)) {
			step_env.valley_wave.acc_y2 = old_value;
			return 0;
		}
		else {
			return 0;
		}

	}
	else if (step_env.detect_peak_mode == ACC_PEAK_Z) {
		step_env.last_status_is_up.acc_z = step_env.direction_is_up.acc_z;
		if (new_value > old_value) {
			step_env.direction_is_up.acc_z = 1;
			step_env.continue_up_count.acc_z++;
		}
		else {
			step_env.continue_up_former_count.acc_z = step_env.continue_up_count.acc_z;
			step_env.continue_up_count.acc_z = 0;
			step_env.direction_is_up.acc_z = 0;
		}
		//printf("wzb detect peak z old v=%d,new v=%d\r\n", (int)old_value, (int)new_value);
		if ((step_env.direction_is_up.acc_z == 0) && (step_env.last_status_is_up.acc_z == 1) && (step_env.continue_up_former_count.acc_z >= 1/*2*/ && (old_value >= step_env.min_acc_value))) {
			step_env.peak_wave.acc_z = old_value;
			//printf("wzb detect peak z 11\r\n");
			return 1;
		}
		else if ((step_env.last_status_is_up.acc_z == 0) && (step_env.direction_is_up.acc_z == 1)) {
			step_env.valley_wave.acc_z = old_value;
			return 0;
		}
		else {
			return 0;
		}

	}
}

static long inv_get_tick_count(int line)
{
	return line*LINE_TO_TICK;
}



void detect_new_step_v2(float acc_x2, float acc_y2, float acc_z, int line)
{
	if (step_env.acc_value_mode.acc_x2_old == 0) {
		step_env.acc_value_mode.acc_x2_old = acc_x2;
	}
	else {
		step_env.detect_peak_mode = ACC_PEAK_X2;
		step_env.min_acc_value = acc_x_abs_min_normal;
		if (detect_peak(acc_x2, step_env.acc_value_mode.acc_x2_old)) {
			step_env.eff_time_of_last_peak = step_env.eff_time_of_this_peak;
			step_env.time_of_now = inv_get_tick_count(line);

			if ((step_env.time_of_now - step_env.eff_time_of_last_peak)*TICK_TO_MS >= 400
				&& step_env.peak_wave.acc_x2 >= step_env.min_acc_value && ((step_env.time_of_now - step_env.jump.fir_peak_time) > 80)) {

				step_env.eff_time_of_this_peak = step_env.time_of_now;

				step_env.flag = 1;
				//reset_jump_state();
				//printf("wzb x2 step\r\n");
				step_env.type = HORIZONTAL;
				printf("wzb x step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak=%f\n", (step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS);
				printf("wzb x line=%d\n", line);

				if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS <= 550) {
					step_env.mode = DASH;
				}
				else if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS <= 720) {
					step_env.mode = RUN;
				}
				else {
					step_env.mode = WALK;
				}

				//write_to_flash(current_step,current_run_count,current_dash_count);
			}

		}
		step_env.acc_value_mode.acc_x2_old = acc_x2;
	}

#if 1
	if (step_env.acc_value_mode.acc_y2_old == 0) {
		step_env.acc_value_mode.acc_y2_old = acc_y2;
	}
	else {
		step_env.detect_peak_mode = ACC_PEAK_Y2;
		step_env.min_acc_value = acc_y_abs_min_normal;
		if (detect_peak(acc_y2, step_env.acc_value_mode.acc_y2_old)) {
			step_env.eff_time_of_last_peak = step_env.eff_time_of_this_peak;
			step_env.time_of_now = inv_get_tick_count(line);

			if ((step_env.time_of_now - step_env.eff_time_of_last_peak)*TICK_TO_MS >= acc_y_min_interval_step
				&& step_env.peak_wave.acc_y2 >= step_env.min_acc_value && ((step_env.time_of_now - step_env.jump.fir_peak_time) > 80)) {

				step_env.eff_time_of_this_peak = step_env.time_of_now;

				step_env.flag = 1;
				//reset_jump_state();
				step_env.type = VERTICAL;
				printf("wzb y step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak=%f\n", (step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS);
				printf("wzb y line=%d\n", line);
				if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS <= acc_y_min_interval_dash) {
					step_env.mode = DASH;
					printf("wzb dash\n");
				}
				else if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak)*TICK_TO_MS <= acc_y_min_interval_run) {
					step_env.mode = RUN;
					printf("wzb run\n");
				}
				else {
					step_env.mode = WALK;
				}

				//write_to_flash(current_step,current_run_count,current_dash_count);
			}

		}
		step_env.acc_value_mode.acc_y2_old = acc_y2;
	}
#endif

#if 1
	if (step_env.acc_value_mode.acc_z_old == 0) {
		step_env.acc_value_mode.acc_z_old = acc_z;
	}
	else {
		step_env.detect_peak_mode = ACC_PEAK_Z;
		step_env.min_acc_value = acc_z_abs_min_jump;
		if (detect_peak(acc_z, step_env.acc_value_mode.acc_z_old)) {
			step_env.eff_time_of_last_peak = step_env.eff_time_of_this_peak;
			step_env.time_of_now = inv_get_tick_count(line);

			if (step_env.peak_wave.acc_z >= step_env.min_acc_value) {

				if (step_env.jump.fir_peak_time == 0) {
					step_env.jump.fir_peak_time = step_env.time_of_now;
					step_env.jump.flag = 1;
					printf("wzb jump.flag=1; line=%d\n",line);
				}
				else {
					step_env.jump.sec_peak_time = step_env.time_of_now;
					step_env.jump.flag = 2;
					printf("wzb jump.flag=2;line=%d\n",line);
				}


				//write_to_flash(current_step,current_run_count,current_dash_count);
			}

			if (step_env.jump.flag == 2) {
				if ((step_env.jump.sec_peak_time - step_env.jump.fir_peak_time) < 408) {
					//printf("wzb z step 1\r\n");
					step_env.eff_time_of_this_peak = step_env.time_of_now;
					step_env.flag = 1;
					step_env.type = JUMP;
					printf("wzb jump step \r\n");
					step_env.jump.time = (step_env.jump.sec_peak_time - step_env.jump.fir_peak_time - 22)*TICK_TO_MS;
					reset_jump_state();
				}
				else {
					step_env.jump.flag = 1;
					step_env.jump.fir_peak_time = step_env.time_of_now;
				}
				//reset_jump_state();
			}

		}
		step_env.acc_value_mode.acc_z_old = acc_z;
	}
#endif


	// statistical data
	if (step_env.flag == 1) {
		//step_env.total_step++;
		step_env.flag = 0;


		if (step_env.type == VERTICAL) {
			step_env.total_step += 2;
			if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak) == 0) {
				step_env.frequency = 1;
			}
			else {
				step_env.frequency = 2 * 1000 / (step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak);
			}
			switch (step_env.mode) {
			case WALK:

				if (step_env.frequency < 2) {
					step_env.stride = 0.64f; //default slow step length
				}
				if (step_env.frequency >= 2 && step_env.frequency < 2.8) {
					step_env.stride = 0.2802*step_env.frequency + 0.09376;
				}
				else {
					step_env.stride = 0.878f;//default fast step length
				}
				break;
			case RUN:
				step_env.total_run += 2;
				if (step_env.frequency < 3) {
					step_env.stride = 0.75f;
				}

				if (step_env.frequency >= 3 && step_env.frequency < 3.4) {
					step_env.stride = 0.9222*step_env.frequency - 1.981;
				}
				else {
					step_env.stride = 1.15f;
				}
				break;
			case DASH:
				//default:
				step_env.total_dash += 2;
				step_env.stride = 1.35f;
				break;
			default:
				break;
			}

			step_env.distance += step_env.stride * 2;
		}
		else if (step_env.type == HORIZONTAL) {
			step_env.total_step++;
			step_env.h_step++;
			if ((step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak) == 0) {
				step_env.frequency = 1;
			}
			else {
				step_env.frequency = 1000 / (step_env.eff_time_of_this_peak - step_env.eff_time_of_last_peak);
			}
			switch (step_env.mode) {
			case WALK:
				if (step_env.frequency < 2) {
					step_env.stride = 0.64f; //default slow step length
				}
				if (step_env.frequency >= 2 && step_env.frequency < 2.8) {
					step_env.stride = 0.2802*step_env.frequency + 0.09376;
				}
				else {
					step_env.stride = 0.878f;//default fast step length
				}
				break;
			case RUN:
				step_env.total_run++;
				step_env.h_run++;
				if (step_env.frequency < 3) {
					step_env.stride = 0.75f;
				}

				if (step_env.frequency >= 3 && step_env.frequency < 3.4) {
					step_env.stride = 0.9222*step_env.frequency - 1.981;
				}
				else {
					step_env.stride = 1.15f;
				}
				break;
			case DASH:
				//default:
				step_env.total_dash++;
				step_env.h_dash++;
				step_env.stride = 1.35f;
				break;
			default:
				break;
			}

			step_env.distance += step_env.stride;
			step_env.h_distance += step_env.stride;
		}
		else if (step_env.type == JUMP) {
			step_env.jump.count++;
			step_env.jump.height = DATA_JUMP_HEIGHT(step_env.jump.time/1000/2);

			jump_time_height[jump_row][0] = step_env.jump.time;
			jump_time_height[jump_row][1] = step_env.jump.height;
			jump_row++;

#if 1
			printf("wzb jump -- step step_env.eff_time_of_this_peak - step_env.last_step.time=%f\n", step_env.eff_time_of_this_peak - step_env.last_step.time);
			if ((step_env.eff_time_of_this_peak - step_env.last_step.time) < 80) {
				if (step_env.last_step.type == VERTICAL) {
					step_env.total_step -= 2;
					switch (step_env.last_step.mode) {
					case WALK:
						//step_env.total_step-=2;
						break;
					case RUN:
						step_env.total_run -= 2;
						break;
					case DASH:
						step_env.total_dash -= 2;
						break;
					default:
						break;
					}
					step_env.distance -= 2 * step_env.last_step.length;
				}
				else if (step_env.last_step.type == HORIZONTAL) {
					step_env.total_step -= 1;
					step_env.h_step -= 1;
					switch (step_env.last_step.mode) {
					case WALK:
						//step_env.total_step-=2;
						break;
					case RUN:
						step_env.h_run--;
						step_env.total_run--;
						break;
					case DASH:
						step_env.h_dash--;
						step_env.total_dash--;
						break;
					default:
						break;
					}
					step_env.distance -= step_env.last_step.length;
					step_env.h_distance -= step_env.last_step.length;
				}
			}
#endif
		}

		printf("wzb total step=%d,total run=%d,total_dash=%d\n", step_env.total_step,step_env.total_run,step_env.total_dash);
		printf("wzb h step=%d,h run=%d,h dash=%d\n", step_env.h_step, step_env.h_run, step_env.h_dash);
		step_env.last_step.type = step_env.type;
		step_env.last_step.mode = step_env.mode;
		step_env.last_step.time = step_env.eff_time_of_this_peak;
		step_env.last_step.length = step_env.stride;


	}

	

}

void print_result()
{
	printf("###################################################\n");
	printf("total step=%d\n", step_env.total_step);
	printf("total run=%d\n", step_env.total_run);
	printf("total dash=%d\n", step_env.total_dash);
	printf("h step=%d\n", step_env.h_step);
	printf("h run=%d\n", step_env.h_run);
	printf("h dash=%d\n", step_env.h_dash);
	printf("jump count=%d\n", step_env.jump.count);
	printf("total distance=%f\n", step_env.distance);
	printf("h distance=%f\n", step_env.h_distance);
	printf("###################################################\n");
	for (int i = 0; i < 20; i++) {
		printf("jump time=%f,height=%f\n", jump_time_height[i][0], jump_time_height[i][1]);
	}
}


static void reset_jump_state()
{
	step_env.jump.flag = 0;
	step_env.jump.sec_peak_time = 0;
	step_env.jump.fir_peak_time = 0;
}

void init_step_env()
{
	step_env.flag = 0;
	step_env.frequency = 0;

	step_env.acc_value_mode.acc_x2_old = 0;
	step_env.acc_value_mode.acc_y2_old = 0;
	step_env.acc_value_mode.acc_z2_old = 0;
	step_env.acc_value_mode.acc_x_old = 0;
	step_env.acc_value_mode.acc_y_old = 0;
	step_env.acc_value_mode.acc_z_old = 0;

	step_env.continue_up_count.acc_x = 0;
	step_env.continue_up_count.acc_y = 0;
	step_env.continue_up_count.acc_z = 0;
	step_env.continue_up_count.acc_x2 = 0;
	step_env.continue_up_count.acc_y2 = 0;
	step_env.continue_up_count.acc_z2 = 0;
	step_env.continue_up_former_count.acc_x = 0;
	step_env.continue_up_former_count.acc_y = 0;
	step_env.continue_up_former_count.acc_z = 0;
	step_env.continue_up_former_count.acc_x2 = 0;
	step_env.continue_up_former_count.acc_y2 = 0;
	step_env.continue_up_former_count.acc_z2 = 0;

	step_env.direction_is_up.acc_x = 0;
	step_env.direction_is_up.acc_y = 0;
	step_env.direction_is_up.acc_z = 0;
	step_env.direction_is_up.acc_x2 = 0;
	step_env.direction_is_up.acc_y2 = 0;
	step_env.direction_is_up.acc_z2 = 0;

	step_env.last_status_is_up.acc_x = 0;
	step_env.last_status_is_up.acc_y = 0;
	step_env.last_status_is_up.acc_z = 0;
	step_env.last_status_is_up.acc_x2 = 0;
	step_env.last_status_is_up.acc_y2 = 0;
	step_env.last_status_is_up.acc_z2 = 0;

	step_env.type = 0xff;
	step_env.mode = 0xff;

	step_env.detect_peak_mode = 0xff;

	step_env.distance = 0;
	step_env.stride = 0;
	step_env.h_distance = 0;

	step_env.peak_wave.acc_x = 0;
	step_env.peak_wave.acc_y = 0;
	step_env.peak_wave.acc_z = 0;
	step_env.peak_wave.acc_x2 = 0;
	step_env.peak_wave.acc_y2 = 0;
	step_env.peak_wave.acc_z2 = 0;
	step_env.valley_wave.acc_x = 0;
	step_env.valley_wave.acc_y = 0;
	step_env.valley_wave.acc_z = 0;
	step_env.valley_wave.acc_x2 = 0;
	step_env.valley_wave.acc_y2 = 0;
	step_env.valley_wave.acc_z2 = 0;


	step_env.total_step = 0;
	step_env.total_run = 0;
	step_env.total_dash = 0;

	step_env.h_step = 0;
	step_env.h_run = 0;
	step_env.h_dash = 0;

	step_env.jump.count = 0;
	step_env.jump.fir_peak_time = 0;
	step_env.jump.sec_peak_time = 0;
	step_env.jump.flag = 0;
	step_env.jump.height = 0;
	step_env.jump.time = 0;

	step_env.min_acc_value = 15.0f;
	step_env.max_acc_value = 2000.0f;

	step_env.eff_time_of_last_peak = 0;
	step_env.eff_time_of_this_peak = 0;
	step_env.time_of_now = 0;

	step_env.last_step.length = 0;
	step_env.last_step.mode = 0xff;
	step_env.last_step.type = 0xff;
	step_env.last_step.time = 0;

}
#endif

