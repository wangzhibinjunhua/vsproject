#include <stdio.h>
#include "lp.h"
#include <math.h>
#include "readCSVFile.h"
#include <math.h>
#include <stdlib.h>
#include "config.h"
//#define FALL_V1
//#define BLE_DATA
#ifdef BLE_DATA
#include "data_v5.h"
#endif 

#ifdef FALL_V1
#include "fall_v1.h"
#endif

#ifdef FALL_V2
#include "fall_v2.h"
#endif


#ifdef FALL_NEW_V1
#include "fall_new_v1.h"
#include "fall_new_p.h"
#endif
//const float filter[8] = {0.0779,0.1124,0.1587,0.1867,0.1867,0.1587,0.1124,0.0779};
const float filter[8] = { 0.0779,0.1124,0.1587,0.1867,0.1867,0.1587,0.1124,0.0779};
static long sample_counter=0;

static float samples_buf[3][8] = { 0 };

static float samples_filter(float input,int type) {
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


#ifdef BLE_DATA
static void data_v2(FILE *file)
{
	init_step_env();
	printf("giNumRow=%d\n", giNumRow);

	int i = 0;
	for (i = 0; i < giNumRow; i++) {
	//for (i = 0; i < 1000; i++) {
		//detect_new_step_v2(samples_filter(giCsvData[i*giNumCol+1],0), samples_filter(giCsvData[i*giNumCol+2],1), (giCsvData[i*giNumCol+3]),i);
		detect_new_step_v5(samples_filter(giCsvData[i*giNumCol+1],0), samples_filter(giCsvData[i*giNumCol+2],1), (giCsvData[i*giNumCol+3]),i, giCsvData[i*giNumCol + 4],-5.6f,file);
		//detect_new_step_v2((giCsvData[i*giNumCol + 1]), (giCsvData[i*giNumCol + 2]),(giCsvData[i*giNumCol + 3]), i);
		sample_counter++;
	}
	print_result();
	getchar();
}
#endif


#ifdef FALL_V1
static fall_data_v1()
{
	init_fall_env();
	printf("giNumRow=%d\n", giNumRow);
	int i = 0;
	for (i = 0; i < giNumRow; i++) {
		//for (i = 2200; i < 3000; i++) {
		float x,y,z,xyz;
		x = giCsvData[i*giNumCol + 1];
		y= giCsvData[i*giNumCol + 2];
		z=giCsvData[i*giNumCol + 3];
		//printf("x=%f,y=%f,z=%f\n", x, y, z);
		xyz = (float)sqrt(x*x + y*y + z*z);
		detect_new_fall_v1(samples_filter(giCsvData[i*giNumCol + 2], 0), samples_filter(xyz, 1)-9.8, i);
		
		sample_counter++;
	}
		print_fall_result();
	getchar();
}

#endif

#ifdef FALL_V2
static fall_data_v2()
{
	init_fall_env();
	printf("giNumRow=%d\n", giNumRow);
	int i = 0;
	for (i = 0; i < giNumRow; i++) {
		//for (i = 3269; i < 3900; i++) {
		float x, y, z, xyz;
		x = giCsvData[i*giNumCol + 1];
		y = giCsvData[i*giNumCol + 2];
		z = giCsvData[i*giNumCol + 3];
		//printf("x=%f,y=%f,z=%f\n", x, y, z);
		xyz = (float)sqrt(x*x + y*y + z*z);
		detect_new_fall_v2(samples_filter(giCsvData[i*giNumCol + 2], 0), samples_filter(xyz, 1) - 9.8, i);

		sample_counter++;
	}
	print_fall_result();
	getchar();
}

#endif


#ifdef FALL_NEW_V1
static fall_new_data_v1()
{
	StateStructure *Blank = (StateStructure *)malloc(sizeof(StateStructure));
	Blank->Buffer = (double *)malloc(sizeof(double)*SIZE_BUFFER);
	detector_init(Blank);
	printf("giNumRow=%d\n", giNumRow);
	int i = 0;
	for (i = 0; i < giNumRow; i++) {
		//for (i = 3269; i < 3900; i++) {
		double x, y, z, xyz;
		x = giCsvData[i*giNumCol + 1];
		y = giCsvData[i*giNumCol + 2];
		z = giCsvData[i*giNumCol + 3];
		//printf("x=%f,y=%f,z=%f\n", x, y, z);
		//xyz = (float)sqrt(x*x + y*y + z*z);
		//detect_new_fall_v2(samples_filter(giCsvData[i*giNumCol + 2], 0), samples_filter(xyz, 1) - 9.8, i);
		Sampled(x, y, z, i, Blank);
		//sample_counter++;
	}
	print_fall_new_result(Blank);
	getchar();
}

#endif

FILE *file;

typedef struct {
	int *b;
	int a ;
	

}ts;

int main(int argc, char **argv)
{


 #ifdef BLE_DATA
	//char *filename = "e:/python-project/test/num/ao_031602.txt";
	//char *filename = "e:/python-project/test/num/ag_0304_jump.txt";
	char *filename = "d:/wzb/project/python-project/test/num/ao_0410_01_3.txt";
	ReadCsvData(filename);           
	//ShowCsvData();     
	fopen_s(&file, "xy.txt", "w+");
	data_v2(file);
	FreeCsvData();                   

	fclose(file);
#endif


#ifdef FALL_V1

	
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-39-35testwb6ci.txt";
	char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-45-20testwzb5ci.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/aaduoci002.txt";
	ReadCsvData(filename);
	fall_data_v1();
	FreeCsvData();
#endif
#ifdef FALL_V2


	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-39-35testwb6ci.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-45-20testwzb5ci.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/aaduoci002.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-47-53testwzbjiade.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-02-55-17test.txt";
	char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-39-35testwb6ci.txt";

	ReadCsvData(filename);
	fall_data_v2();
	FreeCsvData();

#endif

#ifdef FALL_NEW_V1
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-39-35testwb6ci.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-25-06-05-58testjiade.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/aaduoci002.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-05-47-53testwzbjiade.txt";
	//char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2018-12-13-02-55-17test.txt";
	char *filename = "d:/wzb/project/python-project/py_work/num/testdata/2019-01-04-06-17-40test3ci.txt";
	ReadCsvData(filename);
	fall_new_data_v1();
	FreeCsvData();
#endif

	getchar();
	return 0;
}