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


#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "banner.h"

class SourceDevice;
class StateButton;
class TextOnImageLabel;
class VirtualSourceDevice;
class RadioSourceDevice;
class RadioInfo;
class RadioPage;
class QStackedWidget;


/*!
	\ingroup SoundDiffusion
	\brief The abstract base class for all the audio sources
 */
class AudioSource : public BannerNew
{
Q_OBJECT

public:
	// Method called when the source is hidden or showed. It differs from the
	// standard hideEvent/showEvent because it is called when the container
	// of the banner is hidden/shown
	virtual void sourceShowed() {}
	virtual void sourceHidden() {}

signals:
	void sourceStateChanged(bool active);

protected:
	AudioSource(const QString &area, SourceDevice *dev, Page *details = NULL);

	void initBanner(const QString &left_on, const QString &left_off, const QString &center_left,
			const QString &center_right, const QString &right);

protected slots:
	void turnOn();
	virtual void showDetails();

protected:
	QString area;
	SourceDevice *dev;
	Page *details;

	StateButton *left_button, *center_left_button, *center_right_button;
	BtButton *right_button;

private slots:
	void valueReceivedAudioSource(const DeviceValues &values_list);
};


/*!
	\ingroup SoundDiffusion
	\brief Sound diffusion aux source (also used for other BTouch on the SCS bus).
 */
class AuxSource : public AudioSource
{
Q_OBJECT
public:
	AuxSource(const QString &area, SourceDevice *dev, const QString &description);

private:
	TextOnImageLabel *center_icon;
};


/*!
	\ingroup SoundDiffusion
	\brief Sound diffusion multimedia source (local file system, web radio, ...)
 */
class MediaSource : public AudioSource
{
Q_OBJECT
public:
	MediaSource(const QString &area, VirtualSourceDevice *dev, const QString &description, Page *details);

private slots:
	void sourceStateChanged(bool active);

private:
	TextOnImageLabel *center_icon;
};


/*!
	\ingroup SoundDiffusion
	\brief Sound diffusion RDS radio source.
 */
class RadioSource : public AudioSource
{
Q_OBJECT
public:
	RadioSource(const QString &area, RadioSourceDevice *dev, RadioPage *details);

	virtual void sourceHidden();
	virtual void sourceShowed();

protected slots:
	virtual void showDetails();

private slots:
	void sourceStateChanged(bool active);

private:
	RadioInfo *radio_info;
};


/*!
	\ingroup SoundDiffusion
	\brief Source configuration data from configuration file.
 */
struct SourceDescription
{
	enum Type
	{
		AUX,
		RADIO,
		MULTIMEDIA
	};

	QString descr;
	QString where;
	int cid;
	mutable Page *details;
	Type type;
};
Q_DECLARE_TYPEINFO(SourceDescription, Q_MOVABLE_TYPE);


/*!
	\ingroup SoundDiffusion
	\brief Contains one AudioSource subclass for each configured source, and the button to turn on the source.

	There is a separate %SoundSources and AudioSource subclass instance for each environment; however,
	source configuration pages (RDS radio details, multimedia source list) are shared by all environments.
 */
class SoundSources : public QWidget
{
Q_OBJECT
public:
	SoundSources(const QString &source_address, const QString &area, const QList<SourceDescription> &sources);

signals:
	void pageClosed();

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private slots:
	void sourceCycle();
	void sourceStateChanged(bool active);

private:
	QStackedWidget *sources;
};



#endif // AUDIOSOURCE_H
