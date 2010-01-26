#ifndef _ALARM_PAGE_H_
#define _ALARM_PAGE_H_

#include "page.h"

class QLabel;
class QDateTime;


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

	AlarmPage(altype t, const QString &description, const QString &zone,
		  const QDateTime &time);

public slots:
	virtual void showPage();

signals:
	void Next();
	void Prev();
	void Delete();
	void showHomePage();
	void showAlarmList();

private:
	QLabel *image;
	QLabel *description;
};

#endif // _ALARM_PAGE_H_
