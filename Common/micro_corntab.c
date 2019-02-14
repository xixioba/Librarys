#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <windows.h>
#define debug 0

int help(int argc,...)
{
	va_list arg_list;
	va_start(arg_list,argc);
	char *var= va_arg(arg_list, char *);
	va_end(arg_list);
	printf("here %s\n",var);
	return 0;
}

int feed(int argc,...)
{
	return 0;
}

int test(int argc,...)
{
	return 0;
}

struct corntab_data
{
	uint8_t weekday;
	uint32_t monthday;
	uint32_t hour;
	uint64_t minute;	
	uint64_t second;
}corntab_list,corntab_aim;

struct Command 
{
	const char *name;
	const char *desc;
	int (*func)(int argc,...);
	struct corntab_data date;
};

struct Command commands[] = {
{ "help(10.0)", "Display this list of commands",help},
{ "feed(3,int)", "Display this list of commands",feed},
// { "kerninfo", "Display information about the kernel", mon_kerninfo },
// { "backtrace", "Display the current stack trace", mon_backtrace },
};

struct Func
{
	int (*func)(int argc,...);
	const char *name;
	const char *paras;
};

struct Func funcs[]=
{
	{help,"help","char *,char *"},//... || 3 int float int || int
	{feed,"feed","int,..."},
	{test,"test","int,..."},
};

int paras_valid(const char *src,char *target) //ruler_type[int,float,double,char *] target_paras
{
 	int aim;
 	float aim_f;
 	double aim_lf;
    uint8_t src_size=strlen(src);
    char *p1=(char *)malloc((src_size)*sizeof(char));
    memset(p1,0x00,src_size);
    uint8_t target_size=strlen(target);
    char *p2=(char *)malloc((target_size)*sizeof(char));
    memset(p1,0x00,target_size);
    char *p3;
	uint8_t src_comma=0,src_n=0;
	uint8_t target_comma=0,target_n=0;
	uint8_t int_state=0;
	for(src_n=0;src_n<=src_size;src_n++)
	{
		if(src[src_n]==','|| src[src_n]=='\0')
		{
			for (;target_n<=target_size;target_n++)
			{
				if(target[target_n]==','||target[target_n]=='\0')
				{
					target_n++;
					break;
				}
				else
					p2[target_comma++]=target[target_n];
			}
			if(p2[0]==0x00)//too few para
				goto end;
			if(strcmp(p1,"int")==0)
			{
				if(sscanf(p2,"%d",&aim)==0)
					goto end;
				int_state++;
			}
			else if(strcmp(p1,"double")==0)
			{
				if(sscanf(p2,"%lf",&aim_lf)==0)
					goto end;
			}
			else if(strcmp(p1,"float")==0)
			{
				if(sscanf(p2,"%f",&aim_f)==0)
					goto end;
			}
			else if(strcmp(p1,"char *")==0)
			{
				p3=(char *)malloc((target_comma)*sizeof(char));
				memset(p3,0x00,target_comma);
				if(sscanf(p2,"%s",p3)==0)
					goto end;
			}
			else if(strcmp(p1,"...")==0)
			{
				printf("here ...\n");
				if(int_state==1)
				{
					free(p1);
					free(p2);
					free(p3);
					return 10;
				}
			}
			else//wrong type
				goto end;
			target_comma=0;
			memset(p2,0x00,target_size);
			src_comma=0;
			memset(p1,0x00,src_size);		
		}
		else
			p1[src_comma++]=src[src_n];
	}
	if(target_n<target_size)//two many paras
		goto end;
	return 0;
end:
	free(p1);
	free(p2);
	free(p3);
	return -1;
}

int func_register(char* str) //help(100)
{
	char name[20];
	char paras[50];
	if(sscanf(str,"%[^(]%*[(]%[^)]",name,paras)==2)//%[^()]%s
	{
		for (uint32_t i = 0; i < sizeof(funcs)/sizeof(Func); ++i)
		{
			if(strcmp(name,funcs[i].name)==0)
			{	
				if(paras_valid(funcs[i].paras,paras)<0)
				{
					//paras error
				}
				else
					goto success;
			}
		}//no command
		return -1;
	}
	else//grammar error
		return -1;
success:
	return 0;
}

int Corntab_max=10;
typedef struct Node 
{
	
	// const char *name;
	// const char *desc;
	// int (*func)(int argc,...);
	// struct corntab_data date;
	int num;
	struct Node *pre;
	struct Node *next;
}*pNode;

static pNode pHead=NULL;
static pNode pTail=NULL;
static int Node_count=0;


pNode NewNode(int count)//init or append
{
	pNode Node=(pNode)malloc(sizeof(struct Node));
	Node->next=Node->pre=NULL;
	Node->num=count;
	if(NULL==Node)
		return NULL;
	if(NULL==pHead)
		pHead=pTail=Node;
	else
		Node->pre=pTail;
		pTail->next=Node;
		pTail=Node;
	Node_count++;
	return Node;
}

