#ifndef _ALLARME_H_
#define _ALLARME_H_

#include "page.h"

class banner;
class QLabel;

/*!
  \class allarme
  \brief This class represents an alarm condition
  \author Ciminaghi
  \date April 2006
*/
class allarme : public Page
{
Q_OBJECT
public:
	typedef enum
	{
		TECNICO,
		INTRUSIONE,
		MANOMISSIONE,
		PANIC,
	} altype;

	allarme(const QString &name, char *indirizzo, QString IconaDx, altype t);

public slots:
	/*! Draw widget */
	void draw();

private:
	static const int ICON_DIM = 80;
	static const int NAV_DIM = 60;
	banner *bnav;
	QLabel *Immagine;
	QLabel *descr;
	// This function mask the other overload of banner::SetIcons. I don't care about it.
	void SetIcons(QString icon);
	altype type;

signals:
	void Next();
	void Prev();
	void Delete();
	void Back();
};

#endif // _ALLARME_H_
