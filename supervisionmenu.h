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


#ifndef BTOUCH_SUPERVISION_MENU_H
#define BTOUCH_SUPERVISION_MENU_H

#include "bannerpage.h"
#include "stopngo.h"

#include <QDomNode>
#include <QVector>

class LoadsDevice;

/*!
	\defgroup Supervision Supervision

	A container for the \ref StopAndGo and the \ref LoadDiagnostic subsections.
*/


/*!
	\ingroup Supervision
	\brief Contains banners for the \ref StopAndGo and \ref LoadDiagnostic subsections.

	The page shows the banners if both subsections are defined, otherwise shows
	directly the one defined.
*/
class SupervisionMenu : public BannerPage
{
Q_OBJECT
public:
	SupervisionMenu(const QDomNode &config_node);
	virtual int sectionId() const;

public slots:
	virtual void showPage();

private:
	void loadItems(const QDomNode &config_node);

private:
	Page *next_page;
};


/*!
	\defgroup LoadDiagnostic Load Diagnostic

	Allows the user to view the load status for each load in the system, using
	the LoadDiagnosticPage.
*/


/*!
	\ingroup LoadDiagnostic
	\brief Shows the list of the loads, using different icons to represent their status.
*/
class LoadDiagnosticPage : public BannerPage
{
Q_OBJECT
public:
	LoadDiagnosticPage(const QDomNode &config_node);
	virtual int sectionId() const;

protected:
	void showEvent(QShowEvent *);

private:
	QVector<LoadsDevice *> devices;
};

#endif