pNode PopNode(int index)
{
	pNode p=(pNode)malloc(sizeof(struct Node));
	if(Node_count<abs(index))
		return NULL;
	if(index>0)
	{
		p=pHead;
		while(index-->0)
			p=p->next;
		p->pre->next=p->next;
		p->next->pre=p->pre;
		return p;		
	}
	else if((index==0) || (Node_count==abs(index)))
	{
		p=pHead;
		pHead->next->pre=NULL;
		pHead=pHead->next;
		return p;
	}
	else if(index==-1)
	{
		p=pTail;
		p->pre->next=NULL;
		pTail=p->pre;
		return p;	
	}
	else if(index<-1)
	{
		p=pTail;
		while(++index<0)
			p=p->pre;
		p->pre->next=p->next;
		p->next->pre=p->pre;
		return p;		
	}
	return NULL;
}

pNode InsertNode(int index)
{
	pNode p;
	pNode Node=(pNode)malloc(sizeof(struct Node));
	Node->next=Node->pre=NULL;
	if(NULL==Node)
		return NULL;
	if(abs(index)>Node_count)
		return NULL;
	if(index>0)
	{
		p=pHead;
		while(index-->0)
			p=p->next;
		p->pre->next=Node;
		p->pre=Node;
		Node->next=p;
		Node->pre=p->pre;
		return Node;		
	}
	else if((index==0) || (Node_count==abs(index)))
	{
		Node->next=pHead;
		pHead->pre=Node;
		pHead=Node;
		return Node;
	}
	else if(index<0)
	{
		p=pTail;
		while(++index<0)
			p=p->pre;
		p->pre->next=Node;
		p->pre=Node;
		Node->pre=p->pre;
		Node->next=p;
		return Node;		
	}
	return NULL;	
}

int Corntab_init()
{
	for (int i = 0; i < 10; i++)
	{
		NewNode(i);
	}
	pNode p=pHead;
	while(p->next!=NULL)
	{
		printf("%d\n",p->num);
		p=p->next;
	}
	printf("%d\n",p->num);

	// printf("\npop:%d\n",PopNode(-10)->num);
	p=InsertNode(1);
	p->num=100;
	p=pTail;
	printf("\n");
	while(p->pre!=NULL)
	{
		printf("%d\n",p->num);
		p=p->pre;
	}
	printf("%d\n",p->num);
	return 0;
}

uint64_t sec_min_analyze(char *str)
{
	uint64_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(debug) 
					printf("3:%s\n",p1 );
				if(head<0 || head>59 || tail<0 || tail>59 || delim<1 ||delim>59)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i+=delim)
						res|=(uint64_t)1<<i;
				}
				else
				{
					for(i=head;i<=60+tail;i++)
					{
						if(i>=60)
							res|=(uint64_t)1<<(i-60);
						else
							res|=(uint64_t)1<<i;		
					}		
				}
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(debug)
					printf("2:%s\n",p1 );
				if(head<0 || head>59 || tail<0 || tail>59)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i++)
						res|=(uint64_t)1<<i;
				}
				else
				{
					for(i=head;i<=60+tail;i++)
					{
						if(i>=60)
							res|=(uint64_t)1<<(i-60);
						else
							res|=(uint64_t)1<<i;		
					}		
				}
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(debug)
					printf("1:%s\n",p1 );
				if(delim<1 || delim>59)
					goto end;
				for(i=0;i<60;i+=delim)
					res|=(uint64_t)1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(debug)
					printf("0:%s\n",p1 );
				if(head<0 || head>59)
					goto end;
				res|=(uint64_t)1<<head;				
			}
			else
				goto end; 
			comma=0;
			memset(p1,0x00,size);			
		}
		else
			p1[comma++]=str[n];
	}
	free(p1);
	return res;
end:
	res=0;
	free(p1);
	return res;
}

uint32_t hour_analyze(char *str)
{
	uint32_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(debug)
					printf("3:%s\n",p1);
				if(head<0 || head>23 || tail<0 || tail>23 || delim<1)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i+=delim)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=24+tail;i+=delim)
					{
						if(i>=24)
							res|=1<<(i-24);
						else
							res|=1<<i;		
					}			
				}
				
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(debug)
					printf("2:%s\n",p1);
				if(head<0 || head>23 || tail<0 || tail>23)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i++)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=24+tail;i++)
					{
						if(i>=24)
							res|=1<<(i-24);
						else
							res|=1<<i;		
					}		
				}
				
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(debug)
					printf("1:%s\n",p1);
				if(delim<1 || delim>23)
				{
					res=0;
					goto end;
				}
				for(i=0;i<24;i+=delim)
					res|=1<<i;
				
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(debug)
					printf("0:%s\n",p1);
				if(head<0 || head>23)
					goto end;
				res|=1<<head;
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size);			
		}
		else
			p1[comma++]=str[n];
	}
	free(p1);
	return res;
end:
	res=0;
	free(p1);
	return res;
}

