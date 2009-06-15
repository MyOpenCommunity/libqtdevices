#include "version.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "fontmanager.h" // bt_global::font

#include <openwebnet.h>

#include <QStringList>
#include <QVariant> // setProperty
#include <QWidget>
#include <QPixmap>
#include <QLabel>


Version::Version()
{
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
	myHome->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	QLabel *bticino = new QLabel(this);
	bticino->setGeometry(129, 258, 92, 42);
	bticino->setFrameStyle(QFrame::Plain);
	bticino->setPixmap(QPixmap(IMG_PATH "bticino.png"));

	box_text->setFont(bt_global::font->get(FontManager::SUBTITLE));
	indDisp = 0;
}

void Version::gestFrame(char* frame)
{
	openwebnet msg_open;

	bool reload = false;
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"13"))
	{
		if (!strcmp(msg_open.Extract_grandezza(),"16"))
		{
			vers = atoi(msg_open.Extract_valori(0));
			release = atoi(msg_open.Extract_valori(1));
			build = atoi(msg_open.Extract_valori(2));
			reload = true;
		}
	}
	if (!strcmp(msg_open.Extract_chi(),"1013"))
	{
		if (!strcmp(msg_open.Extract_grandezza(),"6"))
		{
			pic_version = atoi(msg_open.Extract_valori(0));
			pic_release = atoi(msg_open.Extract_valori(1));
			pic_build = atoi(msg_open.Extract_valori(2));
			reload = true;
		}
		if (!strcmp(msg_open.Extract_grandezza(),"3"))
		{
			hw_version = atoi(msg_open.Extract_valori(0));
			hw_release = atoi(msg_open.Extract_valori(1));
			hw_build = atoi(msg_open.Extract_valori(2));
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
