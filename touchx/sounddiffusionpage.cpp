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
#include "main.h" // Section
#include "xml_functions.h"
#include "bann2_buttons.h" // Bann2Buttons
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName, scsToLocalVolume
#include "icondispatcher.h" // bt_global::icons_cache
#include "bann_amplifiers.h" // Amplifier
#include "poweramplifier.h" // BannPowerAmplifier, PowerAmplifierPage
#include "audiosource.h" // RadioSource, AuxSource, MediaSource
#include "media_device.h"
#include "devices_cache.h"
#include "multimedia.h" // MultimediaSectionPage
#include "multimedia_filelist.h"
#include "radio.h" // RadioPage
#include "navigation_bar.h"
#include "audiostatemachine.h"
#include "labels.h" // ScrollingLabel
#include "audioplayer.h"
#include "entryphone_device.h"

#include <QDomNode>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>
#include <QStackedWidget>

#define TEMPORARY_OFF_TIMEOUT 1000

bool SoundDiffusionPage::is_multichannel = false;
Page *SoundDiffusionPage::sound_diffusion_page = NULL;
Page *SoundDiffusionPage::alarm_clock_page = NULL;
Page *SoundAmbientPage::current_ambient_page = NULL;

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
}

SoundAmbient::SoundAmbient(const QString &descr, const QString &ambient) :
	BannerNew(0)
{
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;
	ambient_icon = new QLabel;

	QGridLayout *center = new QGridLayout;
	center->addWidget(ambient_icon, 0, 0);
	center->addWidget(center_icon, 0, 1);

	QGridLayout *grid = new QGridLayout;
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	grid->setColumnStretch(0, 1);
	grid->setColumnStretch(2, 1);
	grid->addLayout(center, 0, 1);
	grid->addWidget(right_button, 0, 2);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(grid);
	l->addWidget(text);

	QString amb_icon = bt_global::skin->getImage("ambient");
	amb_icon = getBostikName(amb_icon, ambient);
	initBanner(amb_icon, bt_global::skin->getImage("amplifier"), bt_global::skin->getImage("forward"), descr);
}

void SoundAmbient::initBanner(const QString &_ambient_icon, const QString &_center_icon,
	const QString &right, const QString &banner_text)
{
	right_button->setImage(right);
	ambient_icon->setPixmap(*bt_global::icons_cache.getIcon(_ambient_icon));
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(_center_icon));
	text->setScrollingText(banner_text);
}

void SoundAmbient::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}


enum
{
	SOURCE_RADIO_MONO = 11001,
	SOURCE_AUX_MONO = 11002,
	SOURCE_MULTIMEDIA_MONO = 11003,
	SOURCE_RADIO_MULTI = 12001,
	SOURCE_AUX_MULTI = 12002,
	SOURCE_MULTIMEDIA_MULTI = 12003,
};

SoundSources::SoundSources(const QString &area, const QList<SourceDescription> &src)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(18, 0, 17, 10);
	l->setSpacing(5);

	BtButton *cycle = new BtButton(bt_global::skin->getImage("cycle"));
	sources = new QStackedWidget;

	l->addWidget(cycle);
	l->addWidget(sources);

	foreach (const SourceDescription &s, src)
	{
		SkinContext ctx(s.cid);
		AudioSource *w = NULL;

		switch (s.id)
		{
		case SOURCE_RADIO_MONO:
		case SOURCE_RADIO_MULTI:
		{
			RadioSourceDevice *dev = bt_global::add_device_to_cache(new RadioSourceDevice(s.where));
			if (!s.details)
				s.details = new RadioPage(dev);

			w = new RadioSource(area, dev, static_cast<RadioPage*>(s.details));
			break;
		}
		case SOURCE_AUX_MONO:
		case SOURCE_AUX_MULTI:
		case SOURCE_MULTIMEDIA_MONO:
		case SOURCE_MULTIMEDIA_MULTI:
		{
			if ((s.id == SOURCE_MULTIMEDIA_MONO || s.id == SOURCE_MULTIMEDIA_MULTI) &&
			    s.where == (*bt_global::config)[SOURCE_ADDRESS])
			{
				if (!s.details)
					s.details = new MultimediaSectionPage(getPageNode(MULTIMEDIA),
									      MultimediaSectionPage::ITEMS_AUDIO,
									      new MultimediaFileListPage(getFileFilter(AUDIO)));

				VirtualSourceDevice *dev = bt_global::add_device_to_cache(new VirtualSourceDevice(s.where));

				w = new MediaSource(area, dev, s.descr, s.details);
			}
			else
			{
				SourceDevice *dev = bt_global::add_device_to_cache(new SourceDevice(s.where));

				w = new AuxSource(area, dev, s.descr);
			}

			break;
		}
		default:
			qWarning() << "Ignoring source" << s.id;
			continue;
		};

		sources->addWidget(w);
		connect(w, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
		connect(w, SIGNAL(pageClosed()), SIGNAL(pageClosed()));
	}

	connect(cycle, SIGNAL(clicked()), SLOT(sourceCycle()));
}

