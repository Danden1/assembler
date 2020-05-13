#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
struct Inst{
    char *name;
    char *op;
    char type;
    char *funct;
};

struct Inst inst[22] = {
    {"addiu", "001001", 'I',""},
    {"addu", "000000", 'R',"100001"},
    {"and", "000000", 'R',"100100"},
    {"andi", "001100", 'I', ""},
    {"beq","000100", 'I', ""},
    {"bne", "000101", 'I', ""},
    {"j", "000010", 'J', ""},
    {"jal", "000011", 'J', ""},
    {"jr", "000000", 'J',"001000"},
    {"lui", "001111", 'I', ""},
    {"lw", "100011", 'I', ""},
    {"nor", "000000", 'R' , "100111"},
    {"or", "000000", 'R', "100101"},
    {"ori", "001101", 'I', ""},
    {"sltiu", "001011",'I' ,""},
    {"sltu", "000000", 'R', "101011"},
    {"sll", "000000", 'S' ,"000000"},
    {"srl", "000000", 'S', "000010"},
    {"sw", "101011", 'I', ""},
    {"subu", "000000", 'R', "100011"}
};

//node
struct Data{
    int value;
    struct Data *next;
};



struct Text{
    int idx; //Inst index
    char *d;
    char *s;
    char *t;
    unsigned int address;
    struct Text *next;
};

//.data
struct Sym{
    char *name;
    int size;
    unsigned int address;
    struct Data *first;
    struct Sym *next;
    struct Data *last;
};

struct Sym *Symbols = NULL;
struct Sym *sym_last = NULL;
struct Text *Texts = NULL;
struct Text *text_last = NULL;
int datasize, textsize;

/*
 * You may need the following function
 */
char* NumToBits (unsigned int num, int len) {
    char* bits = (char *) malloc(len+1);
    int idx = len-1, i;

    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        } else {
            bits[idx--] = '0';
        }
        num /= 2;
    }

    for (i = idx; i >= 0; i--){
        bits[i] = '0';
    }

    return bits;
}

/*
 * read the assmebly code
 */
