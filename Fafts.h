#ifndef __AF_FATFS_H
#define __AF_FATFS_H

#include "fatfs.h"
#include "../Own/af_default.h"

extern FATFS fs;							/* FatFs文件系统对象 */
extern FIL file;							/* 文件对象 */
extern FRESULT fres;                      /* 文件操作结果 */
extern UINT fnum;            				/* 文件成功读写数量 */

/* 类型定义 ------------------------------------------------------------------*/
//位图文件头信息结构定义
//其中不包含文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确读取文件信息）
#pragma pack(2) //两字节对齐，否则bmp_fileheader会占16Byte
typedef struct 
{
  uint16_t bfType;                              //固定为：bm  0x4d42
  uint32_t bfSize; 															//文件大小																				4
  uint16_t bfReserved1; 												//保留字，不考虑		 															2
  uint16_t bfReserved2; 												//保留字，同上			 															2
  uint32_t bfOffBits; 													//实际位图数据的偏移字节数，即前三个部分长度之和	4
}BMP_FileHeader;

typedef struct
{
  uint32_t biSize; 														  //指定此结构体的长度，为40		 												4
  uint32_t biWidth; 														//位图宽											 												4
  uint32_t biHeight; 														//位图高											 												4
  uint16_t biPlanes; 														//平面数，为1								 													2
  uint16_t biBitCount; 													//采用颜色位数，可以是1，2，4，8，16，24新的可以是32	2
  uint32_t biCompression; 											//压缩方式，可以是0，1，2，其中0表示不压缩						4
  uint32_t biSizeImage; 												//实际位图数据占用的字节数														4
  uint32_t biXPelsPerMeter; 										//X方向分辨率																					4
  uint32_t biYPelsPerMeter; 										//Y方向分辨率																					4
  uint32_t biClrUsed; 													//使用的颜色数，如果为0，则表示默认值(2^颜色位数)			4
  uint32_t biClrImportant; 										  //重要颜色数，如果为0，则表示所有颜色都是重要的				4
}BMP_InfoHeader;

typedef struct  
{
  uint8_t rgbBlue; 													  //该颜色的蓝色分量
  uint8_t rgbGreen; 													//该颜色的绿色分量
  uint8_t rgbRed; 														//该颜色的红色分量
  uint8_t rgbReserved;											 	//保留值
}BMP_RGBQUAD;

/* 宏定义 --------------------------------------------------------------------*/
// 四个字节对齐  进1制处理
#define WIDTHBYTES(bits)         (((bits)+31)/32*4)		//对于24位真彩色 每一行的像素宽度必须是4的倍数  否则补0补齐
#define GETR_FROM_RGB16(RGB565)  (uint8_t((RGB565>>11)<<3))		  			  //返回8位 R
#define GETG_FROM_RGB16(RGB565)  (uint8_t(((RGB565 & 0x07ff)>>5)<<2)) 	//返回8位 G
#define GETB_FROM_RGB16(RGB565)  (uint8_t(((RGB565 & 0x001f))<<3))      //返回8位 B
//#pragma diag_suppress 870 	//使编译器支持多字节字符,否则会有invalid multibyte character sequence警告 
 void showBmpHeader(BMP_FileHeader *pBmpHead);
 void showBmpInforHeader(BMP_InfoHeader *pBmpInforHead);
#endif
