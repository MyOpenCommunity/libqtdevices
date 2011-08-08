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



#include "sounddiffusionpage.h"
#include "main.h" // Section, bt_global::config
#include "xml_functions.h"
#include "bann2_buttons.h" // Bann2Buttons
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName, scsToLocalVolume
#include "icondispatcher.h" // bt_global::icons_cache
#include "bann_amplifiers.h" // Amplifier
#include "poweramplifier.h" // BannPowerAmplifier, PowerAmplifierPage
#include "media_device.h"
#include "devices_cache.h"
#include "navigation_bar.h"
#include "labels.h" // ScrollingLabel
#include "videodoorentry_device.h"

#ifdef LAYOUT_TS_10
#include "audiostatemachine.h"
#include "multimedia_ts10.h" // MultimediaSectionPage
#include "audioplayer_ts10.h"
#else
#include "multimedia_ts3.h" // MultimediaContainer
#include "audioplayer_ts3.h"
#include "hardware_functions.h" // activateLocalSource, deactivateLocalSource
#endif

#include <QDomNode>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>


#define TEMPORARY_OFF_TIMEOUT 1000
#define RESTORE_VOLUME_SECS 900

bool SoundDiffusionPage::is_multichannel = false;
Page *SoundDiffusionPage::sound_diffusion_page = NULL;
Page *SoundDiffusionPage::alarm_clock_page = NULL;
Page *SoundAmbientPage::current_ambient_page = NULL;

enum SourceType
{
	SOURCE_RADIO_MONO = 11001,
	SOURCE_AUX_MONO = 11002,
	SOURCE_MULTIMEDIA_MONO = 11003,
	SOURCE_RADIO_MULTI = 12001,
	SOURCE_AUX_MULTI = 12002,
	SOURCE_MULTIMEDIA_MULTI = 12003
};

enum AmplifierType
{
	AMPLIFIER = 11020,
	AMPLIFIER_GROUP = 11021,
	BANN_POWER_AMPLIFIER = 11022,
};

enum AmbientType
{
	ITEM_SPECIAL_AMBIENT = 12020,              // special ambient
	ITEM_AMBIENT_1 = 12021,                    // normal ambient (with a zone)
	ITEM_AMBIENT_2 = 12022,                    // normal ambient (with a zone)
	ITEM_AMBIENT_3 = 12023,                    // normal ambient (with a zone)
	ITEM_AMBIENT_4 = 12024,                    // normal ambient (with a zone)
	ITEM_AMBIENT_5 = 12025,                    // normal ambient (with a zone)
	ITEM_AMBIENT_6 = 12026,                    // normal ambient (with a zone)
	ITEM_AMBIENT_7 = 12027,                    // normal ambient (with a zone)
	ITEM_AMBIENT_8 = 12028,                    // normal ambient (with a zone)
};


namespace
{
	QStringList getAddresses(const QDomNode &addresses_node)
	{
		QStringList l;
		foreach (const QDomNode &where_node, getChildren(addresses_node, "where"))
			l << where_node.toElement().text();
		Q_ASSERT_X(!l.isEmpty(), "getAddresses", "Addresses node must have at least one 'where' tag.");
		return l;
	}

	Banner *getAmbientBanner(const QString &descr, SoundAmbientPage::Type type, Page *details)
	{
#ifdef LAYOUT_TS_10
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("ambient"), bt_global::skin->getImage("forward"),
			descr);
		bann->connectRightButton(details);
		return bann;
#else
		if (type == SoundAmbientPage::SPECIAL_AMBIENT)
		{
			Bann2Buttons *bann = new Bann2Buttons;
			bann->initBanner(QString(), bt_global::skin->getImage("ambient"), bt_global::skin->getImage("forward"),
				descr);
			bann->connectRightButton(details);
			return bann;
		}

		Bann4Buttons *bann = new Bann4Buttons;
		bann->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("sounddiffusion"),
			bt_global::skin->getImage("ambient"), QString(), descr);
		bann->setCentralSpacing(false);
		bann->connectRightButton(details);
		return bann;
#endif
	}
}


