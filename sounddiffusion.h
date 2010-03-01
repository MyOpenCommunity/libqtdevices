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


#ifndef SOUND_DIFFUSION_H
#define SOUND_DIFFUSION_H

#include "frame_receiver.h"
#include "sottomenu.h"

#include <QWidget>
#include <QList>
#include <QString>

class banner;
class QLabel;


/// A container for sources (aux, touchscreen, radio..)
class AudioSources : public sottoMenu
{
Q_OBJECT
public:
	AudioSources(QWidget *parent, const QDomNode &config_node);
	/// add amb to all banners (source)
	void addAmb(QString a);

	/// Sets the index of the list forcing it to the object having the address passed by argument.
	void setIndex(QString addr);

	void mostra_all(char);

private:
	void loadItems(const QDomNode &config_node);

private slots:
	void showSoundDiff();

signals:
	void actSrcChanged(int, int);

	/// amb description changed (for MultiSoundDiff)
	void ambChanged(const QString & newdescr, bool multiwhere, QString where);
};


/// A container for various type of amplifier..
class AmpliContainer : public sottoMenu
{
Q_OBJECT
public:
	AmpliContainer(QWidget *parent, const QDomNode &config_node);

	/// Sets the index of the list forcing which is the first item to draw.
	void setIndice(char);

private slots:
	void showSoundDiff();
private:
	void loadAmplifiers(const QDomNode &config_node);
};


/*!
 * \class SoundDiffusion
 * \brief This is a class that manage sound diffusion function of the system. 
 * It is composed of two subtree (sottoMenu): amplifiers and sources. 
 * In the first row of the page is shown the source active at a certain 
 * time while in the remaining rows there are all the amplifiers one ca control.
 */
class SoundDiffusion : public Page, FrameReceiver
{
Q_OBJECT
public:
	SoundDiffusion(const QDomNode &config_node);
	SoundDiffusion(AudioSources *s, const QDomNode &config_node);
	/*!
	 * \brief Sets the row's number.
	 */
	void setNumRighe(uchar);

	/*!
	 * \brief Initialize the class.
	 *
	 * It recall the sources' and amplifiers' inizialization procedure.
	 */
	virtual void inizializza();

	/*!
	 * \brief Sets the geometry passing x,y,w,h.
	 */
	void setGeom(int,int,int,int);

	/*!
	 * \brief Changes the navigation bar mode as described in bannFrecce
	 */
	void setNavBarMode(uchar);

	/*!
	 * \brief Recalls a draw for both amplifiers and sources.
	 */
	void draw();

	/*!
	 * \brief Recalls a forced draw for both amplifiers and sources.
	 *
	 * A forced draw force a draw of the subtree(sottoMenu) also if its internal index remain unchanged.
	 */
	void forceDraw();

	AudioSources *getAudioSources();

	virtual void manageFrame(OpenMsg &msg);

public slots:
	/*!
	 * \brief sets first source given address
	 */
	void setFirstSource(int addr);

	virtual void showPage();

protected:
	AudioSources *sorgenti;
	AmpliContainer *amplificatori;
	QLabel *linea;

private:
	/*!
	 * \brief Set the "sorgenti" submenu"
	 */
	void setSorgenti(AudioSources *s);
	void init(const QDomNode &config_node);
	void drawLine();
	uchar numRighe;
	bool shared_audiosources;

private slots:
	/*!
	 * \brief Emittes the closed signal when amplifiers subtree(sottoMenu) is closed.
	 */
	void fineVis();

	void handleClose();

signals:
	void closed(SoundDiffusion*);
};


class SoundDiffusionAlarm : public SoundDiffusion
{
Q_OBJECT
public:
	SoundDiffusionAlarm(AudioSources *s, const QDomNode &config_node);
	SoundDiffusionAlarm(const QDomNode &config_node);
	virtual void inizializza() { } // intentionally empty, avoid re initialization
	virtual void showPage();

private:
	void createWidgets();
};

#endif // SOUND_DIFFUSION
