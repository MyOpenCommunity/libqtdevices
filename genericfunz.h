/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**genericfunz.h
**
**funzioni generiche usate qua e là
**
****************************************************************/



void getPressName(char*, char*,char);
void getZoneName(char* , char* ,char*, char );
void setContrast(unsigned char);
unsigned char getContrast();
void setBacklight(bool);
void setBeep(bool);
void setOrientation(unsigned char);
void beep();
bool getBeep();
unsigned long getTimePress();
void  rearmWDT();