void read_asm () {

    int tmp = 0, i = 0, j = 0, k = 0;
    char *ptr;  //strtoul
    unsigned int address;
    char temp[0x1000] = {0};
    char rt[10];
    struct Sym *temp_sym = NULL;
    struct Data *temp_data = NULL;
    struct Text *temp_text = NULL;
    Symbols=NULL;
    Texts = NULL;
    //Read .data region
    address = 0x10000000;
    for (i=0; scanf("%s", temp) == 1;) {
       if (strcmp(temp, ".text") == 0) {
			break;
       }
	   else if(temp[strlen(temp)-1] == ':') {
        	temp_sym = (struct Sym*)malloc(sizeof(struct Sym));
        	temp_sym->address = address;
        	temp_sym->first = NULL;
        	temp_sym->last = NULL;
        	temp_sym->next = NULL;
        	temp_sym->size = 0;
          temp_sym->name = (char*)malloc(29);
    		strncpy(temp_sym->name, temp, strlen(temp)-1);
        //temp_sym->name[strlen(temp)-1] = '\0';
        	if(Symbols == NULL){
        		Symbols = temp_sym;
        		sym_last = temp_sym;
		  	 }
			else{
				sym_last->next = temp_sym;
				sym_last = temp_sym;
			}

    }
	   else if(strcmp(temp, ".word") == 0) {
       		address+=4;
       		sym_last->size +=1;
        	temp_data = (struct Data*)malloc(sizeof(struct Data));
			temp_data-> next = NULL;

			scanf("%s",temp);

			if(strncmp(temp,"0x",2) == 0){
				temp_data->value = strtoul(temp+2,&ptr,16);
			}
			else{
				temp_data->value = strtoul(temp,&ptr,10);
			}

			if(sym_last->first == NULL){
				sym_last->first = temp_data;
				sym_last->last = temp_data;
			}
			else{
				sym_last->last->next = temp_data;
				sym_last->last = temp_data;
			}

    	}
    }

    datasize = address - 0x10000000;

    //Read .text region
    address = 0x400000;
    for (i=0; scanf("%s", temp) == 1;) {
       if (temp[strlen(temp)-1] == ':') {
          temp_sym = (struct Sym*)malloc(sizeof(struct Sym));
        	temp_sym->address = address;
        	temp_sym->first = NULL;
        	temp_sym->last = NULL;
        	temp_sym->next = NULL;
        	temp_sym->size = 0;		//no data.(text symbol)
          temp_sym->name = (char*)malloc(29);
    		strncpy(temp_sym->name, temp, strlen(temp)-1);
        	if(Symbols == NULL){
        		Symbols = temp_sym;
        		sym_last = Symbols;
		  	}
			else{
				sym_last->next = temp_sym;
				sym_last = temp_sym;
			}
		}


		else{
        	for(j = 0; j < 20;j++){
    			     if(strcmp("la", temp) ==0){
           		    tmp = 1;
           		     break;
				       }
           		if(strcmp(inst[j].name,temp) == 0){
           			temp_text = (struct Text*)malloc(sizeof(struct Text));
           			temp_text->idx = j;
           			temp_text->next = NULL;
           			temp_text->d = NULL;
           			temp_text->t = NULL;
           			temp_text->s = NULL;
           			temp_text->address = address;
           			address+=4;
           			if(Texts == NULL){
           				Texts = temp_text;
           				text_last = temp_text;
					}
					else{
						text_last->next = temp_text;
						text_last = temp_text;
					}
          			break;
				}
			}


			if(tmp==1){
				//1st, make lui
				temp_text = (struct Text*)malloc(sizeof(struct Text));
     	 		temp_text->idx = 9;
       			temp_text->next = NULL;
       			temp_text->d = NULL;
       			temp_text->t = NULL;
       			temp_text->s = NULL;
           		temp_text->address = address;
           		address+=4;

           		if(Texts == NULL){
           			Texts = temp_text;
           			text_last = temp_text;
				}
				else{
					text_last->next =temp_text;
					text_last = temp_text;
				}
				scanf("%s", rt);
        text_last->d = (char*)malloc(29);
				strncpy(text_last->d,rt,strlen(rt));
				scanf("%s", temp);

				for(temp_sym = Symbols; temp_sym!=NULL; temp_sym =temp_sym->next){
					if(strcmp(temp, temp_sym->name) == 0){
						tmp = temp_sym->address;
						break;
					}
				}
        text_last->t = (char*)malloc(29);
				sprintf(text_last->t,"%d",tmp / 0x10000);
				//check only lui?
				//need ori
				if((tmp & 0xFFFF) != 0){
					temp_text= (struct Text*)malloc(sizeof(struct Text));
					temp_text->idx = 13;
					temp_text->next = NULL;
					temp_text->s = NULL;
          temp_text->t = NULL;
          temp_text->d = NULL;
					temp_text->address = address;
					address+=4;
          text_last->next = temp_text;
          text_last = temp_text;

          text_last->d = (char*)malloc(29);
          strncpy(text_last->d,rt,strlen(rt));
          text_last->t = (char*)malloc(29);
          strncpy(text_last->t,rt,strlen(rt));
          text_last->s = (char*)malloc(29);
					sprintf(text_last->s,"%d",tmp&0xFFFF);


				}


				tmp = 0;
			}
			else{
				for(k = 0; k <3; k++){
					if(k==0){
						scanf("%s", temp);
            text_last->d = (char*)malloc(29);
						if(temp[strlen(temp)-1] != ','){
							strncpy(text_last->d,temp,strlen(temp));
							break;
						}
						strncpy(text_last->d, temp, strlen(temp)-1);

					}
					else if(k==1){
						scanf("%s", temp);
            text_last->t = (char*)malloc(29);
						if(temp[strlen(temp)-1] != ','){
							strncpy(text_last->t,temp,strlen(temp));
							break;
						}
						strncpy(text_last->t, temp, strlen(temp)-1);
					}
					else if(k==2){
            text_last->s = (char*)malloc(29);
						scanf("%s", temp);
						if(temp[strlen(temp)-1] != ','){
							strncpy(text_last->s,temp,strlen(temp));
							break;
						}
						strncpy(text_last->s, temp, strlen(temp)-1);
					}
				}
			}



    	}
    }

    textsize = address - 0x400000;
}

