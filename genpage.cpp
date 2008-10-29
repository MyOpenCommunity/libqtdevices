/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** genpage.cpp
**
**pagine di visualizzazione generiche
**
****************************************************************/

#include "genpage.h"
#include "main.h"

#include <QWidget>
#include <QPixmap>
#include <QCursor>
#include <QFile>


genPage::genPage(QWidget *parent,const char *name , unsigned char tipo,const char *img, unsigned int f)
	: BtLabel(parent, name, (Qt::WindowFlags)f)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
	switch (tipo)
	{
	case RED:
		setStyleSheet("QLabel {color:#FF0000;}");
		break;
	case BLUE:
		setStyleSheet("QLabel {color:#0000FF;}");
		break;
	case GREEN:
		setStyleSheet("QLabel {color:#00FF00;}");
		break;
	case IMAGE:
		if (QFile::exists(img))
			setPixmap(QPixmap(img));
		break;
	}
}
