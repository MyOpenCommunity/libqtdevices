/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** banner.cpp
 **
 **Implementazione di un banner generico
 **
 ****************************************************************/

#include "banner.h"
#include "btbutton.h"
#include "main.h"
#include "generic_functions.h" // getPressName
#include "openclient.h"
#include "fontmanager.h"

#include <QPixmap>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QApplication> //qapp

#include <stdlib.h>

// Init icons_library - Vecchio modo con la cache che è un membro statico di banner
// IconDispatcher  banner::icons_library;

banner::banner(QWidget *parent,const char *name) : QWidget(parent)
{
	BannerIcon = NULL;
	BannerText = NULL;
	SecondaryText = NULL;
	sxButton = NULL;
	dxButton = NULL;
	csxButton = NULL;
	cdxButton = NULL;
	for (int idx = 0; idx < MAX_NUM_ICON; idx++)
		Icon[idx] = NULL;
	for (int idx = 0; idx < MAX_PRESS_ICON; idx++)
		pressIcon[idx] = NULL;
	periodo = 0;
	numFrame = contFrame = 0;
	value = 0;
	attivo = 0;
	minValue = maxValue = 0;
	step = 1;
	animationTimer = NULL;
	memset(group,false,sizeof(group));
	serNum = 1;
}

banner::~banner()
{
	if (BannerIcon)
		delete BannerIcon;
	if (BannerText)
		delete BannerText;
	if (SecondaryText)
		delete SecondaryText;
	if (dxButton)
		delete dxButton;
	if (csxButton)
		delete csxButton;
	if (cdxButton)
		delete cdxButton;
	if (sxButton)
		delete sxButton;
	BannerIcon = NULL;
	BannerText = NULL;
	SecondaryText = NULL;
	sxButton = NULL;
	dxButton = NULL;
	csxButton = NULL;
	cdxButton = NULL;
	for (int idx = 0; idx < MAX_NUM_ICON; idx++)
		Icon[idx] = NULL;
	for (int idx = 0; idx < MAX_PRESS_ICON; idx++)
		pressIcon[idx] = NULL;
}

void banner::SetTextU(const QString & text)
{
	qtesto = text;
	qtesto.truncate(MAX_PATH*2-1);
}

void banner::SetSecondaryTextU(const QString & text)
{
	qtestoSecondario = text;
	qtestoSecondario.truncate(MAX_TEXT_2-1);
}

BtButton *banner::customButton()
{
	return 0;
}

QString banner::getPressedIconName(QString iconname)
{
	/** This method wraps the previous pressIconName function.
	 *  The main fix introduced is to return the NOT-Pressed Icon Name if
	 *  does not exist pressed icon.
	 */
	if (iconname.isEmpty())
		return QString();

	QString pressIconName = getPressName(iconname);

	return QFile::exists(pressIconName) ? pressIconName : iconname;
}

void banner::SetIcons(int id, QString name)
{
	assert(id < MAX_PRESS_ICON && id >= 0 && "Index of icon out of range!");
	Icon[id]      = icons_library.getIcon(name);
	pressIcon[id] = icons_library.getIcon(getPressedIconName(name));
}

void banner::SetIcons(const char *name, int type)
{
	if (type == 1)
	{
		Icon[0]      = icons_library.getIcon(name);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(name));
	}
	if (type == 3)
	{
		Icon[3]      = icons_library.getIcon(name);
		pressIcon[3] = icons_library.getIcon(getPressedIconName(name));
	}
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon,const char *centerIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerIcon)
	{
		Icon[3]      = icons_library.getIcon(centerIcon);
		pressIcon[3] = icons_library.getIcon(getPressedIconName(centerIcon));
		qDebug("Icon[3] <- %s", centerIcon);
	}

	impostaAttivo(1);
}

