#include <stdio.h>
#include <stdlib.h>

struct section{
    char *name;
    char *keys_str;
    char *keys_int;
    char *values_str;
    char *values_int; 
};



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
    FILE* i = fopen("test.ini","r");
    char *line;
    int s;
    while(s=getline(i,&line))
    {   
        if (line[0] == '[')
        {
            char sname[s-2];
            for (size_t i = 1; i < s-2; i++)
            {
                sname[i-1] = line[i];
            }
            
            printf("NEW SECTION: [%s]\n",sname);
        }
        else if (line[0] != '\n' && line[0] != ';')
        {
            char key[s];
            char value[s];
            sscanf(line,"%s = %s",key,value);
            printf("key:%s, value:%s\n",key,value);
        }
    }
    fclose(i);
    return 0;
}
