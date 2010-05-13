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
#include "main.h" // bt_global::config, Section
#include "xml_functions.h"
#include "bann2_buttons.h" // Bann2Buttons
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName, trasformaVol
#include "icondispatcher.h" // bt_global::icons_cache
#include "bann_amplifiers.h" // Amplifier
#include "poweramplifier.h" // BannPowerAmplifier
#include "sorgentiradio.h" // RadioSource
#include "sorgentiaux.h" // AuxSource
#include "sorgentimedia.h" // MultimediaSource
#include "media_device.h"
#include "devices_cache.h"
#include "multimedia.h" // MultimediaSectionPage
#include "multimedia_filelist.h"
#include "radio.h" // RadioPage
#include "navigation_bar.h"
#include "audiostatemachine.h"

#include <QDomNode>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>
#include <QStackedWidget>


bool SoundDiffusionPage::is_source = false, SoundDiffusionPage::is_amplifier = false, SoundDiffusionPage::is_multichannel = false;
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
	text->setText(banner_text);
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
	SOURCE_MYSELF_MONO = 11004,
	SOURCE_RADIO_MULTI = 12001,
	SOURCE_AUX_MULTI = 12002,
	SOURCE_MULTIMEDIA_MULTI = 12003,
	SOURCE_MYSELF_MULTI = 12004,
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

			w = new RadioSource(area, dev, s.details);
			break;
		}
		case SOURCE_AUX_MONO:
		case SOURCE_AUX_MULTI:
		case SOURCE_MULTIMEDIA_MONO:
		case SOURCE_MULTIMEDIA_MULTI:
		{
			SourceDevice *dev = bt_global::add_device_to_cache(new SourceDevice(s.where));

			w = new AuxSource(area, dev, s.descr);
			break;
		}
		case SOURCE_MYSELF_MONO:
		case SOURCE_MYSELF_MULTI:
		{
			if (!s.details)
				s.details = new MultimediaSectionPage(getPageNode(MULTIMEDIA),
								      MultimediaSectionPage::ITEMS_AUDIO,
								      new MultimediaFileListPage(getFileFilter(AUDIO)));

			VirtualSourceDevice *dev = bt_global::add_device_to_cache(new VirtualSourceDevice(s.where));

			w = new MediaSource(area, dev, s.descr, s.details);
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

	if (getTextChild(conf_node, "id").toInt() == DIFSON_MONO)
	{
		section_id = DIFSON_MONO;
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
	Q_ASSERT_X(current_ambient_page == this, "SoundAmbientPage::cleanUp", "Something terrible happened");

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
	case AMPLIFIER_GROUP:
		b = new AmplifierGroup(getAddresses(getChildWithName(item_node, "addresses")), descr);
		break;
	case BANN_POWER_AMPLIFIER:
		b = new BannPowerAmplifier(descr, item_node, where, oid);
		break;
	}
	return b;
}

Page *SoundAmbientPage::currentAmbientPage()
{
	return current_ambient_page;
}


SoundAmbientAlarmPage::SoundAmbientAlarmPage(const QDomNode &conf_node, const QList<SourceDescription> &sources)
{
	SkinContext context(getTextChild(conf_node, "cid").toInt());
	QString area;

	if (getTextChild(conf_node, "id").toInt() == DIFSON_MONO)
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
	BannerContent *content = new BannerContent;
	NavigationBar *nav_bar = new NavigationBar;
	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SIGNAL(saveVolumes()));

	l->addWidget(top_widget);
	l->addWidget(content, 1);
	l->addWidget(ok, 0, Qt::AlignRight);

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
	is_source = !(*bt_global::config)[SOURCE_ADDRESS].isEmpty();
	is_amplifier = !(*bt_global::config)[AMPLIFIER_ADDRESS].isEmpty();
	is_multichannel = getTextChild(config_node, "id").toInt() == DIFSON_MULTI;

	buildPage(getTextChild(config_node, "descr"));
	if (is_multichannel)
		loadItemsMulti(config_node);
	else
		loadItemsMono(config_node);

	sound_diffusion_page = this;

	if (is_source || is_amplifier)
	{
		QString init_frame = VirtualSourceDevice::createMediaInitFrame(is_multichannel,
									       (*bt_global::config)[SOURCE_ADDRESS],
									       (*bt_global::config)[AMPLIFIER_ADDRESS]);
		bt_global::devices_cache.addInitCommandFrame(0, init_frame);
	}

	if (is_amplifier)
		new LocalAmplifier(this);
}

