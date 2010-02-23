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


#ifndef AMBDIFFSON_H
#define AMBDIFFSON_H

#include "bann1_button.h" // bannBut2Icon, bannPuls

#include <QString>
#include <QList>

/// Forward Declarations
class SoundDiffusion;
class MultiSoundDiff;
class AudioSources;


/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class ambDiffSon
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \date jul 2006
 */
class ambDiffSon : public bannBut2Icon
{
Q_OBJECT
public:
	ambDiffSon(QWidget *parent, QString d, QString indirizzo, QString Icona1, QString Icona2,
		QString Icona3, SoundDiffusion *ds, AudioSources *sorg);
	void Draw();
	void setDraw(bool d);
	bool isDraw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
protected:
	virtual void hideEvent(QHideEvent *event);
private:
	SoundDiffusion *diffson;
	AudioSources *sorgenti;
	int actSrc;
	bool is_draw;
	QString descr;
signals:
	void ambChanged(const QString &, bool, QString);
};


/*****************************************************************
 ** Insieme Ambienti diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class insAmbDiffSon
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \date jul 2006
 */
class insAmbDiffSon : public bannPuls
{
Q_OBJECT
public:
	insAmbDiffSon(QWidget *parent, QString d, QString Icona1, QString Icona2, SoundDiffusion *ds,
		AudioSources *sorg);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
private:
	SoundDiffusion *diffson;
	AudioSources *sorgenti;
	QString descr;
signals:
	void ambChanged(const QString &, bool, QString);
};

#endif
