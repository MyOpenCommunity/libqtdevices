/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** homePage.cpp
**
** Finestra principale
**
****************************************************************/

#include "homepage.h"
#include "timescript.h"
#include "genericfunz.h"
#include "openclient.h"
#include "btlabel.h"
#include "fontmanager.h"
#include "scaleconversion.h"

#include <QPixmap>
#include <QDateTime>
#include <QCursor>
#include <QLCDNumber>
#include <QByteArray>

#include <stdlib.h>


homePage::homePage(QWidget *parent, const char *name) : QWidget(parent)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(Qt::BlankCursor));
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
	xClock = xTemp = MAX_WIDTH + 2;
	yClock = yTemp = MAX_HEIGHT + 2;
	freez = FALSE;
	descrizione = NULL;
	tempCont = 0;
	temp_scale = readTemperatureScale();
}

void homePage::addButton(int x, int y, QString iconName, char function, QString chix, QString cosax,QString dovex, char tipo)
{
	BtButton *b1 = new BtButton(this,"BelBottone");
	elencoButtons.append(b1);

	if (function == SPECIAL)
		b1->setGeometry(x, y, DIM_SPECBUT_HOME, DIM_BUT_HOME_SMALL);
	else if (function == BACK)
		b1->setGeometry(x, y, DIM_BUT_HOME_SMALL, DIM_BUT_HOME_SMALL);
	else
		b1->setGeometry(x, y, DIM_BUT_HOME, DIM_BUT_HOME);

	QPixmap Icon;
	if (Icon.load(iconName))
		b1->setPixmap(Icon);
	b1->setPaletteBackgroundColor(backgroundColor());

	if (Icon.load(getPressName(iconName)))
		b1->setPressedPixmap(Icon);

	switch (function)
	{
	case AUTOMAZIONE:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Automazione()));
		break;
	case ILLUMINAZIONE:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Illuminazione()));
		break;
	case ANTIINTRUSIONE:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Antiintrusione()));
		break;
	case CARICHI:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Carichi()));
		break;
	case TERMOREGOLAZIONE:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Termoregolazione()));
		break;
	case DIFSON:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Difson()));
		break;
    case DIFSON_MULTI:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Difmulti()));
		break;
	case SCENARI:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Scenari()));
		break;
	case IMPOSTAZIONI:
	 	connect(b1,SIGNAL(clicked()),this, SIGNAL(Settings()));
		break;
	case SPECIAL:
		tipoSpecial = tipo;
		qDebug("tipoSpecial= %d",tipoSpecial);
		strcpy(chi, chix.toAscii().constData());
		strcpy(cosa, cosax.toAscii().constData());
		strcpy(dove, dovex.toAscii().constData());
		if (tipoSpecial == PULSANTE)
		{
			qDebug("tipoSpecial = PULSANTE");
			connect(b1, SIGNAL(pressed()), this, SLOT(specFunzPress()));
			connect(b1, SIGNAL(released()), this, SLOT(specFunzRelease()));
		}
		else
			connect(b1, SIGNAL(clicked()), this, SLOT(specFunz()));
		break;
    case SCENARI_EVOLUTI:
		connect(b1, SIGNAL(clicked()), this, SIGNAL(ScenariEvoluti()));
		break;
    case VIDEOCITOFONIA:
		connect(b1,SIGNAL(clicked()),this, SIGNAL(Videocitofonia()));
		break;
    case SUPERVISIONE:
		connect(b1, SIGNAL(clicked()), this, SIGNAL(Supervisione()));
		break;
	case BACK:
	 	connect(b1, SIGNAL(clicked()), this, SIGNAL(Close()));
		break;
	}
}

void homePage::setBGColor(int r, int g, int b)
{	
	setPaletteBackgroundColor(QColor::QColor(r,g,b));
}
void homePage::setFGColor(int r, int g, int b)
{
	setPaletteForegroundColor(QColor::QColor(r,g,b));
}

int homePage::setBGPixmap(char* backImage)
{
	QPixmap Back;
	if (Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return 0;
	}
	return 1;
}

void homePage::addClock(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
	dataOra = new timeScript(this,"scrittaHomePage",1);
	dataOra->setGeometry(x,y,width,height);
	dataOra->setPaletteForegroundColor(fg);
	dataOra->setPaletteBackgroundColor(bg);
	dataOra->setFrameStyle(style);
	dataOra->setLineWidth(line);
}

void homePage::addClock(int x, int y)
{
	addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}