SoundAmbientPage::SoundAmbientPage(const QDomNode &conf_node, const QList<SourceDescription> &sources, const QString &descr, Type ambient_type)
{
	SkinContext context(getTextChild(conf_node, "cid").toInt());
	QString area;

	if (getTextChild(conf_node, "id").toInt() == SOUNDDIFFUSION)
	{
		section_id = SOUNDDIFFUSION;
		area = "0";
	}
	else
	{
		section_id = NO_SECTION;
		area = getTextChild(conf_node, "env");
	}

	SoundSources *top_widget = 0;
	// this handles the case for special ambient, which must not show sources
	if (ambient_type != SPECIAL_AMBIENT)
	{
		top_widget = new SoundSources((*bt_global::config)[SOURCE_ADDRESS], descr, area, sources);
		connect(top_widget, SIGNAL(pageClosed()), SLOT(showPage()));
	}
#ifdef LAYOUT_TS_3_5
	else // SPECIAL_AMBIENT for ts3 is a general
	{
		top_widget = new SoundSources((*bt_global::config)[SOURCE_ADDRESS], descr, QString(), sources);
		connect(top_widget, SIGNAL(pageClosed()), SLOT(showPage()));
	}
#endif
	BannerContent *content = new BannerContent;

#ifdef LAYOUT_TS_3_5
	// This page has a strange structure: the 'standard' content of the page
	// must have the normal margins but the line must not. So we cannot use
	// the buildPage() and we have to reproduce all its functionalities.

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 5);
	main_layout->setSpacing(0);
	top_widget->setContentsMargins(5, 0, 5, 0);
	main_layout->addWidget(top_widget);

	main_layout->addSpacing(5);
	QLabel *line = new QLabel;
	line->setFixedHeight(3);
	line->setProperty("noStyle", true);
	main_layout->addWidget(line);
	main_layout->addSpacing(5);

	content->setContentsMargins(5, 0, 5, 0);
	main_layout->addWidget(content, 1);

	__content = content;
	NavigationBar *nav_bar = new NavigationBar;
	// Because the NavigationBar uses a fixed geometry, we cannot use the
	// setContentsMargins method.
	QVBoxLayout *l = new QVBoxLayout;
	l->setContentsMargins(5, 0, 5, 0);
	l->addWidget(nav_bar);
	main_layout->addLayout(l);

	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
#else
	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
	if (top_widget)
		main_layout->addWidget(top_widget);

	main_layout->addWidget(content, 1);
	buildPage(main_widget, content, new NavigationBar, getTextChild(conf_node, "descr"), Page::TITLE_HEIGHT);
#endif

	loadItems(conf_node);

	connect(this, SIGNAL(Closed()), SLOT(clearCurrentAmbient()));
}

int SoundAmbientPage::sectionId() const
{
	return section_id;
}

void SoundAmbientPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		Banner *b = getBanner(item);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("ID %s not handled in SoundAmbientPage", qPrintable(getTextChild(item, "id")));
	}
}

void SoundAmbientPage::showPage()
{
	current_ambient_page = this;

	BannerPage::showPage();
}

void SoundAmbientPage::cleanUp()
{
	Q_ASSERT_X(!current_ambient_page || current_ambient_page == this, "SoundAmbientPage::cleanUp",
		"Something terrible happened");

	current_ambient_page = NULL;
}

void SoundAmbientPage::clearCurrentAmbient()
{
	Q_ASSERT_X(!current_ambient_page || current_ambient_page == this, "SoundAmbientPage::clearCurrentAmbient",
		"There is no end to terrible things");

	current_ambient_page = NULL;
}

Banner *SoundAmbientPage::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString descr = getTextChild(item_node, "descr");
	QString where = getTextChild(item_node, "where");
	int oid = getTextChild(item_node, "openserver_id").toInt();

	Banner *b = 0;
	switch (id)
	{
	case AMPLIFIER:
		b = new Amplifier(descr, where);
		break;
	case BANN_POWER_AMPLIFIER:
	{
		PowerAmplifierDevice *dev = bt_global::add_device_to_cache(new PowerAmplifierDevice(where, oid));
		b = new BannPowerAmplifier(descr, dev, new PowerAmplifierPage(dev, item_node));
		break;
	}
	case AMPLIFIER_GROUP:
		b = new AmplifierGroup(getAddresses(getChildWithName(item_node, "addresses")), descr);
		break;
	}
	return b;
}

Page *SoundAmbientPage::currentAmbientPage()
{
	return current_ambient_page;
}


