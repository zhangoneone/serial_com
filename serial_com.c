#include "serial_com.h"
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

static int fd  = 0;
SERIAL_RET serial_open(const void *filename,int flag){
   // assert(filename);//检测串口路径是否存在
    fd = open(filename,O_RDWR|O_NDELAY);//以只读形式、不将此终端作为此进程的终端控制器、非阻塞的形式打开串口
    if(fd == -1){
        perror("uart open failed!");
        return -1;
    }
    if(fcntl(fd,F_SETFL,0)<0)//设置串口非阻塞，因为这里是以非阻塞形式打开的，所以第三个参数为0，后面会详细介绍fcntl函数
    {
        perror("fcntl failed!");
        return -1;
    }
    return fd;
}
SERIAL_RET serial_config(int fd,int baude,int c_flow, int bits, char parity, int stop){
    struct termios uart;
    if(tcgetattr(fd,&uart)!=0){
        perror("tcgetattr failed!");
        return -1;
    }
    switch(baude){
    	case 4800:
        cfsetispeed(&uart,B4800);//设置输入波特率
        cfsetospeed(&uart,B4800);//设置输出波特率
        break;
    	case 9600:
        cfsetispeed(&uart,B9600);
        cfsetospeed(&uart,B9600);
        break;
    case 19200:
        cfsetispeed(&uart,B19200);
        cfsetospeed(&uart,B19200);
        break;
    case 38400:
        cfsetispeed(&uart,B38400);
        cfsetospeed(&uart,B38400);
    case 115200:
        cfsetispeed(&uart,B115200);
        cfsetospeed(&uart,B115200);
        break;
    default:
        fprintf(stderr,"Unknown baude!");
        return -1;
    }
  switch(c_flow){
  case 'N':
  case 'n':
      uart.c_cflag &= ~CRTSCTS;//不进行硬件流控制
      break;
  case 'H':
  case 'h':
      uart.c_cflag |= CRTSCTS;//进行硬件流控制
      break;
  case 'S':
  case 's':
      uart.c_cflag |= (IXON | IXOFF | IXANY);//进行软件流控制
      break;
  default:
      fprintf(stderr,"Unknown c_cflag");
      return -1;
  }
    switch(bits){
    case 5:
        uart.c_cflag &= ~CSIZE;//屏蔽其他标志位
        uart.c_cflag |= CS5;//数据位为5位
        break;
    case 6:
        uart.c_cflag &= ~CSIZE;
        uart.c_cflag |= CS6;
        break;
    case 7:
        uart.c_cflag &= ~CSIZE;
        uart.c_cflag |= CS7;
        break;
    case 8:
        uart.c_cflag &= ~CSIZE;
        uart.c_cflag |= CS8;
      break;
    default:
        fprintf(stderr,"Unknown bits!");
        return -1;
    }
    switch(parity)
    {
    case 'n':
    case 'N':
        uart.c_cflag &= ~PARENB;//PARENB：产生奇偶校验
        uart.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
        break;
    case 's':
    case 'S':
        uart.c_cflag &= ~PARENB;
        uart.c_cflag &= ~CSTOPB;//使用两位停止位
        break;
    case 'o':
    case 'O':
        uart.c_cflag |= PARENB;
        uart.c_cflag |= PARODD;//使用奇校验
        uart.c_cflag |= INPCK;
        uart.c_cflag |= ISTRIP;//使字符串剥离第八个字符，即校验位
        break;
    case 'e':
    case 'E':
        uart.c_cflag |= PARENB;
        uart.c_cflag &= ~PARODD;//非奇校验，即偶校验
        uart.c_cflag |= INPCK;
        uart.c_cflag |= ISTRIP;
        break;
    default:
        fprintf(stderr,"Unknown parity!\n");
        return -1;
    }
    switch(stop){
    case 1:
        uart.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
        break;
    case 2:
        uart.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr,"Unknown stop!\n");
        return -1;
    }
    uart.c_oflag &= ~OPOST;//OPOST:表示数据经过处理后输出
   if(tcsetattr(fd,TCSANOW,&uart)<0)//激活配置，失败返回-1
    {
        return -1;

    }
    uart.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG );//使串口工作在原始模式下
    uart.c_cc[VTIME] = 0;//设置等待时间为0
    uart.c_cc[VMIN] = 1;//设置最小接受字符为1
    tcflush(fd,TCIFLUSH);//清空输入缓冲区
    if(tcsetattr(fd,TCSANOW,&uart)<0)//激活配置
    {
        perror("tcgetattr failed!");
        return -1;
    }
    return 0;
}

SERIAL_RET serial_close(uint fd){
    assert(fd);//assert先检查文件描述符是否存在
    close(fd);
    return 0;
}

SERIAL_RET serial_read(uint fd,void *dst_buff,uint len){
    size_t left;
    left = len;
    ssize_t nread;
    char *ptr;
    ptr = dst_buff;
    while(left > 0){
        if ((nread = read(fd, ptr, left)) < 0){
            if(nread == 0)
            {
                break;
            }
        }
        left -= nread;//read成功后，剩余要读取的字节自减
        ptr += nread;//指针向后移，避免后读到的字符覆盖先读到的字符
    }
    return (len - left);
}
SERIAL_RET serial_write(uint fd,const void *src_buff,uint len){
    fd_set rfds;
    struct timeval time;
    ssize_t cnt = 0;
    /*将读文件描述符加入描述符集合*/
    FD_ZERO(&rfds);
    FD_SET(fd,&rfds);
    /*设置超时为15s*/
    time.tv_sec = 15;
    time.tv_usec = 0;
    int ret;
    /*实现多路IO*/
    ret = select(fd+1, &rfds ,NULL, NULL, &time);
    switch (ret) {
    case -1:
        fprintf(stderr,"select error!\n");
        break;
    case 0:
        fprintf(stderr, "time over!\n");
        break;
    default:
        cnt = read(fd, src_buff, len);
        if(cnt == -1){
            fprintf(stderr, "safe read failed!\n");
            return -1;
        }
        return cnt;
    }
   return -1;
}
