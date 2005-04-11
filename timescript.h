/****************************************************************************
** $Id: timescript.h,v 1.1 2005/04/11 08:55:34 cvs Exp $
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


class timeScript : public QLCDNumber		
{
    Q_OBJECT
public:
   // timeScript( QWidget *parent=0, const char *name=0 );
    timeScript( QWidget *parent=0, const char *name=0, uchar type=0,QDateTime* mioOrol=NULL);
   void	showDate();
   QDateTime getDataOra();  
signals:   

      
protected:					
    void	timerEvent( QTimerEvent * );
    void	mousePressEvent( QMouseEvent * );
 
	
private slots:					
    void	stopDate();
public slots:
    void	showTime();
    
    void 	diminSec();	
    void 	diminMin();
    void 	diminOra();
    void 	diminDay();
    void 	diminMonth();
    void 	diminYear();
    
    void 	aumSec();	
    void 	aumMin();
    void 	aumOra();
    void 	aumDay();
    void 	aumMonth();
    void 	aumYear();    
    
    
    
private:					

    int	normalTimer;
    int	showDateTimer;
    uchar	type;
    QDateTime* mioClock;
    
};


#endif // TIMESCRIPT_H
