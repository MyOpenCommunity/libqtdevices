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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <qapplication.h>
#include <qfile.h>
#include "genericfunz.h"
#include "openclient.h"


void getPressName(char* name, char* pressName,char len)
{
    memset(pressName,'\000',len);
    strncpy(pressName,name,strstr(name,".")-name);
    strcat(pressName,"P");
    strcat(pressName,strstr(name,"."));
}

void getZoneName(char* name, char* pressName,char*zona, char len)
{
    memset(pressName,'\000',len);
    strncpy(pressName,name,strstr(name,".")-name);
    strncat(pressName,&zona[1],1);
    strcat(pressName,strstr(name,"."));
}

void setContrast(unsigned char c)
{
    char contr[4];
    if ( QFile::exists("/proc/sys/dev/btweb/contrast") )
    {
	int fd = open("/proc/sys/dev/btweb/contrast", O_WRONLY);
	if (fd >= 0 )
	{
	    sprintf(&contr[0],"%d",c);
//	    qDebug("setto il contrasto a : %d", c);
	    write(fd, &contr[0], 4);
	    close(fd);
	}
    }
}
unsigned char getContrast()
{
    char contr[4];
    if ( QFile::exists("/proc/sys/dev/btweb/contrast") )
    {
	int fd = open("/proc/sys/dev/btweb/contrast",O_RDONLY);
	if (fd >= 0 )
	{	
	    read(fd, &contr[0], 4);
	    close(fd);
	    return(atoi(&contr[0]));
	}
      }
    return(0x00);
}


void setBacklight(bool b)
{
    if ( QFile::exists("/proc/sys/dev/btweb/backlight") )
    {
	int fd = open("/proc/sys/dev/btweb/backlight", O_WRONLY);
	if (fd >= 0 )
	{
	    if (b)
		write(fd, "1", 1);
	    else
		write(fd, "0", 1);
	    close(fd);
	}
    }
}

void setBeep(bool b)
{
    if ( QFile::exists("/proc/sys/dev/btweb/buzzer_enable") )
    { 
	// qDebug("buzzer exists");
	FILE* fd = fopen("/proc/sys/dev/btweb/buzzer_enable", "w");
	if (fd >= 0 )
	{
	    if (b)
		fwrite( "1", 1,1,fd);
	    
	    else
		fwrite( "0", 1,1,fd);
	    fclose(fd);
	}
    }
}

bool getBeep()
{
    unsigned char c;
    if ( QFile::exists("/proc/sys/dev/btweb/buzzer_enable") )
    {
	FILE*fd = fopen("/proc/sys/dev/btweb/buzzer_enable",  "r");
	if (fd >= 0 )
	{	 	    
	    fread (&c ,1, 1,fd);	 
	    fclose(fd);
//	    qDebug("ho letto buzzer= %d",c);
	    if (c!='0')
	    {
	//	qDebug("getBeep TRUE");
		return(TRUE);
	    }
	    else
	    {
//		qDebug("getBeep FALSE");
		return(FALSE);		
	    }
	}
    }
    return(FALSE);    
}

bool getBacklight()
{
    unsigned char c;
    if ( QFile::exists("/proc/sys/dev/btweb/backlight") )
    {
	FILE*fd = fopen("/proc/sys/dev/btweb/backlight",  "r");
	if (fd >= 0 )
	{	 	    
	    fread (&c ,1, 1,fd);	 
	    fclose(fd);
	    if (c!='0')
	    {
		return(TRUE);
	    }
	    else
	    {
		return(FALSE);		
	    }
	}
    }
    return(FALSE);    
}


void setOrientation(unsigned char o)
{
    if ( QFile::exists("/proc/sys/dev/btweb/upsidedown") )
    {
	int fd = open("/proc/sys/dev/btweb/upsidedown", O_WRONLY);
	if (fd >= 0 )
	{
	    write(fd, &o, 1);
	    close(fd);
	}
    }
}

void beep(int t)
{
    if ( QFile::exists("/proc/sys/dev/btweb/buzzer") )
    {
	int fd = open("/proc/sys/dev/btweb/buzzer", O_WRONLY);
	if (fd >= 0 )
	{
	    char te[10];
//	    memset(te,'\000',sizeof(te));
	    sprintf(&te[0],"4000 %d",t);
//	    write(fd, "4000 ",6);
	    write(fd, &te[0],strlen(&te[0]));	    
	    close(fd);
	}
    }    
}

void beep()
{
    beep(50);
}


unsigned long getTimePress()
{
    unsigned long t=0;
    char time[50];
    
    memset(time,'\000',sizeof(time));
    if ( QFile::exists("/proc/sys/dev/btweb/touch_ago") )
    {
	int fd = open("/proc/sys/dev/btweb/touch_ago", O_RDONLY);
	
	read(fd, &time[0], 6);
	close(fd);
	t=atol(time);	
	//qDebug("touch_ago: %s  ",&time[0]);
    }
    return(t);    
}

void  rearmWDT()
{
   if (! QFile::exists("/var/tmp/bticino/bt_wd/BTouch_qws") )
    {
	int fd = open("/var/tmp/bticino/bt_wd/BTouch_qws", O_CREAT);
	if (fd >= 0 )
	{
	    close(fd);
	}
    }       
}




