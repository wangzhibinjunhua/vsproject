#include <stdio.h>
#include "lp.h"
#include <math.h>
#include "readCSVFile.h"

#include <stdlib.h>

#include "data_v5.h"

//const float filter[8] = {0.0779,0.1124,0.1587,0.1867,0.1867,0.1587,0.1124,0.0779};
const float filter[8] = { 0.0779,0.1124,0.1587,0.1867,0.1867,0.1587,0.1124,0.0779};
static long sample_counter=0;

static float samples_buf[3][8] = { 0 };

static float samples_filter(float input,uint8_t type) {
	float output=0.0f;
	if (sample_counter <7) {
		samples_buf[type][sample_counter+1] = input;
		output = input;
	}
	else {
		for (int i = 1; i <8; i++) {
			samples_buf[type][i-1] = samples_buf[type][i];
		}
		samples_buf[type][7] = input;

		for (int i = 0; i < 8; i++) {
			output += filter[i] * samples_buf[type][i];
		}

	}
	return output;
}

static float data_abs(float value)
{
	return value > 0 ? value : -value;
}

static void data_v2(FILE *file)
{
	init_step_env();
	printf("giNumRow=%d\n", giNumRow);

	int i = 0;
	for (i = 0; i < giNumRow; i++) {
	//for (i = 0; i < 1200; i++) {
		//detect_new_step_v2(samples_filter(giCsvData[i*giNumCol+1],0), samples_filter(giCsvData[i*giNumCol+2],1), (giCsvData[i*giNumCol+3]),i);
		detect_new_step_v5(samples_filter(giCsvData[i*giNumCol+1],0), samples_filter(giCsvData[i*giNumCol+2],1), data_abs(giCsvData[i*giNumCol+3]),i, giCsvData[i*giNumCol + 4],-5.6f,file);
		//detect_new_step_v2((giCsvData[i*giNumCol + 1]), (giCsvData[i*giNumCol + 2]),(giCsvData[i*giNumCol + 3]), i);
		sample_counter++;
	}
	print_result();
	getchar();
}
FILE *file;
int main(int argc, char **argv)
{
	//char *filename = "e:/python-project/test/num/ao_031602.txt";
	//char *filename = "e:/python-project/test/num/ag_0304_jump.txt";

	init_step_env();

	char *filename = "e:/python-project/test/num/ao_02_01.txt";
	ReadCsvData(filename);           
	//ShowCsvData();     
	fopen_s(&file, "xy.txt", "w+");

	data_v2(file);
	FreeCsvData();                   


	fclose(file);
	getchar();
	return 0;
}