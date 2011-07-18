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

	This subsection manages the Stop&Go systems configured, where each Stop&Go
	controls an electrical grid.

	The StopAndGoMenu page presents a list of BannStopAndGo for each Stop&Go
	system (that shows the status of the related system) and allows the user to
	go to the dedicated page; this can be a StopAndGoPage, a StopAndGoPlusPage or a
	StopAndGoBTestPage depending on the configuration.
*/


namespace StopAndGo
{
	enum Status
	{
		STATUS_CLOSED = 0,
		STATUS_OPENED,
		STATUS_LOCKED,
		STATUS_FAIL,
		STATUS_GROUND_FAIL,
		STATUS_VMAX
	};
}

/*!
	\ingroup StopAndGo
	\brief Banner that reflects the state of the systems check module.

	It is composed by a central image, two optionals buttons and an optional
	description.

	The central image changes acording to the state of the related electrical
	grid systems.

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
	BannStopAndGo(StopAndGoDevice *dev, const QString &left = QString(), const QString &right = QString(), const QString &descr = QString(), QWidget *parent = 0);

	/*!
		\brief Show/hide left / right buttons.
		\note Call this method only if both the buttons are set.
	*/
	void showButtons(bool show);

public slots:
	/*!
		\brief Handles dimensions from device to reflect the device status.
	*/
	void valueReceived(const DeviceValues &values_list);

signals:
	void statusChanged(StopAndGo::Status);

private:
	QMap<int, QString> status_icons;
};


/*!
	\ingroup StopAndGo
	\brief Lists all the configured Stop&Go systems.

	If there is only one system, this page is not displayed and show directly
	the StopAndGoPage dedicated for the system.
*/
class StopAndGoMenu : public BannerPage
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoMenu with the given \a config_node.
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
	\brief Controls a Stop&Go system.

	Contains a banner which reflects the status of the system and a button
	to enable or disable the automatic reset.
*/
class StopAndGoPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoPage with the given \a title and
		\a device.
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
	\brief Controls a Stop&Go plus system.

	It is like the StopAndGoPage, but in addiction, permits to enable or disable
	the system check.

	\sa StopAndGoPage
*/
class StopAndGoPlusPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoPlusPage with the given \a title and
		\a device.
	*/
	StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device);
	virtual int sectionId() const;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();
	void switchTracking();
	void statusChanged(StopAndGo::Status st);

private:
	StopAndGoPlusDevice *dev;
	StateButton *autoreset_button;
	StateButton *tracking_button;
	QLabel *tracking_label;
	BannStopAndGo *status_banner;
};


/*!
	\ingroup StopAndGo
	\brief Controls a Stop&Go BTest system.

	This page is very similar to the StopAndGoPlusPage one, but in addition
	allow the user to set the number of days between tests.

	\sa StopAndGoPlusPage
 */
class StopAndGoBTestPage : public Page
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new StopAndGoBTestPage with the given \a title and
		\a device.
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
	QLabel *autotest_label;
	BannLCDRange *autotest_banner;
};

#endif
