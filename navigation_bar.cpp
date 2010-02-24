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


#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "hardware_functions.h"

#include <QFileInfo>

static const int buttons_dim = 60;


#ifdef LAYOUT_BTOUCH

NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	back_button = createButton(back_icon, SIGNAL(backClick()), 0);
	down_button = createButton(down_icon, SIGNAL(downClick()), 1);
	up_button = createButton(up_icon, SIGNAL(upClick()), 2);
	forward_button = createButton(forward_icon, SIGNAL(forwardClick()), 3);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

#else

NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	// TODO: Btouch code expects the forward button to be in the navigation
	// bar; in TouchX the button is mostly in the page (except for a delete
	// button in a couple of pages)
	forward_button = createButton(forward_icon, SIGNAL(forwardClick()), 0);
	up_button = createButton(up_icon, SIGNAL(upClick()), 1);
	down_button = createButton(down_icon, SIGNAL(downClick()), 2);
	back_button = createButton(back_icon, SIGNAL(backClick()), 3);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

#endif

BtButton *NavigationBar::createButton(QString icon, const char *signal, int pos)
{
	if (!icon.isNull())
	{
		BtButton *b = new BtButton(this);
		connect(b, SIGNAL(clicked()), signal);
		if (QFileInfo(icon).exists())
			b->setImage(icon); // for retrocompatibility
		else
			b->setImage(bt_global::skin->getImage(icon));
#ifdef LAYOUT_BTOUCH
		b->setGeometry(buttons_dim * pos, 0, buttons_dim, buttons_dim);
#else
		b->setGeometry(13, 60 + buttons_dim * pos, buttons_dim, buttons_dim);
#endif
		return b;
	}
	return 0;
}

QSize NavigationBar::sizeHint() const
{
	if (hardwareType() == TOUCH_X)
		return QSize(75, 355);
	else
		return QSize(buttons_dim * 4, buttons_dim);
}

void NavigationBar::displayScrollButtons(bool display)
{
	down_button->setVisible(display);
	up_button->setVisible(display);
}

