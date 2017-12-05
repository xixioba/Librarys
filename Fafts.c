#include "../Own/af_fafts.h"

FATFS fs;							/* FatFs�ļ�ϵͳ���� */
FIL file;							/* �ļ����� */
FRESULT fres;                      /* �ļ�������� */
UINT fnum;            				/* �ļ��ɹ���д���� */ 



/**
  * ��������: ��ӡBMP�ļ���ͷ��Ϣ�����ڵ���
  * �������: pBmpHead:BMP�ļ���ͷ��Ϣ
  * �� �� ֵ: ��
  * ˵    ������
  */
 void showBmpHeader(BMP_FileHeader *pBmpHead)
{
    DEBUG("λͼ�ļ�ͷ:\n");
    DEBUG("�ļ�����:%d\n",(*pBmpHead).bfType);
    DEBUG("�ļ���С:%d\n",(*pBmpHead).bfSize);
    DEBUG("������:%d\n",(*pBmpHead).bfReserved1);
    DEBUG("������:%d\n",(*pBmpHead).bfReserved2);
    DEBUG("ʵ��λͼ���ݵ�ƫ���ֽ���:%d\n",(*pBmpHead).bfOffBits);
		DEBUG("\n");	
}

/**
  * ��������: ��ӡBMP�ļ���ͷ��Ϣ�����ڵ���
  * �������: pBmpHead:BMP�ļ���ͷ��Ϣ
  * �� �� ֵ: ��
  * ˵    ������
  */
 void showBmpInforHeader(BMP_InfoHeader *pBmpInforHead)
{
    DEBUG("λͼ��Ϣͷ:\n");
    DEBUG("�ṹ��ĳ���:%d\n",(*pBmpInforHead).biSize);
    DEBUG("λͼ��:%d\n",(*pBmpInforHead).biWidth);
    DEBUG("λͼ��:%d\n",(*pBmpInforHead).biHeight);
    DEBUG("biPlanesƽ����:%d\n",(*pBmpInforHead).biPlanes);
    DEBUG("biBitCount������ɫλ��:%d\n",(*pBmpInforHead).biBitCount);
    DEBUG("ѹ����ʽ:%d\n",(*pBmpInforHead).biCompression);
    DEBUG("biSizeImageʵ��λͼ����ռ�õ��ֽ���:%d\n",(*pBmpInforHead).biSizeImage);
    DEBUG("X����ֱ���:%d\n",(*pBmpInforHead).biXPelsPerMeter);
    DEBUG("Y����ֱ���:%d\n",(*pBmpInforHead).biYPelsPerMeter);
    DEBUG("ʹ�õ���ɫ��:%d\n",(*pBmpInforHead).biClrUsed);
    DEBUG("��Ҫ��ɫ��:%d\n",(*pBmpInforHead).biClrImportant);
}
	
