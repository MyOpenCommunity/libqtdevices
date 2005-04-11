/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**genericFunz.cpp
**
**funzioni generiche usate qua e là
**
****************************************************************/
#include <stdlib.h>
#include <string.h>


void getPressName(char* name, char* pressName,char len)
{
    memset(pressName,'\000',len);
    strncpy(pressName,name,strstr(name,".")-name);
    strcat(pressName,"P");
    strcat(pressName,strstr(name,"."));
}
