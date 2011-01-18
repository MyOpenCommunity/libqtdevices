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


#ifndef VIDEODOORENTRY_H
#define VIDEODOORENTRY_H

#include "bannerpage.h"
#include "iconpage.h"
#include "bann2_buttons.h"

#include <QWidget>
#include <QHash>


class VCTCallPage;
class BasicVideoDoorEntryDevice;
class IntercomCallPage;
class VideoDoorEntryDevice;
class CallNotifierPage;
class StateButton;
class ItemTuning;
class QDomNode;
class QSignalMapper;

/*!
	\defgroup VideoDoorEntry Video Door Entry

	Allows the user to perform and receive video and intercom calls.
	There are two main classes:
	- VCTCallPage controls all the functionalities and aspects of a
	video call;
	- IntercomCallPage does the same for the intercom calls.

	The classes VideoDoorEntry, VideoControl and IntercomMenu allow the user to
	navigate through the items configured in the Video Door Entry system.

	The are also some small classes that customize the behaviour of the system on an
	incoming call:
	- the HandsFree to answer automatically;
	- the ProfessionalStudio to automatically open the door;
	- the RingtoneExclusion to exclude the bell.
*/


#ifdef LAYOUT_TS_3_5

class VideoDoorEntry : public BannerPage
{
Q_OBJECT
public:
	VideoDoorEntry(const QDomNode &config_node);

private slots:
	void valueReceived(const DeviceValues &values_list);

private:
	void loadItems(const QDomNode &config_node);
	BasicVideoDoorEntryDevice *dev;
	QHash<QString, CallNotifierPage*> popup_pages;
};


/*!
	\ingroup VideoDoorEntry
	\brief Notifies an incoming call.

	This popup page is showed on touchscreen 3.5'' to notifies an incoming call
	and to allows the user to switch on/off the stairlight and to open/close
	the door.
*/
class CallNotifierPage : public Page
{
Q_OBJECT
public:
	CallNotifierPage(QString descr, QString where, bool light, bool key);

	virtual void showPage();

signals:
	void stairLightActivate(QString where);
	void stairLightRelease(QString where);
	void openLock(QString where);
	void releaseLock(QString where);

private slots:
	void lightPressed();
	void lightReleased();
	void lockPressed();
	void lockReleased();

private:
	QTimer *timer;
	QString where;
};


#else

/*!
	\ingroup VideoDoorEntry
	\brief The main menu of the %VideoDoorEntry system.

*/
class VideoDoorEntry : public IconPage
{
friend class BtMain;
Q_OBJECT
public:
	VideoDoorEntry(const QDomNode &config_node);
	virtual int sectionId() const;
	static bool ring_exclusion;

private slots:
	void callGuardUnit();

private:
	VideoDoorEntryDevice *dev;
	VCTCallPage *vct_page;
	IntercomCallPage *intercom_page;
	VideoDoorEntry(); // available only for BtMain
	void loadItems(const QDomNode &config_node);
	void createVctElements();
};


/*!
	\ingroup VideoDoorEntry
	\brief The menu that shows buttons to call external cameras.
*/
class VideoControl : public IconPage
{
Q_OBJECT
public:
	VideoControl(const QDomNode &config_node, VideoDoorEntryDevice *dev);

private slots:
	void cameraOn(QString where);

private:
	VCTCallPage *call_page;
	QSignalMapper *mapper;
	VideoDoorEntryDevice *dev;
};


/*!
	\ingroup VideoDoorEntry
	\brief Shows page and controls for an intercom call.
*/
class IntercomCallPage : public Page
{
Q_OBJECT
public:
	IntercomCallPage(VideoDoorEntryDevice *d);
	virtual int sectionId() const;
	void closeCall();

public slots:
	virtual void cleanUp();
	void showPageAfterCall();

private slots:
	void valueReceived(const DeviceValues &values_list);
	void toggleCall();
	void handleClose();
	void changeVolume(int value);
	void toggleMute();
	void showPageIncomingCall();
	void playRingtone();
	void floorCallFinished();

private:
	VideoDoorEntryDevice *dev;
	StateButton *call_accept;
	StateButton *mute_button;
	ItemTuning *volume;
	bool call_active;
	int ringtone;
	bool already_closed;
	void callStarted();
};


/*!
	\ingroup VideoDoorEntry
	\brief The menu that show buttons to perform intercom calls.
*/
class IntercomMenu : public IconPage
{
Q_OBJECT
public:
	IntercomMenu(const QDomNode &config_node, VideoDoorEntryDevice *dev, IntercomCallPage *call_page);

private:
	QSignalMapper *mapper_ext_intercom;
	QSignalMapper *mapper_int_intercom;
};


/*!
	\ingroup VideoDoorEntry
	\ingroup Settings
	\brief Shows the items for the Video Door Entry settings.
*/
class VctSettings : public BannerPage
{
Q_OBJECT
public:
	VctSettings(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};


/*!
	\ingroup VideoDoorEntry
	\ingroup Settings
	\brief Toggle the professional studio facility.

	The banner represents the professional studio facility, to open automatically
	the door on an incoming call. Associated with the banner there is an icon on
	the Toolbar present if the functionality is on.
*/
class ProfessionalStudio : public BannOnTray
{
Q_OBJECT
public:
	ProfessionalStudio(bool status, int item_id);

protected:
	virtual void updateStatus();
};

 /*!
	\ingroup VideoDoorEntry
	\ingroup Settings
	\brief Toggle the hands free facility.

	The banner represents the hands free facility, to automatically answer on
	an incoming call. Associated with the banner there is an icon on the Toolbar
	present if the functionality is on.
*/
class HandsFree : public BannOnTray
{
Q_OBJECT
public:
	HandsFree(bool status, int item_id);

protected:
	virtual void updateStatus();
};


/*!
	\ingroup VideoDoorEntry
	\ingroup Settings
	\brief Toggle the bell (or ringtone) for video/intercom calls.

	The banner represents the possibility of exclude the ringtone for intercom
	or video calls. Associated with the banner there is an icon on the Toolbar
	present if the ringtone exclusion is on.
*/
class RingtoneExclusion : public BannOnTray
{
Q_OBJECT
public:
	RingtoneExclusion(bool status, int item_id);

protected:
	virtual void updateStatus();
};


#endif // #ifdef LAYOUT_TS_3_5

#endif