void homePage::addDate(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
	dataOra = new timeScript(this,"scrittaHomePage",uchar (25));
	dataOra->setGeometry(x,y,width,height);
	dataOra->setPaletteForegroundColor(fg);
	dataOra->setPaletteBackgroundColor(bg);
	dataOra->setFrameStyle(style);
	dataOra->setLineWidth(line);
}

void homePage::addDate(int x, int y)
{
	addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}	

void homePage::addTemp(QString z, int x, int y, int width, int height, QColor bg, QColor fg,
	int style, int line, const QString & qtext, char *Ext)
{
	switch(tempCont)
	{
	case 0:
	 	strcpy(zonaTermo1, z.toAscii().constData());
		zt[0] = &zonaTermo1[0];
		strcpy(ext1,Ext);
		ext[0] = ext1;
		break;
	case 1:
		strcpy(zonaTermo2, z.toAscii().constData());
		zt[1] = &zonaTermo2[0];
		strcpy(ext2,Ext);
		ext[1] = ext2;
		break;
	case 2:
		strcpy(zonaTermo3, z.toAscii().constData());
		zt[2] = &zonaTermo3[0];
		strcpy(ext3,Ext);
		ext[2] = ext3;
		break;
	}

	temperatura[tempCont] = new QLCDNumber(this);
	temperatura[tempCont]->setGeometry(x, y, width, height - H_SCR_TEMP);
	// TODO: sistemare con i metodi qt4!
	//temperatura[tempCont]->setPaletteForegroundColor(fg);
	//temperatura[tempCont]->setPaletteBackgroundColor(bg);
	temperatura[tempCont]->setFrameStyle(style);
	temperatura[tempCont]->setLineWidth(line);
	temperatura[tempCont]->setNumDigits(6);
	temperatura[tempCont]->setSegmentStyle(QLCDNumber::Flat);

	unsigned default_bt_temp = 1235;
	updateTemperatureDisplay(default_bt_temp, tempCont);

	if (!qtext.isEmpty())
	{
		QFont aFont;
		FontManager::instance()->getFont(font_homepage_bottoni_label, aFont);
		descrTemp[tempCont] = new BtLabel(this);
		descrTemp[tempCont]->setFont(aFont);
		descrTemp[tempCont]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		descrTemp[tempCont]->setText(qtext);
		descrTemp[tempCont]->setGeometry(x,y+height-H_SCR_TEMP,width,H_SCR_TEMP);
		descrTemp[tempCont]->setPaletteForegroundColor(fg);
		descrTemp[tempCont]->setPaletteBackgroundColor(bg);
	}
	tempCont++;
}

void homePage::addTemp(QString z, int x, int y)
{
	addTemp(z,x,y,180, 35, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0,NULL);
}

void homePage::inizializza()
{
	char Frame[50];

	for (unsigned idx = 0; idx < tempCont; idx++)
	{
		strcpy(&Frame[0],"*#4*");
		if (!strcmp(ext[idx], "0"))
		{
			strcat(&Frame[0],zt[idx]);
			strcat(&Frame[0],"*0##");
		}
		else
		{
			strcat(&Frame[0],zt[idx]);
			strcat(&Frame[0],"00*15#");
			strcat(&Frame[0],zt[idx]);
			strcat(&Frame[0],"##");
		}
		emit sendInit(&Frame[0]);
	}
}

void homePage::addDescrU(const QString & qz, int x, int y, int width, int height, QColor bg,
	QColor fg, int style,int line)
{
	QFont aFont;
	FontManager::instance()->getFont(font_homepage_bottoni_descrizione, aFont);
	descrizione = new BtLabel(this);
	descrizione->setFont(aFont);
	descrizione->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	descrizione->setText(qz);
	descrizione->setGeometry(x,y,width,height);
	descrizione->setPaletteForegroundColor(fg);
	descrizione->setPaletteBackgroundColor(bg);
	descrizione->setFrameStyle(style);
	descrizione->setLineWidth(line);
}

void homePage::addDescrU(const QString & qz, int x, int y)
{
	addDescrU(qz,x,y,160, 20, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0);
}

void homePage::mouseReleaseEvent(QMouseEvent * e)
{
	qDebug("Released");
	if (freez)
	{
		freez = FALSE;
		emit freeze(freez);
	}
	QWidget::mouseReleaseEvent(e);
}

