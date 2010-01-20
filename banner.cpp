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
#include "icondispatcher.h" // bt_global::icons_cache
#include "generic_functions.h" // getPressName
#include "openclient.h"
#include "fontmanager.h" // bt_global::font
#include "openclient.h"
#include "page.h"

#include <QPixmap>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QApplication> //qapp
#include <QLayout>


// Inizialization of static member
Client *banner::client_comandi = 0;
Client *banner::client_richieste = 0;


banner::banner(QWidget *parent) : QWidget(parent)
{
	banner_width = 240;
	banner_height = 80;

	linked_sx_page = 0;
	linked_dx_page = 0;
	BannerIcon = 0;
	BannerIcon2 = 0;
	BannerText = 0;
	SecondaryText = 0;
	sxButton = 0;
	dxButton = 0;
	csxButton = 0;
	cdxButton = 0;
	for (int idx = 0; idx < MAX_NUM_ICON; idx++)
		Icon[idx] = 0;
	for (int idx = 0; idx < MAX_PRESS_ICON; idx++)
		pressIcon[idx] = 0;
	periodo = 0;
	numFrame = contFrame = 0;
	value = 0;
	attivo = 0;
	minValue = maxValue = 0;
	step = 1;
	animationTimer = NULL;
	serNum = 1;
	id = -1;
}

banner::~banner()
{
	delete BannerIcon;
	delete BannerText;
	delete SecondaryText;
	delete dxButton;
	delete csxButton;
	delete cdxButton;
	delete sxButton;

	for (int idx = 0; idx < MAX_NUM_ICON; idx++)
		Icon[idx] = NULL;
	for (int idx = 0; idx < MAX_PRESS_ICON; idx++)
		pressIcon[idx] = NULL;

	delete linked_sx_page;
	delete linked_dx_page;
}

void banner::setText(const QString &text)
{
	qtesto = text;
}

void banner::setSecondaryText(const QString &text)
{
	qtestoSecondario = text;
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

void banner::SetIcons(int id, QString name, QString pressed_name)
{
	Q_ASSERT_X(id < MAX_PRESS_ICON && id >= 0, "banner::SetIcons", "Index of icon out of range!");
	Icon[id]      = bt_global::icons_cache.getIcon(name);
	if (pressed_name.isNull())
		pressed_name = getPressedIconName(name);
	pressIcon[id] = bt_global::icons_cache.getIcon(pressed_name);
}

void banner::SetIcons(QString name, int type)
{
	int id;
	switch (type)
	{
		case 1:
			id = 0;
			break;
		case 3:
			id = 3;
			break;
		default:
			qWarning("Type %d not handled on banner::setIcons!", type);
			return;
	}
	SetIcons(id, name);
}

void banner::SetIcons(QString sxIcon, QString dxIcon)
{
	if (!sxIcon.isNull())
		SetIcons(0, sxIcon);

	if (!dxIcon.isNull())
		SetIcons(1, dxIcon);
}

void banner::SetIcons(QString sxIcon , QString dxIcon, QString centerIcon)
{
	SetIcons(sxIcon, dxIcon);

	if (!centerIcon.isNull())
		SetIcons(3, centerIcon);

	// TODO: che cos'e' questo impostaAttivo? Senza di quello potrebbe essere usata
	// questa funzione con argomento di default anche per la variante sopra!
	impostaAttivo(1);
}

void banner::SetIcons(QString sxIcon, QString dxIcon, QString centerActiveIcon, QString centerInactiveIcon)
{
	SetIcons(sxIcon, dxIcon);

	if (!centerActiveIcon.isNull())
		SetIcons(3, centerActiveIcon);

	if (!centerInactiveIcon.isNull())
		SetIcons(2, centerInactiveIcon);
}

void banner::SetIcons(QString sxIcon, QString dxIcon, QString centerInactiveIcon, QString centerUpIcon, QString centerDownIcon)
{
	SetIcons(sxIcon, dxIcon);

	if (!centerInactiveIcon.isNull())
		SetIcons(2, centerInactiveIcon);

	if (!centerUpIcon.isNull())
		SetIcons(3, centerUpIcon);

	if (!centerDownIcon.isNull())
		SetIcons(4, centerDownIcon);
}

void banner::SetIcons(QString sxIcon, QString dxIcon, QString centerActiveIcon, QString centerInactiveIcon, bool inactiveLevel)
{
	SetIcons(sxIcon, dxIcon, centerActiveIcon, centerInactiveIcon, QString(), inactiveLevel);
}

void banner::SetIcons(QString sxIcon, QString dxIcon,QString centerActiveIcon, QString centerInactiveIcon, QString breakIcon, bool inactiveLevel)
{
	SetIcons(sxIcon, dxIcon);
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
	Icon[2] = bt_global::icons_cache.getIcon(QString("%1sxl0.png").arg(inactive_root_of_name));
	Icon[3] = bt_global::icons_cache.getIcon(QString("%1dxl0.png").arg(inactive_root_of_name));

	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1sxl%2.png").arg(active_root_of_name).arg(i);
		Icon[4+y*2] = bt_global::icons_cache.getIcon(nomeFile);
		if (inactiveLevel)
		{
			nomeFile = QString("%1sxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[22+y*2] = bt_global::icons_cache.getIcon(nomeFile);
		}
	}

	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1dxl%2.png").arg(active_root_of_name).arg(i);
		Icon[5+y*2] = bt_global::icons_cache.getIcon(nomeFile);
		if (inactiveLevel)
		{
			nomeFile = QString("%1dxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[23+y*2] = bt_global::icons_cache.getIcon(nomeFile);
		}
	}
	
	if (!breakIcon.isNull())
	{
		QString break_root_of_name = getNameRoot(breakIcon, ".png");
		nomeFile = QString("%1sx.png").arg(break_root_of_name);
		Icon[44] = bt_global::icons_cache.getIcon(nomeFile);
		nomeFile = QString("%1dx.png").arg(break_root_of_name);
		Icon[45] = bt_global::icons_cache.getIcon(nomeFile);
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

QSize banner::sizeHint() const
{
	if (QLayout *l = layout())
		return l->sizeHint();
	return QSize(banner_width, banner_height);
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
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::BANNERTEXT));
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		SecondaryText->setText(qtestoSecondario);
	}
}