SoundAmbientAlarmPage::SoundAmbientAlarmPage(const QDomNode &conf_node, const QList<SourceDescription> &sources,
	const QString &descr, AmplifierDevice *_general)
{
	general = _general;

	SkinContext context(getTextChild(conf_node, "cid").toInt());
	QString area;

	if (getTextChild(conf_node, "id").toInt() == SOUNDDIFFUSION)
		area = "0";
	else
		area = getTextChild(conf_node, "env");

	QList<SourceDescription> filtered_sources;
	foreach (const SourceDescription &s, sources)
		if (s.type == SourceDescription::RADIO || s.type == SourceDescription::AUX)
			filtered_sources.append(s);

	SoundSources *top_widget = new SoundSources((*bt_global::config)[SOURCE_ADDRESS], descr,
		area, filtered_sources);
	connect(top_widget, SIGNAL(pageClosed()), SLOT(showPage()));

#ifdef LAYOUT_TS_3_5
	// See the comment on the SoundAmbientPage to an explaination of the structure of this page
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 5);
	main_layout->setSpacing(0);

	top_widget->setContentsMargins(5, 0, 5, 0);
	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));
	main_layout->addWidget(icon, 0, Qt::AlignHCenter);
	main_layout->addSpacing(5);
	main_layout->addWidget(top_widget);
	main_layout->addSpacing(5);
	QLabel *line = new QLabel;
	line->setFixedHeight(3);
	line->setProperty("noStyle", true);
	main_layout->addWidget(line);
	main_layout->addSpacing(5);

	NavigationBar *nav_bar = new NavigationBar("ok");
	BtButton *ok = nav_bar->forward_button;
#else
	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);

	main_layout->setContentsMargins(0, 0, 0, 0);

	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));

	QHBoxLayout *button_layout = new QHBoxLayout;
	button_layout->setContentsMargins(18, 0, 17, 18);
	button_layout->addWidget(ok, 0, Qt::AlignRight | Qt::AlignBottom);
	main_layout->addWidget(top_widget);

	NavigationBar *nav_bar = new NavigationBar;
#endif

	connect(ok, SIGNAL(clicked()), SIGNAL(saveVolumes()));

	BannerContent *content = new BannerContent;
	main_layout->addWidget(content, 1);

#ifdef LAYOUT_TS_3_5
	content->setContentsMargins(5, 0, 5, 0);
	QVBoxLayout *nav_layout = new QVBoxLayout;
	nav_layout->setContentsMargins(5, 0, 5, 0);
	nav_layout->addWidget(nav_bar);

	main_layout->addLayout(nav_layout);

	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	__content = content;
#else
	main_layout->addLayout(button_layout);
	buildPage(main_widget, content, nav_bar, getTextChild(conf_node, "descr"), Page::SMALL_TITLE_HEIGHT);
#endif
	loadItems(conf_node);
}

void SoundAmbientAlarmPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		Banner *b = SoundAmbientPage::getBanner(item);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("ID %s not handled in SoundAmbientAlarmPage", qPrintable(getTextChild(item, "id")));
	}
}

void SoundAmbientAlarmPage::showPage()
{
	if (general)
		general->turnOff();

	BannerPage::showPage();
}


SoundDiffusionPage::SoundDiffusionPage(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	next_page = NULL;

	// check if this hardware can work as a source/amplifier and create the virtual
	// devices to handle the source/amplifier frames
	is_multichannel = getTextChild(config_node, "id").toInt() == SOUNDDIFFUSION_MULTI;
	// required for the parsing of some frames
	AmplifierDevice::setIsMultichannel(is_multichannel);
	SourceDevice::setIsMultichannel(is_multichannel);

	buildPage(getTextChild(config_node, "descr"));
	if (is_multichannel)
		loadItemsMulti(config_node);
	else
		loadItemsMono(config_node);

	sound_diffusion_page = this;

	if (!(*bt_global::config)[SOURCE_ADDRESS].isEmpty() || !(*bt_global::config)[AMPLIFIER_ADDRESS].isEmpty())
	{
		QString init_frame = VirtualSourceDevice::createMediaInitFrame(is_multichannel,
									       (*bt_global::config)[SOURCE_ADDRESS],
									       (*bt_global::config)[AMPLIFIER_ADDRESS]);
		bt_global::devices_cache.addInitCommandFrame(0, init_frame);
	}

#ifdef LAYOUT_TS_10
	if (!(*bt_global::config)[AMPLIFIER_ADDRESS].isEmpty())
		new LocalAmplifier(this);
#endif
	if (!(*bt_global::config)[SOURCE_ADDRESS].isEmpty())
		new LocalSource(this);
}

int SoundDiffusionPage::sectionId() const
{
	return SOUNDDIFFUSION_MULTI;
}

