/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**antintrusione.cpp
**
**Sottomenù antiintrusione
**
****************************************************************/
#include "antintrusione.h"
#include "tastiera.h"
#include "bann_antintrusione.h"
#include "sottomenu.h"

#include <openwebnet.h> // class openwebnet

#include <QDateTime>
#include <QCursor>
#include <QTimer>

#include <assert.h>

extern unsigned char tipoData;

antintrusione::antintrusione(QWidget *parent, const char *name) : QWidget(parent)
{

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(Qt::BlankCursor));
#endif
	tasti = NULL;
	numRighe = NUM_RIGHE;
	zone = new sottoMenu(this,"Zone",4,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,2);
	zone->setNavBarMode(4, IMG_PATH "btnparzializzazione.png");
	impianto = new sottoMenu(this,"impianto",0,MAX_WIDTH, MAX_HEIGHT/numRighe,1);
	connect(zone, SIGNAL(goDx()), this, SLOT(Parzializza()));
	connect(this, SIGNAL(abilitaParz(bool)), this, SLOT(IsParz(bool)));
	curr_alarm = -1;
	setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
	connect(zone  ,SIGNAL(Closed()),this,SIGNAL(Closed()));
	connect(this,SIGNAL(gestFrame(char *)),zone,SIGNAL(gestFrame(char *)));
	connect(this,SIGNAL(gestFrame(char *)),impianto,SIGNAL(gestFrame(char *)));
	connect(zone,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(zone,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
	connect(impianto,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(impianto,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
	connect(this, SIGNAL(openAckRx()), impianto, SIGNAL(openAckRx()));
	connect(this, SIGNAL(openNakRx()), impianto, SIGNAL(openNakRx()));
	connect(impianto, SIGNAL(goDx()), this, SLOT(showAlarms()));

	connect(this,SIGNAL(freezed(bool)),zone,SLOT(freezed(bool)));
	connect(this,SIGNAL(freezed(bool)),impianto,SLOT(freezed(bool)));
}

antintrusione::~antintrusione()
{
	doClearAlarms();
}

void antintrusione::IsParz(bool ab)
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

void antintrusione::Parzializza()
{
	qDebug("antintrusione::Parzializza()");
	int s[MAX_ZONE];
	for (int i = 0; i < MAX_ZONE; i++)
	{
		s[i] = ((impAnti *)impianto->getLast())->getIsActive(i);
	}
	if (tasti)
		delete tasti;
	tasti = new tastiera_con_stati(s, NULL, "");
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Parz(char*)));
	tasti->setMode(tastiera::HIDDEN);
	tasti->showTastiera();
}

void antintrusione::Parz(char* pwd)
{
	openwebnet msg_open;
	char pippo[50];

	qDebug("antintrusione::Parz()");
	if (!pwd)
		goto end;

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*50#");
	strcat(pippo,pwd);
	strcat(pippo,"#");
	for (int i = 0; i < MAX_ZONE; i++)
		strcat(pippo, ((impAnti *)impianto->getLast())->getIsActive(i) ? "0" : "1");
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	qDebug("sending part frame %s", pippo);
	emit sendFrame(msg_open.frame_open);
	((impAnti *)impianto->getLast())->ToSendParz(false);
end:
	impianto->show();
	zone->show();
}

void antintrusione::testranpo()
{
	QTimer::singleShot(150, this, SLOT(ctrlAllarm()));
}

void antintrusione:: ctrlAllarm()
{
	qDebug("ctrlAllarm %d", allarmi.size());
	if (!allarmi.isEmpty())
		impianto->getLast()->mostra(banner::BUT1);
	else
		impianto->getLast()->nascondi(banner::BUT1);
	impianto->show();
	zone->show();
}

void antintrusione::draw()
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

int antintrusione::addItemU(char tipo, const QString & qdescrizione, void* indirizzo,
	QList<QString*> &icon_names, int periodo, int numFrame)
{
	if (tipo == IMPIANTINTRUS)
	{
		impianto->addItemU(tipo, qdescrizione, indirizzo, icon_names);
		connect(impianto->getLast(), SIGNAL(impiantoInserito()), this,SLOT(doClearAlarms()));
		connect(impianto->getLast(), SIGNAL(abilitaParz(bool)),this, SIGNAL(abilitaParz(bool)));
		connect(impianto->getLast(), SIGNAL(clearChanged()),this, SIGNAL(clearChanged()));
		connect(this, SIGNAL(partChanged(zonaAnti*)), impianto->getLast(),SLOT(partChanged(zonaAnti*)));
		connect(impianto, SIGNAL(openAckRx()), impianto->getLast(),SLOT(openAckRx()));
		connect(impianto, SIGNAL(openNakRx()), impianto->getLast(),SLOT(openNakRx()));

		testoTecnico = tr("technical");
		testoIntrusione = tr("intrusion");
		testoManom = tr("tamper");
		testoPanic = tr("anti-panic");

		// To simulate old behaviour
		testoTecnico.truncate(MAX_PATH);
		testoIntrusione.truncate(MAX_PATH);
		testoManom.truncate(MAX_PATH);
		testoPanic.truncate(MAX_PATH);
		impianto->forceDraw();
	}
	else if (tipo == ZONANTINTRUS)
	{
		zone->addItemU(tipo, qdescrizione, indirizzo, icon_names);
		connect(this, SIGNAL(abilitaParz(bool)), zone->getLast(), SLOT(abilitaParz(bool)));
		connect(this, SIGNAL(clearChanged()), zone->getLast(),SLOT(clearChanged()));
		connect(zone->getLast(), SIGNAL(partChanged(zonaAnti*)),this, SIGNAL(partChanged(zonaAnti*)));
		// Alhtough looking at the source one would say that more than
		// one "impianto" could be configured, in real life only one
		// impianto can exist
		((impAnti *)impianto->getLast())->setZona((zonaAnti *)zone->getLast());
		zone->forceDraw();
	}
	return 1;
}

void antintrusione::setNumRighe(uchar n)
{
	numRighe = n;
	zone->setNumRighe(n - 1);
	impianto->setNumRighe(1);
	zone->draw();
	impianto->draw();
}

void antintrusione::inizializza()
{
	zone->inizializza();
	impianto->inizializza();
	connect(((impAnti *)impianto->getLast()), SIGNAL(clearAlarms()), this, SLOT(doClearAlarms()));
}

void antintrusione::gesFrame(char*frame)
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
			char *tipo = "Z";
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

			strcpy(&zona[0],msg_open.Extract_dove());

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
			connect(this, SIGNAL(freezed(bool)), curr, SLOT(freezed(bool)));
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

void antintrusione::setGeom(int x,int y,int w,int h)
{
	qDebug("antiintrusione::setGeom(%d, %d, %d, %d)", x, y, w, h);
	QWidget::setGeometry(x,y,w,h);
	if (impianto)
		impianto->setGeometry(x,y,w,h/numRighe);
	if (zone)
		zone->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
}

void antintrusione::setNavBarMode(uchar c)
{
	zone->setNavBarMode(c);
}

void antintrusione::nextAlarm()
{
	qDebug("antiintrusione::nextAlarm()");
	assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
	allarmi.at(curr_alarm)->hide();

	if (++curr_alarm >= allarmi.size())
		curr_alarm = 0;

	allarmi.at(curr_alarm)->show();
}

void antintrusione::prevAlarm()
{
	qDebug("antiintrusione::prevAlarm()");
	assert(curr_alarm > 0 && curr_alarm < allarmi.size() && "Current alarm index out of range!");
	allarmi.at(curr_alarm)->hide();

	if (--curr_alarm < 0)
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->show();
}

void antintrusione::deleteAlarm()
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

void antintrusione::closeAlarms()
{
	qDebug("antiintrusione::closeAlarms()");
	for (int i = 0; i < allarmi.size(); ++i)
		allarmi.at(i)->hide();

	impianto->show();
	zone->show();
}

void antintrusione::showAlarms()
{
	qDebug("antiintrusione::showAlarms()");
	if (allarmi.isEmpty())
		return;

	curr_alarm = allarmi.size() - 1;
	impianto->hide();
	zone->hide();
	allarmi.at(curr_alarm)->show();
}

void antintrusione::doClearAlarms()
{
	qDebug("antiintrusione::doClearAlarms()");
	while (!allarmi.isEmpty())
		delete allarmi.takeFirst();
}

void antintrusione::show()
{
	qDebug("antintrusione::show()");
	impianto->show();
	zone->show();
	QWidget::show();
}

void antintrusione::hide()
{
	qDebug("antiintrusione::hide()");
	QWidget::hide();
	impianto->hide();
	zone->hide();
	for (int i = 0; i < allarmi.size(); ++i)
		allarmi.at(i)->hide();

	qDebug("Richiesta stato zone");
	// TODO: fare un ciclo!!
	emit sendFrame("*#5*#1##");
	emit sendFrame("*#5*#2##");
	emit sendFrame("*#5*#3##");
	emit sendFrame("*#5*#4##");
	emit sendFrame("*#5*#5##");
	emit sendFrame("*#5*#6##");
	emit sendFrame("*#5*#7##");
	emit sendFrame("*#5*#8##");
}
