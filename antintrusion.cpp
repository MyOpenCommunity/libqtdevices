#include "antintrusion.h"
#include "tastiera.h"
#include "bann_antintrusion.h"
#include "sottomenu.h"
#include "global.h" // BTouch
#include "xml_functions.h" // getChildren, getTextChild
#include "allarme.h"

#include <openwebnet.h> // class openwebnet

#include <QDateTime>
#include <QDebug>
#include <QDomNode>
#include <QWidget>

#include <assert.h>


Antintrusion::Antintrusion(QDomNode config_node)
{
	tasti = NULL;
	numRighe = NUM_RIGHE;
	zone = new sottoMenu(this,4,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,2);
	zone->setNavBarMode(4, IMG_PATH "btnparzializzazione.png");
	impianto = new sottoMenu(this,0,MAX_WIDTH, MAX_HEIGHT/numRighe,1);
	connect(zone, SIGNAL(goDx()), this, SLOT(Parzializza()));
	connect(this, SIGNAL(abilitaParz(bool)), this, SLOT(IsParz(bool)));
	curr_alarm = -1;
	setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
	connect(zone  ,SIGNAL(Closed()),this,SIGNAL(Closed()));
	connect(this,SIGNAL(gestFrame(char *)),zone,SIGNAL(gestFrame(char *)));
	connect(this,SIGNAL(gestFrame(char *)),impianto,SIGNAL(gestFrame(char *)));
	connect(this, SIGNAL(openAckRx()), impianto, SIGNAL(openAckRx()));
	connect(this, SIGNAL(openNakRx()), impianto, SIGNAL(openNakRx()));
	connect(impianto, SIGNAL(goDx()), this, SLOT(showAlarms()));
	loadItems(config_node);
}

void Antintrusion::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
		QString descr = getTextChild(item, "descr");

		banner *b;

		if (id == IMPIANTINTRUS)
		{
			b = new impAnti(impianto, img1, img2, img3, img4);
			b->setText(descr);
			b->setId(id);
			impianto->appendBanner(b);
			connect(b, SIGNAL(impiantoInserito()), this, SLOT(doClearAlarms()));
			connect(b, SIGNAL(abilitaParz(bool)), this, SIGNAL(abilitaParz(bool)));
			connect(b, SIGNAL(clearChanged()), this, SIGNAL(clearChanged()));
			connect(this, SIGNAL(partChanged(zonaAnti*)), b, SLOT(partChanged(zonaAnti*)));
			connect(impianto, SIGNAL(openAckRx()), b, SLOT(openAckRx()));
			connect(impianto, SIGNAL(openNakRx()), b, SLOT(openNakRx()));

			testoTecnico = tr("technical");
			testoIntrusione = tr("intrusion");
			testoManom = tr("tamper");
			testoPanic = tr("anti-panic");
			impianto->forceDraw();
		}
		else if (id == ZONANTINTRUS)
		{
			b = new zonaAnti(zone, descr, getTextChild(item, "where"), img1, img2, img3);
			b->setText(descr);
			b->setId(id);
			zone->appendBanner(b);
			connect(this, SIGNAL(abilitaParz(bool)), b, SLOT(abilitaParz(bool)));
			connect(this, SIGNAL(clearChanged()), b, SLOT(clearChanged()));
			connect(b, SIGNAL(partChanged(zonaAnti*)), this, SIGNAL(partChanged(zonaAnti*)));
			// Alhtough looking at the source one would say that more than
			// one "impianto" could be configured, in real life only one
			// impianto can exist
			((impAnti *)impianto->getLast())->setZona((zonaAnti *)b);
			zone->forceDraw();
		}
		else
			assert(!"Type of item not handled on antintrusion page!");
	}
}

Antintrusion::~Antintrusion()
{
	doClearAlarms();
}

