#ifndef _ALARM_PAGE_H_
#define _ALARM_PAGE_H_

#include "page.h"

class banner;
class QLabel;


class AlarmPage : public Page
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

	AlarmPage(const QString &name, char *indirizzo, QString IconaDx, altype t);

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
};

#endif // _ALARM_PAGE_H_
