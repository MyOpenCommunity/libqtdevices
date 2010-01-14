#include "antintrusion.h"
#include "keypad.h"
#include "bann_antintrusion.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "alarmpage.h"
#include "btmain.h" // bt_global::btmain
#include "bannercontent.h"
#include "navigation_bar.h"
#include "btbutton.h"
#include "main.h"
#include "skinmanager.h"

#include <openmsg.h>

#include <QDateTime>
#include <QDebug>
#include <QDomNode>
#include <QWidget>
#include <QVBoxLayout>


Antintrusion::Antintrusion(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	tasti = NULL;
	impianto = 0;
	previous_page = 0;

	testoTecnico = tr("technical");
	testoIntrusione = tr("intrusion");
	testoManom = tr("tamper");
	testoPanic = tr("anti-panic");

	top_widget = new QWidget;

	// TODO: we introduce a double dependency to customize the image of the forward
	// button and to obtain a reference of it (to show/hide the button).
	// We can do better!
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("partial"));
	buildPage(new BannerContent, nav_bar, QString(), top_widget);
	forward_button = nav_bar->forward_button;

	connect(this, SIGNAL(abilitaParz(bool)), SLOT(IsParz(bool)));
	connect(this, SIGNAL(forwardClick()), SLOT(Parzializza()));
	curr_alarm = -1;
	loadItems(config_node);
	Q_ASSERT_X(impianto, "Antintrusion::Antintrusion", "Impianto not found on the configuration file!");
	t = new QTimer(this);
	t->setSingleShot(true);
	connect(t, SIGNAL(timeout()), SLOT(ctrlAllarm()));
	connect(this, SIGNAL(Closed()), SLOT(requestZoneStatus()));
	connect(bt_global::btmain, SIGNAL(startscreensaver(Page*)),
			SLOT(requestStatusIfCurrentWidget(Page*)));
	subscribe_monitor(5);
}

void Antintrusion::createImpianto(const QString &descr)
{
	// We have to use a layout for the top_widget, in order to define an appropriate
	// sizeHint (needed by the main layout added to the Page)
	// An alternative is to define a custom widget that rimplement the sizeHint method.
	QVBoxLayout *l = new QVBoxLayout(top_widget);
	l->setSpacing(0);
	int mleft, mright;
	page_content->layout()->getContentsMargins(&mleft, NULL, &mright, NULL);
	l->setContentsMargins(mleft, 0, mright, 0);

	impianto = new impAnti(top_widget,
			       bt_global::skin->getImage("on"),
			       bt_global::skin->getImage("off"),
			       bt_global::skin->getImage("info"),
			       bt_global::skin->getImage("alarm_state"));
	impianto->setText(descr);
	impianto->Draw();
	impianto->setId(IMPIANTINTRUS); // can probably be removed
	top_widget->layout()->addWidget(impianto);

	connect(impianto, SIGNAL(impiantoInserito()), SLOT(plantInserted()));
	connect(impianto, SIGNAL(abilitaParz(bool)), SIGNAL(abilitaParz(bool)));
	connect(impianto, SIGNAL(clearChanged()), SIGNAL(clearChanged()));
	connect(impianto, SIGNAL(pageClosed()), SLOT(showPage()));
	connect(impianto, SIGNAL(sxClick()), SLOT(showAlarms()));

	connect(this, SIGNAL(partChanged(zonaAnti*)), impianto, SLOT(partChanged(zonaAnti*)));
	connect(this, SIGNAL(openAckRx()), impianto, SLOT(openAckRx()));
	connect(this, SIGNAL(openNakRx()), impianto, SLOT(openNakRx()));
}

