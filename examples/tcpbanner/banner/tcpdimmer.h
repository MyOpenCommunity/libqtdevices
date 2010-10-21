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
#ifndef TCPDIMMER_H
#define TCPDIMMER_H

#include "bann_lighting.h"

class TcpDimmer : public AdjustDimmer
{
Q_OBJECT
public:
	TcpDimmer(const QString &name, const QString &descr);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	void sendMessage(const QString &message);

	QString name;
	int light_value;
};

#endif