void Antintrusion::IsParz(bool ab)
{
	qDebug("antintrusione::IsParz(%d)", ab);
	if (ab)
	{
		connect(zone, SIGNAL(goDx()), this, SLOT(Parzializza()));
		zone->setNavBarMode(4, IMG_PATH "btnparzializzazione.png");
	}
	else
	{
		disconnect(zone, SIGNAL(goDx()), this, SLOT(Parzializza()));
		zone->setNavBarMode(3,"");
	}
	zone->forceDraw();
}

void Antintrusion::Parzializza()
{
	qDebug("antintrusione::Parzializza()");
	int s[MAX_ZONE];
	for (int i = 0; i < MAX_ZONE; i++)
	{
		s[i] = ((impAnti *)impianto->getLast())->getIsActive(i);
	}
	if (tasti)
		delete tasti;
	tasti = new tastiera_con_stati(s, NULL);
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Parz(char*)));
	tasti->setMode(tastiera::HIDDEN);
	tasti->showFullScreen();
}

void Antintrusion::Parz(char* pwd)
{
	qDebug("antintrusione::Parz()");
	if (pwd)
	{
		QString f = QString("*5*50#") + pwd + "#";
		for (int i = 0; i < MAX_ZONE; i++)
			f += ((impAnti *)impianto->getLast())->getIsActive(i) ? "0" : "1";
		f += "*0##";
		qDebug() << "sending part frame" << f;
		BTouch->sendFrame(f);
		((impAnti *)impianto->getLast())->ToSendParz(false);
	}
	impianto->show();
	zone->show();
	connect(&request_timer, SIGNAL(timeout()), this, SLOT(request()));
	request_timer.start(5000);
}

void Antintrusion::testranpo()
{
	QTimer::singleShot(150, this, SLOT(ctrlAllarm()));
}

void Antintrusion:: ctrlAllarm()
{
	qDebug("ctrlAllarm %d", allarmi.size());
	if (!allarmi.isEmpty())
		impianto->getLast()->mostra(banner::BUT1);
	else
		impianto->getLast()->nascondi(banner::BUT1);
	impianto->show();
	zone->show();
}

void Antintrusion::draw()
{
	ctrlAllarm();
	if (impianto)
		impianto->draw();
	if (zone)
		zone->draw();
	if (allarmi.isEmpty())
		return;

	for (int i = 0; i < allarmi.size(); ++i)
		allarmi.at(i)->draw();
}

void Antintrusion::setNumRighe(uchar n)
{
	numRighe = n;
	zone->setNumRighe(n - 1);
	impianto->setNumRighe(1);
	zone->draw();
	impianto->draw();
}

void Antintrusion::inizializza()
{
	zone->inizializza();
	impianto->inizializza();
	connect(((impAnti *)impianto->getLast()), SIGNAL(clearAlarms()), this, SLOT(doClearAlarms()));
}

void Antintrusion::gesFrame(char*frame)
{	
	emit gestFrame(frame);
	openwebnet msg_open;
	char aggiorna;

	aggiorna = 0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"5"))
	{
		if ((!strncmp(msg_open.Extract_cosa(),"12",2)) || (! strncmp(msg_open.Extract_cosa(),"15",2)) || \
			(!strncmp(msg_open.Extract_cosa(),"16",2)) || (! strncmp(msg_open.Extract_cosa(),"17",2)))
		{
			QString descr;
			char zona[3];
			QString tipo = "Z";
			allarme::altype t;

			if  (!strncmp(msg_open.Extract_cosa(),"12",2) && !testoTecnico.isNull())
			{
				descr = testoTecnico;
				t = allarme::TECNICO;
				tipo = "AUX";
			}

			if  (!strncmp(msg_open.Extract_cosa(),"15",2) && !testoIntrusione.isNull())
			{
				descr = testoIntrusione;
				t = allarme::INTRUSIONE;
			}

			if  (!strncmp(msg_open.Extract_cosa(),"16",2) && !testoManom.isNull())
			{
				descr = testoManom;
				t = allarme::MANOMISSIONE;
			}

			if  (!strncmp(msg_open.Extract_cosa(),"17",2) && !testoPanic.isNull())
			{
				descr = testoPanic;
				t = allarme::PANIC;
			}

			// To simulate old behaviour
			descr.truncate(MAX_PATH);

			strcpy(zona,msg_open.Extract_dove());

			QString hhmm = QDateTime::currentDateTime().toString("hh:mm");
			QString ddMM = QDateTime::currentDateTime().toString("dd.MM");
			QString time = QString("\n%1   %2    %3 %4").arg(hhmm).arg(ddMM).arg(tipo).arg(&zona[1]);

			descr += time;
			descr.truncate(2 * MAX_PATH);

			allarmi.append(new allarme(NULL, descr, NULL, ICON_DEL, t));
			// The current alarm is the last alarm inserted
			curr_alarm = allarmi.size() - 1;
			allarme *curr = allarmi.at(curr_alarm);
			connect(curr, SIGNAL(Back()), this, SLOT(closeAlarms()));
			connect(curr, SIGNAL(Next()), this, SLOT(nextAlarm()));
			connect(curr, SIGNAL(Prev()), this, SLOT(prevAlarm()));
			connect(curr, SIGNAL(Delete()), this, SLOT(deleteAlarm()));
			aggiorna = 1;
		}
	}
	if (aggiorna)
	{
		qDebug("ARRIVATO ALLARME!!!!");
		assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
		allarme *curr = allarmi.at(curr_alarm);
		curr->show();
		ctrlAllarm();
	}
}

