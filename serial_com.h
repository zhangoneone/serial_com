#ifndef SERIAL_COM_H
#define SERIAL_COM_H

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef int SERIAL_RET;
typedef void *(*generic)(void*);

#ifdef UNIX
SERIAL_RET serial_open(const void *filename,int flag);
SERIAL_RET serial_config(int fd,int baude,int c_flow, int bits, char parity, int stop);
SERIAL_RET serial_close(uint fd);
SERIAL_RET serial_read(uint fd,void *dst_buff,uint len);
SERIAL_RET serial_write(uint fd,const void *src_buff,uint len);
#elif defined WIN32
SERIAL_RET serial_open(const void *filename,int flag);
SERIAL_RET serial_config(int fd,int baude,int c_flow, int bits, char parity, int stop);
SERIAL_RET serial_close(uint fd);
SERIAL_RET serial_read(uint fd,void *dst_buff,uint len);
SERIAL_RET serial_write(uint fd,const void *src_buff,uint len);
#endif

#endif
