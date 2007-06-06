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
#include "btmain.h"
#include <qiodevice.h>
#include <qdir.h>

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

void getAmbName(char *name, char *out, char *amb, char len)
{
    memset(out,'\000',len);
    if (strstr(name,"."))
    {
	strncpy(out,name,strstr(name,".")-name);
	strncat(out, amb, 1);
	strcat(out,strstr(name,"."));
    }
}

/*
bool setCfgValue(char* file, int id, const char* campo, const char* valore,int serNumId)
{
    char appoggio[100];
    QString Line;
    int count;
    
    comChConf();
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
*/
bool setCfgValue(char* file, int id, const char* campo, const char* valore,int serNumId)
{
    char app1[100];
    char app2[100];    
    QString Line;
    int count;
    
    comChConf();
    count=1;   
    if (QFile::exists("cfg/appoggio.xml"))
	QFile::remove("cfg/appoggio.xml");	  
	  
    QFile *fil1=new QFile(file);
    QFile *fil2=new QFile("cfg/appoggio.xml");    
    if ( !fil1->open( IO_WriteOnly | IO_ReadOnly) )
	return(FALSE);
    if ( !fil2->open( IO_WriteOnly) )
    {	
	fil1->close();	         
	return(FALSE);
    }
    QTextStream t1( fil1);
    QTextStream t2( fil2);
    sprintf(&app1[0], "<id>%d</id>",id);
    do{     
	Line= t1.readLine().append('\n');
	t2.writeRawBytes(Line.ascii(), Line.length());
	if  (Line.contains(&app1[0],TRUE)) 
	{
	    if  (count==serNumId)
	    {
		sprintf(&app2[0], "<%s>",campo);
	//	QIODevice::Offset ofs;
		do{
		    Line= t1.readLine().append('\n');
		    if (!Line.contains(&app2[0],TRUE))
			    t2.writeRawBytes(Line.ascii(), Line.length());
		    else
			break;
		}while  (Line.compare("\n") );
		if (!Line.isNull())
		{
		     Line.sprintf("<%s>%s</%s>\n",campo,valore,campo);
		     t2.writeRawBytes(Line.ascii(), Line.length());
		     do{
			 Line= t1.readLine().append('\n');
			 t2.writeRawBytes(Line.ascii(), Line.length());
		     }while( Line.compare("\n"));
		     fil2->flush();
		     fil1->close();
		     fil2->close();		     
		     QDir::current().rename("cfg/appoggio.xml",file,FALSE);
		     return(TRUE);
		 }		
	    }
	    else
		count++;
	}
    }while  (Line.compare("\n"));
    fil1->close();
    fil2->close();	
    return(FALSE);
}   





bool setCfgValue(int id, const char* campo, const char* valore)
{
    return  setCfgValue(id, campo, valore,1);
}

bool setCfgValue(int id, const char* campo, const char* valore,int serNumId)
{
    return  setCfgValue(CONFILENAME, id, campo, valore, serNumId);    
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
     return(TRUE);
}

