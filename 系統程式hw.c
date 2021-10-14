#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define prime 11

typedef struct optab{
	char name[20];
	char info[6];
	char format[5];
	char code[3];
}Optab;

typedef struct Block* Use;
struct Block{
	int addr;
	int counter;  //�p�ƾ� 
	char name[10];
	int key;
	struct Block* next;
};
typedef struct node* nodeptr;
typedef struct node{
	char symname[10]; //�аO�W�� 
	char optag; //#,@,=
	char exformat;
	char opcode[10];
	char optr; // + - * /
	char optr1[10];
	char optr2[10];
	int addr;
	char str[50];
	char destination[20];
	Use use_block;
	struct node* next;
}listnode;



Optab op[] = {
	{"STL","m","3/4","14"},
	{"LDB","m","3/4","68"},
	{"JSUB","m","3/4","48"},
	{"LDA","m","3/4","00"},
	{"COMP","m","3/4","28"},
	{"JEQ","m","3/4","30"},
	{"J","m","3/4","3C"},
	{"STA","m","3/4","0C"},
	{"CLEAR","r1","2","B4"},
	{"LDT","m","3/4","74"},
	{"TD","m","3/4","E0"},
	{"RD","m","3/4","D8"},
	{"COMPR","r1,r2","2","A0"},
	{"STCH","m","3/4","54"},
	{"TIXR","r1","2","B8"},
	{"JLT","m","3/4","38"},
	{"STX","m","3/4","10"},
	{"LDCH","m","3/4","50"},
	{"WD","m","3/4","DC"},
	{"RSUB","null","3/4","4C"},
	};
	
char reg_name[][4] = 
	{"A","X","L","PC","SW","B","S","T","F"};
int reg_num[] = {0,1,2,8,9,3,4,5,6};
//char filename[20];
nodeptr SymTab[prime];
nodeptr LitTab[prime];
nodeptr head=NULL; //literal
nodeptr pool=NULL;
int len_cnt;
int pc, base;
int use_num=1;
Use use=NULL;

Use createblock(){
	Use newnode = (Use)malloc(sizeof(struct Block));
	newnode->next=NULL;
	newnode->counter=0;
	if(newnode){
		return newnode;
	}
	return NULL; 
}
nodeptr create(){	//�s�W�Ŷ� 
	nodeptr newnode = NULL;
	newnode = (nodeptr)malloc(sizeof(struct node));
	newnode->next = NULL;
	if(newnode){
		return newnode;
	}
	return NULL;
}
char* get(char* str,int start,int end){		//���o�r�� 
	char* temp;
	temp = (char*)malloc(sizeof(char)*(end-start+1));
	int index = 0;
	int i;
	for(i=start;i<=end;i++){
		if(str[i]==' '||i>=strlen(str)||str[i]=='\n'){
			break;	
		}
		temp[index++] = str[i];
	}
	temp[index] = '\0';
	return temp;
}
int hash(char *str){
	int i;
	int total=0;
	for(i=0;i<strlen(str);i++){
		total+=str[i];
	}
	int ans;
	ans = total%prime;
	return ans;
}

void addpool(char* str,Use u){
	nodeptr newnode = create();
	newnode->use_block = u;
	newnode->exformat = '=';
	memset(newnode->destination,' ',sizeof(newnode->destination));
	newnode->destination[strlen(newnode->destination)-2] = '\0';
	memset(newnode->str,' ',sizeof(newnode->str));
	char* s = newnode->str;
	s[0] = '*';
	for(int i=0;i<=8;i++){
		s[i+7] = str[i+15];
	}
	s[32] = '\0';
	strcpy(newnode->symname,get(str,16,23));
	if(pool == NULL){
		pool = newnode;
	}
	else{
		nodeptr ptr = pool;
		while(ptr->next!=NULL){
			ptr=ptr->next;
		}
		ptr->next=newnode;
	}
} 