QList<SourceDescription> SoundDiffusionPage::loadSources(const QDomNode &config_node)
{
	QDomNode sources_node = getChildWithName(config_node, "multimediasources");
	QList<SourceDescription> sources_list;
	foreach (const QDomNode &source, getChildren(sources_node, "item"))
	{
		SourceDescription d;
		switch (getTextChild(source, "id").toInt())
		{
		case SOURCE_AUX_MONO:
		case SOURCE_AUX_MULTI:
			d.type = SourceDescription::AUX;
			break;
		case SOURCE_RADIO_MONO:
		case SOURCE_RADIO_MULTI:
			d.type = SourceDescription::RADIO;
			break;
		default: // SOURCE_MULTIMEDIA_MONO, SOURCE_MULTIMEDIA_MULTI
			d.type = SourceDescription::MULTIMEDIA;
			break;
		}

		d.cid = getTextChild(source, "cid").toInt();
		d.descr = getTextChild(source, "descr");
		d.where = getTextChild(source, "where");
		d.details = NULL;
		sources_list << d;
	}

	Q_ASSERT_X(!sources_list.isEmpty(), "SoundDiffusionPage::loadItems", "No sound diffusion sources defined.");

	return sources_list;
}

void SoundDiffusionPage::loadItemsMulti(const QDomNode &config_node)
{
	QList<SourceDescription> sources_list = loadSources(config_node);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();
		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");

		SoundAmbientPage::Type t = SoundAmbientPage::NORMAL_AMBIENT;
		if (id == ITEM_SPECIAL_AMBIENT)
			t = SoundAmbientPage::SPECIAL_AMBIENT;

		QString descr = getTextChild(item, "descr");
		SoundAmbientPage *p = new SoundAmbientPage(page_node, sources_list, descr, t);

		Banner *b = getAmbientBanner(descr, t, p);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("ID %s not handled in SoundDiffusionPage", qPrintable(getTextChild(item, "id")));
	}

	alarm_clock_page = new SoundDiffusionAlarmPage(config_node, sources_list, AmplifierDevice::createDevice("0"));
}

void SoundDiffusionPage::loadItemsMono(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QList<SourceDescription> sources_list = loadSources(config_node);
	next_page = new SoundAmbientPage(config_node, sources_list);
	connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));

	alarm_clock_page = new SoundAmbientAlarmPage(config_node, sources_list, QString(), AmplifierDevice::createDevice("0"));
}

void SoundDiffusionPage::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}

Page *SoundDiffusionPage::alarmClockPage()
{
	return alarm_clock_page;
}

void SoundDiffusionPage::showCurrentAmbientPage()
{
	Page *current_ambient_page = SoundAmbientPage::currentAmbientPage();

	Q_ASSERT_X(current_ambient_page || sound_diffusion_page, "SoundDiffusionPage::showCurrentAmbientPage",
		"Terrible stuff keeps on happening");

	qDebug() << "Sound diffusion" << sound_diffusion_page << "ambient" << current_ambient_page;

	if (current_ambient_page)
		current_ambient_page->showPage();
	else
		sound_diffusion_page->showPage();
}

bool SoundDiffusionPage::isMultichannel()
{
	return is_multichannel;
}


SoundDiffusionAlarmPage::SoundDiffusionAlarmPage(const QDomNode &config_node, const QList<SourceDescription> &sources,
	AmplifierDevice *_general)
{
	general = _general;

	SkinContext context(getTextChild(config_node, "cid").toInt());

#ifdef LAYOUT_TS_3_5
	QWidget *main_widget = new QWidget;
	BannerContent *content = new BannerContent;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(5);

	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));
	main_layout->addWidget(icon, 0, Qt::AlignHCenter);
	main_layout->addWidget(content, 1);
	buildPage(main_widget, content, new NavigationBar);
#else
	buildPage(getTextChild(config_node, "descr"));
#endif
	loadItems(config_node, sources);
}

