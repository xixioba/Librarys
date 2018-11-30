#ifndef __RINGBUFF_H
#define __RINGBUFF_H
//C++
class Ringbuf
{
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
    public:
    	Ringbuf(int len);
        ~Ringbuf(void); 
	    int canwrite(void);
	    int canread(void);
	    int add(uint8_t *buff,int n);
	    int read(uint8_t *buff,int len);
	    int browse(uint8_t *buff,int len);
	    int drop(int len,int enable=1);//drop size，default conut packet loss
	    float pkl(void);//packet loss rate,every time call this function will reset packet loss rate 
};
//C
    void ringbuff_init(int len);
    int ringbuff_canwrite(void);
    int ringbuff_canread(void);
    int ringbuff_w(uint8_t *buff,int n);
    int ringbuff_r(uint8_t *buff,int len);
    int ringbuff_r_ndel(uint8_t *buff,int len);
    int ringbuff_drop(int len,int enable=1);
    float ringbuff_pkl(void);
#endif 
