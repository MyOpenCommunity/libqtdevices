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


#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "generic_functions.h" // getPressName
#include "openclient.h"
#include "fontmanager.h" // bt_global::font
#include "openclient.h"
#include "page.h"
#include "labels.h" // ScrollingLabel

#include <QPixmap>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QApplication> //qapp
#include <QLayout>


// Inizialization of static member
ClientWriter *Banner::client_command = 0;
ClientWriter *Banner::client_request = 0;


Banner::Banner(QWidget *parent) : QWidget(parent)
{
	banner_width = 240;
	banner_height = 80;
	serNum = 1;
	id = -1;
}

QSize Banner::sizeHint() const
{
	if (QLayout *l = layout())
		return l->sizeHint();
	return QSize(banner_width, banner_height);
}

void Banner::sendFrame(QString frame) const
{
	Q_ASSERT_X(client_command, "Banner::sendFrame", "Client command not set!");
	client_command->sendFrameOpen(frame);
}

void Banner::sendInit(QString frame) const
{
	Q_ASSERT_X(client_request, "Banner::sendInit", "Client request not set!");
	client_request->sendFrameOpen(frame);
}

void Banner::setClients(ClientWriter *command, ClientWriter *request)
{
	client_command = command;
	client_request = request;
}

void Banner::setSerNum(int s)
{
	serNum = s;
}

int Banner::getSerNum()
{
	return serNum;
}

void Banner::setId(int i)
{
	id = i;
}

int Banner::getId()
{
	return id;
}

void Banner::setOpenserverConnection(device *dev)
{
	connect(dev, SIGNAL(connectionUp()), SLOT(connectionUp()));
	connect(dev, SIGNAL(connectionDown()), SLOT(connectionDown()));
	if (dev->isConnected())
		connectionUp();
	else
		connectionDown();
}

void Banner::connectionUp()
{
	setEnabled(true);
}

void Banner::connectionDown()
{
	setEnabled(false);
}

ScrollingLabel *Banner::createTextLabel(const QRect &size, Qt::Alignment align, const QFont &font)
{
	ScrollingLabel *text = new ScrollingLabel(this);
	text->setGeometry(size);
	text->setAlignment(align);
	text->setFont(font);
	return text;
}

ScrollingLabel *Banner::createTextLabel(Qt::Alignment align, const QFont &font)
{
	ScrollingLabel *text = new ScrollingLabel(this);
	text->setAlignment(align);
	text->setFont(font);
	return text;
}

void Banner::connectButtonToPage(BtButton *b, Page *p)
{
	// check both the page and the button, which can be deleted if there's no icon set (see initButton)
	if (p && b)
	{
		linked_pages.append(p);
		connect(b, SIGNAL(clicked()), p, SLOT(showPage()));
		connect(p, SIGNAL(Closed()), SIGNAL(pageClosed()));
	}
}

void Banner::hideEvent(QHideEvent *event)
{
	foreach (Page *p, linked_pages)
		if (!p->isHidden())
			p->hide();
}

void Banner::inizializza(bool forza)
{
	foreach (Page *p, linked_pages)
		p->inizializza();
}

bool Banner::initButton(BtButton *btn, const QString &icon)
{
	// We use a null icon path to mark that we want to delete a button usually
	// in the banner. Instead, the empty icon path is used when the requested
	// path is not found (see the return value of the method SkinManager::getImage)
	if (icon.isNull())
	{
		Q_ASSERT_X(linked_pages.isEmpty(), "Banner::initButton",
			"Deleting a button with (possibly) linked pages. Maybe you called connectButton() before initBanner()? Aborting.");
		btn->hide();
		btn->disconnect();
		btn->deleteLater();
		return false;
	}

	btn->setImage(icon);
	return true;
}

bool Banner::initLabel(ScrollingLabel *lbl, const QString &text, const QFont &font)
{
	if (text.isEmpty())
	{
		lbl->hide();
		lbl->disconnect();
		lbl->deleteLater();
		return false;
	}

	lbl->setScrollingText(text);
	lbl->setFont(font);
	return true;
}
