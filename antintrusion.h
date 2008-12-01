
/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** antintrusione.h
**
**definizioni della pagina di sottoMenu antiintrusione
**
****************************************************************/

#ifndef ANTINTRUSION_H
#define ANTINTRUSION_H


#include "main.h" // MAX_PATH
#include "allarme.h"

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QList>

class sottoMenu;
class zonaAnti;
class tastiera;


/*!
  \class Antintrusion
  \brief This is a class that contains the alarm-plant, the zone's list and the alarm queue  

  each of that are \a sottomenu type. The plant is always kept on the top, the zones in the remaining lines. Alarm queue is active only when there's some alarm pending (and it's possible from \a antintrusione object to access to \a alarm \a queue one though a button). When there's an alarm the alarm queue becomes automatically visible 
  \author Davide
  \date lug 2005
*/
class Antintrusion : public QWidget
{
Q_OBJECT
public:
	Antintrusion(QWidget *parent=0);
	~Antintrusion();
/*!
  \brief inserts a new item in the Antintrusion class. If the new element is a plant it is added to \a impianto; if it is a zone it is added to \a zone; if it is an alarm it is added to \a allarmi
*/
	void addItem(int id, const QString &descrizione, QString indirizzo, QList<QString*> &icon_names);
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
	void inizializza();
/*!
  \brief sets the geometry of the object
*/
	void setGeom(int,int,int,int);
/*!
  \brief sets the kind of the navigation bar on the bottom
*/
	void setNavBarMode(uchar);
/*!
  \brief force the object to draw all its components (impianto-zone-allarmi)
*/
	void draw();

signals:
/*!
  \brief it's emitted when the object is closed
*/
	void Closed();
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

public slots:
	void Parzializza();
	void Parz(char* pwd);
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
  \brief Invoked when the current alarm window must be closed and this window 
must be displayed.
*/
	void closeAlarms();
/*!
  \brief Invoked when alarm list must be shown
*/
	void showAlarms();
/*!
  \brief clear alarm list
*/
	void doClearAlarms();
	void request();

protected:
	virtual void hideEvent(QHideEvent *event);

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
	tastiera *tasti;
	static const int MAX_ZONE = 8;
	QTimer request_timer;
};


#endif // ANTINTRUS_H
