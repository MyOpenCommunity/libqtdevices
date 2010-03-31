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



#ifndef SOUNDDIFFUSIONPAGE_H
#define SOUNDDIFFUSIONPAGE_H

#include "page.h"
#include "banner.h"

class QDomNode;
class QLabel;
class BtButton;

// TODO: this should go to its own file if we need more banners
class SoundAmbient : public BannerNew
{
Q_OBJECT
public:
	SoundAmbient(const QString &descr, const QString &ambient);
	void initBanner(const QString &_ambient_icon, const QString &_center_icon,
		const QString &right, const QString &banner_text);
	void connectRightButton(Page *p);

private:
	QLabel *ambient_icon, *center_icon, *text;
	BtButton *right_button;
};


struct SourceDescription
{
	QString descr;
	QString where;
	int id;
	int cid;
};
Q_DECLARE_TYPEINFO(SourceDescription, Q_MOVABLE_TYPE);



/**
 * Ambient page: has sources as top widget and amplifiers below
 */
class SoundAmbientPage : public BannerPage
{
Q_OBJECT
public:
	SoundAmbientPage(const QDomNode &conf_node, const QList<SourceDescription> &sources = QList<SourceDescription>());
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};

/**
 * General sound diffusion page. Shown only in multi sound diffusion.
 */
class SoundDiffusionPage : public BannerPage
{
Q_OBJECT
public:
	SoundDiffusionPage(const QDomNode &config_node);
	virtual int sectionId();

private:
	banner *getAmbientBanner(const QDomNode &item_node, const QList<SourceDescription> &sources);
	void loadItems(const QDomNode &config_node);

};

#endif // SOUNDDIFFUSIONPAGE_H
