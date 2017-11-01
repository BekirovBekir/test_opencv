#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "ModemSARA.h"
#include "ComPort.h"

static int port_id;

void Init_SARA(char* port_name, int port_speed)
{
	char buf_tx[SARA_BUF_SIZE]={0};
	char buf_rx[SARA_BUF_SIZE]={0};
	int cnt_byte;

	port_id=OpenPort(port_name);
	SetPort(port_id, port_speed);
	//ClosePort(port_id);

	sleep(5);

	// ========== set configuration for SARA ==========================
	cnt_byte=sprintf(buf_tx, "ATE0\r");	 // echo off
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts(buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}

memset(buf_tx, 0, sizeof(buf_tx));
memset(buf_rx, 0, sizeof(buf_rx));

	cnt_byte=sprintf(buf_tx, "AT+CMEE=2\r");	 // errors information is off
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts(buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}

	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	cnt_byte=sprintf(buf_tx, "AT+CREG=1\r");	 // registration in network
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts(buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}
	sleep(5);

	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	cnt_byte=sprintf(buf_tx, "AT+COPS=0\r");	 // registration in network
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts(buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}

	sleep(60);

	/*memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));
	cnt_byte=sprintf(buf_tx, "AT+USAPIND=1\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")!=NULL)
		{
			perror("\r\nError while reading");
		}*/

	/*memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));
	cnt_byte=sprintf(buf_tx, "AT+USAPMODE=1\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")!=NULL)
		{
			perror("\r\nError while reading");
		}*/


}

void Call_SARA(char* number)
{
	char buf_tx[SARA_BUF_SIZE]={0};
	char buf_rx[SARA_BUF_SIZE]={0};
	int cnt_byte;
	int state;

	cnt_byte=sprintf(buf_tx, "ATD%s;\r", number);
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	state=0;
	for(;;)//while (getchar()!='q')
	{
		switch (state)
		{
		case 0:
			while (ReadPort(port_id, buf_rx, sizeof(buf_rx))==0);
			state=1;
			break;
		case 1:
			if (strstr(buf_rx, "NO ANSWER")!=NULL)
			{
				puts (buf_rx);
				cnt_byte=sprintf(buf_tx, "ATD%s;\r", number);
				puts (buf_tx);
				WritePort(port_id, buf_tx, cnt_byte);
				usleep(300000);
				cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
				puts (buf_rx);
					if (strstr(buf_rx, "OK")==NULL)
					{
						perror("\r\nError while reading\r\n");
					}
			}
			memset(buf_tx, 0, sizeof(buf_tx));
			memset(buf_rx, 0, sizeof(buf_rx));
			state=0;
			break;
		}
		/*if (getchar()=='q')	// exit from call mode and return in main
		{
			return;
		}*/
	usleep(100000);
	}
		/*while (ReadPort(port_id, buf_rx, sizeof(buf_rx))!=0)
		{
			if (strstr(buf_rx, "NO ANSWER")!=NULL)
			{
				cnt_byte=sprintf(buf_tx, "ATD%s;\r", number);
				puts (buf_tx);
				WritePort(port_id, buf_tx, cnt_byte);
				usleep(300000);
				cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
				puts (buf_rx);
					if (strstr(buf_rx, "OK")==NULL)
					{
						perror("\r\nError while reading");
					}
			}
		memset(buf_tx, 0, sizeof(buf_tx));
		memset(buf_rx, 0, sizeof(buf_rx));
		usleep(500000);
		}*/
}

void PPP_SARA(char* number, char* apn)
{
	char buf_tx[SARA_BUF_SIZE]={0};
	char buf_rx[SARA_BUF_SIZE]={0};
	int cnt_byte;

	cnt_byte=sprintf(buf_tx, "AT+CGDCONT=1,\"IP\",\"%s\"\r", apn);
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError connection\r\n");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(10); //20

	/*cnt_byte=sprintf(buf_tx, "AT+CGEQREQ=1,3,64,64,,,0,320,\"1E4\",\"1E5\",1,,3\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError connection");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(5); //20

	cnt_byte=sprintf(buf_tx, "AT+CGQREQ=1,1,3,4,5,6\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError connection");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(5); //20*/


	cnt_byte=sprintf(buf_tx, "AT+CGACT=1,1\r"); // "ATD%s\r" "AT+CGACT=1,2\r"
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	sleep(20);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(1);

	cnt_byte=sprintf(buf_tx, "AT+CGDCONT?\r"); //"AT+CGDATA=\"PPP\",1"
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}

	sleep(1); //20

	cnt_byte=sprintf(buf_tx, "AT+UPSDA=0,3\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}

	sleep(5); //20

	/*cnt_byte=sprintf(buf_tx, "AT+USOCR=6\r"); //"AT+CGDATA=\"PPP\",1" "ATD%s\r"
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	sleep(5);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(5);

	cnt_byte=sprintf(buf_tx, "AT+UDNSRN=0,\"gw1-int.syncwise.com\"\r"); //"AT+CGDATA=\"PPP\",1" "ATD%s\r"
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	sleep(5);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(5);

	cnt_byte=sprintf(buf_tx, "AT+USOCO=0,\"54.235.194.197\",7071\r"); //"AT+CGDATA=\"PPP\",1" "ATD%s\r"
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	sleep(5);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));

	sleep(5);*/

}

void EndCall_SARA(void)
{
	char buf_tx[SARA_BUF_SIZE]={0};
	char buf_rx[SARA_BUF_SIZE]={0};
	int cnt_byte;

	cnt_byte=sprintf(buf_tx, "ATH0\r");
	puts (buf_tx);
	WritePort(port_id, buf_tx, cnt_byte);
	usleep(300000);
	cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
	puts (buf_rx);
		if (strstr(buf_rx, "OK")==NULL)
		{
			perror("\r\nError while reading\r\n");
		}
	memset(buf_tx, 0, sizeof(buf_tx));
	memset(buf_rx, 0, sizeof(buf_rx));
}

void ICMPPing_SARA(char* apn)
{
	char buf_tx[SARA_BUF_SIZE]={0};
	char buf_rx[SARA_BUF_SIZE]={0};
	int cnt_byte;

	for (;;)
	{
		cnt_byte=sprintf(buf_tx, "AT+UPING=\"%s\",1,64,5000,32\r", apn);
		puts (buf_tx);
		WritePort(port_id, buf_tx, cnt_byte);
		sleep(5);
		cnt_byte=ReadPort(port_id, buf_rx, sizeof(buf_rx));
		puts (buf_rx);
			if (strstr(buf_rx, "UUPING")==NULL)
			{
				perror("\r\nError while ping\r\n");
			}
			else
			{
				printf("\r\nPing OK!\r\n");
			}
		memset(buf_tx, 0, sizeof(buf_tx));
		memset(buf_rx, 0, sizeof(buf_rx));

	sleep(30);
	}
}

void Close_SARA(void)
{
	ClosePort(port_id);
}



