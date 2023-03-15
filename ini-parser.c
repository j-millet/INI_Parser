#include <stdio.h>
#include <stdlib.h>

struct section{
    char *name;
    char **keys;
    char **values;
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
    int section_count = 0;
    struct section * *sections;
    FILE* i = fopen("test.ini","r");
    char *line;
    int s;
    while(s=getline(i,&line))
    {   
        if (line[0] == '[')
        {
            line[s-2] = ' '; //remove ] character
            char *sname = (char *)malloc((s-1)*sizeof(char));
            sscanf(line,"[%s",sname);
            struct section newsec;
            newsec.name = sname;

            section_count++;
            sections = (struct section* *)realloc(sections,sizeof(struct section *)*section_count);

            sections[section_count-1] = &newsec;
            printf("NEW SECTION: [%s]\n",sname);
        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char key[s];
            char value[s];
            sscanf(line,"%s = %s",key,value);
            if(!isvalidkey(key)){printf("invalid key: %s",key); exit(1);}
            printf("key:%s, value:%s\n",key,value);
        }
    }
    fclose(i);
    return 0;
}
