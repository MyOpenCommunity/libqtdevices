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


#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QList>
#include <QHash>
#include <QString>


class SkinManager
{
public:
	typedef QList<int> CidState;

	SkinManager(QString filename);
	QString getStyle();

	void addToContext(int cid);
	void removeFromContext();

	bool hasContext();

	QString getImage(QString name);

	bool exists(QString name);
	CidState getCidState();
	void setCidState(const CidState &state);

private:
	QString style;
	QList<int> cid_lookup_list;
	QHash<int, QHash<QString, QString> > images;
};



class SkinContext
{
public:
	SkinContext(int cid);
	~SkinContext();
};

namespace bt_global { extern SkinManager *skin; }


#endif // SKINMANAGER_H
