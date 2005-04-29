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
void setContrast(unsigned char,bool);
unsigned char getContrast();
void setBacklight(bool);
bool getBacklight();
void setBeep(bool,bool);
void setOrientation(unsigned char);
void beep();
void beep(int);
bool getBeep();
unsigned long getTimePress();
void  rearmWDT();
bool setCfgValue(int , const char* , const char* );
bool setCfgValue(int , const char*, const char* ,int);
bool setCfgValue(char*, int , const char*, const char* ,int);
bool copyFile(char*, char*);

