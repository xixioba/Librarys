#include <iostream>
#include <string.h>
#include <stdio.h>
#include "ringbuff.h"

struct Ringbuff
{
    uint8_t *buff;
    int read;
    int write;
    int len;
}Cache;


void ringbuff_init(int len)
{
    Cache.buff=new uint8_t[len];
    memset(Cache.buff,0,len);
    Cache.read=0;
    Cache.write=0;
    Cache.len=len;
}

int ringbuff_canwrite(void)
{
   int lave=0;
   if(Cache.write>Cache.read)
   {
       lave=Cache.len-Cache.write+Cache.read-1;
   }
   else if(Cache.write<Cache.read)
   {
       lave=Cache.read-Cache.write-1;
   }
   else
   {
       lave=Cache.len-1;
   }
    return lave;
}
int ringbuff_canread(void)
{
    int lave=0;
    if(Cache.write>Cache.read)
    {
       lave=Cache.write-Cache.read;
    }
    else if(Cache.write<Cache.read)
    {
       lave=Cache.len-Cache.read+Cache.write;
    }
    return lave;
}
int ringbuff_w(uint8_t *buff,int n)
{
    int lave=0;
    if(ringbuff_canwrite()>=n)
    {
        if(Cache.write>=Cache.read)
        {
            lave=Cache.len-Cache.write;
            if(lave>=n)
            {
                memcpy(Cache.buff+Cache.write,buff,n);
                Cache.write+=n;
            }
            else
            {
                memcpy(Cache.buff+Cache.write,buff,lave);
                memcpy(Cache.buff,buff+lave,n-lave);
                Cache.write=n-lave;
            }
        }
        else
        {
            lave=Cache.read-Cache.write;
            memcpy(Cache.buff+Cache.write,buff,n);
            Cache.write+=n;
        }
    return n;
    }
    return 0;
}

int ringbuff_r(uint8_t *buff,int len)
{
    int lave;
    if(ringbuff_canread()>=len)
    {
       if(Cache.write>=Cache.read)
       {
           memcpy(buff,Cache.buff+Cache.read,len);
//           memset(Cache.buff+Cache.read,0,len);
           Cache.read+=len;
       }
       else
       {
           lave=Cache.len-Cache.read;
           if(lave>=len)
           {
                memcpy(buff,Cache.buff+Cache.read,len);
//                memset(Cache.buff+Cache.read,0,len);
                Cache.read+=len;
           }
           else
           {
               memcpy(buff,Cache.buff+Cache.read,lave);
//               memset(Cache.buff+Cache.read,0,lave);
               memcpy(buff+lave,Cache.buff,len-lave);
//               memset(Cache.buff,0,len-lave);
               Cache.read=len-lave;
           }
       }
    return len;
    }
    return 0;
}

//int main()
//{
//    int size=10;
//    uint8_t tmp[size]={0};
//    uint8_t buff[size]={10,9,8,7,6,5,4,3,2,1};
//    ringbuff_init(size);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//    ringbuff_w(buff,9);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//    ringbuff_r(tmp,5);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//    ringbuff_w(buff,5);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//
//    ringbuff_r(tmp,3);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//
//    ringbuff_r(tmp,4);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//}