void SoundSources::hideEvent(QHideEvent *)
{
	for (int i = 0; i < sources->count(); ++i)
		static_cast<AudioSource*>(sources->widget(i))->sourceHidden();
}

void SoundSources::showEvent(QShowEvent *)
{
	for (int i = 0; i < sources->count(); ++i)
		static_cast<AudioSource*>(sources->widget(i))->sourceShowed();
}

void SoundSources::sourceCycle()
{
	int index = sources->currentIndex();
	int next = (index + 1) % sources->count();

	sources->setCurrentIndex(next);
}

void SoundSources::sourceStateChanged(bool active)
{
	if (!active)
		return;

	AudioSource *source = static_cast<AudioSource*>(sender());

	sources->setCurrentWidget(source);
}


enum BannerType
{
	AMPLIFIER = 11020,
	AMPLIFIER_GROUP = 11021,
	BANN_POWER_AMPLIFIER = 11022,
};

SoundAmbientPage::SoundAmbientPage(const QDomNode &conf_node, const QList<SourceDescription> &sources)
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
	if (!sources.isEmpty())
	{
		top_widget = new SoundSources(area, sources);
		connect(top_widget, SIGNAL(pageClosed()), SLOT(showPage()));
	}

	buildPage(getTextChild(conf_node, "descr"), Page::TITLE_HEIGHT, top_widget);
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
		banner *b = getBanner(item);
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
	Q_ASSERT_X(!current_ambient_page || current_ambient_page == this, "SoundAmbientPage::cleanUp", "Something terrible happened");

	current_ambient_page = NULL;
}

void SoundAmbientPage::clearCurrentAmbient()
{
	Q_ASSERT_X(!current_ambient_page || current_ambient_page == this, "SoundAmbientPage::clearCurrentAmbient", "There is no end to terrible things");

	current_ambient_page = NULL;
}

banner *SoundAmbientPage::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString descr = getTextChild(item_node, "descr");
	QString where = getTextChild(item_node, "where");
	int oid = getTextChild(item_node, "openserver_id").toInt();

	banner *b = 0;
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
					     AmplifierDevice *_general)
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
		if (s.id == SOURCE_RADIO_MONO || s.id == SOURCE_RADIO_MULTI ||
		    s.id == SOURCE_AUX_MONO || s.id == SOURCE_AUX_MULTI)
			filtered_sources.append(s);

	SoundSources *top_widget = new SoundSources(area, filtered_sources);
	connect(top_widget, SIGNAL(pageClosed()), SLOT(showPage()));

	QWidget *main_widget = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(main_widget);
	QHBoxLayout *b = new QHBoxLayout;
	l->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TOUCHX
	b->setContentsMargins(18, 0, 17, 0);
#endif
	BannerContent *content = new BannerContent;
	NavigationBar *nav_bar = new NavigationBar;
	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SIGNAL(saveVolumes()));

	b->addWidget(ok, 0, Qt::AlignRight);

	l->addWidget(top_widget);
	l->addWidget(content, 1);
	l->addLayout(b);

	buildPage(main_widget, content, nav_bar, getTextChild(conf_node, "descr"), Page::SMALL_TITLE_HEIGHT);
	loadItems(conf_node);
}

void SoundAmbientAlarmPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = SoundAmbientPage::getBanner(item);
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


enum Items
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

	if (bt_global::audio_states->isSource() || bt_global::audio_states->isAmplifier())
	{
		QString init_frame = VirtualSourceDevice::createMediaInitFrame(is_multichannel,
									       (*bt_global::config)[SOURCE_ADDRESS],
									       (*bt_global::config)[AMPLIFIER_ADDRESS]);
		bt_global::devices_cache.addInitCommandFrame(0, init_frame);
	}

	if (bt_global::audio_states->isAmplifier())
		new LocalAmplifier(this);
	if (bt_global::audio_states->isSource())
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
		d.id = getTextChild(source, "id").toInt();
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
		banner *b = getAmbientBanner(item, sources_list);
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

	alarm_clock_page = new SoundAmbientAlarmPage(config_node, sources_list, AmplifierDevice::createDevice("0"));
}