int searchOpTab(char* opname){
	int i;
	for(i=0;i<20;i++){
		if(strcmp(op[i].name,opname) == 0){	//���ۦP���W�ٴN�^�� 			
			return i;
		}		
	}
	return -1;
}
int searchreg(char* regname){
	int i;
	for(i=0;i<9;i++){
		if(strcmp(reg_name[i],regname) == 0){ 
			return reg_num[i];
		}
		
	}
	return 0;
}
nodeptr searchsym(nodeptr tab,char* str){ //��symbol 
	while(tab!=NULL){
		if(strcmp(tab->symname,str) == 0){
			return tab;
		}
		tab = tab->next;
	}
	return NULL;
}
Use searchblock(Use u,char* str){
	while(u!=NULL){
		if(strcmp(u->name,str) == 0){
			return u;
		}
		u = u->next;
	}
	return NULL;
}
nodeptr searchlit(nodeptr tab,char* str){
	while(tab!=NULL){
		if(strcmp(tab->symname,str) == 0){
			return tab;
		}
		tab=tab->next;
	}
	return NULL;
}
void clear(Use u){
	int index;
	nodeptr n;
	while(pool!=NULL){
		
		index = hash(pool->symname);
		n=searchlit(LitTab[index],pool->symname);
		pool->addr = u->counter;
		n->addr = u->counter; 
		pool->use_block = u;
		n->use_block = u;
		if(pool->symname[0] == 'C'){
			
			u->counter += strlen(pool->symname)-3;
		}else if(pool->symname[0] == 'X'){
			u->counter += (strlen(pool->symname)-3)/2;
		}
		pool=pool->next;
	}
	pool = NULL;
}
void printlit(){
	printf("litable\n");
	printf("------------------------------------------------------------------\n");
	printf("row\thash\tname\taddress\tblock\n");
	int i,row=1;
	for(i=0;i<prime;i++){
		nodeptr ptr = LitTab[i];
		while(ptr!=NULL){
			printf("%d\t%d\t%s\t%04x\t%s\n",row++,i,ptr->symname,ptr->addr,ptr->use_block->name);
			ptr=ptr->next;
		}				
	}	
}
void Printoptab(){	//�L�Xop 
	printf("OPTAB\n");
	printf("----------------------------------------------------------------\n");
	int i;
	printf("row\tOp_name\tFormat\tOpcode\tinfo\n");
	for(i=0;i<20;i++){
		printf("%2d\t%s\t%s\t%s\t%s\n",i+1,op[i].name,op[i].format,op[i].code,op[i].info);
	}
}
void Printregtab(){		//�L�Xreg 
	printf("REGTAB\n");
	printf("-----------------------------------------------------------------\n");
	int i;
	printf("row\tREG_Name\tREG_Code\n");
	for(i=0;i<9;i++){
		printf("%2d\t%5s\t\t%4d\n",i+1,reg_name[i],reg_num[i]);
	}
}
void addsym(nodeptr *head,nodeptr ptr){
	nodeptr newnode = create();
	newnode->addr = ptr->addr;
	strcpy(newnode->symname,ptr->symname);
	newnode->use_block = ptr->use_block;
	if(*head == NULL){
		*head = newnode;
	}
	else{
		nodeptr p = *head;
		while(p->next!=NULL){
			p=p->next;
		}
		p->next=newnode;
	}
}
void addlit(nodeptr* head,char* str,Use u){
	nodeptr ptr = create();	//���Ŷ� 
	ptr->use_block = u;
	//printf("/%d/",u->counter);
	memset(ptr->str,' ',sizeof(str));
	strcpy(ptr->symname,get(str,16,23)); //���o �B�⤸ 
	if(*head == NULL){  //head�L���� 
		*head = ptr;
	}else{
		nodeptr temp = *head;
		while(temp->next!=NULL){
			temp=temp->next; 
		}
		temp->next = ptr;
	}	
}
Use addBlock(Use *head,char *str){
	Use newnode = createblock();
	newnode->key = use_num++;
	strcpy(newnode->name,get(str,16,23));
	if(*head == NULL){
		*head = newnode;
	}else{
		Use ptr = *head;
		while(ptr->next!=NULL){
			ptr=ptr->next;
		}
		ptr->next=newnode;
	}
	return newnode;
}

