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


/*!
	\ingroup Core
	\brief Tests if \a msg is in the form *who*what*where##
*/
bool isCommandFrame(OpenMsg &msg);

/*!
	\ingroup Core
	\brief Tests if \a msg is in the form *#who*where*dimension##
*/
bool isDimensionFrame(OpenMsg &msg);

/*!
	\ingroup Core
	\brief Tests if \a msg is in the form *#who*where*#dimension*val1*..*valn##
*/
bool isWriteDimensionFrame(OpenMsg &msg);

/*!
	\ingroup Core
	\brief Tests if \a msg is in the form *#who*where##
*/
bool isStatusRequestFrame(OpenMsg &msg);

/*!
	\ingroup Core
	\brief Returns a string in the form *who*what*where##
*/
QString createCommandFrame(QString who, QString what, QString where);

/*!
	\ingroup Core
	\brief Returns a string in the form *#who*where*dimension##
*/
QString createDimensionFrame(QString who, QString dimension, QString where);

/*!
	\ingroup Core
	\brief Returns a string in the form *#who*where*#dimension*val1*..*valn##
*/
QString createWriteDimensionFrame(QString who, QString dimension, QString where);

/*!
	\ingroup Core
	\brief Returns a string in the form *#who*where##
*/
QString createStatusRequestFrame(QString who, QString where);



#endif // FRAME_FUNCTIONS_H
