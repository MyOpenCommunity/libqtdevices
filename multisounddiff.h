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


#ifndef MULTI_SOUNDDIFF_H
#define MULTI_SOUNDDIFF_H

#include "sottomenu.h"

#include <QWidget>
#include <QString>
#include <QList>


class SoundDiffusion;
class AudioSources;
class device;
class device_status;


class MultiSoundDiffInterface : public sottoMenu
{
Q_OBJECT
public:
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf)
		{ return 0; }
	void loadAmbienti(const QDomNode &config_node);
	~MultiSoundDiffInterface();

protected:
	MultiSoundDiffInterface() { }
	QList<SoundDiffusion*> dslist;
	static AudioSources *sorgenti;

private slots:
	void ds_closed();
signals:
	void actSrcChanged(int, int);
};


class MultiSoundDiff : public MultiSoundDiffInterface
{
Q_OBJECT
public:
	MultiSoundDiff(const QDomNode &config_node);

	/*!
	 *  \brief Changes the type of navigation bar present at the
	 *  bsubtree (see bannFrecce). Also calls downstream SoundDiffusion setNavBarMode
	 */
	virtual void setNavBarMode(uchar=0, QString IconBut4=ICON_FRECCIA_DX);
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);
	virtual void inizializza();
	virtual void setNumRighe(uchar);

public slots:
	void status_changed(QList<device_status*> sl);

private:
	device *matr;

signals:
	void actSrcChanged(int, int);
};


class MultiSoundDiffAlarm : public MultiSoundDiffInterface
{
Q_OBJECT
public:
	MultiSoundDiffAlarm(const QDomNode &config_node);
	virtual void inizializza() { } // avoid a second initialization (see .cpp for details)
	virtual void showPage();
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);

private slots:
	void status_changed(QList<device_status*> sl);

signals:
	void actSrcChanged(int, int);
};

#endif // MULTI_SOUNDDIFF_H