uint32_t day_month_analyze(char *str)
{
	uint32_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(debug)
					printf("3:%s\n",p1);
				if(head<1 || head>31 || tail<1 || tail>31 || delim<1 || delim>31)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i+=delim)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=31+tail;i+=delim)
					{
						if(i>=31)
							res|=1<<(i-30);
						else
							res|=1<<i;		
					}		
				}
				
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(debug)
					printf("2:%s\n",p1);
				if(head<1 || head>31 || tail<1 || tail>31)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i++)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=31+tail;i++)
					{
						if(i>=31)
							res|=1<<(i-30);
						else
							res|=1<<i;		
					}		
				}
				
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(debug)
					printf("1:%s\n",p1);
				if(delim<1 || delim>31)
					goto end;
				for(i=1;i<=31;i+=delim)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(debug)
					printf("0:%s\n",p1);
				if(head<1 || head>31)
					goto end;
				res|=1<<head;
				
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size);			
		}
		else
			p1[comma++]=str[n];
	}
	free(p1);
	return res;
end:
	res=0;
	free(p1);
	return res;
}

uint8_t day_week_analyze(char *str)
{
	uint8_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(debug)
					printf("3:%s\n",p1);
				if(head<0 || head>7 || tail<0 || tail>7 || delim<1 || abs(head-tail)==7)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i+=delim)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=8+tail;i+=delim)
					{
						if(i>7)
							res|=1<<(i-7);
						else
							res|=1<<i;
					}
				}
				
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(debug)
					printf("2:%s\n",p1);
				if(head<0 || head>7 || tail<0 || tail>7)
					goto end;
				if(head<=tail)
				{
					for(i=head;i<=tail;i++)
						res|=1<<i;
				}
				else
				{
					for(i=head;i<=8+tail;i++)
					{
						if(i>7)
							res|=1<<(i-8);
						else
							res|=1<<i;
					}
				}
				
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(debug)
					printf("1:%s\n",p1);
				if(delim<0 || delim>7)
					goto end;
				for(i=1;i<8;i+=delim)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(debug)
					printf("0:%s\n",p1);
				if(head<1 || head>7)
					goto end;
				if(head==0 ||head==7)
				{
					res|=1<<7;
					res|=1<<0;					
				}
				else
					res|=1<<head;
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size);			
		}
		else
			p1[comma++]=str[n];
	}
	free(p1);
	return res;
end:
	res=0;
	free(p1);
	return res;
}

uint8_t corn_trigger()
{
	uint8_t res;

	return res;
}

void corntab_clear()
{
	memset(&corntab_list,0x00,sizeof(corntab_list)*sizeof(char));
}
uint8_t corn_update_line(char *str)
{
	char s[20],m[20],h[40],dm[20],dw[20],M[20],cmd[20];
	if(sscanf(str,"%s %s %s %s %s %s %s",s,m,h,dm,dw,M,cmd)==7)
	{
	    // corntab_list.hour|=hour_analyze(h);
	    // corntab_list.minute|=sec_min_analyze(m);
	    // corntab_list.second|=sec_min_analyze(s);
	    // corntab_list.weekday|=day_week_analyze(dw);
	    // corntab_list.monthday|=day_month_analyze(dm);		
	}
	return 0;
}

int corntab_pool(char *ptr,...)
{
	struct corntab_data date;
	time_t seconds=time(NULL);
	commands[0].date.second=0x1;
	while(1)
	{
		seconds=time(NULL);
		date.second=(uint64_t)(seconds%60);
		// printf("%d\n",seconds);
		for (unsigned int i = 0; i <sizeof(commands)/sizeof(Command); ++i)
		{
			// if((commands[i].date.second>>date.second) ==1)
			{
				char name[20];
				char paras[50];
				if(sscanf(commands[i].name,"%[^(]%*[(]%[^)]",name,paras)==2)//%[^()]%s
				{
					// printf("name=%s %s\n",name,paras);
					commands[i].func(1,paras);
				}
				// printf("%I64u %I64u\n",commands[i].date.second,date.second);
			}
			// if (strcmp(ptr, commands[i].name) == 0)
			// {
			// 	commands[i].func(1,2);
			// }
		}
		Sleep(100);		
	}

}


 



int main(int argc, char const *argv[])
{
	FILE *p=fopen("1.txt","w+");
	char s[20],m[20],h[40],dm[20],dw[20],M[20],cmd[20];
	// char Year,Month,Week,Day,Hour,Minute,Second;
	fprintf(p,"{second:0-59} {minute:0-59} {hour:0-23} {day-of-month:0-31} {month:0-12} {day-of-week:0-7} {command}\n");
	for(int i=0;i<1;i++)
		fprintf(p,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n","*","1,2","1-5/2,9-15/3,18-22/1","20","*/2","0-6","help(1000)");
	fclose(p);
	p=fopen("1.txt","r");
	//fseek(stream,0,SEEK_SET);
	fscanf(p,"%s %s %s %s %s %s %s\n",s,m,h,dm,dw,M,cmd);
		printf("%s %s %s %s %s %s %s\n",s,m,h,dm,dw,M,cmd);
	while(fscanf(p,"%s %s %s %s %s %s %s\n",s,m,h,dm,dw,M,cmd)!=-1)
	{

	}
	fclose(p);
	Corntab_init();
	return 0;
}

