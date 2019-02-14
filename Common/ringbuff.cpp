#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "ringbuff.h"

using namespace std;



Ringbuf::Ringbuf(int len)
{
    Cache.buff=new uint8_t[len];
    memset(Cache.buff,0,len);
    Cache.read=0;
    Cache.write=0;
    Cache.len=len;
    
}

Ringbuf::~Ringbuf(void)
{
    delete Cache.buff;
}

int Ringbuf::canwrite(void)
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


int Ringbuf::canread(void)
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

int Ringbuf::add(uint8_t *buff,int n)
{
    int lave=0;
    if(canwrite()>=n)
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
        Cache.pk_total+=n;
        return n;
    }
    return 0;
    
}

int Ringbuf::read(uint8_t *buff,int len)
{
    int lave;
    if(canread()>=len)
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

int Ringbuf::browse(uint8_t *buff,int len)
{
    int lave;
    if(canread()>=len)
    {
       if(Cache.write>=Cache.read)
       {
           memcpy(buff,Cache.buff+Cache.read,len);
       }
       else
       {
           lave=Cache.len-Cache.read;
           if(lave>=len)
           {
                memcpy(buff,Cache.buff+Cache.read,len);           
           }
           else
           {
               memcpy(buff,Cache.buff+Cache.read,lave);
               memcpy(buff+lave,Cache.buff,len-lave);
           }
       }
    return len;
    }
    return 0;
}

int Ringbuf::drop(int len,int enable)//丢弃，filter=0，使能PKL
{
    int lave;
    if(canread()>=len)
    {
       if(Cache.write>=Cache.read)
       {
           Cache.read+=len;
       }
       else
       {
           lave=Cache.len-Cache.read;
           if(lave>=len)
           {
                Cache.read+=len;
           }
           else
           {
               Cache.read=len-lave;
           }
       }
    if(enable==1)
      Cache.pk_loss+=len;
    return len;
    }
    return 0;
}

float Ringbuf::pkl(void)
{
  Cache.pkl=Cache.pk_total==0?0:(float)Cache.pk_loss/Cache.pk_total*100.0;
  Cache.pk_total=0;
  Cache.pk_loss=0;
  return Cache.pkl;
}

// int main(int argc, char const *argv[])
// {
//   Ringbuf rb1;
//   Ringbuf rb2;
//   uint8_t tmp[5]={1,2,3,4,5};
//   uint8_t buff[5];
//   rb1.init(10);
//   rb2.init(20);
//   printf("%d %d\n",rb1.canwrite(),rb2.canwrite() );
//   rb1.add(tmp,5);
//   rb2.add(tmp,3);
//   printf("%d %d\n",rb1.canwrite(),rb2.canwrite() );
//   rb1.browse(buff,2);
//   printf("%x %x %d %d\n",buff[0],buff[1],rb1.canread(),rb1.canwrite());
//   rb1.read(buff,2);
//   printf("%x %x %d %d \n",buff[0],buff[1],rb1.canread(),rb1.canwrite());
//   rb1.browse(buff,2);
//   printf("%x %x %d %d\n",buff[0],buff[1],rb1.canread(),rb1.canwrite());
//   rb1.drop(2);
//   printf("%d %d\n",rb1.canread(),rb1.canwrite());
//   // rb1.add();
//   return 0;
// }

struct Ringbuff
{
    uint8_t *buff;
    int read;
    int write;
    int len;
    int pk_total;
    int pk_loss;
    float pkl;
}Cache;


void ringbuff_init(int len)
{
    Cache.buff=new uint8_t[len];
    memset(Cache.buff,0,len);
    Cache.read=0;
    Cache.write=0;
    Cache.len=len;
    Cache.pk_total=0;
    Cache.pk_loss=0;
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
    Cache.pk_total+=n;
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

int ringbuff_r_ndel(uint8_t *buff,int len)
{
    int lave;
    if(ringbuff_canread()>=len)
    {
       if(Cache.write>=Cache.read)
       {
           memcpy(buff,Cache.buff+Cache.read,len);
       }
       else
       {
           lave=Cache.len-Cache.read;
           if(lave>=len)
           {
                memcpy(buff,Cache.buff+Cache.read,len);           
           }
           else
           {
               memcpy(buff,Cache.buff+Cache.read,lave);
               memcpy(buff+lave,Cache.buff,len-lave);
           }
       }
    return len;
    }
    return 0;
}

int ringbuff_drop(int len,int enable)//丢弃，filter=0，使能PKL
{
    int lave;
    if(ringbuff_canread()>=len)
    {
       if(Cache.write>=Cache.read)
       {
           Cache.read+=len;
       }
       else
       {
           lave=Cache.len-Cache.read;
           if(lave>=len)
           {
                Cache.read+=len;
           }
           else
           {
               Cache.read=len-lave;
           }
       }
    if(enable==1)
      Cache.pk_loss+=len;
    return len;
    }
    return 0;
}

float ringbuff_pkl(void)
{
  Cache.pkl=Cache.pk_total==0?0:(float)Cache.pk_loss/Cache.pk_total*100.0;
  Cache.pk_total=0;
  Cache.pk_loss=0;
  return Cache.pkl;
}
// int main()
// {
//    int size=10;
//    uint8_t tmp[size]={0};
//    uint8_t buff[size]={10,9,8,7,6,5,4,3,2,1};
//    printf("init\n");
//    ringbuff_init(size);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("wtire len=9\n");
//    ringbuff_w(buff,9);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("read len=5\n");
//    ringbuff_r(tmp,5);
//    for(int i=0;i<5;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);
//    printf("write len=5\n");
//    ringbuff_w(buff,5);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("read len=3\n");
//    ringbuff_r(tmp,3);
//    for(int i=0;i<3;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("read len=4\n");
//    ringbuff_r(tmp,4);
//    for(int i=0;i<4;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("read len=4\n");
//    ringbuff_r_ndel(tmp,2);
//    for(int i=0;i<2;i++)
//        printf("%d ",*(tmp+i));
//    printf("\n");
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);

//    printf("drop len=1\n");
//    ringbuff_drop(1);
//    for(int i=0;i<size;i++)
//        printf("%d ",*(Cache.buff+i));
//    printf("space=%d canread=%d r=%d,w=%d\n",ringbuff_canwrite(),ringbuff_canread(),Cache.read,Cache.write);   
// }
