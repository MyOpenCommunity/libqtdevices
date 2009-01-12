#include "version.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "openclient.h"
#include "fontmanager.h"

#include <openwebnet.h>

#include <QVariant>
#include <QWidget>
#include <QPixmap>
#include <QLabel>

#include <stdlib.h>


Version::Version()
{
	setProperty("noStyle", true);
	box_text = new QLabel(this);
	box_text->setGeometry(15, 150, 210, 160);
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);
	box_text->setLineWidth(3);
	box_text->setText("");
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);
	box_text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	QLabel *myHome = new QLabel(this);
	myHome->setGeometry(30, 12, 181, 128);
	myHome->setFrameStyle(QFrame::Panel | QFrame::Raised);
	myHome->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	QLabel *bticino = new QLabel(this);
	bticino->setGeometry(129, 258, 92, 42);
	bticino->setFrameStyle(QFrame::Plain);
	bticino->setPixmap(QPixmap(IMG_PATH "bticino.png"));

	QFont aFont;
	FontManager::instance()->getFont(font_versio_datiGen, aFont);
	box_text->setFont(aFont);
	indDisp = 0;
}

void Version::gestFrame(char* frame)
{
	openwebnet msg_open;
	char aggiorna;

	aggiorna = 0;
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"13"))
	{
		if (!strcmp(msg_open.Extract_grandezza(),"16"))
		{
			vers = atoi(msg_open.Extract_valori(0));
			release = atoi(msg_open.Extract_valori(1));
			build = atoi(msg_open.Extract_valori(2));
			aggiorna = 1;
		}
	}
	if (!strcmp(msg_open.Extract_chi(),"1013"))
	{
		if (!strcmp(msg_open.Extract_grandezza(),"6"))
		{
			pic_version = atoi(msg_open.Extract_valori(0));
			pic_release = atoi(msg_open.Extract_valori(1));
			pic_build = atoi(msg_open.Extract_valori(2));
			aggiorna = 1;
		}
		if (!strcmp(msg_open.Extract_grandezza(),"3"))
		{
			hw_version = atoi(msg_open.Extract_valori(0));
			hw_release = atoi(msg_open.Extract_valori(1));
			hw_build = atoi(msg_open.Extract_valori(2));
			aggiorna = 1;
			qDebug("presa vers HW = %d.%d.%d",hw_version, hw_release, hw_build);
		}
	}
	if (aggiorna)
	{
		char scritta[100];
		QFont aFont;
		FontManager::instance()->getFont(font_versio_datiGenFw, aFont);

		box_text->setFont(aFont);
		box_text->setIndent(15);
		box_text->setAlignment(Qt::AlignLeft|Qt::AlignTop);
		QByteArray buf = model.toAscii();
		sprintf(scritta, "art. %s\n\nFIRMWARE: %d.%d.%d\nPIC REL: %d.%d.%d\nHARDWARE: %d.%d.%d\nT.S. n. %d",
			buf.constData(), vers, release, build, pic_version, pic_release, pic_build, hw_version, hw_release, hw_build, indDisp);

		box_text->setText(scritta); // FIXME da tradurre??
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

void Version::setModel(const QString & m)
{
	model = m;
	box_text->setText(model);
}
