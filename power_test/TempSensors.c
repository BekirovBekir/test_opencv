/*
 * TempSensors.c

 *
 *  Created on: 31 окт. 2017 г.
 *      Author: bekir
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "TempSensor.h"

float Read_Temp(int num)
{
	int fd;
	char buf[150]={0};
	char* buf_num[]={"1-0048", "1-0049", "1-004a", "1-004b"};
	float temp;

	//memset(buf, 0, BUT_BUF_SIZE);
	snprintf(buf, sizeof(buf), "/sys/class/i2c-dev/i2c-1/device/%s/hwmon/hwmon%i/temp1_input", buf_num[num], num);
	fd=open(buf, O_RDONLY);
		if (fd<0)
		{
			perror("\r\nError while opening sensor file");
			return -1000;
		}
		memset(buf, 0, 150);
		if (read(fd, &buf, sizeof(buf))==0)
		{
			perror("\r\nError while reading temperature");
			close(fd);
			return -1000;
		}
	close(fd);
	temp=(float)(atoi(buf))/1000;
	return temp;
}

float Read_Pressuare_Temp(void)
{
	int fd;
	char buf[150]={0};
	float temp;

	//memset(buf, 0, BUT_BUF_SIZE);
	for (int i=0; i<=2; i++)
	{
		snprintf(buf, sizeof(buf), "/sys/class/i2c-dev/i2c-1/device/1-0060/iio:device%i/in_temp_raw", i);
		fd=open(buf, O_RDONLY);
			if (fd>=0)
			{
				memset(buf, 0, 150);
				if (read(fd, &buf, sizeof(buf))==0)
				{
					perror("\r\nError while reading temperature");
					close(fd);
					return -1000;
				}
			close(fd);
			break;
			}
	}
	temp=(float)(atoi(buf))/16;
	return temp;
}