/*
 * convert the assembly code to num
 */
void subst_asm_to_num () {
    struct Text *text;
    struct Sym *sym;
    char *ptr;
    int i =0;
	int is_sym = 0; //check symbol or imm
    for (text = Texts; text != NULL; text = text->next) {
    	is_sym = 0;
    	ptr=NULL;

        if(inst[text->idx].type == 'J'){
        	if(text->d[0] == '$'){
        		if(strcmp(inst[text->idx].name,"jr")==0)
        			strcpy(text->d,NumToBits(strtoul(text->d+1,&ptr,10),5));
        		else{
        			strcpy(text->d,strcat(NumToBits((text->address)>>28,4),NumToBits(strtoul(text->d+1,&ptr,10)/4,22)));
				}
        	}
    		else if(strncmp(text->d,"0x",2)==0){
        		if(strcmp(inst[text->idx].name,"jr")==0)
        			strcpy(text->d, NumToBits(strtoul(text->d+2,&ptr,16),5));
        		else{
        			strcpy(text->d ,strcat(NumToBits(text->address>>28,4),NumToBits(strtoul(text->d+2,&ptr,16)/4,22)));
				}
    		}
        else{
				for(i=0; i< strlen(text->d); i++){
					if(!isdigit(text->d[i])){
						is_sym=1;
						break;
					}
				}
				if(is_sym==1){
					for(sym=Symbols;sym!=NULL; sym = sym->next){
						if(strcmp(text->d,sym->name)==0){
							sprintf(text->d, "%d",sym->address);
							break;
						}
					}
					if(strcmp(inst[text->idx].name,"jr")==0)
        			strcpy(text->d,NumToBits(strtoul(text->d,&ptr,10),5));
        			else
        				strcpy(text->d, strcat(NumToBits((text->address+4)>>28,4),NumToBits(strtoul(text->d,&ptr,10)/4,22)));

				}
				else{
					if(strcmp(inst[text->idx].name,"jr")==0)
        				strcpy(text->d, NumToBits(strtoul(text->d,&ptr,10),5));
        			else{
        				strcpy(text->d , strcat(NumToBits((text->address)>>28,4),NumToBits(strtoul(text->d,&ptr,10)/4,22)));
					}
				}
			}

		}
		else if(inst[text->idx].type == 'I'){

			strcpy(text->d,NumToBits(strtoul(text->d+1, &ptr, 10),5));
			if(strcmp(inst[text->idx].name,"bne") ==0 || strcmp(inst[text->idx].name,"beq") ==0){
				strcpy(text->t,NumToBits(strtoul(text->t+1, &ptr,10),5));
				for(sym = Symbols; sym!= NULL; sym = sym->next){
					if(strcmp(sym->name,text->s)==0){
						strcpy(text->s,NumToBits((sym->address-(text->address+4))/4,16));

					break;
        }
				}
			}
			else if(strcmp(inst[text->idx].name,"lw") == 0|| strcmp(inst[text->idx].name,"sw")==0){
        text->s = (char*)malloc(29);
				if(strncmp(text->t, "0x", 2) == 0)
					strcpy(text->s, NumToBits(strtoul(text->t+2,&ptr,16),16));

				else
					strcpy(text->s,  NumToBits(strtoul(text->t,&ptr,10),16));

				strcpy(text->t,NumToBits(strtoul(ptr+2,&ptr,10),5));
			}
			else if(strcmp(inst[text->idx].name, "lui") == 0){
				if(strncmp(text->t, "0x", 2) == 0)
					strcpy(text->t, NumToBits(strtoul(text->t+2, &ptr,16),16));

				else
					strcpy(text->t,NumToBits(strtoul(text->t, &ptr, 10),16));


      }
			else{
				strcpy(text->t ,NumToBits(strtoul(text->t+1, &ptr, 10),5));
				if(strncmp(text->s, "0x", 2) == 0){
					strcpy(text->s,NumToBits(strtoul(text->s+2, &ptr, 16),16));
				}
				else{
					strcpy(text->s ,NumToBits(strtoul(text->s,&ptr, 10),16));
				}
			}
		}
		else if(inst[text->idx].type =='R'){
		    strcpy(text->d,NumToBits(strtoul(text->d+1,&ptr,10),5));
        strcpy(text->t,NumToBits(strtoul(text->t+1,&ptr,10),5));
      strcpy(text->s,NumToBits(strtoul(text->s+1,&ptr,10),5));

		}
		else if(inst[text->idx].type == 'S'){
        	strcpy(text->d,NumToBits(strtoul(text->d+1,&ptr,10),5));
        	strcpy(text->t,NumToBits(strtoul(text->t+1,&ptr,10),5));
    		strcpy(text->s ,NumToBits(strtoul(text->s,&ptr,10),5));
		}

    }
}