void SoundDiffusionAlarmPage::loadItems(const QDomNode &config_node, const QList<SourceDescription> &sources)
{
	foreach (const QDomNode &item_node, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		int id = getTextChild(item_node, "id").toInt();
		QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");

		SoundAmbientPage::Type t = SoundAmbientPage::NORMAL_AMBIENT;
		if (id == ITEM_SPECIAL_AMBIENT)
			t = SoundAmbientPage::SPECIAL_AMBIENT;

#ifdef LAYOUT_TS_10
		if (t == SoundAmbientPage::SPECIAL_AMBIENT)
			continue;
#endif

		QString descr = getTextChild(item_node, "descr");
		SoundAmbientAlarmPage *p = new SoundAmbientAlarmPage(page_node, sources, descr);
		Banner *b = getAmbientBanner(descr, t, p);

		page_content->appendBanner(b);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		connect(p, SIGNAL(saveVolumes()), SIGNAL(saveVolumes()));
	}
}

void SoundDiffusionAlarmPage::showPage()
{
	general->turnOff();

	BannerPage::showPage();
}


#ifdef LAYOUT_TS_10

LocalAmplifier::LocalAmplifier(QObject *parent) : QObject(parent)
{
	freezed_level = -1;
	state = false;
	level = localVolumeToAmplifier(bt_global::audio_states->getLocalAmplifierVolume());

	dev = bt_global::add_device_to_cache(new VirtualAmplifierDevice((*bt_global::config)[AMPLIFIER_ADDRESS]));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(audioStateChanged(int,int)));

	dev->updateStatus(state);
	dev->updateVolume(level);

	if (!(*bt_global::config)[PI_ADDRESS].isEmpty())
	{
		VideoDoorEntryDevice *vct_dev = new VideoDoorEntryDevice((*bt_global::config)[PI_ADDRESS], (*bt_global::config)[PI_MODE]);
		vct_dev = bt_global::add_device_to_cache(vct_dev);
		connect(vct_dev, SIGNAL(valueReceived(DeviceValues)), SLOT(vctValueReceived(DeviceValues)));
	}
}

void LocalAmplifier::vctValueReceived(const DeviceValues &values_list)
{
	// Because the "silence frame" can arrive more times we simply ignore it if
	// we have already silenced the local amplifier.
	if (freezed_level == -1 && values_list.contains(VideoDoorEntryDevice::SILENCE_MM_AMPLI))
	{
		//  We want the same behaviour than the actual amplifier
		const int scs_silenced_level = 1;
		dev->updateVolume(scs_silenced_level);
		// If not arrived the frame that close the videodoorentry call, we have to wait 15 minutes (like the others amplifiers),
		// after which we restore the volume of the local amplifier.
		vct_end_call_frame = false;
		QTimer::singleShot(RESTORE_VOLUME_SECS * 1000, this, SLOT(restoreVolume()));

		// The freezed_level is used both to save and restore the volume of the
		// local amplifier and to mark the "silenced state".
		freezed_level = level;

		// If we aren't in the DIFSON state the local amplifier is turned off, so
		// we only notify the volume.
		if (bt_global::audio_states->currentState() == AudioStates::PLAY_DIFSON)
			bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(scs_silenced_level));
	}
	else if (freezed_level > -1 && values_list.contains(VideoDoorEntryDevice::RESTORE_MM_AMPLI))
	{
		if (bt_global::audio_states->currentState() != AudioStates::PLAY_DIFSON)
			dev->updateVolume(0);
		else
		{
			level = freezed_level; // we have to restore the level, because transitions between audio states can change it
			bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(freezed_level));
			dev->updateVolume(freezed_level);
		}
		vct_end_call_frame = true;
		freezed_level = -1;
	}
}

void LocalAmplifier::restoreVolume()
{
	if (!vct_end_call_frame)
	{
		if (bt_global::audio_states->currentState() != AudioStates::PLAY_DIFSON)
		{
			dev->updateVolume(0);
		}
		else
		{
			level = freezed_level;
			bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(freezed_level));
			dev->updateVolume(freezed_level);
		}
		vct_end_call_frame = true;
		freezed_level = -1;
	}
}

void LocalAmplifier::audioStateChanged(int new_state, int old_state)
{
	if (old_state != AudioStates::PLAY_DIFSON && new_state != AudioStates::PLAY_DIFSON)
		return;

	// re-read the status, since state transitions might change it
	state = bt_global::audio_states->getLocalAmplifierStatus();

	if (state)
		dev->updateVolume(bt_global::audio_states->currentState() == AudioStates::PLAY_DIFSON ? level : 0);
}

