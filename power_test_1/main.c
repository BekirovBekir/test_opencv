
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

#include "GPIO.h"
#include "ComPort.h"
#include "ModemSARA.h"
#include "FrameBuffer.h"

pthread_t threadcall;	 //thread for call from SARA
pthread_t threadping;	 //thread for pingfrom SARA
int id1=1;
int id2=1;

char buf_number[20];
char buf_MAC[30];
char key;

void Cmd_Run(char* cmd)
{
FILE* name;
char os[80]={0};
int lastchar;
char buf_cmd[20]={0};

sprintf(buf_cmd, "%s -o", cmd);
name = popen(buf_cmd, "r");
lastchar = fread(os, 1, 80, name);
os[lastchar] = '\0';
//printf("Your OS is %s", os);
pclose(name);
}

void* threadFunc1(void* thread_data)	 //thread for call
{
	Call_SARA(buf_number);
}

void* threadFunc2(void* thread_data)	 //thread for ping
{
	ICMPPing_SARA("www.google.com");
}

int main(int argc, char* argv[])
{
	char buf[50]={0};

	if (argc!=5)
	{
	printf ("Please usage: power_test <number of telephone> <MAC for bluetooth> <charge level> <APN>\r\n");
	exit (0);
	}

	//thread_id=1;

	/*if (pthread_create(&thread_read, NULL, thread_func, &thread_id))	//create_prthead for reading data from SARA module)
	{
		perror("\r\nThread created\r\n");
	}*/

	printf("\r\n==== Start Power Test ====\r\n");
	sprintf (buf_number, "%s", argv[1]);
	sprintf (buf_MAC, "%s", argv[2]);


	/*if (Init_GPIO(SENSOR_PWR_EN, "out")==1)
	{
		if (Write_GPIO(SENSOR_PWR_EN, "1")==1)
		printf("\r\nSensors power enable\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(CODEC_PWR_EN, "out")==1)
	{
		if (Write_GPIO(CODEC_PWR_EN, "1")==1)
		printf("\r\nCodec power enable\r\n");
	}

	sleep(1);*/

	/*if (Init_GPIO(DISP_PWR_EN, "out")==1)
	{
		if (Write_GPIO(DISP_PWR_EN, "1")==1)
		printf("\r\nDisplay power enable\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(GPS_PWR_EN, "out")==1)
	{
		if (Write_GPIO(GPS_PWR_EN, "1")==1)
		printf("\r\nGPS power enable\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(WIFI_PWR_EN, "out")==1)
	{
		if (Write_GPIO(WIFI_PWR_EN, "1")==1)
		printf("\r\nWi-Fi power enable\r\n");
	}

	sleep(1);*/

	/*if (Init_GPIO(WIFI_EN, "out")==1)
	{
		if (Write_GPIO(WIFI_EN, "1")==1)
		printf("\r\nWi-Fi enable\r\n");
	}*/

	sleep(1);

	/*if (Set_PWM_LED(10)==1)
	{
		printf("\r\nBacklight is max\r\n");
	}*/

	sleep(1);

	/*if (Set_I_Charge(atoi(argv[3]))==1)
	{
		int I;
		I=atoi(argv[3])*125;
		printf("\r\nCharge current is %i mA\r\n", I);
	}*/

	sleep(1);

	/*if (Init_GPIO(MODEM_MDM1_PWR, "out")==1)
	{
		if (Write_GPIO(MODEM_MDM1_PWR, "1")==1)
		printf("\r\nSARA power enable\r\n");
	}*/

	sleep(1);

	//system("./wi-fi.sh");
	//sleep(2);
	//system("./bt_scan.sh");
	//sleep(2);
	//snprintf(buf, sizeof(buf_MAC), "l2ping %s &", buf_MAC);
	//system(buf);

	//sleep(10);

	//printf ("\r\nWaiting 1 minute for ping start...\r\n");
	//Init_SARA("/dev/ttyACM0", 115200);
	//PPP_SARA("*99#", argv[4]); //argv[5]
	//Call_SARA(argv[1]);

	/*int trhread_state=pthread_create(&threadcall, NULL, threadFunc1, &id1);
	if (trhread_state==0)
	{
		printf ("\r\nCall thread started\r\n");
	}
	else
	{
		perror("\r\nCall thread fail\r\n");
	}*/

	/*int trhread_state=pthread_create(&threadping, NULL, threadFunc2, &id2);
	if (trhread_state==0)
	{
		printf ("\r\nPing thread started\r\n");
	}
	else
	{
		perror("\r\nPing thread fail\r\n");
	}

		while (getchar()!='q');*/
	//pthread_cancel(threadcall);
	//pthread_join(threadcall, NULL);
	//EndCall_SARA();
	//printf ("\r\nCall is over\r\n");
	//pthread_cancel(threadping);
	//pthread_join(threadping, NULL);
	//printf ("\r\nPing is over\r\n");


	//sleep(2);
	//Close_SARA();

	/*if (Init_GPIO(SENSOR_PWR_EN, "out")==1)
	{
		if (Write_GPIO(SENSOR_PWR_EN, "0")==1)
		printf("\r\nSensors power disable\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(GPS_PWR_EN, "out")==1)
	{
		if (Write_GPIO(GPS_PWR_EN, "0")==1)
		printf("\r\nGPS power disable\r\n");
	}*/

	sleep(1);

	/*if (Set_PWM_LED(7)==1)
	{
		printf("\r\nBacklight 70%\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(DISP_PWR_EN, "out")==1)
	{
		if (Write_GPIO(DISP_PWR_EN, "0")==1)
		printf("\r\nDisplay power disable\r\n");
	}

	sleep(1);*/

	/*if (Set_I_Charge(1)==1)
	{
		printf("\r\nCharge level is min\r\n");
	}*/

	sleep(1);

	/*if (Init_GPIO(MODEM_MDM1_PWR, "out")==1)
	{
		if (Write_GPIO(MODEM_MDM1_PWR, "0")==1)
		printf("\r\nSARA power disable\r\n");
	}*/

	sleep(1);


	/*if (Init_GPIO(WIFI_EN, "out")==1)
	{
		if (Write_GPIO(WIFI_EN, "0")==1)
		printf("\r\nWi-Fi disable\r\n");
	}

	sleep(1);*/

	//printf ("\r\n===== Power Test is over =====\r\n");

	Fill_Buffer(0x00, 0x80, 0xFF); ///wwwwwwwwwwwwwwww
	return 1;
}
