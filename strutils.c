#include <ctype.h>
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
int isexpression(const char *str)
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
    return count==1;
}