void LocalAmplifier::valueReceived(const DeviceValues &device_values)
{
	foreach (int key, device_values.keys())
	{
		switch (key)
		{
		case VirtualAmplifierDevice::REQ_AMPLI_ON:
		{
			bool new_state = device_values[key].toBool();

			// refuse to activate amplifier when playing locally
			if (new_state && bt_global::audio_states->contains(AudioStates::PLAY_MEDIA_TO_SPEAKER))
			{
				qDebug() << "Not activating amplifier from bus";
				if (device_values.contains(VirtualAmplifierDevice::DIM_SELF_REQUEST))
					dev->turnOff();
				else
					dev->updateStatus(state);

				return;
			}

			if (state != new_state)
			{
				// assign it here so the audioStateChanged() notification sees the correct state
				state = new_state;
				bt_global::audio_states->setLocalAmplifierStatus(new_state);
			}

			dev->updateStatus(state);
			// in some cases this and audioStateChanged() will send the volume frame twice; this is OK
			// since they are de-duplicated by the openclient
			if (state)
				dev->updateVolume(level);
			break;
		}
		case VirtualAmplifierDevice::REQ_VOLUME_UP:
			if (state && level < 31 && freezed_level == -1)
			{
				level += 1;
				dev->updateVolume(level);
				bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(level));
			}
			break;
		case VirtualAmplifierDevice::REQ_VOLUME_DOWN:
			if (state && level > 0 && freezed_level == -1)
			{
				level -= 1;
				dev->updateVolume(level);
				bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(level));
			}
			break;
		case VirtualAmplifierDevice::REQ_SET_VOLUME:
			if (state && freezed_level == -1)
			{
				level = device_values[key].toInt();
				dev->updateVolume(level);
				bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(level));
			}
			break;
		case VirtualAmplifierDevice::REQ_TEMPORARY_OFF:
			bt_global::audio_states->setLocalAmplifierTemporaryOff(true);
			QTimer::singleShot(TEMPORARY_OFF_TIMEOUT, this,  SLOT(reenableLocalAmplifier()));
			break;
		}
	}
}

void LocalAmplifier::reenableLocalAmplifier()
{
	bt_global::audio_states->setLocalAmplifierTemporaryOff(false);
}

#endif

LocalSource::LocalSource(QObject *parent) : QObject(parent)
{
	source_status = false;
	dev = bt_global::add_device_to_cache(new VirtualSourceDevice((*bt_global::config)[SOURCE_ADDRESS]));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void LocalSource::startLocalPlayback(bool force)
{
	foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
	{
		if (!page)
			continue;

		if (page->isPlayerPaused())
		{
			page->resume();
			return;
		}
		else if (page->isPlayerInstanceRunning())
			return;
	}

#ifdef LAYOUT_TS_10
	// try to play something searching on each media source
	// using the multimedia configuration order (es. usb -> sd -> ip radio)
	if (force)
		MultimediaSectionPage::playSomethingRandomly();
#else
	if (force)
		MultimediaContainer::playSomethingRandomly();
#endif
}

void LocalSource::pauseLocalPlayback()
{
	foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
	{
		if (!page)
			continue;

		if (page->isPlayerInstanceRunning() && !page->isPlayerPaused())
		{
			page->pause();
			return;
		}
	}
}

void LocalSource::valueReceived(const DeviceValues &device_values)
{
	foreach (int key, device_values.keys())
	{
		switch (key)
		{
		case VirtualSourceDevice::REQ_SOURCE_ON:
		case VirtualSourceDevice::REQ_SOURCE_OFF:
		{
			bool new_state = (key == VirtualSourceDevice::REQ_SOURCE_ON);

			if (source_status != new_state)
			{
				source_status = new_state;
#ifdef LAYOUT_TS_10
				bt_global::audio_states->setLocalSourceStatus(new_state);
#else // LAYOUT_TS_3_5
				if (new_state)
					activateLocalSource();
				else
					deactivateLocalSource();
#endif
			}

			if (new_state)
				startLocalPlayback(!device_values.contains(VirtualSourceDevice::DIM_SELF_REQUEST));
			else if (!new_state)
				pauseLocalPlayback();
			break;
		}
		case SourceDevice::DIM_AREAS_UPDATED:
		{
			bool status = dev->isActive();

			if (!status)
				pauseLocalPlayback();

			break;
		}
		case VirtualSourceDevice::REQ_NEXT_TRACK:
		{
			foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
				if (page && page->isPlayerInstanceRunning())
					page->next();
			break;
		}
		case VirtualSourceDevice::REQ_PREV_TRACK:
		{
			foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
				if (page && page->isPlayerInstanceRunning())
					page->previous();
			break;
		}
		}
	}
}
