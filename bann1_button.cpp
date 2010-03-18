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


#include "bann1_button.h"
#include "titlelabel.h" // TextOnImageLabel
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "fontmanager.h" //FontManager
#include "generic_functions.h" // getBostikName

#include <QHBoxLayout>
#include <QDebug>

#define BUT_DIM 60
#define BUTONDX_H_SCRITTA 20
#define BANNONICONS_ICON_DIM 60
#define BANON2SCR_TEXT1_DIM_X 100
#define BANON2SCR_TEXT1_DIM_Y 60
#define BUTON2SCR_ICON_DIM_X 40
#define BUTON2SCR_ICON_DIM_Y 60
#define BANNBUT2ICON_TEXT_DIM_X 240
#define BANNBUT2ICON_TEXT_DIM_Y 20
#define BANNBUT2ICON_ICON_DIM_X 60
#define BANNBUT2ICON_ICON_DIM_Y 60

BannSinglePuls::BannSinglePuls(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	connect(right_button, SIGNAL(clicked()), SIGNAL(rightClick()));
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center = new TextOnImageLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(center, 1, Qt::AlignRight);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannSinglePuls::initBanner(const QString &right, const QString &_center, const QString &banner_text)
{
	right_button->setImage(right);
	center->setBackgroundImage(_center);
	text->setText(banner_text);
	// always set a text on the label, otherwise the sizeHint() height changes
	if (banner_text.isEmpty())
		text->setText(" ");
}

void BannSinglePuls::setCentralText(const QString &t)
{
	center->setInternalText(t);
}

void BannSinglePuls::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}



BannCenteredButton::BannCenteredButton(QWidget *parent) :
	BannerNew(parent)
{
	center_button = new BtButton;
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(center_button, 0, Qt::AlignTop);
}

void BannCenteredButton::initBanner(const QString &center)
{
	center_button->setImage(center);
}

void BannCenteredButton::connectButton(Page *linked_page)
{
	connectButtonToPage(center_button, linked_page);
}


BannSimple::BannSimple(const QString &icon)
	: BannCenteredButton(0)
{
	banner_height = BUT_DIM;
	initBanner(icon);
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}


BannOn2Labels::BannOn2Labels(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::TEXT));
	left_icon = new QLabel;
	right_icon = new QLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(center_text, 1, Qt::AlignCenter);
	hbox->addWidget(left_icon);
	hbox->addWidget(right_icon);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannOn2Labels::initBanner(const QString &right, const QString &_right_icon, const QString &_left_icon,
		const QString &banner_text, const QString &_center_text)
{
	right_button->setImage(right);
	center_right = _right_icon;
	center_left = _left_icon;
	text->setText(banner_text);
	center_text->setText(_center_text);
	setState(OFF);
	setElapsedTime(0);
}

void BannOn2Labels::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		right_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_right, "on")));
		break;
	case OFF:
		left_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_left, "0")));
		right_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_right, "off")));
		break;
	}
}

void BannOn2Labels::setElapsedTime(int time)
{
	if (time >= 1 && time <= 8)
		left_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_left,
			QString::number(time))));
	// TODO: should we also set the light on??
}


bannPuls::bannPuls(QWidget *parent) : BannerOld(parent)
{
	addItem(BUT1, banner_width - BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	addItem(ICON,BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(TEXT, 0, BANPULS_BUT_DIM, banner_width, banner_height - BANPULS_BUT_DIM);

	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
	connect(this,SIGNAL(sxPressed()),this,SIGNAL(pressed()));
	connect(this,SIGNAL(sxReleased()),this,SIGNAL(released()));
}


bannOnDx::bannOnDx(QWidget *parent, QString icon, Page *page) : BannerOld(parent)
{
	banner_height = BUT_DIM;
	addItem(BUT1, banner_width - BUT_DIM, (banner_height - BUT_DIM)/2, BUT_DIM, BUT_DIM);
	addItem(TEXT, 0, 0, banner_width-BUT_DIM, banner_height);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnSx::bannOnSx(QWidget *parent, QString icon) : BannerOld(parent)
{
	banner_height = BUT_DIM;
	addItem(BUT1, 0, (banner_height - BUT_DIM)/2, BUT_DIM, BUT_DIM);
	addItem(TEXT, BUT_DIM, 0, banner_width - BUT_DIM, banner_height);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);
	Draw();
}

BtButton *bannOnSx::getButton()
{
	return sxButton;
}


#if 0

bannOnIcons::bannOnIcons(QWidget *parent) : BannerOld(parent)
{
	addItem(BUT1, 0, 0, BUT_DIM, BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height-BUT_DIM);
	addItem(ICON, BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(ICON2, 2*BUT_DIM, 0, BUT_DIM, BUT_DIM);
}

#endif


bannBut2Icon::bannBut2Icon(QWidget *parent) : BannerOld(parent)
{
	addItem(BUT1, banner_width - BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(ICON, BUT_DIM, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(ICON2, BUT_DIM + BANNBUT2ICON_ICON_DIM_X, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height - BUT_DIM);
}


BannStates::BannStates(QWidget *parent) : BannerNew(parent)
{
	banner_height = BUT_DIM;

	left_button = new BtButton;
	current_index = 0;
	text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, Qt::AlignLeft);
	l->addWidget(text, 1, Qt::AlignHCenter);
	connect(left_button, SIGNAL(clicked()), SLOT(changeState()));
}

void BannStates::addState(int id, QString descr)
{
	states_list.append(qMakePair(id, descr));
}

void BannStates::initBanner(const QString &left, int current_state)
{
	Q_ASSERT_X(states_list.size() > 0, "BannStates::initBanner", "The list of states is empty!");
	left_button->setImage(left);

	setCurrentState(current_state);
}

// To maintain the same semantics of currentState(), we need to search
// the new state in the whole array.
void BannStates::setCurrentState(int new_state)
{
	for (int i = 0; i < states_list.size(); ++i)
		if (states_list[i].first == new_state)
		{
			// we can't use new_state because it can be an invalid index
			// for example if we set state 5 but we have defined only 3 states
			current_index = i;
			break;
		}
	updateText();
	emit stateChanged(currentState());
}

void BannStates::changeState()
{
	current_index = ++current_index % states_list.size();
	updateText();
	emit stateChanged(currentState());
}

void BannStates::updateText()
{
	text->setText(states_list.at(current_index).second);
}

int BannStates::currentState()
{
	Q_ASSERT_X(states_list.size() > 0, "BannStates::currentState", "The list of states is empty!");
	return states_list.at(current_index).first;
}



