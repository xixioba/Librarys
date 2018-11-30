#include "../Own/af_fafts.h"

FATFS fs;							/* FatFs文件系统对象 */
FIL file;							/* 文件对象 */
FRESULT fres;                      /* 文件操作结果 */
UINT fnum;            				/* 文件成功读写数量 */ 



/**
  * 函数功能: 打印BMP文件的头信息，用于调试
  * 输入参数: pBmpHead:BMP文件的头信息
  * 返 回 值: 无
  * 说    明：无
  */
 void showBmpHeader(BMP_FileHeader *pBmpHead)
{
    DEBUG("位图文件头:\n");
    DEBUG("文件类型:%d\n",(*pBmpHead).bfType);
    DEBUG("文件大小:%d\n",(*pBmpHead).bfSize);
    DEBUG("保留字:%d\n",(*pBmpHead).bfReserved1);
    DEBUG("保留字:%d\n",(*pBmpHead).bfReserved2);
    DEBUG("实际位图数据的偏移字节数:%d\n",(*pBmpHead).bfOffBits);
		DEBUG("\n");	
}

/**
  * 函数功能: 打印BMP文件的头信息，用于调试
  * 输入参数: pBmpHead:BMP文件的头信息
  * 返 回 值: 无
  * 说    明：无
  */
 void showBmpInforHeader(BMP_InfoHeader *pBmpInforHead)
{
    DEBUG("位图信息头:\n");
    DEBUG("结构体的长度:%d\n",(*pBmpInforHead).biSize);
    DEBUG("位图宽:%d\n",(*pBmpInforHead).biWidth);
    DEBUG("位图高:%d\n",(*pBmpInforHead).biHeight);
    DEBUG("biPlanes平面数:%d\n",(*pBmpInforHead).biPlanes);
    DEBUG("biBitCount采用颜色位数:%d\n",(*pBmpInforHead).biBitCount);
    DEBUG("压缩方式:%d\n",(*pBmpInforHead).biCompression);
    DEBUG("biSizeImage实际位图数据占用的字节数:%d\n",(*pBmpInforHead).biSizeImage);
    DEBUG("X方向分辨率:%d\n",(*pBmpInforHead).biXPelsPerMeter);
    DEBUG("Y方向分辨率:%d\n",(*pBmpInforHead).biYPelsPerMeter);
    DEBUG("使用的颜色数:%d\n",(*pBmpInforHead).biClrUsed);
    DEBUG("重要颜色数:%d\n",(*pBmpInforHead).biClrImportant);
}
	
