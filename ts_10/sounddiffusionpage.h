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


/*!
	\defgroup SoundDiffusion Sound Diffusion

	This section allows:
	- managing the sound diffusion sources (change the playing track/radio station, change radio tuning, play a local mp3 file, ...)
	- turning on a source on one or more areas
	- turning on and changing the volume of SCS amplifiers

	The code in this section also manages the SCS commands for the local source/amplifiers.

	For multichannel sound diffusion, SoundDiffusionPage displays the list of environments, linking to a
	different SoundAmbient page for each environment.  Each SoundAmbient page contains a SoundSources instance
	to display/change the active source and instances of Amplifier, AmplifierGroup and BannPowerAmplifier
	to control the amplifiers.

	For monochannel sound diffusion, SoundDiffusionPage is instantiated and linked to the homepage, but
	is never shown to the user.

	LocalSource and LocalAmplifier are non-user-visible classes that handle the commands received by
	VirtualSourceDevice and VirtualAmplifierDevice and call the relevant AudioStateMachine methods to
	perform the changes.

	SoundAmbientAlarmPage and SoundDiffusionAlarmPage are variants of SoundAmbient and SoundDiffusionPage
	for sound diffusion alarm clock.  SoundDiffusionAlarmPage does not contain a link to the special environment
	and SoundAmbientAlarmPage only contains RDS and %Aux sources and has an OK button to save the sound diffusion
	state for the alarm clock.
 */


// TODO: this should go to its own file if we need more banners
// TODO: use Bann2Buttons directly
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


/*!
	\ingroup SoundDiffusion
	\brief Source configuration data from configuration file.
 */
struct SourceDescription
{
	QString descr;
	QString where;
	int id;
	int cid;
	mutable Page *details;
};
Q_DECLARE_TYPEINFO(SourceDescription, Q_MOVABLE_TYPE);



/*!
	\ingroup SoundDiffusion
	\brief Contains one AudioSource subclass for each configured source, and the button to turn on the source.

	There is a separate %SoundSources and AudioSource subclass instance for each environment; however,
	source configuration pages (RDS radio details, multimedia source list) are shared by all environments.
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


/*!
	\ingroup SoundDiffusion
	\brief %Page for a sound diffusion environment; contains SoundSources as top widget and a list of amplifiers below it.
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


/*!
	\ingroup SoundDiffusion
	\brief %Page for a sound diffusion environment for alarm clock.

	Only contains RDS radio and %Aux sources and has an OK button to save/activate alarm clock.
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


/*!
	\ingroup SoundDiffusion
	\brief General sound diffusion page. Shown only in multi sound diffusion.

	In practice this page is always created, even for mono sound diffusion, but
	its showPage() method skips directly to the only ambient page.
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


/*!
	\ingroup SoundDiffusion
	\brief General sound diffusion page for the alarm clock. Shown only in multi sound diffusion.

	The main difference from SoundDiffusionPage is that the special ambient is not shown.
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


/*!
	\ingroup SoundDiffusion
	\brief Handles the device logic for the local sound diffusion amplifier

	Instantiated during the parsing of the sound diffusion section.
 */
class LocalAmplifier : public QObject
{
Q_OBJECT
public:
	LocalAmplifier(QObject *parent);

private slots:
	void valueReceived(const DeviceValues &device_values);
	void vctValueReceived(const DeviceValues &device_values); // Values received from the VCT device (if present)

	void audioStateChanged(int new_state, int old_state);
	void reenableLocalAmplifier();

private:
	VirtualAmplifierDevice *dev;
	bool state;
	int freezed_level; // the (scs) amplifier level, used when we have to freeze the current level and restore it later.
	int level; // the current (scs) level of the amplifier
};

/**
	\ingroup SoundDiffusion
	\brief Handles the device logic for the local sound diffusion source.

	Instantiated during the parsing of the sound diffusion section.
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