/*
 * print the results of assemble
 */
void print_bits () {
    struct Text* text;
    struct Sym* sym;
    struct Data* data;

    // print the header
    printf("%s", NumToBits(textsize, 32));
    printf("%s", NumToBits(datasize, 32));

    // print the body
    for (text = Texts; text != NULL ; text = text->next) {
        printf("%s", inst[text->idx].op);

        if (inst[text->idx].type == 'R') {

            printf("%s%s%s%s%s", text->t, text->s, text->d, NumToBits(0,5), inst[text->idx].funct);
        }
		else if(inst[text->idx].type == 'I') {
            if(strcmp(inst[text->idx].name,"bne") == 0 || strcmp(inst[text->idx].name,"beq") == 0){
            	printf("%s%s%s", text->d, text->t, text->s);
			}
			else if(strcmp(inst[text->idx].name, "lui")==0){
				printf("%s%s%s", NumToBits(0,5), text->d, text->t);
			}
			else{
				printf("%s%s%s", text->t, text->d, text->s);
			}

        }
		else if(inst[text->idx].type == 'S') {
            printf("%s%s%s%s%s", NumToBits(0,5), text->t, text->d, text->s,inst[text->idx].funct);

        }
		else {
    		if(strcmp(inst[text->idx].name , "jr") == 0)
        		printf("%s%s%s%s%s", text->d, NumToBits(0,5), NumToBits(0,5), NumToBits(0,5), inst[text->idx].funct);
		   	else
		   		printf("%s", text->d);

        }
    }

    for (sym = Symbols; sym != NULL; sym = sym->next) { //sym = Symbols->next dif. my way
        for(data = sym->first; data!= NULL; data= data->next){
			printf("%s", NumToBits(data->value,32));
		}

    }
    printf("\n");
}

/*
 * main function
 */
 int main (int argc, char* argv[]) {

     if (argc != 2) {

         printf("Usage: ./assembler <assembly file>\n");
         exit(0);

     } else {

         // reading the input file
         char *filename = (char *)malloc(strlen(argv[1]) + 3);
         strncpy(filename, argv[1], strlen(argv[1]));

         if (freopen(filename, "r", stdin) == 0) {
             printf("File open Error\n");
             exit(1);
         }

         Symbols = (struct Sym *)malloc(sizeof(struct Sym));
         Texts = (struct Text *)malloc(sizeof(struct Text));

         // creating the output file (*.o)
         filename[strlen(filename)-1] = 'o';
         freopen(filename, "w", stdout);

         // Let's complete the below functions!
         read_asm();
         subst_asm_to_num();
         print_bits();

         // freeing the memory
         free(filename);
         free(Symbols);
         free(Texts);
     }
     return 0;
 }

