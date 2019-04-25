#ifndef XP_LOGS_H
#define XP_LOGS_H
#include <time.h>
#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include "dirent.h"
#include <direct.h>
#include <io.h>
#endif
#include "ringbuff.h"
class xp_logs
{
    Ringbuf *cachebuff;
    int Cachesize;
    char logpath[512];
    int maxtotalsize=10*1024*1024;//单位 K
    int maxlogsize=1*1024*1024;//单位 K
public:
    xp_logs(char *path,int cachesize=0);//单位BYTE,cachesize=0,普通模式，cachesize>0,启用cachepool，数据将存入缓存中，稍后自动写入,保证缓存大小可靠，减少丢包
    ~xp_logs();
    int add(char *str,int len=0);//len==0,写入字符串，len>0，写入char *数组
    int get_size(char *filename);//获取文件大小
    int get_dir_size(char *path);//获取文件夹大小
    int list_files(char *path);//索引当前文件夹下所有文件
    int list_all_files(char *basePath);//索引所有文件，逐层
    int creat_log();//创建日期命名的文件
    char* get_first_file(char *path);//获取指定目录下第一个文件名
    char* get_last_file(char *path);//获取指定目录下最后一个文件名
    int writefile(char* path ,const void *str,int len);
    //cache mode，加入缓存机制，每1s操作一次disk
    void cachepool(void);//缓存循环
    int add_cache(char *str,int len=0);//len==0,写入字符串，len>0，写入char *数组
};

#endif // XP_LOGS_H
