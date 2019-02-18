#ifndef __SERIAL_H
#define __SERIAL_H

int Serial_init(char *devname,int baudrate);
int Serial_R(int fd,char * data,int len);
int Serial_W_RAW(int fd,char * data,int len);
int Serial_W(int fd,char * data);
#endif