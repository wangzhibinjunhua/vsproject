
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "readCSVFile.h"
#include <math.h>
#include "findSmallestPolygon.h"
#include "mat.h"

void test_findsp()
{
	char *filename = "d:/wzb/project/python-project/test/num/xy100.txt";
	ReadCsvData(filename);
	printf("giNumRow=%d,giNumCol=%d\n", giNumRow, giNumCol);
	//ShowCsvData();
	double *polyX, *polyY, *x, *y;
	long *polyCorners;
	int N = giNumRow*sizeof(double);
	//int N = giNumRow;
	x = (double*)malloc(N);
	y = (double*)malloc(N);
	polyX = (double*)malloc(N);
	polyY = (double*)malloc(N);

	polyCorners = (long*)malloc(giNumRow*sizeof(long));
	memset(x, 0, N);
	memset(y, 0, N);
	memset(polyX, 0, N);
	memset(polyY, 0, N);
	int i = 0;
	for (i = 0; i < giNumRow; i++) {
		printf("%f,%f\n", giCsvData[i*giNumCol + 0], giCsvData[i*giNumCol + 1]);
		//memcpy(x + i*sizeof(giCsvData[i*giNumCol + 0]), &(giCsvData[i*giNumCol + 0]), sizeof(giCsvData[i*giNumCol + 0]));
		//memcpy(y + i*sizeof(giCsvData[i*giNumCol + 1]), &(giCsvData[i*giNumCol + 1]), sizeof(giCsvData[i*giNumCol + 1]));
		x[i] = giCsvData[i*giNumCol + 0];
		y[i] = giCsvData[i*giNumCol + 1];
	}

	for (i = 0; i < giNumRow; i++) {
		printf("x=%f,y=%f\n", x[i], y[i]);
	}

	findSmallestPolygon(x, y, giNumRow, polyX, polyY, polyCorners);

	for (i = 0; i < giNumRow; i++) {
		printf("px=%f,py=%f\n", polyX[i], polyY[i]);
	}
	printf("polyCorners=%ld\n", *polyCorners);
	FreeCsvData();
	free(x);
	free(y);
	free(polyY);
	free(polyX);
	free(polyCorners);
}

void test_mat()
{
	int n, i, length;
	char *filename = "e:/python-project/test/num/xy_wzb07081half.txt";
	ReadCsvData(filename);
	printf("giNumRow=%d,giNumCol=%d\n", giNumRow, giNumCol);

	POINT a[] = { { 1.0, 2.0 },{ 2.0, 0.5 },{ 2.5, 1.0 },{ 2.0, 0.0 },{ 4.0, 2.0 } };

	POINT *old_data;
	POINT b[4];
	
	int N = giNumRow*sizeof(POINT);
	printf("N=%d\n", N);

	old_data = (POINT*)malloc(N);
	memset(b, 0, sizeof(b));
	memset(old_data, 0, sizeof(POINT)*giNumRow);
	
	for (i = 0; i < giNumRow; i++) {
		printf("%f,%f\n", giCsvData[i*giNumCol + 0], giCsvData[i*giNumCol + 1]);
		//memcpy(x + i*sizeof(giCsvData[i*giNumCol + 0]), &(giCsvData[i*giNumCol + 0]), sizeof(giCsvData[i*giNumCol + 0]));
		//memcpy(y + i*sizeof(giCsvData[i*giNumCol + 1]), &(giCsvData[i*giNumCol + 1]), sizeof(giCsvData[i*giNumCol + 1]));
		old_data[i].x = giCsvData[i*giNumCol + 0];
		old_data[i].y = giCsvData[i*giNumCol + 1];
	}

	printf("####################\n");
	for (int i = 0; i <giNumRow; i++) {

		//printf("%f,%f\n", old_data[i].x, old_data[i].y);
	}
	
	//length = sizeof(a) / sizeof(POINT);
	//printf("length=%d\n", length);
	//rotatingcalipers(old_data, giNumRow, b);

	//for (i = 0; i < 4; i++) {
		//printf("[%f, %f] ", b[i].x, b[i].y);
	//}
	//printf("\n");

	POINT *new_data;
	new_data= (POINT*)malloc(N);
	memset(new_data, 0, sizeof(POINT)*giNumRow);
	data_analysis_v3(old_data, giNumRow,new_data,0);
	FILE *file;
	fopen_s(&file, "newxy.txt", "w+");
	for (i = 0; i < giNumRow; i++) {
		fprintf(file, "%f", new_data[i].x);
		fprintf(file, "%s", ",");
		fprintf(file, "%f", new_data[i].y);
		fprintf(file, "%s", "\n");
	}
	fclose(file);
	free(old_data);
	free(new_data);
	old_data = NULL;
	new_data = NULL;
	FreeCsvData();
}

