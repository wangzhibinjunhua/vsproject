#include <iostream>
#include <string>
#include <vector>
#include <TCHAR.H>  
#include <windows.h> 
using namespace std;

HANDLE hCom;


int Serial_open(LPCSTR COMx, int BaudRate)
{
	DCB dcb = { 0 };
	hCom = CreateFile(COMx,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,//FILE_FLAG_OVERLAPPED,  
		0
		);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		return -1;
	}

	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(hCom, &dcb))
	{
		DWORD dwError = GetLastError();
		return -1;
	}

	dcb.BaudRate = BaudRate;    
	dcb.ByteSize = 8;            
	dcb.Parity = NOPARITY;       
	dcb.StopBits = ONESTOPBIT;   

	if (!SetCommState(hCom, &dcb))
	{
		DWORD dwError = GetLastError();
		return -1;
	}
	if (!PurgeComm(hCom, PURGE_RXCLEAR))    return -1;

	SetupComm(hCom, 1024, 1024);
	return 0;
}

/**
serial read
@param Buf:data buf
@param size:
@return The len of read
*/
int Serial_read(void *OutBuf, int size)
{
	DWORD cnt = 0;
	ReadFile(hCom, OutBuf, size, &cnt, 0);
	return cnt;

}


int Serial_write(const void *Buf, int size)
{
	DWORD dw;
	WriteFile(hCom, Buf, size, &dw, NULL);
	return dw;
}


void Serial_close(void)
{
	CloseHandle(hCom);
}


#if 0
int main(void)
{
	//char *buf;
	unsigned char revc_buf[512];
	int len = 0;
	int ret=Serial_open(_T("com4"), 115200);
	cout << ret << endl;
	if (ret < 0) {
		cout << "open com err" << endl;
	}
	else {
		char buf[] = { 0xff,0x11,0x23 ,0xaa,0x0b,0x22,0xa9,0xa8,0xff,0x11,0x23 };
		Serial_write(buf, 0x0b);
	}
	memset(revc_buf, 0, 512);
	
	while(1){
		if (Serial_read(revc_buf, 32)>0) {
			for (int i = 0; i < len; i++)
				printf("read %x ", revc_buf[i]);
			printf("\n");
		}
		//len = Serial_read(revc_buf, 32);
		//printf("len=%d\n", len);
		//if (len) {
			//for (int i = 0; i < len; i++)
				//printf("read %x ", revc_buf[i]);
			//printf("\n");
		//}
	}

	getchar();
	Serial_close();
	return 0;
}
#else
void parse(unsigned char data[])
{
	switch (data[2]) {
	case 0xa1:
		printf("ring\n");
		break;
	case 0xa2:
		printf("tel\n");
		break;
	}
}

int a_main(void)
{
	//char *buf;
	if (16 == 0xf) {
		printf("111111");
	}

	
	unsigned char revc_buf[512];
	unsigned int len = 0;
	int ret = Serial_open(_T("com4"), 115200);
	cout << ret << endl;
	if (ret < 0) {
		cout << "open com err" << endl;
	}
	else {
		char buf[] = { 0xff,0x11,0x23 ,0xaa,0x0b,0x22,0xa9,0xa8,0xff,0x11,0x23};
		Serial_write(buf, 3);
	}
	memset(revc_buf, 0, 512);
	unsigned int has_read = 0;


	while (1) {

	loop:
		len = Serial_read(&revc_buf[has_read], 1);
		if (len < 1) {
			has_read = 0;
			goto loop;
		}
		printf("has_read=%x,revc_buf=%x\n", has_read, revc_buf[has_read]);
		if (revc_buf[has_read] == 0xff) {
			has_read++;
			goto loop;
		}
		has_read += len;
		if (revc_buf[1] >(has_read-2)) {
			printf("has_read=%x\n", has_read);
			goto loop;
			//for (int i = 0; i < has_read; i++)printf("%x", revc_buf[i]);
		}
		else {
			for (int i = 0; i < has_read; i++)printf("%02x", revc_buf[i]);
			parse(revc_buf);
			has_read = 0;
			memset(revc_buf, 0, 512);
			//goto loop;
		}

		
	}
	
	getchar();
	Serial_close();
	return 0;
}


#endif
