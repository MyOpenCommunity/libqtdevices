/****************************************************************************
** $Id: timescript.h,v 1.5 2006/07/28 05:53:52 cvs Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TIMESCRIPT_H
#define TIMESCRIPT_H

#include <qlcdnumber.h>
#include <qdatetime.h>

/*!
  \class timeScript
  \brief This class shows a time and eventually a date with the possibility to modify them using a set of appropriate slots.

  The argument are the pointer to the parent Widget, the pointer to the name, the type (0=shows the local time with hh:mm:ss, 1=, 2=shows a stopped time with hh:mm, 25= shows the date), 
  \author Davide
  \date lug 2005
*/  
class timeScript : public QLCDNumber		
{
    Q_OBJECT
public:
   // timeScript( QWidget *parent=0, const char *name=0 );
    timeScript( QWidget *parent=0, const char *name=0, uchar type=0,QDateTime* mioOrol=NULL);
   void	showDate();
   QDateTime getDataOra();  
   void setDataOra(QDateTime);
signals:   

      
protected:					
    void	timerEvent( QTimerEvent * );
    void	mousePressEvent( QMouseEvent * );
 
	
private slots:					
    void	stopDate();
public slots:
/*!
  \brief Shows the time.
*/   
    void	showTime();
/*!
  \brief Decreases seconds value.
*/       
    void 	diminSec();	
/*!
  \brief Decreases minuts value.
*/           
    void 	diminMin();
/*!
  \brief Decreases hours value.
*/           
    void 	diminOra();
/*!
  \brief Decreases days value.
*/           
    void 	diminDay();
/*!
  \brief Decreases months value.
*/           
    void 	diminMonth();
/*!
  \brief Decreases years value.
*/           
    void 	diminYear();
/*!
  \brief Increases seconds value.
*/           
    void 	aumSec();	
/*!
  \brief Increases minutes value.
*/           
    void 	aumMin();
/*!
  \brief Increases hours value.
*/           
    void 	aumOra();
/*!
  \brief Increases days value.
*/           
    void 	aumDay();
/*!
  \brief Increases months value.
*/               
    void 	aumMonth();
/*!
  \brief Increases years value.
*/           
    void 	aumYear();    
    
    
    
private:					

    int	normalTimer;
    int	showDateTimer;
    uchar	type;
    QDateTime* mioClock;
    
};


#endif // TIMESCRIPT_H
