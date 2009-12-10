/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class represent the SpecialPage.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef SPECIALPAGE_H
#define SPECIALPAGE_H

#include "frame_receiver.h"
#include "page.h"

class timeScript;
class TemperatureViewer;

class QDomNode;


class SpecialPage : public Page, FrameReceiver
{
Q_OBJECT
public:
	SpecialPage(const QDomNode &config_node);
	virtual void inizializza();
	virtual void manageFrame(OpenMsg &msg);

private:
	void loadItems(const QDomNode &config_node);
	void loadSpecial(const QDomNode &config_node);

	// Define the behaviour of special button
	enum specialType
	{
		NORMAL = 0,  /*!<  Clicking the \a special \a button the device always make the same function.*/
		CYCLIC = 1,  /*!< Clicking the \a special \a button the device one time make a \a ON command and then an \a OFF command an so on.*/
		BUTTON = 2   /*!<  Pushing the \a special \a button the device make a \a ON command while Releasing the button a \a OFF command is made.*/
	};

	specialType type; // The type of special button
	QString who, what, where;
	TemperatureViewer *temp_viewer;

private slots:
	void clickedButton();
	void pressedButton();
	void releasedButton();
};

#endif // SPECIALPAGE_H
