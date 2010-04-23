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
#include "main.h"
#include "xml_functions.h"
#include "bann2_buttons.h" // Bann2Buttons
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName
#include "icondispatcher.h" // bt_global::icons_cache
#include "bannercontent.h"
#include "bann_amplifiers.h" // Amplifier
#include "poweramplifier.h" // BannPowerAmplifier
#include "sorgentiradio.h" // RadioSource
#include "pagestack.h"

#include <QDomNode>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>


Page *SoundDiffusionPage::sound_diffusion_page;
Page *SoundAmbientPage::current_ambient_page;

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


enum BannerType
{
	AMPLIFIER = 11020,
	AMPLIFIER_GROUP = 11021,
	BANN_POWER_AMPLIFIER = 11022,
};

SoundAmbientPage::SoundAmbientPage(const QDomNode &conf_node, const QList<SourceDescription> &sources)
{
	if (getTextChild(conf_node, "id").toInt() == DIFSON_MONO)
		section_id = DIFSON_MONO;
	else
		section_id = NO_SECTION;

	QWidget *top_widget = 0;
	// this handles the case for special ambient, which must not show sources
	if (!sources.isEmpty())
	{
		// TODO: top widget should be a stackedWidget
		SkinContext ctx(sources.at(0).cid);
		// TODO: correctly create the top widget
		top_widget = new RadioSource;
	}
	buildPage(getTextChild(conf_node, "descr"), Page::TITLE_HEIGHT, top_widget);
	loadItems(conf_node);
}

int SoundAmbientPage::sectionId()
{
	return section_id;
}

void SoundAmbientPage::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
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

	bt_global::page_stack.showUserPage(this);
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
		b = new BannPowerAmplifierNew(descr, item_node, where, oid);
		break;
	}
	return b;
}

Page *SoundAmbientPage::currentAmbientPage()
{
	return current_ambient_page;
}


enum Items
{
	ITEM_SPECIAL_AMBIENT = 12020,              // special ambient
	ITEM_AMBIENT = 12021,                      // normal ambient (with a zone)
};

SoundDiffusionPage::SoundDiffusionPage(const QDomNode &config_node)
{
	next_page = NULL;

	buildPage(getTextChild(config_node, "descr"));
	if (getTextChild(config_node, "id").toInt() == DIFSON_MULTI)
		loadItemsMulti(config_node);
	else
		loadItemsMono(config_node);

	sound_diffusion_page = this;
}

int SoundDiffusionPage::sectionId()
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
		sources_list << d;
	}
	Q_ASSERT_X(!sources_list.isEmpty(), "SoundDiffusionPage::loadItems", "No sound diffusion sources defined.");

	return sources_list;
}

void SoundDiffusionPage::loadItemsMulti(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QList<SourceDescription> sources_list = loadSources(config_node);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = getAmbientBanner(item, sources_list);
		if (b)
			page_content->appendBanner(b);
		else
			qFatal("ID %s not handled in SoundDiffusionPage", qPrintable(getTextChild(item, "id")));
	}
}

void SoundDiffusionPage::loadItemsMono(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QList<SourceDescription> sources_list = loadSources(config_node);
	next_page = new SoundAmbientPage(config_node, sources_list);
	connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

banner *SoundDiffusionPage::getAmbientBanner(const QDomNode &item_node, const QList<SourceDescription> &sources)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");

	banner *b = 0;
	switch (id)
	{
	case ITEM_AMBIENT:
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
	// TODO otherwise it remains connected to SectionPage::showPage
	disconnect(SIGNAL(Closed()), 0, 0);

	if (next_page)
	{
		next_page->showPage();
	}
	else
	{
		bt_global::page_stack.showUserPage(this);
		BannerPage::showPage();
	}
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
