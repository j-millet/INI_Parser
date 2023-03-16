#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct entry{
    char *key;
    char *value;
};
struct section{
    char *name;
    int numkeys;
    struct entry **entries;
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

int main(int argc, char const *argv[])
{
    /*
    if(argc != 3){
        printf("wrong amount of arguments (%d, expected 2)",argc-1);
        exit(1);
    }*/
    int section_count = 1;
    struct section** sections = calloc(section_count,8);
    FILE* i = fopen("test.ini","r");
    char *line;
    int s;

    struct section *current_section;
    struct entry **entries;

    while(s=getline(i,&line))
    {   
        if (line[0] == '[')
        {
            line[s-2] = ' '; //remove ] character
            char *sname = (char *)malloc((s-1)*sizeof(char));
            sscanf(line,"[%s",sname);

            struct section* newsec = malloc(sizeof(struct section));//help
            newsec->name = sname;
            newsec->numkeys = 1;
            newsec->entries = (struct entry **)calloc(1,8);
            sections[section_count-1] = newsec;
            current_section = newsec;
            entries = newsec->entries;
            //printf("%s\n",sections[section_count-1]->name);
            sections = (struct section**)realloc(sections,8*(++section_count));
            if(sections == 0x0){perror("failed to reallocate memory");exit(1);}
            //printf("NEW SECTION: [%s]\n",sname);
        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char key[s];
            char value[s];
            sscanf(line,"%s = %s",key,value);
            if(!isvalidkey(key)){printf("invalid key: %s",key); exit(1);}
            struct entry* newent = malloc(sizeof(struct entry));
            newent->key = (char *)malloc(sizeof(char)*s);
            newent->value = (char *)malloc(sizeof(char)*s);
            strcpy(newent->key,key);
            strcpy(newent->value,value);

            entries[current_section->numkeys-1] = newent;
            entries = (struct entry **)realloc(entries,++current_section->numkeys*8);
            //entries = (struct entry **)realloc(entries,++current_section->numkeys*8);

            //printf("key:%s, value:%s\n",key,value);
        }
    }
    fclose(i);
    for (size_t i = 0; i < section_count-1; i++)
    {
        
        current_section = sections[i];
        printf("%s\n",current_section->name);
        for (size_t j = 0; j < current_section->numkeys; j++)
        {
            struct entry* ent = current_section->entries[j];
            printf("%s %s\n",ent->key,ent->value);
        }
        
    }
    
    return 0;
}