void banner::Draw()
{
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
	address = addr;
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

QString banner::getAddress()
{
	return address;
}

void banner::inizializza(bool forza)
{
	if (linked_sx_page)
		linked_sx_page->inizializza();

	if (linked_dx_page)
		linked_dx_page->inizializza();
}

void banner::openAckRx()
{
	qDebug("openAckRx()");
}

void banner::openNakRx()
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

int banner::getId()
{
	return id;
}

void banner::setId(int i)
{
	id = i;
}

void banner::ambChanged(const QString &, bool, QString)
{
	qDebug("[WARNING] empty slot ambChanged called!");
};

void banner::parentChanged(QWidget *newParent)
{
}

void banner::addAmb(QString)
{
}

QString banner::getNameRoot(QString full_string, QString text_to_strip)
{
	return full_string.mid(0, full_string.lastIndexOf('.'));
}

void banner::sendFrame(QString frame) const
{
	Q_ASSERT_X(client_comandi, "banner::sendFrame", "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void banner::sendInit(QString frame) const
{
	Q_ASSERT_X(client_richieste, "banner::sendInit", "Client richieste not set!");
	QByteArray buf = frame.toAscii();
	client_richieste->ApriInviaFrameChiudi(buf.constData());
}

void banner::setClients(Client *command, Client *request)
{
	client_comandi = command;
	client_richieste = request;
}

void banner::connectDxButton(Page *page)
{
	linked_dx_page = page;
	if (page)
	{
		connect(this, SIGNAL(sxClick()), page, SLOT(showPage()));
		connect(page, SIGNAL(Closed()), SIGNAL(pageClosed()));
	}
}

void banner::connectSxButton(Page *page)
{
	linked_sx_page = page;
	if (page)
	{
		connect(this, SIGNAL(dxClick()), page, SLOT(showPage()));
		connect(page, SIGNAL(Closed()), SIGNAL(pageClosed()));
	}
}

void banner::hideEvent(QHideEvent *event)
{
	if (linked_sx_page && !linked_sx_page->isHidden())
		linked_sx_page->hide();

	if (linked_dx_page && !linked_dx_page->isHidden())
		linked_dx_page->hide();
}



QLabel *BannerNew::createTextLabel(const QRect &size, Qt::Alignment align, const QFont &font)
{
	QLabel *text = new QLabel(this);
	text->setGeometry(size);
	text->setAlignment(align);
	text->setFont(font);
	return text;
}

QLabel *BannerNew::createTextLabel(Qt::Alignment align, const QFont &font)
{
	QLabel *text = new QLabel(this);
	text->setAlignment(align);
	text->setFont(font);
	return text;
}

void BannerNew::connectButtonToPage(BtButton *b, Page *p)
{
	// check both the page and the button, which can be deleted if there's no icon set (see initButton)
	if (p && b)
	{
		linked_pages.append(p);
		connect(b, SIGNAL(clicked()), p, SLOT(showPage()));
		connect(p, SIGNAL(Closed()), SIGNAL(pageClosed()));
	}
}

void BannerNew::hideEvent(QHideEvent *event)
{
	foreach (Page *p, linked_pages)
		if (!p->isHidden())
			p->hide();
}

void BannerNew::inizializza(bool forza)
{
	foreach (Page *p, linked_pages)
		p->inizializza();
}

void BannerNew::initButton(BtButton *btn, const QString &icon)
{
	if (icon.isEmpty())
	{
		Q_ASSERT_X(linked_pages.isEmpty(), "BannerNew::initButton", "Deleting a button with (possibly) linked pages. Maybe you called connectButton() before initBanner()? Aborting.");
		btn->hide();
		btn->disconnect();
		btn->deleteLater();
	}
	else
		btn->setImage(icon);
}