void homePage::freezed(bool f)
{
	freez = f;
	if (freez)
	{
		for (int i = 0; i < elencoButtons.size(); ++i)
			elencoButtons.at(i)->setEnabled(FALSE);
		if (descrizione)
			descrizione->setEnabled(FALSE);
	}
	else
	{
		for (int i = 0; i < elencoButtons.size(); ++i)
			elencoButtons.at(i)->setEnabled(TRUE);
		if (descrizione)
			descrizione->setEnabled(TRUE);
	}
}

void homePage::updateTemperatureDisplay(unsigned new_bt_temperature, unsigned which_display)
{
	qDebug("vedo temperatura per Temp in Homepage: %u", new_bt_temperature);
	QString displayed_temp;

	switch (temp_scale)
	{
	case CELSIUS:
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
		break;
	case FAHRENHEIT:
		displayed_temp = fahrenheitString(bt2Fahrenheit(new_bt_temperature));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp_scale = CELSIUS;
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
	}
	// qlcdnumber can display the degree sign, but not as utf-8 text.
	// We have to change the char TEMP_DEGREES with the single quote char (')
	displayed_temp = displayed_temp.replace(TEMP_DEGREES, " '");

	temperatura[which_display]->display(displayed_temp);
}

void homePage::gestFrame(char* frame)
{
	openwebnet msg_open;
	char dovex[30];
	unsigned icx;
	bool my_frame;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"4"))
	{
		qDebug("vedo temperatura per Temp: %s",msg_open.frame_open);
		strcpy(dovex, msg_open.Extract_dove());
		if (dovex[0] == '#')
			strcpy(&dovex[0], &dovex[1]);
	
		for (unsigned idx = 0; idx < tempCont; idx++)
		{
			my_frame = false;
			qDebug("dove frame %s dove oggetto %s",dovex, zt[idx]);
			if (!strcmp(ext[idx], "0") && !strcmp(dovex,zt[idx]) && !strcmp(msg_open.Extract_grandezza(),"0"))
			{
				//Temperatura misurata
				icx = static_cast<unsigned>(atoi(msg_open.Extract_valori(0)));
				my_frame = true;
			}
			else if (!strcmp(ext[idx], "1") && !strcmp(msg_open.Extract_grandezza(),"15") && !strncmp(dovex,zt[idx], 1))
			{
				icx = static_cast<unsigned>(atoi(msg_open.Extract_valori(1)));
				my_frame = true;
			}
			if (my_frame)
			{
				updateTemperatureDisplay(icx, idx);
			}
		}
	}
	if (!strcmp(msg_open.Extract_chi(),chi) && tipoSpecial == CICLICO)
	{
		if (!strcmp(dove,msg_open.Extract_dove()))
		{
			strcpy(cosa, msg_open.Extract_cosa());
		}
	}
}

void homePage::specFunz()
{
	char specialFrame[50];

	if (tipoSpecial == CICLICO)
	{
		if (strcmp(cosa, "0"))
			strcpy(cosa, "0");
		else
			strcpy(cosa, "1");
	}

	strcpy(specialFrame, "*");
	strcat(specialFrame, chi);
	strcat(specialFrame, "*");
	strcat(specialFrame, cosa);
	strcat(specialFrame, "*");
	strcat(specialFrame, dove);
	strcat(specialFrame, "##");

	emit sendFrame(specialFrame);
}

void homePage::specFunzPress()
{
	qDebug("tipoSpecial= PRESSED");
	char specialFrame[50];

	strcpy(specialFrame, "*");
	strcat(specialFrame, chi);
	strcat(specialFrame, "*1*");
	strcat(specialFrame, dove);
	strcat(specialFrame, "##");
	emit sendFrame(specialFrame);
}

void homePage::specFunzRelease()
{
	qDebug("tipoSpecial= RELEASED");
	char specialFrame[50];

	strcpy(specialFrame, "*");
	strcat(specialFrame, chi);
	strcat(specialFrame, "*0*");
	strcat(specialFrame, dove);
	strcat(specialFrame, "##");
	emit sendFrame(specialFrame);
}

void homePage::setPaletteBackgroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(backgroundRole(), c);
	setPalette(palette);
}

void homePage::setPaletteForegroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(foregroundRole(), c);
	setPalette(palette);
}

void homePage::setPaletteBackgroundPixmap(const QPixmap &pixmap)
{
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);
}

const QColor& homePage::backgroundColor()
{
	return palette().color(backgroundRole());
}

const QColor& homePage::foregroundColor()
{
	return palette().color(foregroundRole());
}