void banner::SetIcons(const char *sxIcon, const char *dxIcon, const char *centerActiveIcon, const char *centerInactiveIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerActiveIcon)
	{
		Icon[3]      = icons_library.getIcon(centerActiveIcon);
		pressIcon[3] = icons_library.getIcon(getPressedIconName(centerActiveIcon));
		qDebug("Icon[3] <- %s", centerActiveIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon(centerInactiveIcon);
		pressIcon[2] = icons_library.getIcon(getPressedIconName(centerInactiveIcon));
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon, const char*centerInactiveIcon, const char*centerUpIcon, const char*centerDownIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon(centerInactiveIcon);
		pressIcon[2] = icons_library.getIcon(getPressedIconName(centerInactiveIcon));
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}

	if (centerUpIcon)
	{
		Icon[3]      = icons_library.getIcon(centerUpIcon);
		pressIcon[3] = icons_library.getIcon(getPressedIconName(centerUpIcon));
		qDebug("Icon[3] <- %s", centerUpIcon);
	}

	if (centerDownIcon)
	{
		Icon[4]      = icons_library.getIcon(centerDownIcon);
		pressIcon[4] = icons_library.getIcon(getPressedIconName(centerDownIcon));
		qDebug("Icon[4] <- %s", centerDownIcon);
	}
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,int period, int number)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon(centerInactiveIcon);
		pressIcon[2] = icons_library.getIcon(getPressedIconName(centerInactiveIcon));
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}

	if (centerActiveIcon && number)
	{
		for (int i = 1; i <= number; i++)
		{
			QString root_of_name = getNameRoot(centerActiveIcon, ".png");
			QString name = QString("%1%2.png").arg(root_of_name).arg(i);
			Icon[2+i] = icons_library.getIcon(name, "PNG", Qt::ThresholdDither | Qt::DitherMode_Mask | Qt::AvoidDither);
		}
	}
	else if (centerActiveIcon)
	{
		Icon[3]      = icons_library.getIcon(centerActiveIcon);
		pressIcon[3] = icons_library.getIcon(getPressedIconName(centerActiveIcon));
		qDebug("Icon[3] <- %s", centerActiveIcon);
	}

	periodo = period;
	numFrame = number;
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,char inactiveLevel)
{
	SetIcons(sxIcon, dxIcon, centerActiveIcon, centerInactiveIcon,(const char*)NULL, inactiveLevel);
}

void banner::SetIcons(const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon, const char*breakIcon, char inactiveLevel)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon(sxIcon);
		pressIcon[0] = icons_library.getIcon(getPressedIconName(sxIcon));
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon(dxIcon);
		pressIcon[1] = icons_library.getIcon(getPressedIconName(dxIcon));
		qDebug("Icon[1] <- %s", dxIcon);
	}
	
	// Load base Icon that can be *sxl0 or *dxl0
	//FIXME FIXME FIXME(anche sopra)
	QString inactive_root_of_name = getNameRoot(centerInactiveIcon, ".png");
	QString active_root_of_name   = getNameRoot(centerActiveIcon, ".png");
	QString nomeFile;

	qDebug() << "________________________________________________________________________________";
	qDebug() << inactive_root_of_name;
	qDebug() << active_root_of_name;
	qDebug() << "________________________________________________________________________________";

	// Set first 2 icons from inactive root
	Icon[2] = icons_library.getIcon(QString("%1sxl0.png").arg(inactive_root_of_name));
	Icon[3] = icons_library.getIcon(QString("%1dxl0.png").arg(inactive_root_of_name));
	
	qDebug() << "New Icon[2] <- " << QString("%1sxl0.png").arg(inactive_root_of_name);
	qDebug() << "New Icon[3] <- " << QString("%1dxl0.png").arg(inactive_root_of_name);
	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1sxl%2.png").arg(active_root_of_name).arg(i);
		Icon[4+y*2] = icons_library.getIcon(nomeFile);
		qDebug() << "New Icon[" << 4+y*2 << "] <- " << nomeFile;
		if (inactiveLevel)
		{
			nomeFile = QString("%1sxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[22+y*2] = icons_library.getIcon(nomeFile);
			qDebug() << "New Icon[" << 22+y*2 << "] <- " << nomeFile;
		}
	}

	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1dxl%2.png").arg(active_root_of_name).arg(i);
		Icon[5+y*2] = icons_library.getIcon(nomeFile);
		qDebug() << "New Icon[" << 5+y*2 << "] <- " << nomeFile;
		if (inactiveLevel)
		{
			nomeFile = QString("%1dxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[23+y*2] = icons_library.getIcon(nomeFile);
			qDebug() << "New Icon[" << 23+y*2 << "] <- " << nomeFile;
		}
	}
	
	if (breakIcon)
	{
		QString break_root_of_name = getNameRoot(breakIcon, ".png");
	
		nomeFile = QString("%1sx.png").arg(break_root_of_name);
		Icon[44] = icons_library.getIcon(nomeFile);
		qDebug() << "New Icon[" << 44 << "] <- " << nomeFile;

		nomeFile = QString("%1dx.png").arg(break_root_of_name);
		Icon[45] = icons_library.getIcon(nomeFile);
		qDebug() << "New Icon[" << 45 << "] <- " << nomeFile;
		
	}
}


