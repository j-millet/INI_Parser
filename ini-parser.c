#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct section{
    char *name;
    int numkeys;
    char ** keys;
    char ** values;
};

//checks if string is a number
int isnum(char *string)
{
    char c;
    int i = 0;
    while((c = string[i]))
    {
        if (!isdigit(c))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//checks validity of a key (only contains [a-z],[A-Z] and '-')
int isvalidkey(char *string)
{
    char c;
    int i = 0;
    while((c = string[i]))
    {
        if (!(c == 45 || (c > 47 && c < 58) ||(c > 64 && c < 91) ||(c > 96 && c < 123)))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//checks if string is an expression (string [+-*/] string)
int isexpression(char *str)
{
    int count = 0;
    char c;
    for(int i = 1; (c=str[i]); i++)
    {
        if ((c == '+' || c == '-' || c == '*' || c == '/') && str[i-1] == ' ' && str[i+1] == ' ')
        {
            count ++;
        }
        
    }
    return count;
}

/*
Gets line from file, stores in out.
Returns length of line read.
*/
int get_line(FILE* f, char **out)
{
    int startpos = ftell(f);
    size_t size = 1;
    char a;
    do{
        a = fgetc(f);
        if(feof(f)){break;}
        size++;
    }
    while(a!= '\n');
    *out = (char *)realloc(*out,sizeof(char)*size);
    fseek(f,startpos,SEEK_SET);
    fgets(*out,size,f);
    return size-1;
}

//gets value specified by section and key from sections array
char *getval(struct section *sections,int len, char *section, char *key, char **out)
{
    for (int i = 0; i < len; i++)
    {
        struct section s = sections[i];
        if(strcmp(s.name,section) == 0)
        {
            for (int j = 0; j < s.numkeys; j++)
            {
                if (strcmp(s.keys[j],key)== 0)
                {
                    *out = (char*)realloc(*out,sizeof(char)*strlen(s.values[j]));
                    strcpy(*out,s.values[j]); 
                    return 0;
                }
                
            }
            printf("No key named '%s' in section '%s'\n",key,section);
            exit(EXIT_FAILURE);
        }
    }
    printf("No section named '%s'\n",section);
    exit(EXIT_FAILURE);
}

//parses string formatted as 'section.key' into separate section and key strings, stores output in outsection and outkey
int parse_access(char *inpt, char** outsection, char** outkey)
{
    char c;
    int i = 0;
    int len = strlen(inpt);

    char* inptcp = (char *)malloc(sizeof(char)*len);
    strcpy(inptcp,inpt);

    while((c = inptcp[i]))
    {
        if(c == '.'){
            break;
        }
        i++;
    }

    inptcp[i] = ' ';
    if(!c)
    {
        free(inptcp);
        return 0;
    }
    *outsection = (char *)realloc(*outsection,sizeof(char)*(i+1));
    size_t rest = len-i;
    *outkey = (char *)realloc(*outkey,sizeof(char)*rest);
    sscanf(inptcp,"%s %s",*outsection,*outkey);
    free(inptcp);
    return 1;
}

/*
Parses ini file and stores results in 'out'
Returns length of 'out' array;
*/
int parseINI(FILE *f, struct section **out)
{
    int section_count = 1;
    struct section *sections = malloc(section_count*sizeof(struct section));//initialize array of sections

    char *line = (char *)malloc(sizeof(char));
    int s;//length of line read
    int lineNumber = 0;

    struct section *current_section = NULL;

    while((s=get_line(f,&line)))
    {   
        lineNumber++;
        if (line[0] == '[')
        {
            if (current_section != NULL)//add current section to sections array
            {
                struct section newsec = {current_section->name,current_section->numkeys,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
                free(current_section);
                sections = (struct section *)realloc(sections,sizeof(struct section)*(++section_count));
                if(sections == 0x0){perror("failed to reallocate memory");exit(EXIT_FAILURE);}
            }

            line[s-2] = ' '; //remove ] character
            char *sname = (char *)malloc((s-1)*sizeof(char));
            sscanf(line,"[%s",sname);

            current_section = (struct section *)malloc(sizeof(struct section));//create new current section struct
            current_section->name = sname;
            current_section->numkeys = 0;
            current_section->keys = (char **)malloc(sizeof(char *));
            current_section->values = (char **)malloc(sizeof(char *));

        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char *key = (char *)malloc(s*sizeof(char));
            char *value = (char *)malloc(s*sizeof(char));

            sscanf(line,"%s = %s",key,value);
            if(!isvalidkey(key)){printf("INVALID KEY IN FILE: line %d, key '%s'\n",lineNumber,key); exit(EXIT_FAILURE);}

            current_section->numkeys++;
            current_section->keys = (char **)realloc(current_section->keys,current_section->numkeys*sizeof(char *));
            current_section->values = (char **)realloc(current_section->values,current_section->numkeys*sizeof(char *));

            current_section->keys[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));
            current_section->values[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));

            strcpy(current_section->keys[current_section->numkeys-1],key);
            strcpy(current_section->values[current_section->numkeys-1],value);

            free(key);
            free(value);

        }
    }
    if (current_section != NULL)//add last section to sections array
            {
                struct section newsec = {current_section->name,current_section->numkeys,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
                free(current_section);
            }
    free(line);
    *out = sections;
    return section_count;
}

int main(int argc, char const *argv[])
{
    if(argc != 3){
        printf("wrong amount of arguments (%d, expected 2)\n",argc-1);
        exit(EXIT_FAILURE);
    }

    struct section *sections;
    FILE* i = fopen(argv[1],"r");
    int section_count = parseINI(i,&sections);
    fclose(i);
    printf("file parsed\n");
    if(isexpression(argv[2]))
    {
        char *acc1 = (char *)malloc(strlen(argv[2])*sizeof(char));
        char *acc2 = (char *)malloc(strlen(argv[2])*sizeof(char));
        char action;
        sscanf(argv[2],"%s %c %s",acc1,&action,acc2);
        char *sec1 = (char *)malloc(sizeof(char)*strlen(argv[2]));
        char *sec2 = (char *)malloc(sizeof(char)*strlen(argv[2]));
        char *key1 = (char *)malloc(sizeof(char)*strlen(argv[2]));
        char *key2 = (char *)malloc(sizeof(char)*strlen(argv[2]));

        
        

        if(!(parse_access(acc1,&sec1,&key1) && parse_access(acc2,&sec2,&key2)))
        {
            perror("Wrong expression\n");
            exit(EXIT_FAILURE);
        }

        free(acc1);
        free(acc2);

        char *val1 = (char *)malloc(1);
        getval(sections,section_count,sec1,key1,&val1);
        char *val2 = (char*)malloc(1);
        getval(sections,section_count,sec2,key2,&val2);

        free(sec1);
        free(sec2);
        free(key1);
        free(key2);

        if(!isnum(val1) && !isnum(val2))
        {
            if (action == '+')
            {            
                printf("%s%s\n",val1,val2);
            }
            else{
                printf("Unsupported operand '%c' on strings.\n",action);
                exit(EXIT_FAILURE);
            }
        }
        else if(isnum(val1)&&isnum(val2))
        {
            switch (action)
            {
            case '+':
                printf("%d\n",atoi(val1) + atoi(val2));
                break;
            case '-':
                printf("%d\n",atoi(val1) - atoi(val2));
                break;
            case '*':
                printf("%d\n",atoi(val1) * atoi(val2));
                break;
            case '/':
                printf("%d\n",atoi(val1) / atoi(val2));
                break;
            default:
                printf("Unsupported operand '%c'.",action);
                exit(EXIT_FAILURE);
                break;
            }
        }
        else{
            printf("Cannot perform operations on mixed data types.\n");
            exit(EXIT_FAILURE);
        }
        free(val1);
        free(val2);
    }
    else{
        char* key = (char *)malloc(1);
        char* section = (char *)malloc(1);
        if(parse_access(argv[2],&section,&key))
        {
            char *val = (char *)malloc(1);
            getval(sections,section_count,section,key,&val);
            printf("%s\n",val);
            free(val);
        }
        else{perror("Wrong expression\n");exit(EXIT_FAILURE);}
        free(key);
        free(section);
    }

    printf("Freeing memory...\n");
    for (int i = 0; i < section_count; i++)
    {
        printf("Freeing keys&values...\n");
        struct section s = sections[i];
        for (int j = 0; j < s.numkeys; j++)
        {
            free(s.values[j]);
            free(s.keys[j]);
        }
        printf("Freeing rest...\n");
        free(s.keys);
        free(s.values);
        free(s.name);
    }
    printf("Freeing sections array...\n");
    free(sections);
    return 0;
}