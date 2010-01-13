/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Video-EntryPhone section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef VIDEOENTRYPHONE_H
#define VIDEOENTRYPHONE_H

#include "page.h"
#include "sectionpage.h"

#include <QWidget>

class VCTCallPage;
class EntryphoneDevice;
class EnablingButton;
class ItemTuning;
class QDomNode;
class QSignalMapper;


#ifdef LAYOUT_BTOUCH

class VideoEntryPhone : public BannerPage
{
Q_OBJECT
public:
	VideoEntryPhone(const QDomNode &config_node);

private:
	void loadDevices(const QDomNode &config_node);
};

#else

class VideoEntryPhone : public SectionPage
{
Q_OBJECT
public:
	VideoEntryPhone(const QDomNode &config_node);
	virtual int sectionId();
};


class CallExclusion : public BannerPage
{
Q_OBJECT
public:
	CallExclusion(const QDomNode &config_node);
};


class VideoControl : public IconPage
{
Q_OBJECT
public:
	VideoControl(const QDomNode &config_node);
	virtual void inizializza();

private slots:
	void cameraOn(QString where);

private:
	VCTCallPage *call_page;
	QSignalMapper *mapper;
	EntryphoneDevice *dev;
};



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

private:
	EntryphoneDevice *dev;
	BtButton *call_accept;
	EnablingButton *mute_button;
	ItemTuning *volume;
	Page *prev_page;
};


class Intercom : public IconPage
{
Q_OBJECT
public:
	Intercom(const QDomNode &config_node);

private:
	QSignalMapper *mapper_ext_intercom;
	QSignalMapper *mapper_int_intercom;
};

#endif // #ifdef LAYOUT_BTOUCH

#endif