void test1()
{
	POINT a, new_a;
	a.x = 12.0;
	a.y = 9;
	new_a = a;
	printf("%f,%f\n", new_a.x, new_a.y);
}

void test2(char *b) {
	char c[15] = { 'a','b','c','a','b','c','b','c','a','b','c','a','b','c' ,'e' };
	memcpy(b + 2, c, 15);
	char d[6] = { 0x80,0x00,0x00,0xec,0x82,0x81 };
	memcpy(b + 17, d, 6);
}

void testw(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{

	
	int tmpCh=0;
	if (1)
	{
		for (int i = 0; i < outBufSize; i++)
		{
			tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
			if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
			{
				pOutData[(i * 2 + 1)] = tmpCh + '0';
			}
			else if (10 <= tmpCh && tmpCh <= 15) //a - f
			{
				pOutData[(i * 2 + 1)] = (tmpCh - 10) + 'A';
			}

			//tmpCh = ((pInDatabuf[i] >> 4)  & 15);
			tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
			if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
			{
				pOutData[i * 2] = tmpCh + '0';
			}
			else if (10 <= tmpCh && tmpCh <= 15) //A - F
			{
				pOutData[i * 2] = (tmpCh - 10) + 'A';
			}
		}
	}
	else
	{
		_strupr_s(pInDatabuf, strlen(pInDatabuf) + 1);
		for (int i = 0; i < outBufSize; i++)
		{
			if ('0' <= pInDatabuf[i * 2] && pInDatabuf[i * 2] <= '9')
			{
				tmpCh = pInDatabuf[i * 2] - '0';
				tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
			}
			else if ('A' <= pInDatabuf[i * 2] && pInDatabuf[i * 2] <= 'F')
			{
				tmpCh = pInDatabuf[i * 2] - 'A' + 10;
				tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
			}

			if ('0' <= pInDatabuf[(i * 2 + 1)] && pInDatabuf[(i * 2 + 1)] <= '9')
			{
				tmpCh += pInDatabuf[(i * 2 + 1)] - '0'; //get Low 4bits
			}
			else if ('A' <= pInDatabuf[(i * 2 + 1)] && pInDatabuf[(i * 2 + 1)] <= 'F')
			{
				tmpCh += pInDatabuf[(i * 2 + 1)] - 'A' + 10; //get Low 4bits
			}

			pOutData[i] = tmpCh&0xff;
		}
	}

	printf("pindata:%s\n", pInDatabuf);
	for (int j = 0; j < 6; j++) {
		printf("poutdata[%d]=%x\n", j, pOutData[j]);
	}
	printf("poutdata:%s\n", pOutData);

}

int main(int argc, char **argv)
{
	//test_findsp();
	//test_mat();
	//test1();
	char *pindata = "5452";
	printf("%d,%d,%d,%d\n", pindata[0], pindata[1], pindata[2], pindata[3]);
	char pout[60] = { 0 };
	testw(pout,1, pindata,2);
	getchar();
	return 0;
}