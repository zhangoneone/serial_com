#include"serial_com.h"
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
static int fd;
static int ret;
typedef struct{
   uint head;
   uchar length;
   uint time_stamp;
   float gx;
   float gy;
   float gz;
   float ax;
   float ay;
   float az;
   uint tail;
}IMU;
static IMU imu;
int main(int argc,char **argv){
    char r_buf[1024];
     bzero(r_buf,1024);
     fd = serial_open("/dev/ttyUSB0",0);//选择的是ttsY1串口
     if(fd == -1)
     {
         fprintf(stderr,"open failed!\n");
         exit(EXIT_FAILURE);
     }
     if(serial_config(fd,4800,'N',8,'N',1) == -1)
     {
         fprintf(stderr,"configure failed!\n");
         exit(EXIT_FAILURE);
     }
     while (1) {
	 bzero(r_buf, 1024);
         ret = serial_read(fd,r_buf,sizeof( IMU ));                
         if(ret == -1)
         {
             fprintf(stderr, "uart_read failed!\n");
             exit(EXIT_FAILURE);
         }
  	 printf("%s",r_buf);/*
         memcpy(&imu,r_buf,sizeof( IMU ));
        printf("head:%u \n", imu.head);
 	printf("length:%c \n", imu.length);
	printf("time_stamp:%u \n",imu.time_stamp);
	printf("gx:%f \n", imu.gx);
	printf("gy:%f \n", imu.gy);
	printf("gz:%f \n", imu.gz);
	printf("ax:%f \n", imu.ax);
	printf("ay:%f \n", imu.ay);
	printf("az:%f \n", imu.az);
	printf("tail:%u\n", imu.tail);*/
     }
     ret = close(fd);
     if(ret == -1)
     {
         fprintf(stderr, "close failed!\n");
         exit(EXIT_FAILURE);
     }
     exit(EXIT_SUCCESS);
}
