#pragma once
#include <stdio.h>
#include <memory.h> 
#include <stdlib.h> 
#include <string.h> 


#define MAX_LINE_SIZE 256   


float* giCsvData;           //动态分配数据内存的指针
int giNumData;            //读取的数据字节数
int giNumRow;             //每行的字节数
int giNumCol;             // 每列的字节数


int GetTotalLineCount(FILE* fp);              
int GetTotalColCount(FILE * fp);              
int AssignSpaceForData(int inumdata);         
void FreeCsvData();                           
int ReadCsvData(char* csvFilePath);           
void  ShowCsvData();                           