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


#include <QDomNode>
#include <QGridLayout>
#include <QLabel>

SoundAmbient::SoundAmbient() :
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




enum Items
{
	ITEM_SPECIAL_AMBIENT = 1700,              // special ambient
	ITEM_AMBIENT = 1701,                      // normal ambient (with a zone)
};

SoundDiffusionPage::SoundDiffusionPage(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int SoundDiffusionPage::sectionId()
{
	return DIFSON_MULTI;
}

void SoundDiffusionPage::loadItems(const QDomNode &config_node)
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
	}
}

banner *SoundDiffusionPage::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	banner *b = 0;
	switch (id)
	{
	case ITEM_AMBIENT:
	{
		SoundAmbient *bann = new SoundAmbient;
		QString amb_icon = bt_global::skin->getImage("ambient");
		amb_icon = getBostikName(amb_icon, getTextChild(item_node, "env"));
		bann->initBanner(amb_icon, bt_global::skin->getImage("amplifier"), bt_global::skin->getImage("forward"),
			getTextChild(item_node, "descr"));
		b = bann;
	}
		break;
	case ITEM_SPECIAL_AMBIENT:
	{
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("amplifier"), bt_global::skin->getImage("forward"),
			getTextChild(item_node, "descr"));
		b = bann;
	}
	}
	return b;
}
