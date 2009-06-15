#ifndef SCREENSAVERPAGE_H
#define SCREENSAVERPAGE_H

#include "singlechoicepage.h"


class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage();
	virtual void showPage();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);
};

#endif // SCREENSAVERPAGE_H
