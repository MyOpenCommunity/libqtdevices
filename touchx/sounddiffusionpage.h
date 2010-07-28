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



#ifndef SOUNDDIFFUSIONPAGE_H
#define SOUNDDIFFUSIONPAGE_H

#include "bannerpage.h"
#include "banner.h"

class QDomNode;
class QLabel;
class BtButton;
class QStackedWidget;
class AmplifierDevice;
class VirtualAmplifierDevice;
class VirtualSourceDevice;
class ScrollingLabel;


// TODO: this should go to its own file if we need more banners
class SoundAmbient : public BannerNew
{
Q_OBJECT
public:
	SoundAmbient(const QString &descr, const QString &ambient);
	void initBanner(const QString &_ambient_icon, const QString &_center_icon,
		const QString &right, const QString &banner_text);
	void connectRightButton(Page *p);

private:
	QLabel *ambient_icon, *center_icon;
	ScrollingLabel *text;
	BtButton *right_button;
};


struct SourceDescription
{
	QString descr;
	QString where;
	int id;
	int cid;
	mutable Page *details;
};
Q_DECLARE_TYPEINFO(SourceDescription, Q_MOVABLE_TYPE);



/**
 * The widget container for the audio sources.
 */
class SoundSources : public QWidget
{
Q_OBJECT
public:
	SoundSources(const QString &area, const QList<SourceDescription> &sources);

signals:
	void pageClosed();

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private slots:
	void sourceCycle();
	void sourceStateChanged(bool active);

private:
	QStackedWidget *sources;
};


/**
 * Ambient page: has sources as top widget and amplifiers below
 */
class SoundAmbientPage : public BannerPage
{
Q_OBJECT
public:
	SoundAmbientPage(const QDomNode &conf_node, const QList<SourceDescription> &sources = QList<SourceDescription>());
	virtual int sectionId() const;
	static banner *getBanner(const QDomNode &item_node);

	virtual void showPage();
	virtual void cleanUp();

	// do not use directly, use SoundDiffusionPage::showCurrentAmbientPage
	static Page *currentAmbientPage();

private:
	void loadItems(const QDomNode &config_node);

private slots:
	void clearCurrentAmbient();

private:
	int section_id;
	static Page *current_ambient_page;
};


/**
 * Ambient page for the alarm clock: has sources as top widget and amplifiers below
 */
class SoundAmbientAlarmPage : public BannerPage
{
Q_OBJECT
public:
	SoundAmbientAlarmPage(const QDomNode &conf_node, const QList<SourceDescription> &sources,
			      AmplifierDevice *general = NULL);

	virtual void showPage();

signals:
	void saveVolumes();

private:
	void loadItems(const QDomNode &config_node);

private:
	AmplifierDevice *general;
};


/**
 * General sound diffusion page. Shown only in multi sound diffusion.
 *
 * In practice this page is always created, even for mono sound diffusion, but
 * its showPage method skips directly to the only ambient page.
 */
class SoundDiffusionPage : public BannerPage
{
Q_OBJECT
public:
	SoundDiffusionPage(const QDomNode &config_node);
	virtual int sectionId() const;

	static banner *getAmbientBanner(const QDomNode &item_node, const QList<SourceDescription> &sources);

	static void showCurrentAmbientPage();
	static Page *alarmClockPage();
	static bool isMultichannel();

	virtual void showPage();

private:
	QList<SourceDescription> loadSources(const QDomNode &config_node);
	void loadItemsMulti(const QDomNode &config_node);
	void loadItemsMono(const QDomNode &config_node);

private:
	Page *next_page;
	static Page *sound_diffusion_page, *alarm_clock_page;
	static bool is_multichannel;
};


/**
 * General sound diffusion page for the alarm clock. Shown only in multi sound diffusion.
 *
 * The main difference from SoundDiffusionPage is that the special ambient is not shown.
 */
class SoundDiffusionAlarmPage : public BannerPage
{
Q_OBJECT
public:
	SoundDiffusionAlarmPage(const QDomNode &config_node, const QList<SourceDescription> &sources,
				AmplifierDevice *general);

	virtual void showPage();

signals:
	void saveVolumes();

private:
	void loadItems(const QDomNode &config_node, const QList<SourceDescription> &sources);

private:
	AmplifierDevice *general;
};


/**
 * Handles the device logic for the local sound diffusion amplifier
 */
class LocalAmplifier : public QObject
{
Q_OBJECT
public:
	LocalAmplifier(QObject *parent);

private slots:
	void valueReceived(const DeviceValues &device_values);
	void audioStateChanged(int new_state, int old_state);
	void reenableLocalAmplifier();

private:
	VirtualAmplifierDevice *dev;
	bool state;
	int level;
};

/**
 * Handles the device logic for the local sound diffusion source
 */
class LocalSource : public QObject
{
Q_OBJECT
public:
	LocalSource(QObject *parent);

private slots:
	void valueReceived(const DeviceValues &device_values);

private:
	void startLocalPlayback(bool force);
	void pauseLocalPlayback();

private:
	VirtualSourceDevice *dev;
};

#endif // SOUNDDIFFUSIONPAGE_H
