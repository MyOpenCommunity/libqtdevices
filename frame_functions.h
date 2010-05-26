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

#ifndef FRAME_FUNCTIONS_H
#define FRAME_FUNCTIONS_H

#include <QString>

class OpenMsg;

/** The following functions can be used to test the format of a frame or build it.
 *  The frame format can be:
 *  - a command frame, in the form *who*what*where##
 *  - a read dimension frame, in the form *#who*where*dimension##
 *  - a write dimension frame, in the form *#who*where*#dimension*val1*..*valn##
 *  - a status request frame, in the form *#who*where##
 */

bool isCommandFrame(OpenMsg &msg);
bool isDimensionFrame(OpenMsg &msg);
bool isWriteDimensionFrame(OpenMsg &msg);
bool isStatusRequestFrame(OpenMsg &msg);

QString createCommandFrame(QString who, QString what, QString where);
QString createDimensionFrame(QString who, QString dimension, QString where);
QString createWriteDimensionFrame(QString who, QString dimension, QString where);
QString createStatusRequestFrame(QString who, QString where);



#endif // FRAME_FUNCTIONS_H