void Antintrusion::setGeom(int x,int y,int w,int h)
{
	qDebug("antiintrusione::setGeom(%d, %d, %d, %d)", x, y, w, h);
	QWidget::setGeometry(x,y,w,h);
	if (impianto)
		impianto->setGeometry(x,y,w,h/numRighe);
	if (zone)
		zone->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
}

void Antintrusion::setNavBarMode(uchar c)
{
	zone->setNavBarMode(c);
}

void Antintrusion::nextAlarm()
{
	qDebug("antiintrusione::nextAlarm()");
	assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
	allarmi.at(curr_alarm)->hide();

	if (++curr_alarm >= allarmi.size())
		curr_alarm = 0;

	allarmi.at(curr_alarm)->show();
}

void Antintrusion::prevAlarm()
{
	qDebug("antiintrusione::prevAlarm()");
	assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
	allarmi.at(curr_alarm)->hide();

	if (--curr_alarm < 0)
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->show();
}

void Antintrusion::deleteAlarm()
{
	qDebug("antiintrusione::deleteAlarm()");
	assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
	allarmi.at(curr_alarm)->hide();
	allarmi.takeAt(curr_alarm)->deleteLater();

	if (allarmi.isEmpty())
	{
		curr_alarm = -1;
		testranpo();
		return;
	}
	else if (curr_alarm >= allarmi.size())
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->show();
}

void Antintrusion::closeAlarms()
{
	qDebug("antiintrusione::closeAlarms()");
	for (int i = 0; i < allarmi.size(); ++i)
		allarmi.at(i)->hide();

	impianto->show();
	zone->show();
}

void Antintrusion::showAlarms()
{
	qDebug("antiintrusione::showAlarms()");
	if (allarmi.isEmpty())
		return;

	curr_alarm = allarmi.size() - 1;
	impianto->hide();
	zone->hide();
	allarmi.at(curr_alarm)->show();
}

void Antintrusion::doClearAlarms()
{
	qDebug("antiintrusione::doClearAlarms()");
	while (!allarmi.isEmpty())
		delete allarmi.takeFirst();
}

void Antintrusion::hideEvent(QHideEvent *event)
{
	qDebug("antiintrusione::hideEvent()");
	for (int i = 0; i < allarmi.size(); ++i)
		allarmi.at(i)->hide();

	qDebug("Richiesta stato zone");
	for (int i = 1; i <= 8; ++i)
		BTouch->sendFrame(QString("*#5*#%1##").arg(i));
}

void Antintrusion::request()
{
	request_timer.stop();
	disconnect(&request_timer, SIGNAL(timeout()), this, SLOT(request()));
	BTouch->sendInit("*#5*0##");
}