void Antintrusion::loadItems(const QDomNode &config_node)
{
#ifndef CONFIG_BTOUCH
	createImpianto("");
#endif

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");

		banner *b;

#ifdef CONFIG_BTOUCH
		if (id == IMPIANTINTRUS)
			createImpianto(descr);
		else
#endif
		if (id == ZONANTINTRUS)
		{
			b = new zonaAnti(this, descr, getTextChild(item, "where"),
					 bt_global::skin->getImage("zone"),
					 bt_global::skin->getImage("alarm_off"),
					 bt_global::skin->getImage("alarm_on"));
			b->setText(descr);
			b->setId(id);
			b->Draw();
			page_content->appendBanner(b);
			connect(this, SIGNAL(abilitaParz(bool)), b, SLOT(abilitaParz(bool)));
			connect(this, SIGNAL(clearChanged()), b, SLOT(clearChanged()));
			connect(b, SIGNAL(partChanged(zonaAnti*)), SIGNAL(partChanged(zonaAnti*)));
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
			// We assume that the antintrusion impianto came before all the zones
			Q_ASSERT_X(impianto, "Antintrusion::loadItems", "Found a zone before the impianto!");
			impianto->setZona((zonaAnti *)b);
		}
		else
			Q_ASSERT_X(false, "Antintrusion::loadItems", qPrintable(QString("Type of item %1 not handled!").arg(id)));
	}
}

void Antintrusion::plantInserted()
{
	clearAlarms();
	if (previous_page)
	{
		previous_page->showPage();
		// this is the only place where we call a showPage() and we need to restore the screensaver
		bt_global::btmain->showScreensaverIfNeeded();
		previous_page = 0;
	}
}

Antintrusion::~Antintrusion()
{
	clearAlarms();
}

void Antintrusion::IsParz(bool ab)
{
	qDebug("antintrusione::IsParz(%d)", ab);

	if (ab)
	{
		connect(this, SIGNAL(forwardClick()), SLOT(Parzializza()));
		forward_button->show();
	}
	else
	{
		disconnect(this, SIGNAL(forwardClick()), this, SLOT(Parzializza()));
		forward_button->hide();
	}

}

void Antintrusion::Parzializza()
{
	qDebug("antintrusione::Parzializza()");
	int s[MAX_ZONE];
	for (int i = 0; i < MAX_ZONE; i++)
		s[i] = impianto->getIsActive(i);

	if (tasti)
		delete tasti;
	tasti = new KeypadWithState(s);
	connect(tasti, SIGNAL(Closed()), SLOT(Parz()));
	connect(tasti, SIGNAL(Closed()), SLOT(showPage()));
	tasti->setMode(Keypad::HIDDEN);
	tasti->showPage();
}

void Antintrusion::Parz()
{
	qDebug("antintrusione::Parz()");
	QString pwd = tasti->getText();
	if (!pwd.isEmpty())
	{
		QString f = "*5*50#" + pwd + "#";
		for (int i = 0; i < MAX_ZONE; i++)
			f += impianto->getIsActive(i) ? "0" : "1";
		f += "*0##";
		qDebug() << "sending part frame" << f;
		sendFrame(f);
		impianto->ToSendParz(false);
	}
	connect(&request_timer, SIGNAL(timeout()), SLOT(request()));
	request_timer.start(5000);
}

void Antintrusion::testranpo()
{
	t->start(150);
}

void Antintrusion:: ctrlAllarm()
{
	qDebug("ctrlAllarm %d", allarmi.size());
	if (!allarmi.isEmpty())
		impianto->mostra(banner::BUT1);
	else
		impianto->nascondi(banner::BUT1);
}

void Antintrusion::draw()
{
	ctrlAllarm();
	if (allarmi.isEmpty())
		return;
}

void Antintrusion::inizializza()
{
	BannerPage::inizializza();

	impianto->inizializza();
	connect(impianto, SIGNAL(clearAlarms()), this, SLOT(doClearAlarms()));
}

