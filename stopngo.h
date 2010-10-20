/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef STOPNGO_H
#define STOPNGO_H

#include "bannerpage.h"
#include "bann2_buttons.h"


class StopAndGoDevice;
class StopAndGoPlusDevice;
class StopAndGoBTestDevice;
class QDomNode;


/*!
	\defgroup StopAndGo Stop&Go

	The section allows to manage the various Stop&Go systems configured.

	The StopAndGoMenu presents a list of BanStopAndGo which move to the
	appropriate StopAndGoPage depending on the type of Stop&Go systems configured.
	The page can be a StopAndGoPage, a StopAndGoPlusPage or a StopAndGoBTestPage.

	The BannStopAndGo and the various StopAndGoPage operates in conjuction with
	the StopAndGoDevice to reflect the system status and to operate on it.
*/

/*!
	\ingroup StopAndGo
	\brief Banner that reflects the state of the systems check module.

	It is composed by a central image, two optionals buttons and an optional
	description.

	The central image changes acording to the state of the systems check module
	by the StopAndGoDevice passed as parameter in constructor.

	\note The central image automatically changes to reflect the device status.
*/
class BannStopAndGo : public Bann2Buttons
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Creates a new BannStopAndGo with the given StopAndGoDevice \a dev,
		left and right buttons using \a left and \a right tags, and with the
		description \a descr.

		\sa Bann2Buttons
	*/
	BannStopAndGo(StopAndGoDevice *dev, const QString &left, const QString &right, const QString &descr = QString(), QWidget *parent = 0);

public slots:
	/*!
		\brief Handles dimensions from device to reflect the device status.
	*/
	void valueReceived(const DeviceValues &values_list);

private:
	QMap<int, QString> status_icons;
};


/*!
	\ingroup StopAndGo
	\brief Lists all the configured stop and go pages.

	If there is only one StopAndGoPage, this page isn't displayed and will
	forward directly to the StopAndGoPage.
*/
class StopAndGoMenu : public BannerPage
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoMenu with the given \a config_node.

		\sa BannerPage
	*/
	StopAndGoMenu(const QDomNode &config_node);
	virtual int sectionId() const;

	virtual void showPage();

private:
	void loadItems(const QDomNode &config_node);

	Page *next_page;
};


/*!
	\ingroup StopAndGo
	\brief Page for the StopAndGo.

	Contains a banner which reflects the device status and a button
	used to enable or disable the autoreset.
*/
class StopAndGoPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoPage with the given \a title and
		\a device.

		\sa Page
	*/
	StopAndGoPage(const QString &title, StopAndGoDevice *device);
	virtual int sectionId() const;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();

private:
	StopAndGoDevice *dev;
	StateButton *autoreset_button;
};


/*!
	\ingroup StopAndGo
	\brief Page for the StopAndGo Plus.

	It's like the StopAndGoPage, but in addiction, permits to open
	and close, and track fails.
*/
class StopAndGoPlusPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoPlusPage with the given \a title and
		\a device.

		\sa Page
	*/
	StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device);
	virtual int sectionId() const;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();
	void switchTracking();

private:
	StopAndGoPlusDevice *dev;
	StateButton *autoreset_button;
	StateButton *tracking_button;
};


/*!
	\ingroup StopAndGo
	\brief Page for the StopAndGo BTest.

	It is like the StopAndGoPage, but in addiction, permits to switch on
	and off the self tests, and to set the interval in days to perform
	the tests at.
 */
class StopAndGoBTestPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoBTestPage with the given \a title and
		\a device.

		\sa Page
	*/
	StopAndGoBTestPage(const QString &title, StopAndGoBTestDevice *device);
	virtual int sectionId() const;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();
	void switchAutoTest();

private:
	StopAndGoBTestDevice *dev;
	StateButton *autoreset_button;
	StateButton *autotest_button;
	BannLCDRange *autotest_banner;
};

#endif
