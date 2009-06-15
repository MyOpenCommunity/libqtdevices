
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


#include "page.h"

#include <QString>
#include <QTimer>
#include <QList>

class sottoMenu;
class zonaAnti;
class Keypad;
class allarme;
class QDomNode;
class QWidget;


/*!
  \class Antintrusion
  \brief This is a class that contains the alarm-plant, the zone's list and the alarm queue  

  each of that are \a sottomenu type. The plant is always kept on the top, the zones in the remaining lines. Alarm queue is active only when there's some alarm pending (and it's possible from \a antintrusione object to access to \a alarm \a queue one though a button). When there's an alarm the alarm queue becomes automatically visible 
  \author Davide
  \date lug 2005
*/
class Antintrusion : public Page
{
Q_OBJECT
public:
	Antintrusion(const QDomNode &config_node);
	~Antintrusion();
/*!
  \brief sets the rows number fot the object. This method automatically give the exact row number to the \a sottomenu: impianto-zone-allarmi
  arguments:
       - kind of object to add
       - name
       - open address
       - 4 buttons image file name
       - period for animation images
       - number of frame for animation images
       - tecnical alarm text
       - intrusion alarm text
       - manomission alarm text
       - panic alarm text
*/
	void setNumRighe(uchar);
/*!
  \brief asks the starting point for the plant 
  \bug the plant doesn't answer to this question at his time \date lug 2005
*/
	virtual void inizializza();
/*!
  \brief sets the kind of the navigation bar on the bottom
*/
	void setNavBarMode(uchar);
/*!
  \brief force the object to draw all its components (impianto-zone-allarmi)
*/
	void draw();

public slots:
	void Parzializza();
	void Parz();
	void IsParz(bool);
/*!
  \brief analyzes the open frame coming from the plant. If there are an allarm is added to the queue; if the plant is inserted the alarm queue is resetted
*/
	void gesFrame(char*);
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

private:
/*!
  \param <numRighe> row number of the object
*/
	uchar numRighe;
/*!
  \param <zone> zone's list
*/
	sottoMenu* zone;
/*!
  \param <impianto> alarm plant
*/
	sottoMenu* impianto;
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

private slots:
	void closeAlarms();
	void requestZoneStatus();
	void requestStatusIfCurrentWidget(Page *curr);

signals:
/*!
  \brief emitted when a open frame comes from the plant so impianto-zone-allarmi can analyze it
*/
	void gestFrame(char*);
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
};


#endif // ANTINTRUSION_H
