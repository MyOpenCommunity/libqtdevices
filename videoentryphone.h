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
#include "vctcallpage.h"

#include <QWidget>

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

private slots:
	void cameraOn(QString where);

private:
	VCTCallPage *call_page;
	QSignalMapper *mapper;
	EntryphoneDevice *dev;
};


class Intercom : public IconPage
{
Q_OBJECT
public:
	Intercom(const QDomNode &config_node);
};

#endif // #ifdef LAYOUT_BTOUCH

#endif
