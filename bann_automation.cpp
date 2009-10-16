#include "bann_automation.h"
#include "generic_functions.h" // createMsgOpen
#include "device_status.h"
#include "btbutton.h"
#include "device.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "automation_device.h" // PPTStatDevice
#include "xml_functions.h" // getTextChild
#include "automation_device.h"
#include "icondispatcher.h"
#include "fontmanager.h"

#include <QTimer>
#include <QDebug>
#include <QEvent>
#include <QVariant>
#include <QLabel>

#include <openwebnet.h> // class openwebnet

#define BUT_DIM     60


BannOpenClose::BannOpenClose(QWidget *parent) :
	banner(parent)
{
	const int BANONOFF_BUT_DIM = 60;
	const int BUTONOFF_ICON_DIM_Y = 60;
	const int BUTONOFF_ICON_DIM_X = 120;

	left_button = new BtButton(this);
	left_button->setGeometry(0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	right_button = new BtButton(this);
	right_button->setGeometry(banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	text = new QLabel(this);
	text->setGeometry(0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	text->setFont(bt_global::font->get(FontManager::TEXT));

	center_icon = new QLabel(this);
	center_icon->setGeometry(BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}

void BannOpenClose::loadIcons(QString _left, QString _center, QString _right, QString _alternate)
{
	right = _right;
	left =_left;
	center = _center;
	alternate = _alternate;
}

void BannOpenClose::setState(States new_state)
{
	switch (new_state)
	{
	case STOP:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
		left_button->setImage(left);
		right_button->setImage(right);
		break;
	case OPENING:
	{
		int pos = center.indexOf(".");
		QString img = center.left(pos) + "o" + center.mid(pos);
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(img));

		right_button->setImage(alternate);
		left_button->setImage(left);
	}
		break;
	case CLOSING:
	{
		int pos = center.indexOf(".");
		QString img = center.left(pos) + "c" + center.mid(pos);
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(img));

		right_button->setImage(right);
		left_button->setImage(alternate);
	}
		break;
	}
}

void BannOpenClose::setPrimaryText(QString str)
{
	text->setText(str);
}



InterblockedActuator::InterblockedActuator(QWidget *parent, const QDomNode &config_node)
	: BannOpenClose(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new AutomationDevice(where, PULL));

	loadIcons(bt_global::skin->getImage("close"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("open"), bt_global::skin->getImage("stop"));
	setPrimaryText(getTextChild(config_node, "descr"));
	setState(STOP);

	connect(right_button, SIGNAL(clicked()), SLOT(sendGoUp()));
	connect(left_button, SIGNAL(clicked()), SLOT(sendGoDown()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void InterblockedActuator::inizializza(bool forza)
{
	dev->requestStatus();
}

void InterblockedActuator::sendGoUp()
{
	dev->goUp();
}

void InterblockedActuator::sendGoDown()
{
	dev->goDown();
}

void InterblockedActuator::sendStop()
{
	dev->stop();
}

void InterblockedActuator::status_changed(const StatusList &sl)
{
	qWarning() << "InterblockedActuator::status_changed";
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case AutomationDevice::DIM_UP:
			setState(OPENING);
			right_button->enable();
			left_button->disable();
			break;
		case AutomationDevice::DIM_DOWN:
			setState(CLOSING);
			right_button->disable();
			left_button->enable();
			break;
		case AutomationDevice::DIM_STOP:
			setState(STOP);
			right_button->enable();
			left_button->enable();
			break;
		}
		++it;
	}
}



#if 1
automCancAttuatVC::automCancAttuatVC(QWidget *parent, QString where, QString IconaSx, QString IconaDx)
	: bannPuls(parent)
{
	qDebug("automCancAttuatVC::automCancAttuatVC()");
	SetIcons(IconaSx, QString(), IconaDx);
	setAddress(where);
	connect(this, SIGNAL(pressed()), SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void automCancAttuatVC::Attiva()
{
	dev->sendFrame(createMsgOpen("6", "10", getAddress()));
}


automCancAttuatIll::automCancAttuatIll(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString t)
	: bannPuls(parent)
{
	qDebug("automCancAttuatIll::automCancAttuatIll()");
	SetIcons(IconaSx, QString(), IconaDx);
	setAddress(where);
	connect(this, SIGNAL(click()), SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	time = !t.isEmpty() ? t : QString("0*0*18");
	Draw();
}

void automCancAttuatIll::Attiva()
{
	openwebnet msg_open;
	char val[100];
	char *v[] = { val , };

	QByteArray buf = time.toAscii();
	strncpy(val, buf.constData(), sizeof(val));

	msg_open.CreateNullMsgOpen();
	msg_open.CreateWrDimensionMsgOpen("1", getAddress().toAscii().data(), "2", v, 1);
	dev->sendFrame(msg_open.frame_open);
}


attuatAutomIntSic::attuatAutomIntSic(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon, QString StopIcon)
	: bannOnOff(parent)
{
	int pos = icon.indexOf(".");
	if (pos != -1)
	{
		QString img1 = icon.left(pos) + "o" + icon.mid(pos);
		QString img2 = icon.left(pos) + "c" + icon.mid(pos);
		SetIcons(IconaSx, IconaDx, icon, img1, img2);
	}
	else
		qWarning() << "Cannot find dot on image " << icon;

	icon_sx = IconaSx;
	icon_dx = IconaDx;
	icon_stop = StopIcon;

	setAddress(where);
	connect(this,SIGNAL(sxPressed()),this,SLOT(upPres()));
	connect(this,SIGNAL(dxPressed()),this,SLOT(doPres()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(upRil()));
	connect(this,SIGNAL(dxReleased()),this,SLOT(doRil()));

	uprunning = dorunning = 0;
	dev = bt_global::devices_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomIntSic::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AUTOM:
		{
			qDebug("Autom status variation");
			ds->read(device_status_autom::STAT_INDEX, curr_status);
			int v = curr_status.get_val();
			qDebug("status = %d", v);
			switch(v)
			{
			case 0:
				if (isActive())
				{
					impostaAttivo(0);
					uprunning = dorunning = 0;
					aggiorna = true;
					SetIcons(0, icon_sx);
					SetIcons(1, icon_dx);
					sxButton->enable();
					dxButton->enable();
				}
				break;
			case 1:
				if (!isActive() || (isActive() == 2))
				{
					impostaAttivo(1);
					dorunning = 0;
					uprunning = 1;
					aggiorna = true;
					SetIcons(0, icon_stop);
					SetIcons(1, icon_dx);
					dxButton->disable();
					sxButton->enable();
				}
				break;
			case 2:
				if (!isActive() || (isActive() == 1))
				{
					impostaAttivo(2);
					dorunning = 1;
					uprunning = 0;
					aggiorna = true;
					SetIcons(0, icon_sx);
					SetIcons(1, icon_stop);
					sxButton->disable();
					dxButton->enable();
				}
				break;
			default:
				qDebug("Unknown status in autom. message");
				break;
			}
		}
		default:
			qDebug("Unknown device status type");
			break;
		}
	}

	if (aggiorna)
	{
		Draw();
		sxButton->setStatus(sxButton->isDown());
		dxButton->setStatus(dxButton->isDown());
	}

}

void attuatAutomIntSic::upPres()
{
	if (!dorunning && !isActive())
		dev->sendFrame(createMsgOpen("2", "1", getAddress()));
}

void attuatAutomIntSic::doPres()
{
	if (!uprunning && !isActive())
		dev->sendFrame(createMsgOpen("2", "2", getAddress()));
}

void attuatAutomIntSic::upRil()
{
	if (!dorunning)
		QTimer::singleShot(500, this, SLOT(sendStop()));
}

void attuatAutomIntSic::doRil()
{
	if (!uprunning)
		QTimer::singleShot(500, this, SLOT(sendStop()));
}

void attuatAutomIntSic::sendStop()
{
	dev->sendFrame(createMsgOpen("2", "0", getAddress()));
}

void attuatAutomIntSic::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "##");
}


attuatAutomInt::attuatAutomInt(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon, QString StopIcon)
	: bannOnOff(parent)
{

	int pos = icon.indexOf(".");
	if (pos != -1)
	{
		QString img1 = icon.left(pos) + "o" + icon.mid(pos);
		QString img2 = icon.left(pos) + "c" + icon.mid(pos);
		SetIcons(IconaSx, IconaDx, icon, img1, img2);
	}
	else
		qWarning() << "Cannot find dot on image " << icon;

	icon_sx = IconaSx;
	icon_dx = IconaDx;
	icon_stop = StopIcon;

	setAddress(where);
	connect(this,SIGNAL(sxClick()),this,SLOT(analizzaUp()));
	connect(this,SIGNAL(dxClick()),this,SLOT(analizzaDown()));

	uprunning = dorunning = 0;
	dev = bt_global::devices_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomInt::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AUTOM:
		{
			qDebug("Autom status variation");
			ds->read(device_status_autom::STAT_INDEX, curr_status);
			int v = curr_status.get_val();
			qDebug("status = %d", v);
			qDebug("isActive = %d", isActive());
			switch (v)
			{
			case 0:
				if (isActive())
				{
					impostaAttivo(0);
					uprunning = dorunning = 0;
					aggiorna = 1;
					SetIcons(0, icon_sx);
					SetIcons(1, icon_dx);
					sxButton->enable();
					dxButton->enable();
				}
				break;
			case 1:
				if (!isActive() || (isActive() == 2))
				{
					impostaAttivo(1);
					dorunning = 0;
					uprunning = 1;
					aggiorna = 1;
					SetIcons(0, icon_stop);
					SetIcons(1, icon_dx);
					sxButton->enable();
					dxButton->disable();
				}
				break;
			case 2:
				if (!isActive() || (isActive() == 1))
				{
					impostaAttivo(2);
					dorunning = 1;
					uprunning = 0;
					aggiorna = 1;
					SetIcons(0, icon_sx);
					SetIcons(1, icon_stop);
					sxButton->disable();
					dxButton->enable();
				}
				break;
			default:
				qDebug("Unknown status in autom. message");
				break;
			}
			break;
		}
		default:
			qDebug("Unknown device status type");
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutomInt::analizzaUp()
{
	if (!dorunning)
		dev->sendFrame(createMsgOpen("2", uprunning ? "0" : "1", getAddress()));
}

void attuatAutomInt::analizzaDown()
{
	if (!uprunning)
		dev->sendFrame(createMsgOpen("2", dorunning ? "0" : "2", getAddress()));
}

void attuatAutomInt::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "##");
}


grAttuatInt::grAttuatInt(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString icon)
	: bann3But(parent)
{
	SetIcons(IconaDx, IconaSx, QString(), icon);
	elencoDisp = addresses;
	connect(this,SIGNAL(dxClick()), SLOT(Alza()));
	connect(this,SIGNAL(sxClick()), SLOT(Abbassa()));
	connect(this,SIGNAL(centerClick()), SLOT(Ferma()));
}

void grAttuatInt::sendAllFrames(QString msg)
{
	for (int i = 0; i < elencoDisp.size();++i)
		sendFrame(createMsgOpen("2", msg, elencoDisp.at(i)));
}

void grAttuatInt::Alza()
{
	sendAllFrames("1");
}

void grAttuatInt::Abbassa()
{
	sendAllFrames("2");
}

void grAttuatInt::Ferma()
{
	sendAllFrames("0");
}

#endif


PPTStat::PPTStat(QWidget *parent, QString where, int cid) : banner(parent)
{
	dev = bt_global::add_device_to_cache(new PPTStatDevice(where));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));

	// This banner shows only an icon in central position and a text below.
	addItem(ICON, (banner_width - BUT_DIM * 2) / 2, 0, BUT_DIM*2 ,BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height - BUT_DIM);

	SkinContext context(cid);
	img_open = bt_global::skin->getImage("pptstat_open");
	img_close = bt_global::skin->getImage("pptstat_close");

	if (!img_open.isEmpty())
		SetIcons(2, img_open);
	Draw();
}

void PPTStat::status_changed(const StatusList &status_list)
{
	bool st = status_list[PPTStatDevice::DIM_STATUS].toBool();
	SetIcons(2, st ? img_close : img_open);
	Draw();
}

void PPTStat::inizializza(bool forza)
{
	dev->requestStatus();
}