banner *SoundDiffusionPage::getAmbientBanner(const QDomNode &item_node, const QList<SourceDescription> &sources)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");

	Bann2Buttons *bann = new Bann2Buttons;
	bann->initBanner(QString(), bt_global::skin->getImage("amplifier"), bt_global::skin->getImage("forward"),
		getTextChild(item_node, "descr"));

	switch (id)
	{
	case ITEM_AMBIENT_1:
	case ITEM_AMBIENT_2:
	case ITEM_AMBIENT_3:
	case ITEM_AMBIENT_4:
	case ITEM_AMBIENT_5:
	case ITEM_AMBIENT_6:
	case ITEM_AMBIENT_7:
	case ITEM_AMBIENT_8:
	{
		SoundAmbientPage *p = new SoundAmbientPage(page_node, sources);
		bann->connectRightButton(p);
	}
		break;
	case ITEM_SPECIAL_AMBIENT:
	{
		SoundAmbientPage *p = new SoundAmbientPage(page_node);
		bann->connectRightButton(p);

		break;
	}
	default:
		qFatal("ID %s not handled in SoundDiffusionAlarmPage", qPrintable(getTextChild(item_node, "id")));
	}

	return bann;
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

	Q_ASSERT_X(current_ambient_page || sound_diffusion_page, "SoundDiffusionPage::showCurrentAmbientPage", "Terrible stuff keeps on happening");

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

	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node, sources);
}

void SoundDiffusionAlarmPage::loadItems(const QDomNode &config_node, const QList<SourceDescription> &sources)
{
	foreach (const QDomNode &item_node, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		int id = getTextChild(item_node, "id").toInt();
		QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");

		if (id == ITEM_SPECIAL_AMBIENT)
			continue;
		if (id < ITEM_AMBIENT_1 || id > ITEM_AMBIENT_8)
			qFatal("ID %s not handled in SoundDiffusionAlarmPage", qPrintable(getTextChild(item_node, "id")));

		SoundAmbient *b = new SoundAmbient(getTextChild(item_node, "descr"), getTextChild(item_node, "env"));
		SoundAmbientAlarmPage *p = new SoundAmbientAlarmPage(page_node, sources);
		b->connectRightButton(p);

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
		EntryphoneDevice *vct_dev = new EntryphoneDevice((*bt_global::config)[PI_ADDRESS], (*bt_global::config)[PI_MODE]);
		vct_dev = bt_global::add_device_to_cache(vct_dev);
		connect(vct_dev, SIGNAL(valueReceived(DeviceValues)), SLOT(vctValueReceived(DeviceValues)));
	}
}

void LocalAmplifier::vctValueReceived(const DeviceValues &values_list)
{
	// Because the "silence frame" can arrive more times we simply ignore it if
	// we have already silenced the local amplifier.
	if (freezed_level == -1 && values_list.contains(EntryphoneDevice::SILENCE_MM_AMPLI))
	{
		//  We want the same behaviour than the actual amplifier
		const int scs_silenced_level = 1;
		dev->updateVolume(scs_silenced_level);

		// The freezed_level is used both to save and restore the volume of the
		// local amplifier and to mark the "silenced state".
		freezed_level = level;

		// If we aren't in the DIFSON state the local amplifier is turned off, so
		// we only notify the volume.
		if (bt_global::audio_states->currentState() == AudioStates::PLAY_DIFSON)
			bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(scs_silenced_level));
	}
	else if (freezed_level > -1 && values_list.contains(EntryphoneDevice::RESTORE_MM_AMPLI))
	{
		if (bt_global::audio_states->currentState() != AudioStates::PLAY_DIFSON)
			dev->updateVolume(0);
		else
		{
			level = freezed_level; // we have to restore the level, because transitions between audio states can change it
			bt_global::audio_states->setLocalAmplifierVolume(scsToLocalVolume(freezed_level));
			dev->updateVolume(freezed_level);
		}
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


LocalSource::LocalSource(QObject *parent) : QObject(parent)
{
	dev = bt_global::add_device_to_cache(new VirtualSourceDevice((*bt_global::config)[SOURCE_ADDRESS]));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void LocalSource::startLocalPlayback(bool force)
{
	foreach (MediaPlayerPage *page, AudioPlayerPage::audioPlayerPages())
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

	// try to play something searching on each media source
	// using the multimedia configuration order (es. usb -> sd -> ip radio)
	if (force)
		MultimediaSectionPage::playSomethingRandomly();
}

void LocalSource::pauseLocalPlayback()
{
	foreach (MediaPlayerPage *page, AudioPlayerPage::audioPlayerPages())
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
			bool new_state = key == VirtualSourceDevice::REQ_SOURCE_ON;
			bool state = bt_global::audio_states->getLocalSourceStatus();

			if (state != new_state)
				bt_global::audio_states->setLocalSourceStatus(new_state);

			if (new_state)
				startLocalPlayback(!device_values.contains(VirtualSourceDevice::DIM_SELF_REQUEST));
			else if (!new_state)
				pauseLocalPlayback();

			state = new_state;
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