void setContrast(unsigned char c,bool b)
{
    char contr[4];
    
    if ( QFile::exists("/proc/sys/dev/btweb/contrast") )
    {
	int fd = open("/proc/sys/dev/btweb/contrast", O_WRONLY);
	if (fd >= 0 )
	{
	    sprintf(&contr[0],"%d",c);
	    qDebug("setto il contrasto a : %d", c);
	    write(fd, &contr[0], 4);
	    close(fd);
	}
    }
    if (b)
    {
	sprintf(&contr[0],"%03d",c);
	setCfgValue(CONTRASTO, "value",&contr[0] ); 
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
  char name[50];

  getName(name);
  if(!strncmp(name, "H4684_IP", strlen("H4684_IP")))
  {
    if ( QFile::exists("/proc/sys/dev/btweb/brightness") )
    {
	int fd = open("/proc/sys/dev/btweb/brightness", O_WRONLY);
	if (fd >= 0 )
	{
	    if (b)
		write(fd, "10", 2);
	    else
		write(fd, "210", 3);
	    close(fd);
	}
    }
    if ( QFile::exists("/proc/sys/dev/btweb/backlight") )
    {
        int fd = open("/proc/sys/dev/btweb/backlight", O_WRONLY);
        if (fd >= 0 )
        {
            if (b)
                write(fd, "1", 1);
            close(fd);
        }
    }
  }
  else
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
}

void setBeep(bool b,bool ab)
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
    if (ab)
    {
	if (b)
	    setCfgValue(SUONO, "value","1"); 
	else
	    setCfgValue(SUONO, "value","0"); 
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
  char c[4];
  char name[50];
  int fd;

  getName(name);
  if(!strncmp(name, "H4684_IP", strlen("H4684_IP")))
  {
    if ( QFile::exists("/proc/sys/dev/btweb/brightness") )
    {
        fd = open("/proc/sys/dev/btweb/brightness", O_RDONLY);
        if (fd >= 0 )
        {
            read(fd, &c[0], 4);
            close(fd);
            if (atoi(&c[0]) < 100)
              return(TRUE);
        }
    }
  }
  else
  {
    if ( QFile::exists("/proc/sys/dev/btweb/backlight") )
    {
      fd = open("/proc/sys/dev/btweb/backlight", O_RDONLY);
      if (fd >= 0 )
      {	 	    
        read (fd, &c[0], 1);	 
        close(fd);
        if (c[0]!='0')
          return(TRUE);
      }
    }
  }
  return(FALSE);
}


void setOrientation(char* o)
{
    if ( QFile::exists("/proc/sys/dev/btweb/upsidedown") )
    {
	int fd = open("/proc/sys/dev/btweb/upsidedown", O_WRONLY);
	if (fd >= 0 )
	{
	    write(fd, &o[0], 1);
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
#ifndef   HAS_HP      
//	    memset(te,'\000',sizeof(te)); 
        	    sprintf(&te[0],"4000 %d",t);
//	    write(fd, "4000 ",6);
	    write(fd, &te[0],strlen(&te[0]));	    
#endif        
#ifdef  HAS_HP
        sprintf(&te[0],"%d %d",2400,t/3);
        write(fd, &te[0],strlen(&te[0]));	           
        sleep(t/3000);
        sprintf(&te[0],"%d %d",1600,t/3);
        write(fd, &te[0],strlen(&te[0]));	          
        sleep(t/3000);
        sprintf(&te[0],"%d %d",800,t/3);
        write(fd, &te[0],strlen(&te[0]));	 
#endif        
        
        
/*        sleep(t/3000);
        sprintf(&te[0],"%d %d",800,t/3);
        write(fd, &te[0],strlen(&te[0]));	          
        sleep(t/3000);*/
        
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
   if (! QFile::exists(FILE_WDT) )
    {
//	int fd = open("/var/tmp/bticino/bt_wd/BTouch_qws", O_CREAT, 0666);
	int fd = creat(FILE_WDT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd != -1 )
	{
	    close(fd);
	}
    }       
}


void  comChConf()
{
    int fd = open(FILE_CHANGE_CONF, O_CREAT, 0666);
    if (fd >= 0 )
    {
	close(fd);
    }
    
}

void getName(char *name)
{
  memset(name, '\0', sizeof(name));
  if (QFile::exists("/proc/sys/dev/btweb/name"))
  {
    int fd = open("/proc/sys/dev/btweb/name", O_RDONLY);
    if (fd >= 0 )
    {
      read(fd, &name[0], 50);
      close(fd);
      return;
    }
  }
  return;
}

extern BtMain *BTouch;

void ResetTimer(int signo)
{
    qDebug("ResetTimer()");
    BTouch->ResetTimer();
}
#if 0
bool isForMe(openwebnet& m, char *chi = "1")
{
    if(strcmp(m.Extract_chi(), chi)) return false ;
    if(!strcmp(m.Extract_dove(),getAddress())) return true;
    // BAH
    return (!getPul() && ((!strcmp(m.Extract_dove(),"0")) ||
			  ((strlen(m.Extract_dove())==1) && 
			   (!strncmp(m.Extract_dove(), getAddress(), 1)) ) || 
			  ((!strncmp(m.Extract_dove(),"#",1)) && 
			   *(getGroup()+(atoi(m.Extract_dove()+1))-1))));
}
#endif
