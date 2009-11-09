#include "version.h"
#include "skinmanager.h"
#include "fontmanager.h" // bt_global::font

#include <openmsg.h>

#include <QStringList>
#include <QVariant> // setProperty
#include <QLabel>


Version::Version()
{
	SkinContext version_cxt(806);

	setProperty("noStyle", true);
	box_text = new QLabel(this);
	box_text->setGeometry(15, 150, 210, 160);
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);
	box_text->setLineWidth(3);
	box_text->setText("");
	box_text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	QLabel *myHome = new QLabel(this);
	myHome->setGeometry(30, 12, 181, 128);
	myHome->setFrameStyle(QFrame::Panel | QFrame::Raised);
	myHome->setPixmap(bt_global::skin->getImage("logo"));

	QLabel *bticino = new QLabel(this);
	bticino->setGeometry(129, 258, 92, 42);
	bticino->setFrameStyle(QFrame::Plain);
	bticino->setPixmap(bt_global::skin->getImage("bticino"));

	box_text->setFont(bt_global::font->get(FontManager::SUBTITLE));
	indDisp = 0;
	subscribe_monitor(13);
	subscribe_monitor(1013);

	vers = 0;
	release = 0;
	build = 0;
	pic_version = 0;
	pic_release = 0;
	pic_build = 0;
	hw_version = 0;
	hw_release = 0;
	hw_build = 0;
}

void Version::manageFrame(OpenMsg &msg)
{
	bool reload = false;

	if (msg.who() == 13)
	{
		if (!strcmp(msg.Extract_grandezza(),"16"))
		{
			vers = atoi(msg.Extract_valori(0));
			release = atoi(msg.Extract_valori(1));
			build = atoi(msg.Extract_valori(2));
			reload = true;
		}
	}
	else // who == 1013
	{
		if (!strcmp(msg.Extract_grandezza(),"6"))
		{
			pic_version = atoi(msg.Extract_valori(0));
			pic_release = atoi(msg.Extract_valori(1));
			pic_build = atoi(msg.Extract_valori(2));
			reload = true;
		}
		if (!strcmp(msg.Extract_grandezza(),"3"))
		{
			hw_version = atoi(msg.Extract_valori(0));
			hw_release = atoi(msg.Extract_valori(1));
			hw_build = atoi(msg.Extract_valori(2));
			reload = true;
			qDebug("presa vers HW = %d.%d.%d",hw_version, hw_release, hw_build);
		}
	}
	if (reload)
	{
		box_text->setFont(bt_global::font->get(FontManager::SMALLTEXT));
		box_text->setIndent(15);
		box_text->setAlignment(Qt::AlignLeft|Qt::AlignTop);
		QStringList text;
		text << QString("art. %1").arg(model) << "";
		text << QString("FIRMWARE: %1.%2.%3").arg(vers).arg(release).arg(build);
		text << QString("PIC REL: %1.%2.%3").arg(pic_version).arg(pic_release).arg(pic_build);
		text << QString("HARDWARE: %1.%2.%3").arg(hw_version).arg(hw_release).arg(hw_build);
		text << QString("T.S. n. %1").arg(indDisp);
		box_text->setText(text.join("\n"));
	}
}

void Version::inizializza()
{
	qDebug("*************** versio::inizializza() ***************");
	sendFrame("*#1013**6##");
	sendFrame("*#1013**3##");
	sendFrame("*#13**16##");
}

void Version::setAddr(int a)
{
	indDisp = a;
}

void Version::setModel(const QString &m)
{
	model = m;
	box_text->setText(model);
}
