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
    while(c = string[i])
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
    while(c = string[i])
    {
        if (!(c == 45 || (c > 47 && c < 58) ||(c > 64 && c < 91) ||(c > 96 && c < 123)))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

int getline(FILE* f, char **out)
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
    *out = (char *)malloc(sizeof(char)*size);
    fseek(f,startpos,SEEK_SET);
    fgets(*out,size,f);
    return size-1;
}

int parseINI(FILE *f, struct section **out)
{
    int section_count = 1;
    struct section *sections = calloc(section_count,sizeof(struct section));
    char *line;
    int s;

    struct section *current_section = NULL;
    struct entry *entries;

    while(s=getline(f,&line))
    {   
        if (line[0] == '[')
        {
            if (current_section != NULL)
            {
                struct section newsec = {current_section->name,current_section->numkeys,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
                sections = (struct section *)realloc(sections,sizeof(struct section)*(++section_count));
                if(sections == 0x0){perror("failed to reallocate memory");exit(1);}
            }
            line[s-2] = ' '; //remove ] character
            char *sname = (char *)malloc((s-1)*sizeof(char));
            sscanf(line,"[%s",sname);

            current_section = (struct section *)malloc(sizeof(struct section));//help
            current_section->name = sname;
            current_section->numkeys = 1;
            current_section->keys = (char **)malloc(sizeof(char *));
            current_section->values = (char **)malloc(sizeof(char *));
        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char key[s];
            char value[s];
            sscanf(line,"%s = %s",key,value);
            if(!isvalidkey(key)){printf("invalid key: %s",key); exit(1);}
            current_section->keys[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));
            current_section->values[current_section->numkeys-1] = (char *)malloc(s*sizeof(char));
            strcpy(current_section->keys[current_section->numkeys-1],key);
            strcpy(current_section->values[current_section->numkeys-1],value);

            current_section->numkeys++;
            current_section->keys = (char **)realloc(current_section->keys,current_section->numkeys*sizeof(char *));
            current_section->values = (char **)realloc(current_section->values,current_section->numkeys*sizeof(char *));

        }
    }
    if (current_section != NULL)
            {
                struct section newsec = {current_section->name,current_section->numkeys,current_section->keys,current_section->values};//help
                sections[section_count-1] = newsec;
            }
    
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
    FILE* i = fopen("test2.ini","r");
    int section_count = parseINI(i,&sections);
    fclose(i);

    printf("{\n");
    for (size_t i = 0; i < section_count; i++)
    {
        struct section s = sections[i];
        printf("%c%s%c:{\n",'"',s.name,'"');
        for (size_t j = 0; j < s.numkeys-1; j++)
        {
            char *key = s.keys[j];
            char *value = s.values[j];
            if(strisnum(value))
            {
                printf("\t%c%s%c:%d%s\n",'"',key,'"',atoi(s.values[j]),(j==s.numkeys-2)?"":",");
            }
            else{
                printf("\t%c%s%c:%c%s%c%s\n",'"',key,'"','"',value,'"',(j==s.numkeys-2)?"":",");
            }
            
        }
        printf("}%s\n",(i==section_count-1)?"":",");
        
    }
    printf("}");
    return 0;
}
