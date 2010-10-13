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

#include "frame_functions.h"

#include <openmsg.h>


/*!
	\brief Tests if "msg" is in the form *who*what*where##
 */
bool isCommandFrame(OpenMsg &msg)
{
	return msg.IsNormalFrame();
}

/*!
	\brief Tests if "msg" is in the form *#who*where*dimension##
 */
bool isDimensionFrame(OpenMsg &msg)
{
	return msg.IsMeasureFrame();
}

/*!
	\brief Tests if "msg" is in the form *#who*where*#dimension*val1*..*valn##
 */
bool isWriteDimensionFrame(OpenMsg &msg)
{
	return msg.IsWriteFrame();
}

/*!
	\brief Tests if "msg" is in the form *#who*where##
 */
bool isStatusRequestFrame(OpenMsg &msg)
{
	return msg.IsStateFrame();
}

/*!
	\brief Returns a string in the form *who*what*where##
 */
QString createCommandFrame(QString who, QString what, QString where)
{
	return QString("*%1*%2*%3##").arg(who).arg(what).arg(where);
}

/*!
	\brief Returns a string in the form *#who*where*dimension##
 */
QString createDimensionFrame(QString who, QString what, QString where)
{
	return QString("*#%1*%2*%3##").arg(who).arg(where).arg(what);
}

/*!
	\brief Returns a string in the form *#who*where*#dimension*val1*..*valn##
 */
QString createWriteDimensionFrame(QString who, QString what, QString where)
{
	return QString("*#%1*%2*#%3##").arg(who).arg(where).arg(what);
}

/*!
	\brief Returns a string in the form *#who*where##
 */
QString createStatusRequestFrame(QString who, QString where)
{
	return QString("*#%1*%2##").arg(who).arg(where);
}

