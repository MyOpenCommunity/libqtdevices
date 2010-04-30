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


#ifndef SORGENTI_MEDIA_H
#define SORGENTI_MEDIA_H

#include "bannciclaz.h"
#include "frame_receiver.h"
#include "multimedia_source.h"
#include "audiosource.h"

#include <QStringList>
#include <QString>
#include <QWidget>

#define MAX_AMB 9

class QDomNode;
class VirtualSourceDevice;
class TextOnImageLabel;


/**
 * Banner for multimedia sources.
 */
class MediaSource : public AudioSource
{
Q_OBJECT
public:
	MediaSource(const QString &area, VirtualSourceDevice *dev, const QString &description);

protected:
	void initBanner(const QString &left, const QString &center_left, const QString &center, const QString &center_right, const QString &right);

private:
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;
	TextOnImageLabel *center_icon;
};


/**
 * \brief This class is made to manage a multimedia source.
 */
class BannerSorgenteMultimedia : public bannCiclaz, FrameReceiver
{
Q_OBJECT
public:
	BannerSorgenteMultimedia(QWidget *parent, const QDomNode &config_node, QString indirizzo, int where, int nbutt);
	virtual void inizializza(bool forza = false);
	void set_status(bool stat);
	bool get_status();
	virtual void manageFrame(OpenMsg &msg);

private:
	bool status;
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
protected:
	MultimediaSource source_menu;
};

/**
 * \brief This class is made to manage a multichannel multimedia source.
 */
class BannerSorgenteMultimediaMC : public BannerSorgenteMultimedia
{
Q_OBJECT
public:
	BannerSorgenteMultimediaMC(QWidget *parent, const QDomNode &config_node, QString indirizzo, int where,
		QString icon_onoff, QString icon_cycle, QString icon_settings);
	void inizializza(bool forza = false);
	void setstatusAmb(int Amb, bool status);
	bool statusAmb(int Amb);
	bool get_status();
	virtual void manageFrame(OpenMsg &msg);

public slots:
	void attiva();
	void addAmb(QString a);
	void ambChanged(const QString &, bool, QString);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
	bool status_Amb[MAX_AMB];
signals:
	void active(int, int);
};

#endif
