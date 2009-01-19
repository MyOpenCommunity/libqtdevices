#ifndef SCREENSAVERPAGE_H
#define SCREENSAVERPAGE_H

#include "singlechoicepage.h"
#include "screensaver.h"


class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage(ScreenSaver::Type type);

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);

private:
	ScreenSaver::Type current_type;
};

#endif // SCREENSAVERPAGE_H