void Antintrusion::manageFrame(OpenMsg &msg)
{
	bool aggiorna = false;


	if ((!strncmp(msg.Extract_cosa(),"12",2)) || (! strncmp(msg.Extract_cosa(),"15",2)) || \
		(!strncmp(msg.Extract_cosa(),"16",2)) || (! strncmp(msg.Extract_cosa(),"17",2)))
	{
		QString descr;
		char zona[3];
		QString tipo = "Z";
		AlarmPage::altype t;

		if  (!strncmp(msg.Extract_cosa(),"12",2) && !testoTecnico.isNull())
		{
			descr = testoTecnico;
			t = AlarmPage::TECNICO;
			tipo = "AUX";
		}

		if  (!strncmp(msg.Extract_cosa(),"15",2) && !testoIntrusione.isNull())
		{
			descr = testoIntrusione;
			t = AlarmPage::INTRUSIONE;
		}

		if  (!strncmp(msg.Extract_cosa(),"16",2) && !testoManom.isNull())
		{
			descr = testoManom;
			t = AlarmPage::MANOMISSIONE;
		}

		if  (!strncmp(msg.Extract_cosa(),"17",2) && !testoPanic.isNull())
		{
			descr = testoPanic;
			t = AlarmPage::PANIC;
		}

		// To simulate old behaviour
		descr.truncate(MAX_PATH);

		strcpy(zona,msg.Extract_dove());

		QString hhmm = QDateTime::currentDateTime().toString("hh:mm");
		QString ddMM = QDateTime::currentDateTime().toString("dd.MM");
		QString time = QString("\n%1   %2    %3 %4").arg(hhmm).arg(ddMM).arg(tipo).arg(&zona[1]);

		descr += time;
		descr.truncate(2 * MAX_PATH);

		allarmi.append(new AlarmPage(descr, NULL, bt_global::skin->getImage("alarm_del"), t));
		// The current alarm is the last alarm inserted
		curr_alarm = allarmi.size() - 1;
		AlarmPage *curr = allarmi.at(curr_alarm);
		connect(curr, SIGNAL(Closed()), SLOT(closeAlarms()));
		connect(curr, SIGNAL(Next()), SLOT(nextAlarm()));
		connect(curr, SIGNAL(Prev()), SLOT(prevAlarm()));
		connect(curr, SIGNAL(Delete()), SLOT(deleteAlarm()));
		aggiorna = true;
	}

	if (aggiorna)
	{
		qDebug("ARRIVATO ALLARME!!!!");
		Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::gesFrame",
			qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));
		AlarmPage *curr = allarmi.at(curr_alarm);

		// if the alarm arrive during the screensaver, we want to turn back to the alarm when the screensaver exit
		if (bt_global::btmain->screenSaverRunning())
		{
			if (!previous_page)
				previous_page = bt_global::btmain->getPreviousPage();
		}
		else
		{
			if (!previous_page)
				previous_page = currentPage();
		}

		curr->showPage();
		ctrlAllarm();
	}
}

void Antintrusion::closeAlarms()
{
	// An alarm can arrive in every moment, so when closing the alarm page it is
	// required to move back to the original page.
	if (previous_page)
	{
		previous_page->showPage();
		previous_page = 0;
	}
	else
		showPage();
}

void Antintrusion::nextAlarm()
{
	qDebug("antiintrusione::nextAlarm()");
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::nextAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));

	if (++curr_alarm >= allarmi.size())
		curr_alarm = 0;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::prevAlarm()
{
	qDebug("antiintrusione::prevAlarm()");
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::prevAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));
	if (--curr_alarm < 0)
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::deleteAlarm()
{
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::deleteAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));
	allarmi.takeAt(curr_alarm)->deleteLater();

	if (allarmi.isEmpty())
	{
		curr_alarm = -1;
		closeAlarms();
		testranpo();
		return;
	}
	else if (curr_alarm >= allarmi.size())
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::showAlarms()
{
	qDebug("antiintrusione::showAlarms()");
	if (allarmi.isEmpty())
		return;

	curr_alarm = allarmi.size() - 1;
	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::doClearAlarms()
{
	clearAlarms();
	previous_page = 0;
}

void Antintrusion::clearAlarms()
{
	qDebug("antiintrusione::clearAlarms()");
	while (!allarmi.isEmpty())
	{
		AlarmPage *a = allarmi.takeFirst();
		a->disconnect();
		a->deleteLater();
	}
}

void Antintrusion::requestZoneStatus()
{
	qDebug("Request zone status");
	for (int i = 1; i <= 8; ++i)
		sendFrame(QString("*#5*#%1##").arg(i));
}

void Antintrusion::request()
{
	request_timer.stop();
	disconnect(&request_timer, SIGNAL(timeout()), this, SLOT(request()));
	sendInit("*#5*0##");
}

void Antintrusion::requestStatusIfCurrentWidget(Page *curr)
{
	// We can't use currentPage() because at the time when startscreensaver
	// is emitted the currentPage is the screensaver page.
	if (curr == this)
		requestZoneStatus();
}

