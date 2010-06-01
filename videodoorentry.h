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
friend class BtMain;
Q_OBJECT
public:
	VideoDoorEntry(const QDomNode &config_node);
	virtual int sectionId() const;

private slots:
	void toggleRingExclusion();

private:
	VideoDoorEntry(); // available only for BtMain
	StateButton *ring_exclusion;
	EntryphoneDevice *dev;
	void loadItems(const QDomNode &config_node);
};


/**
 * The main class of an videdoorentry call, which shows a button for each external place,
 * allowing the user to call the place.
 */
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
 * The page for an intercom call (that is only audio).
 */
class IntercomCallPage : public Page
{
Q_OBJECT
public:
	IntercomCallPage(EntryphoneDevice *d);
	virtual int sectionId() const;

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

private:
	EntryphoneDevice *dev;
	StateButton *call_accept;
	StateButton *mute_button;
	ItemTuning *volume;
	bool call_active;
	int ringtone;
};


/**
 * The main class of an intercom call, which shows a button for each internal place,
 * allowing the user to call the place.
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


/**
 * The button (actually, the couple of buttons) that represents the professional
 * studio facility (automatically open the door on an incoming call)
 */
class ProfessionalStudio : public IconButtonOnTray
{
Q_OBJECT
public:
	ProfessionalStudio();

protected:
	virtual void updateStatus();
};


/**
 * The button (actually, the couple of buttons) that represents the hands
 * free facility (automatically answer on an incoming call)
 */
class HandsFree : public IconButtonOnTray
{
Q_OBJECT
public:
	HandsFree();

protected:
	virtual void updateStatus();
};


#endif // #ifdef LAYOUT_BTOUCH

#endif
