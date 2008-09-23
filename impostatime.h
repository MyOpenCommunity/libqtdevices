/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** impostaTime.h
**
**definizioni della pagine di impostazione data/ora
**
****************************************************************/

#ifndef IMPOSTA_TIME_H
#define IMPOSTA_TIME_H

#include "btbutton.h"
#include "main.h"
#include "timescript.h"
#include "btlabel.h"

#include <QFrame>

/*!
  \class impostaTime
  \brief This is the class used to set time and date.

  \author Davide
  \date lug 2005
*/

class  impostaTime : public QFrame
{
Q_OBJECT
public:
	impostaTime(QWidget *parent=0, const char *name=0);
/*!
  \brief Sets the background color for the banner.

  The argument is the QColor description of the color.
*/
	void setBGColor(QColor);

/*!
  \brief Sets the foreground color for the banner.

  The argument is the QColor description of the color.
*/
	void setFGColor(QColor);

/*!
  \brief Sets the background color for the banner.

  The arguments are RGB components for the color.
*/
	void setBGColor(int, int , int);

/*!
  \brief Sets the foreground color for the banner.

  The arguments are RGB components for the color.
*/
	void setFGColor(int , int , int);

/*!
  \brief Sets the background pixmap for the banner.
*/
	int setBGPixmap(char*);
	BtButton *but[7];

signals:
/*!
  \brief Emitted when the object is closed.
*/
	void Closed();

/*!
  \brief Emitted to send \a Open \a frame on the system.
*/
	void sendFrame(char*);
	void sendInit(char*);

public slots:
/*!
  \brief Draws the object.
*/
	void mostra();
	void hide();

private slots:
	void OKTime();
	void OKDate();

private:
	timeScript *dataOra;
	BtLabel *Immagine;

	// TODO: rimuovere questi metodi qt3!
	void setPaletteBackgroundColor(const QColor &c);
	void setPaletteForegroundColor(const QColor &c);
	void setPaletteBackgroundPixmap(const QPixmap &pixmap);
	const QColor& backgroundColor();
	const QColor& foregroundColor();
};


#endif // IMPOSTA_TIME_H
