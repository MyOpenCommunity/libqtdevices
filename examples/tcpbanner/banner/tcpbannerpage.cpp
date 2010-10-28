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
#include "tcpbannerpage.h"

#include <QStringList>
#include <QHBoxLayout>

#include "main.h"
#include "navigation_bar.h"
#include "tcpdimmer.h"
#include "tcpstatebanner.h"

namespace
{
	const char *TCP_BANNER_HOST = "localhost";
	const int TCP_BANNER_PORT = 12345;
}


TcpBannerPage::TcpBannerPage(QWidget *parent)
	: BannerPage(parent)
{
	BannerContent *content = new BannerContent(0, 1);

	// Container for banners
	QWidget *container = new QWidget;
	QHBoxLayout *layout = new QHBoxLayout(container);
	layout->addStretch(1);
	layout->addWidget(content);
	layout->addStretch(1);

	buildPage(container, content, new NavigationBar, tr("TCP Banner example page"));
	setSpacing(20);

	TcpStateBanner *state_banner = new TcpStateBanner("TCP State Banner", QStringList() << "State 1" << "State 2" << "State 3");
	state_banner->setConnectionParameters(TCP_BANNER_HOST, TCP_BANNER_PORT);
	page_content->appendBanner(state_banner);

	TcpDimmer *dimmer = new TcpDimmer(tr("TCP Dimmer"), "Test TCP Dimmer");
	dimmer->setConnectionParameters(TCP_BANNER_HOST, TCP_BANNER_PORT);
	page_content->appendBanner(dimmer);
}

int TcpBannerPage::sectionId() const
{
	return TCP_BANNER_TEST;
}
