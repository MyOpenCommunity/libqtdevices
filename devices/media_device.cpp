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

#include "media_device.h"
#include "generic_functions.h"

#include <openmsg.h>


enum RequestDimension
{
	REQ_FREQUENCE_UP = 5,
	REQ_FREQUENCE_DOWN = 6,
	REQ_NEXT_TRACK = 9,
	REQ_PREV_TRACK = 10,
	REQ_SOURCE_ON = 35,
	REQ_SAVE_STATION = 33,
	START_RDS = 31,
	STOP_RDS = 32,
};


SourceDevice::SourceDevice(QString source, int openserver_id) :
	device(QString("22"), "2#" + source, openserver_id)
{
	mmtype = 4;
	source_id = source;
}

void SourceDevice::nextTrack() const
{
	sendCommand(REQ_NEXT_TRACK);
}

void SourceDevice::prevTrack() const
{
	sendCommand(REQ_PREV_TRACK);
}

void SourceDevice::turnOn(QString area) const
{
	QString what = QString("%1#%2#%3#%4").arg(REQ_SOURCE_ON).arg(mmtype).arg(area).arg(source_id);
	QString where = QString("3#%1#0").arg(area);
	sendCommand(what, where);
}

void SourceDevice::requestTrack() const
{
	sendRequest(DIM_TRACK);
}

bool SourceDevice::parseFrame(OpenMsg &msg, StatusList &status_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (!isDimensionFrame(msg))
		return false;

	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_STATUS:
		v.setValue(msg.whatArgN(0) == 1);
		break;
	case DIM_TRACK:
		v.setValue(msg.whatArgN(0));
	default:
		return false;
	}

	status_list[what] = v;
	return !status_list.isEmpty();
}


RadioSourceDevice::RadioSourceDevice(QString source_id, int openserver_id) :
	SourceDevice(source_id, openserver_id)
{
}

void RadioSourceDevice::frequenceUp(QString value) const
{
	sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_UP).arg(value));
}

void RadioSourceDevice::frequenceDown(QString value) const
{
	sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_DOWN).arg(value));
}

void RadioSourceDevice::saveStation(QString station) const
{
	sendCommand(QString("%1#%2").arg(REQ_SAVE_STATION).arg(station));
}

void RadioSourceDevice::requestFrequency() const
{
	sendRequest(DIM_FREQUENCY);
}

void RadioSourceDevice::requestRDS() const
{
	sendCommand(START_RDS);
}

bool RadioSourceDevice::parseFrame(OpenMsg &msg, StatusList &status_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (SourceDevice::parseFrame(msg, status_list))
		return true;

	if (isCommandFrame(msg) && static_cast<int>(msg.what()) == STOP_RDS)
	{
		requestRDS();
		return true;
	}

	if (!isDimensionFrame(msg))
		return false;

	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_FREQUENCY:
		v.setValue(msg.whatArgN(1));
		break;
	case DIM_RDS:
	{
		QString rds_message;
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
			rds_message.append(QChar(msg.whatArgN(i)));
		v.setValue(rds_message);
		break;
	}
	default:
		return false;
	}

	status_list[what] = v;
	return !status_list.isEmpty();
}


VirtualSourceDevice::VirtualSourceDevice(QString address, int openserver_id) :
	SourceDevice(address, openserver_id)
{

}

bool VirtualSourceDevice::parseFrame(OpenMsg &msg, StatusList &status_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (SourceDevice::parseFrame(msg, status_list))
		return true;

	// TODO: e' necessario implementare la parte "attiva" del device e in particolare
	// i comandi on e off. Resta da capire se e' meglio implementarli sul comando
	// vero e proprio o sulla frame di notifica che comunque arriva al device.

	if (!isCommandFrame(msg))
		return false;


	// TODO: non sono per niente convinto di questa soluzione. Questo e' il primo
	// caso di device che deve fare qualcosa quando riceve dei comandi.. e nello
	// specifico quando arrivano le frame di comando dovranno essere poi lanciati
	// degli script per accendere/spegnere la sorgente o andare alla traccia
	// precedente/successiva. Resta da capire però quest'ultima parte dove ha senso
	// che sia messa, se dentro al device (così pero' diventa dipendente
	// dall'hardware, e non mi piace molto) oppure che lo comunichi all'esterno e
	// che sia la parte grafica a chiamare le funzioni specifiche dell'hardware.
	// In questo secondo caso però non e' particolarmente bello usare la status_changed
	// come fatto per il momento, in quanto come il nome suggerisce dovrebbe segnalare
	// un cambiamento stato relativo ad un device, e non un comando per il quale è
	// necessario fare qualcosa. Altre alternative potrebbero essere:
	// - avere un altro segnale per esprimere la richiesta di lanciare comandi
	// - avere segnali custom
	// boh??

	int what = msg.what();
	switch (what)
	{
	case REQ_FREQUENCE_UP:
	case REQ_FREQUENCE_DOWN:
		break;
	default:
		return false;
	}
	status_list[what] = true;
	return true;
}

