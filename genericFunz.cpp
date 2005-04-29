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
#include "main.h"
#include <qiodevice.h>


#define CONFILENAME	"cfg/conf.xml"



void getPressName(char* name, char* pressName,char len)
{
    memset(pressName,'\000',len);
    if (strstr(name,"."))
    {
	strncpy(pressName,name,strstr(name,".")-name);
	strcat(pressName,"p");
	strcat(pressName,strstr(name,"."));
    }
}

void getZoneName(char* name, char* pressName,char*zona, char len)
{
    memset(pressName,'\000',len);
    if (strstr(name,"."))
    {
	strncpy(pressName,name,strstr(name,".")-name);
	strncat(pressName,&zona[1],1);
	strcat(pressName,strstr(name,"."));
    }
}


bool setCfgValue(char* file, int id, const char* campo, const char* valore,int serNumId)
{
    char appoggio[100];
    QString Line;
    int count;
    
    count=1;    
    QFile *fil=new QFile(file);
    if ( !fil->open( IO_WriteOnly | IO_ReadOnly) )
	return(FALSE);
    QTextStream t( fil);
    do{     
	Line= t.readLine();
	sprintf(&appoggio[0], "<id>%d</id>",id);
	
	if  (Line.contains(&appoggio[0],TRUE)) 
	{
	    if  (count==serNumId)
	    {
		sprintf(&appoggio[0], "<%s>",campo);
		QIODevice::Offset ofs;
		do{
		    ofs=t.device()->at();
		    Line= t.readLine();
		}while( (!Line.isNull()) && !Line.contains(&appoggio[0],TRUE));
		if (!Line.isNull())
		{
		     t.device()->at(ofs);
		     Line.sprintf("<%s>%s</%s>",campo,valore,campo);
		     t.writeRawBytes(Line.ascii(), Line.length());
		     fil->flush();
//		     Line= t.readLine();
		     fil->close();
		     return(TRUE);
		 }		
	    }
	    else
		count++;
	}
    }while  (!Line.isNull());
    fil->close();    
    return(FALSE);
}   

bool setCfgValue(int id, const char* campo, const char* valore)
{
    setCfgValue(id, campo, valore,1);
}

bool setCfgValue(int id, const char* campo, const char* valore,int serNumId)
{
    setCfgValue(CONFILENAME, id, campo, valore, serNumId);
}

bool copyFile(char* orig, char* dest)
{
    QFile *filIN=new QFile(orig);
    if ( !filIN->open( IO_ReadOnly) )
	return(FALSE);
    if (QFile::exists(dest))
	QFile::remove(dest);
    QFile *filOUT=new QFile(dest);
        
    if ( !filOUT->open( IO_WriteOnly ) )
	return(FALSE);
    QDataStream tIN( filIN);
    QDataStream tOUT( filOUT);
    unsigned char i;
    do{
	 tIN>>i;
	 tOUT<<i;
    }while(!tIN.atEnd());
     filIN->close();    
     filOUT->close();             
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
    sprintf(&contr[0],"%03d",c);
    setCfgValue(CONTRASTO, "value",&contr[0] ); 
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
    if (b)
	setCfgValue(SUONO, "value","1"); 
    else
	setCfgValue(SUONO, "value","0"); 
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




