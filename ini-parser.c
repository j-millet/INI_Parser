#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct section{
    char *name;
    int numkeys;
    char ** keys;
    char ** values;
};

int strisnum(char *string)
{
    char c;
    int i = 0;
    while((c = string[i]))
    {
        if (c<48 || c>57)
        {
            return 0;
        }
        i++;
    }
    return 1;
}

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

/*
Gets line from file.
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

char *getval(struct section *sections,int len, char *section, char *key)
{
    for (size_t i = 0; i < len; i++)
    {
        struct section s = sections[i];
        if(strcmp(s.name,section) == 0)
        {
            for (size_t j = 0; j < s.numkeys; j++)
            {
                if (strcmp(s.keys[j],key)== 0)
                {
                    return s.values[j];
                }
                
            }
            printf("No key named '%s' in section '%s'",key,section);
            exit(1);
        }
    }
    printf("No section named '%s'",section);
    exit(1);
}

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
    *outsection = (char *)malloc(sizeof(char)*i);
    *outkey = (char *)malloc(sizeof(char)*(len-i-1));
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
    struct section *sections = calloc(section_count,sizeof(struct section));
    char *line = (char *)malloc(sizeof(char));
    int s;
    int lineNumber = 0;

    struct section *current_section = NULL;

    while((s=get_line(f,&line)))
    {   
        lineNumber++;
        if (line[0] == '[')
        {
            if (current_section != NULL)//add current section to sections array
            {
                struct section newsec = {current_section->name,current_section->numkeys-1,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
                free(current_section);
                sections = (struct section *)realloc(sections,sizeof(struct section)*(++section_count));
                if(sections == 0x0){perror("failed to reallocate memory");exit(1);}
            }

            line[s-2] = ' '; //remove ] character
            char *sname = (char *)malloc((s-1)*sizeof(char));
            sscanf(line,"[%s",sname);

            current_section = (struct section *)malloc(sizeof(struct section));//create new current section struct
            current_section->name = sname;
            current_section->numkeys = 1;
            current_section->keys = (char **)malloc(sizeof(char *));
            current_section->values = (char **)malloc(sizeof(char *));

        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char *key = (char *)malloc(s*sizeof(char));
            char *value = (char *)malloc(s*sizeof(char));
            sscanf(line,"%s = %s",key,value);
            if(!isvalidkey(key)){printf("INVALID KEY IN FILE: line %d, key '%s'",lineNumber,key); exit(1);}

            current_section->keys[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));
            current_section->values[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));
            strcpy(current_section->keys[current_section->numkeys-1],key);
            strcpy(current_section->values[current_section->numkeys-1],value);

            free(key);
            free(value);

            current_section->numkeys++;
            current_section->keys = (char **)realloc(current_section->keys,current_section->numkeys*sizeof(char *));
            current_section->values = (char **)realloc(current_section->values,current_section->numkeys*sizeof(char *));

        }
        //free(line);
    }
    if (current_section != NULL)//add last section to sections array
            {
                struct section newsec = {current_section->name,current_section->numkeys-1,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
            }
    //free(line);
    *out = sections;
    return section_count;
}

int main(int argc, char const *argv[])
{
    /*
    if(argc != 3){
        printf("wrong amount of arguments (%d, expected 2)",argc-1);
        exit(1);
    }*/
    struct section *sections;
    FILE* i = fopen("test.ini","r");
    int section_count = parseINI(i,&sections);
    fclose(i);
    /*
    printf("{\n");
    for (size_t i = 0; i < section_count; i++)
    {
        struct section s = sections[i];
        printf("%c%s%c:{\n",'"',s.name,'"');
        for (size_t j = 0; j < s.numkeys; j++)
        {
            char *key = s.keys[j];
            char *value = s.values[j];
            if(strisnum(value))
            {
                printf("\t%c%s%c:%d%s\n",'"',key,'"',atoi(s.values[j]),(j==s.numkeys-1)?"":",");
            }
            else{
                printf("\t%c%s%c:%c%s%c%s\n",'"',key,'"','"',value,'"',(j==s.numkeys-1)?"":",");
            }
        }
        printf("}%s\n",(i==section_count-1)?"":",");
        
    }
    printf("}");*/
    char* key;
    char* section;
    if(parse_access("test.strong",&section,&key))
    {
        printf("%s",getval(sections,section_count,section,key));
    }
    else{perror("wat doink men");exit(1);}

    for (size_t i = 0; i < section_count; i++)
    {
        struct section s = sections[i];
        for (size_t j = 0; j < s.numkeys; j++)
        {
            free(s.values[j]);
            free(s.keys[j]);
        }
        
        free(s.keys);
        free(s.values);
        free(s.name);
    }
    
    free(sections);
    return 0;
}