nodeptr addnode(nodeptr *head,char* str,Use u){
	nodeptr newnode = create();
	newnode->use_block = u;
	newnode->exformat = str[7];
	newnode->optag = str[15];
	newnode->optr = str[23];
	newnode->addr = u->counter;
	strcpy(newnode->symname,get(str,0,5));
	strcpy(newnode->opcode,get(str,8,13));
	strcpy(newnode->optr1,get(str,16,23));
	strcpy(newnode->optr2,get(str,25,32));
	strcpy(newnode->str,str);
	nodeptr ptr;
	if(*head == NULL){
		*head = newnode;
	} else {
		ptr = *head;
		while(ptr->next != NULL){
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
	return newnode;
}
void destination(){
	nodeptr ptr = head;
	nodeptr n;
	int pre,xbpe;
	int disp,i;
	char temp[8];
	char strdisp[8];
	int pc; //�p�ƾ� 
	int symindex,index;
	while(ptr!=NULL){
		if(strcmp(ptr->opcode,"BASE") == 0){  //�����򩳪���} 
			symindex = hash(ptr->optr1);
			n = searchsym(SymTab[symindex],ptr->optr1);
			base = n->addr;
		}
		else{
			index = searchOpTab(ptr->opcode);
			if(index!=-1){  //�s�bopcode 
				/*ni xbpe*/ 
				pre = strtol(op[index].code,NULL,16);	//���Xopcode��16�i�� 
				if(ptr->optag == '@'){	 //�����w�} 
					pre+=2;
				}
				else if(ptr->optag == '#'){ 
					pre+=1;
				}
				else{
					pre+=3;
				}
				if(ptr->optag != '='){ //�D�`�� 
					symindex = hash(ptr->optr1);
					n = searchsym(SymTab[symindex],ptr->optr1);
				}
				else{
					symindex = hash(ptr->optr1);
					n = searchlit(LitTab[symindex],ptr->optr1);
				}
				xbpe = 0;
				if(ptr->exformat == '+'){		//�榡4 
					pc = ptr->addr+4;
					
				}
				else if(op[index].format[0] == '3'){
					pc=ptr->addr+3;
				}
				else{
					pc=ptr->addr+2;		//reg
				}
				if(ptr->exformat == '+'){
				if(n){
						sprintf(temp,"%2x1%05x",pre,n->addr);
					}
					else{
						sprintf(temp,"%2x1%05x",pre,atoi(ptr->optr1));
					}
					strcpy(ptr->destination,temp);
				}
				else{
					if(op[index].format[0] == '3'){		//�s�bbase 
						if(n){
							//printf("in");
							disp = n->addr + n->use_block->addr - pc;
							if(disp>2047||disp<-2048){
								xbpe += 4;
								disp = n->addr - base;
							}
							else{
							xbpe += 2;
							}
						}
						sprintf(strdisp,"%.3x",disp);
						if(disp<0){
							sprintf(strdisp,"%s",strdisp+5);
						}
						if(strcmp(ptr->optr2,"X") == 0){
							xbpe += 8;
						}
						if(n == NULL){
							i = atoi(ptr->optr1);
							sprintf(strdisp,"%.3x",i);
						}
						sprintf(temp,"%.2x%x%s",pre,xbpe,strdisp);
						strcpy(ptr->destination,temp);
					}
					else{
						pre -= 3;
						sprintf(temp,"%.2x%x%x",pre,searchreg(ptr->optr1),searchreg(ptr->optr2));
						//printf("%d %d",searchreg(ptr->optr1),searchreg(ptr->optr2));
						strcpy(ptr->destination,temp);
					}	
				}	
			}		
			else if(ptr->exformat == '='){
				pre = 0;
				for(i=0;i<strlen(ptr->symname)-3;i++){
					if(ptr->symname[0] == 'C'){
						ptr->destination[pre++] = ptr->symname[i+2]/16 + '0';
						if(ptr->symname[i+2]%16 >=10){
							ptr->destination[pre++] = ptr->symname[i+2]%16 + 55;
						}else{
							ptr->destination[pre++] = ptr->symname[i+2]%16 + 48;
						}
					}else{
						ptr->destination[pre++] = ptr->symname[i+2];	
					}
				}
				ptr->destination[pre] = '\0';
				printf("/%s/",ptr->destination);
			}
			else if(strcmp(ptr->opcode,"BYTE") == 0){
				pre = 0;
				for(i=0;i<strlen(ptr->optr1)-3;i++){
					if(ptr->optr1[0] == 'C'){
						ptr->destination[pre++] = ptr->optr1[i+2]/16 + '0';
						if(ptr->optr1[i+2]%16 >=10){
							ptr->destination[pre++] = ptr->optr1[i+2]%16 + 55;
						}else{
							ptr->destination[pre++] = ptr->optr1[i+2]%16 + 48;
						}
					}else{
						ptr->destination[pre++] = ptr->optr1[i+2];	
					}
				}
				ptr->destination[pre] = '\0';
			}else if(strcmp(ptr->opcode,"WORD") == 0){
				sprintf(ptr->destination,"%.6x",atoi(ptr->optr1));
			}else if(strcmp(ptr->opcode,"START") == 0){
				strcpy(ptr->destination,"");
			}
			if(strlen(ptr->destination) != 0){
				int j = 0;
				char *s = ptr->destination;
				while(s[j]){
					if(s[j]>='a'&&s[j]<='z'){
						s[j]-=32;
					}
					j++;
				}
			}
		}	
		ptr=ptr->next;
	}
}
void printnode(nodeptr ptr){
	int i;
	printf("��l�{��");
	printf("-----------------------------------------\n");
	printf("row/addr/use\tcode\t\t\t\t Target Address\n");
	printf("-------------------------------------------------------------------------------\n");
	while(ptr!=NULL){
		printf("%2d %04x %2d\t%-4s\t%s\n",++i,ptr->addr,ptr->use_block->key,ptr->str,ptr->destination);
		ptr=ptr->next;
	}	
}
void printsym(){
	int i=0,row=1;
	printf("Symtab\n");
	printf("---------------------------------------------\n");
	printf("row\thash\tname\taddr\n");
	for(i=0;i<prime;i++){
		nodeptr ptr = SymTab[i];
		while(ptr!=NULL){
			printf("%2d\t%d\t%4s\t%04x\n",row++,i,ptr->symname,ptr->addr);
			ptr=ptr->next;
		}
	}
}
int main(){
	
	FILE* fp_r = fopen("srcpro.txt","r");
	FILE* fp_w = fopen("OBJECTFILE.txt","w");
	
	char str[50];
	char temp;
	int index=0;
	Use u;
	int r ;//�P�_�榡 
	int len = 0; //�榡�j�p 
	int Index;
	int i;
	int count = 0;
	nodeptr n,optr_1,optr_2;
	use = createblock();
	strcpy(use->name,"DEFAULT"); //�@�}�l���w�] 
	use->key = 0;  
	u = use;
	memset(str,' ',strlen(str));
	while(!feof(fp_r)){
		fgets(str,50,fp_r);
		for(i=0;i<strlen(str);i++){
			if(str[i] == '\n'){
				str[i] = '\0';
			}
		}
		if(str[0]=='.'){
			continue;
		}
		if(strcmp(get(str,8,13),"USE") == 0){
			if(strlen(get(str,16,23))==0){  //default
				u = use;
			}
			else{
				u = searchblock(use,get(str,16,23));
				if(u == NULL){
					addBlock(&use,str);
				}
			}
		}
		nodeptr newnode = addnode(&head,str,u);
		if(newnode->optag == '='){		//�`�� 
			index = hash(newnode->optr1);
			n = searchlit(LitTab[index],newnode->optr1);
			if(n == NULL){
				addlit(&LitTab[index],str,u);
				addpool(str,u);
			}
		}
		if(strcmp(get(str,8,13),"LTORG") == 0){
			newnode->next = pool;
			clear(u);
		}
		if(strcmp(get(str,8,13),"END")==0){
			newnode->next=pool;
			clear(u);
		}
		if(strcmp(get(str,8,13),"EQU") == 0){
			index = hash(newnode->optr1);
			optr_1 = searchsym(SymTab[index],newnode->optr1);
			index = hash(newnode->optr2);	
			optr_2 = searchsym(SymTab[index],newnode->optr2);			
			if(newnode->optr == '+'){
				newnode->addr = optr_1->addr + optr_2->addr;
			}
			else if(newnode->optr == '-'){				
				newnode->addr = optr_1->addr - optr_2->addr;
			}
			else if(newnode->optr == '*'){
				newnode->addr = optr_1->addr * optr_2->addr;
			}
			else if(newnode->optr == '/'){
				newnode->addr = optr_1->addr / optr_2->addr;
			}
		}
		if(strcmp(get(str,8,13), "START") == 0){
			newnode->addr = atoi(newnode->optr1);  //reset
			u->addr = atoi(newnode->optr1);
			u->counter = atoi(newnode->optr1);			
		}
		else{
			r = searchOpTab(get(str,8,13));
			if(r!=-1){
				if(str[7] == '+'){
					len = 4;
				} else if (op[r].format[0] == '2'){	//�榡�G 
					len = 2;
				} else{
					len = 3;
				}
			}
			/* word, resw, resb, byte*/
			/*atoi�r�ꤺ�e����*/
			else if(strcmp(get(str,8,13),"WORD") == 0){
				len = 3;
			} else if(strcmp(get(str,8,13),"RESW") == 0){
				len = atoi(get(str,16,23)) * 3;
			} else if(strcmp(get(str,8,13),"RESB") == 0){
				len = atoi(get(str,16,23)) ;					
			} else if(strcmp(get(str,8,13),"BYTE") == 0){
				len = strlen(get(str,16,23)) - 3;
				if(str[16] == 'X'){
					len/=2;
				}
			}
			if(str[0] != ' '){  //���аO�A�s�Wsymbol 				
				index = hash(newnode->symname);
				n=searchsym(SymTab[index],newnode->symname);
				if(n==NULL){					
					addsym(&SymTab[index],newnode);				
				}
				else{
					//printf("repeat symname\n");
				}				
			}								
			u->counter+=len;			
			len = 0;
		}								
	}
	count=use->addr;
	u=use;
	while(u!=NULL){
			u->addr=count;
			count+=u->counter;
			u=u->next;
	}
	destination();
	
	printnode(head);
	printsym();
	printlit();
	Printoptab();
	Printregtab();
}


