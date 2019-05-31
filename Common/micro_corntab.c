#include "micro_corntab.h"

#define debug 0

#include <string.h>

#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <Windows.h>

LinkList *pHead=NULL;
LinkList *pTail=NULL;

LinkList *NewNode()//init or append
{
	LinkList *Node=(LinkList *)malloc(sizeof(LinkList));
	memset(Node,0x00,sizeof(LinkList));
	if(pHead==NULL)
	{
		pHead=Node;
		pTail=Node;
	}
	else
	{
		pTail->next=Node;
		Node->pre=pTail;
		pTail=Node;
	}
	return Node;
}

LinkList *PopNode(int index)
{
	LinkList *Node=pHead;
	while(index-->0)
	{
		if(Node->next!=NULL)
			Node=Node->next;
		else
			return NULL;
	}
	if(Node->next!=NULL)
		Node->next->pre=Node->pre;
	else
		pTail=pTail->pre;
	if(Node->pre!=NULL)
		Node->pre->next=Node->next;
	else
		pHead=Node->next;
	return Node;
}

LinkList *InsertNode(int index)//insert front of index:0-x
{
	LinkList *Node=pHead;
	while(index-->0)
	{
		if(Node->next!=NULL)
			Node=Node->next;
		else
			return NewNode();
	}
	LinkList *newNode=(LinkList *)malloc(sizeof(LinkList));
	memset(newNode,0x00,sizeof(LinkList));
	if(Node->pre!=NULL)
		Node->pre->next=newNode;
	else
		pHead=newNode;
	Node->pre=newNode;
	newNode->next=Node;
	newNode->pre=Node->pre;
	return newNode;
}
LinkList *ModifyNode(int index)
{
	LinkList *Node=pHead;
	while(index-->0)
	{
		if(Node->next!=NULL)
			Node=Node->next;
		else
			return NULL;
	}
	return Node;
}

