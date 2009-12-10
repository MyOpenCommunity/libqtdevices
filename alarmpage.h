#ifndef _ALARM_PAGE_H_
#define _ALARM_PAGE_H_

#include "page.h"

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

	AlarmPage(const QString &name, char *indirizzo, QString Icon, altype t);

private:
	QLabel *image;
	QLabel *description;

	altype type;

signals:
	void Next();
	void Prev();
	void Delete();
};

#endif // _ALARM_PAGE_H_
