
#include "readCSVFile.h"



int GetTotalLineCount(FILE * fp)
{
	int i = 0;
	char strLine[MAX_LINE_SIZE];
	fseek(fp, 0, SEEK_SET);
	while (fgets(strLine, MAX_LINE_SIZE, fp))
		i++;
	fseek(fp, 0, SEEK_SET);
	return i;
}
//计算csv文件中的总列数（以第一行的列数为基准）
int GetTotalColCount(FILE * fp)
{
	int i = 0;
	char strLine[MAX_LINE_SIZE];
	fseek(fp, 0, SEEK_SET);
	if (fgets(strLine, MAX_LINE_SIZE, fp))
	{
		i = strlen(strLine) / 2;   //因为csv文件以逗号','作为分隔符，所以此处除以2
	}
	else
	{
		fseek(fp, 0, SEEK_SET);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);
	return i;
}

int AssignSpaceForData(int inumdata) {
	giCsvData = NULL;
	giCsvData = (int*)malloc(sizeof(int)*inumdata);
	if (giCsvData == NULL)
		return 0;
	memset(giCsvData, 0, sizeof(int)*inumdata);
	return 1;
}


void FreeCsvData() {
	free(giCsvData);
	giCsvData = NULL;
}


int ReadCsvData(char* csvFilePath)
{
	FILE* fCsv;
	char *ptr;
	char strLine[MAX_LINE_SIZE];
	int i;
	int j;
	
	if (giCsvData != NULL)
		FreeCsvData();
	
	if (fopen_s(&fCsv, csvFilePath, "r") != 0)
	{
		printf("open file %s failed", csvFilePath);
		return -1;
	}
	else
	{
		// 确定动态数组的大小，然后开辟空间
		giNumRow = GetTotalLineCount(fCsv);
		giNumCol = GetTotalColCount(fCsv);
		giNumData = giNumRow*giNumCol;
		AssignSpaceForData(giNumData);

		char *temp_buf = NULL;

		// 读取数据
		for (i = 0; i < giNumRow; i++)
		{

			j = 0;
			if (fgets(strLine, MAX_LINE_SIZE, fCsv))
			{
				ptr = strtok_s(strLine, ",",&temp_buf);
				//ptr = strtok(strLine, ",");  
												
				while (ptr != NULL)
				{
					giCsvData[i*giNumCol + j] = atof(ptr);
					j++;
					ptr = strtok_s(NULL, ",", &temp_buf);
					//ptr = strtok(NULL, ",");                
				}
			}
		}
		
		fclose(fCsv);
	}
	return 1;
}


void  ShowCsvData()
{
	int i;
	int j;
	for (i = 0; i < giNumRow; i++)
	{
		printf("Line %i :", i + 1);    
		for (j = 0; j < giNumCol; j++)
		{
			printf("%f", giCsvData[i*giNumCol + j]);  
			printf(" ");
		}
		printf("\n");                          
	}

	//printf("%f\n", giCsvData[0, 0]);
}
