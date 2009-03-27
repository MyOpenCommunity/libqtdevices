#ifndef SINGLECHOICEPAGE_H
#define SINGLECHOICEPAGE_H

#include "sottomenu.h"

#include <QButtonGroup>

/*
 * This abstract class is designed to provide a list of bannOnDx. Each of them
 * contains a toggle button that can be toggle in exclusive mode with the other
 * buttons.
 */
class SingleChoicePage : public sottoMenu
{
Q_OBJECT
public:
	SingleChoicePage(bool paging=false);
private slots:
	void okPressed();

protected:
	// The current id used to choose which button is toggled
	virtual int getCurrentId() = 0;
	// A method called when a button is pressed
	virtual void bannerSelected(int id) = 0;

protected:
	void addBanner(const QString &text, int id);
	QButtonGroup buttons;
};

#endif // SINGLECHOICEPAGE_H