int SoundDiffusionPage::sectionId() const
{
	return DIFSON_MULTI;
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

	alarm_clock_page = new SoundDiffusionAlarmPage(config_node, sources_list);
}

void SoundDiffusionPage::loadItemsMono(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QList<SourceDescription> sources_list = loadSources(config_node);
	next_page = new SoundAmbientPage(config_node, sources_list);
	connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));

	alarm_clock_page = new SoundAmbientAlarmPage(config_node, sources_list);
}

banner *SoundDiffusionPage::getAmbientBanner(const QDomNode &item_node, const QList<SourceDescription> &sources)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");

	banner *b = 0;
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
		SoundAmbient *bann = new SoundAmbient(getTextChild(item_node, "descr"), getTextChild(item_node, "env"));
		SoundAmbientPage *p = new SoundAmbientPage(page_node, sources);
		bann->connectRightButton(p);
		b = bann;
	}
		break;
	case ITEM_SPECIAL_AMBIENT:
	{
		Bann2Buttons *bann = new Bann2Buttons;
		SoundAmbientPage *p = new SoundAmbientPage(page_node);
		bann->initBanner(QString(), bt_global::skin->getImage("amplifier"), bt_global::skin->getImage("forward"),
			getTextChild(item_node, "descr"));
		bann->connectRightButton(p);
		b = bann;
	}
	}
	return b;
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

bool SoundDiffusionPage::isSource()
{
	return is_source;
}

bool SoundDiffusionPage::isAmplifier()
{
	return is_amplifier;
}

bool SoundDiffusionPage::isMultichannel()
{
	return is_multichannel;
}


SoundDiffusionAlarmPage::SoundDiffusionAlarmPage(const QDomNode &config_node, const QList<SourceDescription> &sources)
{
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


LocalAmplifier::LocalAmplifier(QObject *parent) : QObject(parent)
{
	state = false;
	// TODO add a method to read current volume w/o changing the machine state
	level = 16;

	dev = bt_global::add_device_to_cache(new VirtualAmplifierDevice((*bt_global::config)[AMPLIFIER_ADDRESS]));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	dev->updateStatus(state);
	dev->updateVolume(level);
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

			if (state != new_state)
			{
				if (new_state)
					bt_global::audio_states->toState(AudioStates::PLAY_FROM_DIFSON_TO_SPEAKER);
				else
					// TODO need a "removeState" or something
					bt_global::audio_states->exitCurrentState();
			}

			state = new_state;
			dev->updateStatus(state);
			if (state)
				dev->updateVolume(level);
			break;
		}
		case VirtualAmplifierDevice::REQ_VOLUME_UP:
			if (level < 31)
			{
				level += 1;
				dev->updateVolume(level);
				if (bt_global::audio_states->currentState() == AudioStates::PLAY_FROM_DIFSON_TO_SPEAKER)
					bt_global::audio_states->setVolume(trasformaVol(level));
			}
			break;
		case VirtualAmplifierDevice::REQ_VOLUME_DOWN:
			if (level > 0)
			{
				level -= 1;
				dev->updateVolume(level);
				if (bt_global::audio_states->currentState() == AudioStates::PLAY_FROM_DIFSON_TO_SPEAKER)
					bt_global::audio_states->setVolume(trasformaVol(level));
			}
			break;
		case VirtualAmplifierDevice::REQ_SET_VOLUME:
			level = device_values[key].toInt();
			dev->updateVolume(level);
			if (bt_global::audio_states->currentState() == AudioStates::PLAY_FROM_DIFSON_TO_SPEAKER)
				bt_global::audio_states->setVolume(trasformaVol(level));
			break;
		}
	}
}
