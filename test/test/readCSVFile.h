#pragma once
#include <stdio.h>
#include <memory.h> 
#include <stdlib.h> 
#include <string.h> 


#define MAX_LINE_SIZE 256   


float* giCsvData;           //��̬���������ڴ��ָ��
int giNumData;            //��ȡ�������ֽ���
int giNumRow;             //ÿ�е��ֽ���
int giNumCol;             // ÿ�е��ֽ���


int GetTotalLineCount(FILE* fp);              
int GetTotalColCount(FILE * fp);              
int AssignSpaceForData(int inumdata);         
void FreeCsvData();                           
int ReadCsvData(char* csvFilePath);           
void  ShowCsvData();                           