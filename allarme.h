#ifndef _ALLARME_H_
#define _ALLARME_H_

#include <QFrame>

class banner;
class BtLabel;


/*!
  \class allarme
  \brief This class represents an alarm condition
  \author Ciminaghi
  \date April 2006
*/
class allarme : public QFrame
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
private:
	static const int ICON_DIM = 80;
	static const int NAV_DIM = 60;
	banner *bnav;
	BtLabel *Immagine;
	BtLabel *descr;
	void SetIcons(char*);
	altype type;

public:
	allarme(QWidget *parent, const QString & name, char *indirizzo, char *IconaDx, altype t);

	void show();
public slots:
	/*! Draw widget */
	void draw();
 signals:
	void Next();
	void Prev();
	void Delete();
	void Back();
};

#endif // _ALLARME_H_
