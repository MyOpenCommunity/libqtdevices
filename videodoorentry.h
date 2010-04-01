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

#include "page.h"
#include "iconpage.h"

#include <QWidget>

class VCTCallPage;
class EntryphoneDevice;
class StateButton;
class ItemTuning;
class RingExclusion;
class RingExclusionPage;
class QDomNode;
class QSignalMapper;


#ifdef LAYOUT_BTOUCH

class VideoDoorEntry : public BannerPage
{
Q_OBJECT
public:
	VideoDoorEntry(const QDomNode &config_node);

private:
	void loadDevices(const QDomNode &config_node);
};

#else

class VideoDoorEntry : public IconPage
{
Q_OBJECT
public:
	VideoDoorEntry(const QDomNode &config_node);
	virtual int sectionId();

	static void loadHiddenPages();

private slots:
	void status_changed(const StatusList &sl);
	void toggleRingExclusion();

private:
	StateButton *ring_exclusion;
	EntryphoneDevice *dev;
	void loadItems(const QDomNode &config_node);
};


class VideoControl : public IconPage
{
Q_OBJECT
public:
	VideoControl(const QDomNode &config_node);

private slots:
	void cameraOn(QString where);

private:
	VCTCallPage *call_page;
	QSignalMapper *mapper;
	EntryphoneDevice *dev;
};


/**
 * The page for an intercom call, that allow the user to set the parameters of
 * the call (that is only audio).
 */
class IntercomCallPage : public Page
{
Q_OBJECT
public:
	IntercomCallPage(EntryphoneDevice *d);
	virtual void showPage();

private slots:
	void status_changed(const StatusList &sl);
	void toggleCall();
	void handleClose();
	void changeVolume(int value);
	void toggleMute();
	void showPageIncomingCall();

private:
	EntryphoneDevice *dev;
	StateButton *call_accept;
	StateButton *mute_button;
	ItemTuning *volume;
};


/**
 * The main class of an intercom call, which shows a button for each internal place,
 * that allow the user to call the place.
 */
class Intercom : public IconPage
{
Q_OBJECT
public:
	Intercom(const QDomNode &config_node);

private:
	QSignalMapper *mapper_ext_intercom;
	QSignalMapper *mapper_int_intercom;
};


class IconButtonOnTray : public IconPageButton
{
Q_OBJECT
public:
	IconButtonOnTray(const QString &label, const QString &icon_on, const QString &icon_off,
		const QString & tray_icon);

private slots:
	void toggleActivation();
	void turnOff();

protected:
	virtual void updateStatus();

private:
	BtButton *tray_button;
};


class ProfessionalStudio : public IconButtonOnTray
{
Q_OBJECT
public:
	ProfessionalStudio();

protected:
	virtual void updateStatus();
};


class HandsFree : public IconButtonOnTray
{
Q_OBJECT
public:
	HandsFree();

protected:
	virtual void updateStatus();
};


class SettingsVideoDoorEntry : public IconPage
{
Q_OBJECT
public:
	SettingsVideoDoorEntry();
};


#endif // #ifdef LAYOUT_BTOUCH

#endif