uint64_t sec_min_analyze(char *str)
{
	uint64_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
    memset(p1,0,size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(head<0 || head>59 || tail<0 || tail>59 || delim<1 ||delim>59 || head>tail)
					goto end;
				for(i=head;i<=tail;i+=delim)
					res|=(uint64_t)1<<i;
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(head<0 || head>59 || tail<0 || tail>59 || head>tail)
					goto end;
				for(i=head;i<=tail;i++)
					res|=(uint64_t)1<<i;
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(delim<1 || delim>59)
					goto end;
				for(i=delim-1;i<60;i+=delim)
					res|=(uint64_t)1<<i;
			}
			else if(p1[0]=='*')
			{
				for (int i = 0; i < 60; ++i)
				{
					res|=(uint64_t)1<<i;
				}
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(head<0 || head>59)
					goto end;
				res|=(uint64_t)1<<head;
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size*sizeof(char));
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
    memset(p1,0,size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(head<0 || head>23 || tail<0 || tail>23 || delim<1 || head>tail)
					goto end;
				for(i=head;i<=tail;i+=delim)
					res|=1<<i;

			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{

				if(head<0 || head>23 || tail<0 || tail>23 || head>tail)
					goto end;
				for(i=head;i<=tail;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(delim<1 || delim>23)
					goto end;
				for(i=delim-1;i<24;i+=delim)
					res|=1<<i;
			}
			else if(p1[0]=='*')
			{
				for (int i = 0; i < 24; ++i)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(head<0 || head>23)
					goto end;
				res|=1<<head;
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size*sizeof(char));
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
    memset(p1,0,size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(head<1 || head>7 || tail<1 || tail>7 || delim<1 || head>tail)
					goto end;
				for(i=head-1;i<tail;i+=delim)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(head<1 || head>7 || tail<1 || tail>7 || delim<1 || head>tail)
					goto end;
				for(i=head-1;i<tail;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(delim<1 || delim>7)
					goto end;
				for(i=delim-1;i<7;i+=delim)
					res|=1<<i;
			}
			else if(p1[0]=='*')
			{
				for(i=0;i<7;i++)
					res|=1<<i;
			}			
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(head<1 || head>7)
					goto end;
				res|=1<<(head-1);
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size*sizeof(char));
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
    memset(p1,0,size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(head<1 || head>31 || tail<1 || tail>31 || delim<1 || delim>31 || head>tail)
					goto end;
				for(i=head-1;i<tail;i+=delim)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(head<1 || head>31 || tail<1 || tail>31 || head>tail)
					goto end;
				for(i=head-1;i<tail;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(delim<1 || delim>31)
					goto end;
				for(i=delim-1;i<31;i+=delim)
					res|=1<<i;
			}
			else if(p1[0]=='*')
			{
				for(i=0;i<31;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(head<1 || head>31)
					goto end;
				res|=1<<(head-1);
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size*sizeof(char));
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

uint32_t month_analyze(char *str)
{
	uint16_t res=0;
 	int head,tail,delim;
    uint8_t size=strlen(str);
    char *p1=(char *)malloc(size*sizeof(char));
    memset(p1,0,size*sizeof(char));
	uint8_t comma=0,i,n;
	for(n=0;n<=size;n++)
	{
		if(str[n]==','|| str[n]=='\0')
		{
			if(sscanf(p1,"%d-%d/%d",&head,&tail,&delim)==3)
			{
				if(head<1 || head>12 || tail<1 || tail>12 || delim<1 || delim>12 || head>tail)
					goto end;
				for(i=head-1;i<tail;i+=delim)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d-%d",&head,&tail)==2)
			{
				if(head<1 || head>12 || tail<1 || tail>12 || head>tail)
					goto end;
				for(i=head-1;i<tail;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"*/%d",&delim)>0)
			{
				if(delim<1 || delim>12)
					goto end;
				for(i=delim-1;i<12;i+=delim)
					res|=1<<i;
			}
			else if(p1[0]=='*')
			{
				for(i=0;i<12;i++)
					res|=1<<i;
			}
			else if(sscanf(p1,"%d",&head)>0)
			{
				if(head<1 || head>12)
					goto end;
				res|=1<<(head-1);
			}
			else
				goto end;
			comma=0;
			memset(p1,0x00,size*sizeof(char));
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

int help(int argc,...)
{
	// va_list arg_list;
	// va_start(arg_list,argc);
	// char *var= va_arg(arg_list, char *);
	// va_end(arg_list);
	printf("this is help %d\n",argc);
	return 0;
}

int feed(int argc,...)
{
	va_list arg_list;
	va_start(arg_list,argc);
	char *var= va_arg(arg_list, char *);
	va_end(arg_list);
	printf("feed %d %s\n",argc,var);
	return 0;
}

int test(int argc,...)
{
	printf("test %d\n",argc);
	return 0;
}


//预定义函数及参数
struct Func funcs[]=
{
	{help,"help","int,..."},//... || 3 int float int || int
	{feed,"feed","int,..."},
	{test,"test","int,..."},
};

LinkList *func_register(char* str)
{
	char name[20];
	char paras[50];
	if(sscanf(str,"%[^(]%*[(]%[^)]",name,paras)==2)//%[^()]%s
	{
		for (uint32_t i = 0; i < sizeof(funcs)/sizeof(struct Func); ++i)
		{
			if(strcmp(name,funcs[i].name)==0)
			{
			    LinkList *newcmd=NewNode();
				struct Command line={str,paras,funcs[i].func};
				newcmd->line=line;
				return newcmd;
			}
		}//no this func
		return NULL;
	}
	else//format error
		return NULL;
}

int Corntab_pool()
{
	struct corntab_data date;
	time_t seconds=time(NULL);
	char paras[50];
    LinkList *p;
	while(1)
	{
		seconds=time(NULL);
		date.second=(uint64_t)(seconds%60);
		printf("%d\n",seconds);
		p=pHead;
		while(p!=NULL)
		{
		    p->line.func(3,paras);
            p=p->next;
		}
		Sleep(1000);
	}

}



int Corntab_init(char * path)
{
	char Line[512];
	char s[20],m[20],h[40],dm[20],dw[20],M[20],cmd[20];// char Year,Month,Week,Day,Hour,Minute,Second;
	FILE *p=fopen(path,"r");
	fgets(Line,sizeof(Line),p);
	while(fgets(Line,sizeof(Line),p)!=NULL)
	{
		if(sscanf(Line,"%s %s %s %s %s %s %s\n",s,m,h,dw,dm,M,cmd)==7)
		{
			LinkList *node=func_register(cmd);
			 if(node!=NULL)
			 {
			 	node->line.date.hour|=hour_analyze(h);
			 	node->line.date.minute|=sec_min_analyze(m);
			 	node->line.date.second|=sec_min_analyze(s);
			 	node->line.date.weekday|=day_week_analyze(dw);
			 	node->line.date.monthday|=day_month_analyze(dm);
			 	node->line.date.month|=month_analyze(M);
			 	// printf("%#x %#x %#llx weekday:%#x monthday:%#x month:%x\n",node->line.date.hour,node->line.date.minute,node->line.date.second,\
			 	// 	node->line.date.weekday,node->line.date.monthday,node->line.date.month);
			 }
		}
	}
	fclose(p);
	return 0;
}

//
int main(int argc, char const *argv[])
{
	// FILE *p=fopen("1.txt","w+");
	// fprintf(p,"{second:0-59} {minute:0-59} {hour:0-23} {day-of-month:1-31} {month:1-12} {day-of-week:1-7} {command}\n");
	// for(int i=0;i<1;i++)
	// 	fprintf(p,"%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n","*","1,2","1-5/2,9-15/3,18-22/1","20,1-3,8-12/2,*/10","*/2","0-6","help(999)");
	// fclose(p);
	Corntab_init((char *)"1.txt");
	Corntab_pool();
	return 0;
}

