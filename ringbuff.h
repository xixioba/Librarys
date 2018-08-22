#ifndef __ringbuff_h

void ringbuff_init(int len);
int ringbuff_canwrite(void);
int ringbuff_canread(void);
int ringbuff_w(uint8_t *buff,int n);
int ringbuff_r(uint8_t *buff,int len);

#endif // __ringbuff_h