void banner::addItem(char item,int x,int y,int dimX, int dimY)
{
	QWidget *Item = NULL;

	switch(item)
	{
	case BUT1:
		sxButton = new BtButton(this);
		connect(sxButton,SIGNAL(clicked()),this,SIGNAL(sxClick()));
		connect(sxButton,SIGNAL(pressed()),this,SIGNAL(sxPressed()));
		connect(sxButton,SIGNAL(released()),this,SIGNAL(sxReleased()));
		Item = sxButton;
		break;
	case BUT2:
		dxButton = new BtButton(this);
		connect(dxButton,SIGNAL(clicked()),this,SIGNAL(dxClick()));
		connect(dxButton,SIGNAL(pressed()),this,SIGNAL(dxPressed()));
		connect(dxButton,SIGNAL(released()),this,SIGNAL(dxReleased()));
		Item = dxButton;
		break;
	case TEXT: 
		BannerText = new QLabel(this);
		Item = BannerText;
		break;
	case ICON:
		BannerIcon = new QLabel(this);
		Item = BannerIcon;
		break;
	case ICON2:
		BannerIcon2 = new QLabel(this);
		Item = BannerIcon2;
		break;
	case BUT3:
		csxButton = new BtButton(this);
		connect(csxButton,SIGNAL(clicked()),this,SIGNAL(csxClick()));
		connect(csxButton,SIGNAL(pressed()),this,SIGNAL(csxPressed()));
		connect(csxButton,SIGNAL(released()),this,SIGNAL(csxReleased()));
		Item = csxButton;
		break;
	case BUT4:
		cdxButton = new BtButton(this);
		connect(cdxButton,SIGNAL(clicked()),this,SIGNAL(cdxClick()));
		connect(cdxButton,SIGNAL(pressed()),this,SIGNAL(cdxPressed()));
		connect(cdxButton,SIGNAL(released()),this,SIGNAL(cdxReleased()));
		Item = cdxButton;
		break;
	case TEXT2:
		SecondaryText = new QLabel(this);
		Item = SecondaryText;
		break;
	}
	Item->setGeometry(x,y,dimX,dimY);
}

void banner::nascondi(char item)
{
	switch(item)
	{
	case BUT1:
		if (sxButton)
		   sxButton->hide();
		break;
	case BUT2:
		if (dxButton)
		   dxButton->hide();
		break;
	case TEXT:
		if (BannerText)
			BannerText->hide();
		break;
	case ICON:
		if (BannerIcon)
			BannerIcon->hide();
		break;
	case BUT3:
		if (csxButton)
			csxButton->hide();
		break;
	case BUT4:
		if (cdxButton)
			cdxButton->hide();
		break;
	case TEXT2:
		if (SecondaryText)
			SecondaryText->hide();
		break;
	}
}

void banner::mostra(char item)
{
	switch(item)
	{
	case BUT1:
			if (sxButton)
				sxButton->show();
			break;
	case BUT2: if (dxButton)
				dxButton->show();
			break;
	case TEXT: if (BannerText)
				BannerText->show();
			break;
	case ICON: if (BannerIcon)
				BannerIcon->show();
			break;
	case BUT3: if (csxButton)
				csxButton->show();
			break;
	case BUT4: if (cdxButton)
				cdxButton->show();
			break;
	case TEXT2: if (SecondaryText)
				SecondaryText->show();
			break;
	}
}

void banner::drawAllButRightButton()
{
	if (sxButton && Icon[0])
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}
	if (minValue == maxValue)
	{
		QPixmap *pntIcon = NULL;

		if (attivo == 1)
			pntIcon = Icon[3+contFrame];
		if (attivo == 2)
			pntIcon = Icon[4+contFrame];
		else if (!attivo)
			pntIcon = Icon[2];
		if (pntIcon && BannerIcon)
		{
			BannerIcon->repaint();
			BannerIcon->setPixmap(*pntIcon);
			BannerIcon->repaint();
		}

		if (Icon[2] && csxButton)
		{
			csxButton->setPixmap(*Icon[2]);
			if (pressIcon[2])
				csxButton->setPressedPixmap(*pressIcon[2]);
		}

		if (cdxButton && Icon[3])
		{
			cdxButton->setPixmap(*Icon[3]);
			if (pressIcon[3])
				cdxButton->setPressedPixmap(*pressIcon[3]);
		}
		qApp->flush();
	}
	else
	{
		if (attivo == 1)
		{
			if (Icon[4+((value-step)/step)*2] && csxButton)
			{
				csxButton->setPixmap(*Icon[4+((value-step)/step)*2]);
				qDebug("* Icon[%d]", 4+((value-step)/step)*2);
			}
			if (cdxButton && Icon[5+((value-step)/step)*2])
			{
				cdxButton->setPixmap(*Icon[5+((value-step)/step)*2]);
				qDebug("** Icon[%d]", 5+((value-step)/step)*2);
			}
		}
		else if (attivo == 0)
		{
			if (Icon[22])
			{
				if (Icon[22+(value-step)/step*2] && csxButton)
				{
					csxButton->setPixmap(*Icon[22+((value-step)/step)*2]);
					qDebug("*** Icon[%d]", 22+((value-step)/step)*2);
				}

				if (cdxButton && Icon[23+((value-step)/step)*2])
				{
					cdxButton->setPixmap(*Icon[23+((value-step)/step)*2]);
					qDebug("**** Icon[%d]", 23+((value-step)/step)*2);
				}
			}
			else
			{
				if (Icon[2] && csxButton)
				{
					csxButton->setPixmap(*Icon[2]);	
					qDebug("***** Icon[%d]", 2);
				}

				if (cdxButton && Icon[3])
				{
					cdxButton->setPixmap(*Icon[3]); 
					qDebug("****** Icon[%d]", 3);
				}
			}
		}
		else if (attivo == 2)
		{
			if (Icon[44] && csxButton)
			{
				csxButton->setPixmap(*Icon[44]);
				qDebug("******* Icon[%d]", 44);
			}

			if (cdxButton && Icon[45])
			{
				cdxButton->setPixmap(*Icon[45]);
				qDebug("******* Icon[%d]", 45);
			}
		}
	}

	/*TODO
	  icona animata -> armare timer per cambiare centerActiveIcon
	  qui devo copiare period e number e in draw attivare il timer*/
	if (periodo && numFrame)
	{
		if (!animationTimer)
		{
			animationTimer = new QTimer(this);
			connect(animationTimer,SIGNAL(timeout()),this,SLOT(animate()));
		}
		if (!animationTimer->isActive() && attivo)
			animationTimer->start(periodo);
	}

	if (BannerText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_banner_BannerText, aFont);
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(aFont);
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_banner_SecondaryText, aFont);
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(aFont);
		SecondaryText->setText(qtestoSecondario);
	}
}

