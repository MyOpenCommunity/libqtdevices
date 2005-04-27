/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**timeScript.cpp
**
**scritta con data/ora
**
****************************************************************/

#include "timescript.h"
#include <qdatetime.h>


timeScript::timeScript( QWidget *parent, const char *name, uchar tipo,QDateTime* mioOrol)
    : QLCDNumber( parent, name )
{
    type=tipo;
    if (tipo!=2)
	setNumDigits(8);
    else
	setNumDigits(5);
    if(type!=2)
	normalTimer = startTimer( 1000 );			
    showDateTimer = -1;			
    setSegmentStyle(Flat);    
    mioClock=NULL;

    if  (mioOrol)
    {
	mioClock = new QDateTime(*mioOrol);
	/*mioClock->setDate(mioOrol->date());
	mioClock->setTime(mioOrol->time());*/
	//qDebug("MIOCLOCK %s", mioClock->toString("h:mm:ss"));
    }   
    if (type!=25)	
	showTime();	
    else
	showDate();
}


void timeScript::timerEvent( QTimerEvent *e )
{
    if (mioClock)
	*mioClock=mioClock->addSecs(1);
    if   ( e->timerId() == showDateTimer )
	if (!type) 
	    stopDate();
                else
  	     showDate();	        	 
    else {					
	if  ( showDateTimer == -1 ) 
	{	   
	    showTime();
	}
    }
}


void timeScript::mousePressEvent( QMouseEvent * )
{
    if(!mioClock)
	showDate();
}


void timeScript::showDate()
{
    QDate date;
    
    if  ( ( showDateTimer != -1 ) && (!type) )
	return;
    if (mioClock)
	 date = mioClock->date();
   else		    
                 date = QDate::currentDate(Qt::LocalTime);
   
   
    QString s = date.toString("dd:MM:yy"/*,Qt::TextDateQt::LocalDate*/);
//    QLocale s = date.toString(/*"dd:MM:yy",*/Qt::TextDate/*Qt::LocalDate*/);    
    setNumDigits(((QString)s).length());
    display( s );		
    if  ( showDateTimer == -1 ) 
    {
	if (!type)
	    showDateTimer = startTimer( 4000 );
	else
	    showDateTimer = startTimer( 1000 );
    }
}


void timeScript::stopDate()
{
    killTimer( showDateTimer );
    showDateTimer = -1;
   
    if(type!=25)
    {
	showTime();
	if (type!=2)
	    setNumDigits(8);
	else
	    setNumDigits(5);
    }
    else
	showDate();	
}


void timeScript::showTime()
{
    QString s;
    if (mioClock)
    {
	if (type==2)
	    s = mioClock->toString("h:mm");
	else
	    s = mioClock->toString("h:mm:ss");	
    }
    else
    {
	if (type==2)
	    s = QTime::currentTime(Qt::LocalTime).toString("h:mm");
	else
	    s = QTime::currentTime(Qt::LocalTime).toString("h:mm:ss");
    }
     display( s );	
}



void 	timeScript::diminSec(){*mioClock=mioClock->addSecs(-1);showTime(); }	
void 	timeScript::diminMin(){*mioClock=mioClock->addSecs(-60);showTime();  }
void 	timeScript::diminOra(){*mioClock=mioClock->addSecs(-3600);showTime();}
void 	timeScript::diminDay(){*mioClock=mioClock->addDays(-1);showDate(); }
void 	timeScript::diminMonth(){*mioClock=mioClock->addMonths(-1);showDate();}
void 	timeScript::diminYear(){*mioClock=mioClock->addYears(-1);showDate();}
    
void 	timeScript::aumSec(){*mioClock=mioClock->addSecs(1);showTime();}	
void 	timeScript::aumMin(){*mioClock=mioClock->addSecs(60);showTime();}
void 	timeScript::aumOra(){*mioClock=mioClock->addSecs(3600);showTime(); }


void 	timeScript::aumDay(){*mioClock=mioClock->addDays(1);showDate(); }
void 	timeScript::aumMonth(){*mioClock=mioClock->addMonths(1);showDate();   }
void 	timeScript::aumYear(){*mioClock=mioClock->addYears(1);showDate();  }
    

QDateTime timeScript::getDataOra()
{
    if  (type) 
	return (* mioClock);
    return (QDateTime::currentDateTime(Qt::LocalTime));
    
}




/* PER SCRIVERE VERAMENTE SUL CLOCK
    
    QDateTime data;    
    QStringList comando=QStringList ("date");
    QProcess* dateChange;

    data=    QDateTime::currentDateTime();
    data.setTime(QTime(data.time().hour()+1,data.time().minute(),data.time().second()));
    QTextOStream (stdout)<<"\n"<<data.time().toString()<<"\n";  
    comando.append("-s "+data.time().toString());
    dateChange = new QProcess(comando,this);
    dateChange->start();
    while (dateChange->isRunning()) {}
    QTextOStream (stdout)<<"\nfinito Running";
    */
