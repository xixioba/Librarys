#ifndef __AF_FATFS_H
#define __AF_FATFS_H

#include "fatfs.h"
#include "../Own/af_default.h"

extern FATFS fs;							/* FatFs�ļ�ϵͳ���� */
extern FIL file;							/* �ļ����� */
extern FRESULT fres;                      /* �ļ�������� */
extern UINT fnum;            				/* �ļ��ɹ���д���� */

/* ���Ͷ��� ------------------------------------------------------------------*/
//λͼ�ļ�ͷ��Ϣ�ṹ����
//���в������ļ�������Ϣ�����ڽṹ����ڴ�ṹ������Ҫ�Ǽ��˵Ļ���������ȷ��ȡ�ļ���Ϣ��
#pragma pack(2) //���ֽڶ��룬����bmp_fileheader��ռ16Byte
typedef struct 
{
  uint16_t bfType;                              //�̶�Ϊ��bm  0x4d42
  uint32_t bfSize; 															//�ļ���С																				4
  uint16_t bfReserved1; 												//�����֣�������		 															2
  uint16_t bfReserved2; 												//�����֣�ͬ��			 															2
  uint32_t bfOffBits; 													//ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��	4
}BMP_FileHeader;

typedef struct
{
  uint32_t biSize; 														  //ָ���˽ṹ��ĳ��ȣ�Ϊ40		 												4
  uint32_t biWidth; 														//λͼ��											 												4
  uint32_t biHeight; 														//λͼ��											 												4
  uint16_t biPlanes; 														//ƽ������Ϊ1								 													2
  uint16_t biBitCount; 													//������ɫλ����������1��2��4��8��16��24�µĿ�����32	2
  uint32_t biCompression; 											//ѹ����ʽ��������0��1��2������0��ʾ��ѹ��						4
  uint32_t biSizeImage; 												//ʵ��λͼ����ռ�õ��ֽ���														4
  uint32_t biXPelsPerMeter; 										//X����ֱ���																					4
  uint32_t biYPelsPerMeter; 										//Y����ֱ���																					4
  uint32_t biClrUsed; 													//ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)			4
  uint32_t biClrImportant; 										  //��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��				4
}BMP_InfoHeader;

typedef struct  
{
  uint8_t rgbBlue; 													  //����ɫ����ɫ����
  uint8_t rgbGreen; 													//����ɫ����ɫ����
  uint8_t rgbRed; 														//����ɫ�ĺ�ɫ����
  uint8_t rgbReserved;											 	//����ֵ
}BMP_RGBQUAD;

/* �궨�� --------------------------------------------------------------------*/
// �ĸ��ֽڶ���  ��1�ƴ���
#define WIDTHBYTES(bits)         (((bits)+31)/32*4)		//����24λ���ɫ ÿһ�е����ؿ�ȱ�����4�ı���  ����0����
#define GETR_FROM_RGB16(RGB565)  (uint8_t((RGB565>>11)<<3))		  			  //����8λ R
#define GETG_FROM_RGB16(RGB565)  (uint8_t(((RGB565 & 0x07ff)>>5)<<2)) 	//����8λ G
#define GETB_FROM_RGB16(RGB565)  (uint8_t(((RGB565 & 0x001f))<<3))      //����8λ B
//#pragma diag_suppress 870 	//ʹ������֧�ֶ��ֽ��ַ�,�������invalid multibyte character sequence���� 
 void showBmpHeader(BMP_FileHeader *pBmpHead);
 void showBmpInforHeader(BMP_InfoHeader *pBmpInforHead);
#endif