void banner::Draw()
{
	qDebug("banner::Draw(), attivo = %d, value = %d", attivo, value);
	drawAllButRightButton();
	if (dxButton && Icon[1])
	{
		dxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			dxButton->setPressedPixmap(*pressIcon[1]);
	}
}

void banner::impostaAttivo(char Attivo)
{
	attivo = Attivo;
	if (animationTimer && !Attivo)
	{
		animationTimer->stop();
		qDebug("KILLanimationTimer");
	}
}

void banner::setAddress(QString addr)
{
	QByteArray buf = addr.toAscii();
	// TODO: rendere address una qstring!
	strncpy(address,buf.constData(),sizeof(address));
}

void banner::gestFrame(char*)
{
}

void banner::setValue(char val)
{
	if (val >= minValue && val <= maxValue)
		value = val;
}

void banner::setRange(char minval,char maxval)
{
	maxValue = maxval;
	minValue = minval;
}

void banner::setStep(char s)
{
	step = s;
}

unsigned char banner::isActive()
{
	return attivo;
}

char banner::getValue()
{
	return value;
}

void banner::animate()
{
	contFrame++;
	if (contFrame >= numFrame)
		contFrame = 0;
	Draw();
}

void banner::setAnimationParams(int per, int num)
{
	periodo = per;
	numFrame = num;
}

void banner::getAnimationParams(int& per, int& num)
{
	per = periodo;
	num = numFrame;
}

char* banner::getAddress()
{
	return address;
}

bool* banner::getGroup()
{
	return group;
}

bool banner::isForMe(openwebnet * m)
{
	if (strcmp(m->Extract_chi(), "1"))
		 return false;
	if (!strcmp(m->Extract_dove(), getAddress()))
		 return true;
	// BAH
	return (((!strcmp(m->Extract_dove(),"0")) ||
			((strlen(m->Extract_dove())==1) && 
			(!strncmp(m->Extract_dove(), getAddress(), 1))) ||
			((!strncmp(m->Extract_dove(),"#",1)) && 
			 // assuming that the following code stays inside group array,
			 // the result is always false, since nobody ever sets anything
			 // different in group
			*(getGroup()+(atoi(m->Extract_dove()+1))-1))));
}

void banner::inizializza(bool forza)
{
}

void  banner::rispStato(char*)
{
}

void banner::openAckRx(void)
{
	qDebug("openAckRx()");
}

void banner::openNakRx(void)
{
	qDebug("openNakRx()");
}

void banner::setSerNum(int s)
{
	serNum = s;
}

int banner::getSerNum()
{
	return serNum;
}

char banner::getId()
{
	return id;
}

void banner::setId(char i)
{
	id = i;
}

void banner::ambChanged(const QString &, bool, char *)
{
	qDebug("[WARNING] empty slot ambChanged called!");
};

void banner::parentChanged(QWidget *newParent)
{
}

void banner::grandadChanged(QWidget *newGrandad)
{
}

void banner::addAmb(char *)
{
}

QString banner::getNameRoot(QString full_string, QString text_to_strip)
{
	return full_string.mid(0, full_string.lastIndexOf('.'));

}

QString banner::getNameRoot(char *full_string, QString text_to_strip)
{
	return getNameRoot(QString(full_string), text_to_strip);
}
