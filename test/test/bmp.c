#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
//#include <conio.h>
#include <pshpack2.h>
#include <poppack.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#define SIZE 40

#pragma pack(push)
#pragma pack(1)

/*����λͼ�ļ�ͷ*/
typedef struct
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BIT_MAP_FILE_HEADER;

/*������Ϣͷ*/
typedef struct
{
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BIT_MAP_INFO_HEADER;

/*��ɫ��*/
typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
} RGB_QUAD;

#pragma pack(pop)

int odd_or_even(int x)
{
	if ((x % 2) != 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

#if 0
int main(int argc,char **argv)
{
	RGB_QUAD *bmp_data = NULL;
	FILE *bmp_file;
	int i, j, k, q;
	long width = 400;  //����ͼƬ���
	long height = 400; //����ͼƬ�߶�
	long date_size = width*height;
	BIT_MAP_FILE_HEADER bmp_map_file;
	BIT_MAP_INFO_HEADER bmp_map_info;
	
	fopen_s(&bmp_file, "cubs.bmp", "wb+");
	//if ((bmp_file = fopen("cubs.bmp", "wb+")) == NULL)
	if(bmp_file == NULL)
	{
		printf("Error!\n");
		system("pause");
		exit(0);
	}

	/*д��ͼλ�ļ�ͷ*/
	bmp_map_file.bfType = 0x4d42;
	bmp_map_file.bfSize = 14 + 40 + width * height * 3;
	bmp_map_file.bfReserved1 = 0;
	bmp_map_file.bfReserved2 = 0;
	bmp_map_file.bfOffBits = 0x36;
	fwrite(&bmp_map_file, sizeof(BIT_MAP_FILE_HEADER), 1, bmp_file);

	/*д����Ϣͷ*/
	bmp_map_info.biSize = 40;
	bmp_map_info.biPlanes = 1;
	bmp_map_info.biHeight = height;
	bmp_map_info.biWidth = width;
	bmp_map_info.biBitCount = 24;
	bmp_map_info.biClrImportant = 0;
	bmp_map_info.biClrUsed = 0;
	bmp_map_info.biSizeImage = width*height * 3;
	bmp_map_info.biCompression = 0;
	bmp_map_info.biXPelsPerMeter = 0;
	bmp_map_info.biYPelsPerMeter = 0;
	fwrite(&bmp_map_info, sizeof(BIT_MAP_INFO_HEADER), 1, bmp_file);

	/*��ͼ������ڴ�*/
	if ((bmp_data = (RGB_QUAD*)malloc(width * height * 3)) == NULL)
	{
		printf("�����ڴ�ʧ�ܣ�\n");
		exit(0);
	}

	/*����RGB��Ϣ��iΪ������jΪ�᷽��*/
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			k = odd_or_even(i / SIZE);
			q = odd_or_even(j / SIZE);
			if (0 > (k * q))
			{
				bmp_data[i*width + j].blue = 255;
				bmp_data[i*width + j].green = 255;
				bmp_data[i*width + j].red = 255;
			}
			else
			{
				bmp_data[i*width + j].blue = 0;
				bmp_data[i*width + j].green = 0;
				bmp_data[i*width + j].red = 0;
			}

		}
	}
	fwrite(bmp_data, sizeof(RGB_QUAD), date_size, bmp_file);

	printf("BMPͼ�����ɳɹ���\n\n\n");
	fclose(bmp_file);
	free(bmp_data);
	bmp_data = NULL;
	return 0;
}
#endif