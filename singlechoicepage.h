#ifndef SINGLECHOICEPAGE_H
#define SINGLECHOICEPAGE_H

#include "page.h"

class QString;
class SingleChoiceContent;
class CheckableBanner;

/*
 * This abstract class is designed to provide a list of bannOnDx. Each of them
 * contains a toggle button that can be toggle in exclusive mode with the other
 * buttons.
 */
class SingleChoicePage : public BannerPage
{
Q_OBJECT
public:
	typedef SingleChoiceContent ContentType;

	SingleChoicePage();

private slots:
	void okPressed();
	void confirmSelection();

protected:
	// The current id used to choose which button is toggled
	virtual int getCurrentId() = 0;
	// A method called when a button is pressed
	virtual void bannerSelected(int id) = 0;
	// Set the checked button
	void setCheckedId(int id);

protected:
	void addBanner(CheckableBanner *bann, int id);
};

#endif // SINGLECHOICEPAGE_H
