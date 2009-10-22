
/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** antintrusion.h
**
**definizioni della pagina di antiintrusione
**
****************************************************************/

#ifndef ANTINTRUSION_H
#define ANTINTRUSION_H

#include "frame_receiver.h"
#include "page.h"


#include <QString>
#include <QTimer>
#include <QList>

class impAnti;
class zonaAnti;
class Keypad;
class allarme;
class QDomNode;
class QWidget;


/*!
  \class Antintrusion
  \brief This is a class that contains the alarm-plant, the zone's list and the alarm queue.
  The plant is always kept on the top, the zones in the remaining lines. Alarm queue is active
  only when there's some alarm pending (and it's possible from \a antintrusione object to access
to \a alarm \a queue one though a button). When there's an alarm the alarm queue becomes automatically visible
  \author Davide
  \date lug 2005
*/
class Antintrusion : public Page, FrameReceiver
{
Q_OBJECT
public:
	Antintrusion(const QDomNode &config_node);
	~Antintrusion();
	virtual void inizializza();
	void draw();
	virtual void manageFrame(OpenMsg &msg);

public slots:
	void Parzializza();
	void Parz();
	void IsParz(bool);

/*!
  \brief if there are no allarms in the queue the button in the plant area which give the possibility to see the queue is hidden
*/
	void ctrlAllarm();
/*!
  \brief arms a timer and calls ctrlAllarm after 150ms. This is necessary because some time is necessary to destroy the object from the queue
*/
	void testranpo();
/*!
  \brief Invoked when next alarm must be displayed
*/
	void nextAlarm();
/*!
  \brief Invoked when prev alarm must be displayed
*/
	void prevAlarm();
/*!
  \brief Invoked when current alarm must be deleted
*/
	void deleteAlarm();
/*!
  \brief Invoked when alarm list must be shown
*/
	void showAlarms();
/*!
  \brief clear alarm list
*/
	void doClearAlarms();
	void request();

signals:
	void openAckRx();
	void openNakRx();

/*!
  \brief enable/disable area partialization enable events
*/
	void abilitaParz(bool ab);
/*!
  \brief part. changed events
*/
	void partChanged(zonaAnti*);
/*!
  \brief clear changed flags
*/
	void clearChanged();

protected slots:
	virtual void forwardClick();

private:
/*!
  \param <numRighe> row number of the object
*/
	uchar numRighe;
/*!
  \param <impianto> alarm plant
*/
	impAnti* impianto;
	QWidget* top_widget;
/*!
  \param <allarmi> alarm's queue
*/
	QList<allarme*> allarmi;
	int curr_alarm;
/*!
  \param <testoManom> text for a manomission alarm
  \param <testoTecnico> text for a tecnical alarm
  \param <testoPanic> text for a panic alarm  
  \param <testoIntrusione> text for a intrusion alarm  
*/
	QString testoManom, testoTecnico, testoIntrusione, testoPanic;
	Keypad *tasti;
	static const int MAX_ZONE = 8;
	QTimer request_timer;
	QTimer *t;

	Page *previous_page;
	void loadItems(const QDomNode &config_node);

	void clearAlarms();

private slots:
	void closeAlarms();
	void requestZoneStatus();
	void requestStatusIfCurrentWidget(Page *curr);
	void plantInserted();
};


#endif // ANTINTRUSION_H
