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
#include "iconpage.h"

#include <QWidget>

class VCTCallPage;
class EntryphoneDevice;
class EnablingButton;
class ItemTuning;
class CallExclusion;
class CallExclusionPage;
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

class VideoEntryPhone : public IconPage
{
Q_OBJECT
public:
	VideoEntryPhone(const QDomNode &config_node);
	virtual int sectionId();

	static void loadHiddenPages();

private slots:
	void status_changed(const StatusList &sl);
	void toggleCallExclusion();

private:
	BtButton *call_exclusion;
	EntryphoneDevice *dev;
	CallExclusionPage *call_ex_page;
	void loadItems(const QDomNode &config_node);
};


/**
 * The page that contains the banner of call exclusion.
 */
class CallExclusionPage : public BannerPage
{
Q_OBJECT
public:
	CallExclusionPage(const QDomNode &config_node);
	void setStatus(bool st);

signals:
	void statusChanged(bool on);

private:
	CallExclusion *b;
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
	BtButton *call_accept;
	EnablingButton *mute_button;
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

#endif // #ifdef LAYOUT_BTOUCH

#endif
