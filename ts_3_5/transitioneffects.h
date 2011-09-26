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


#ifndef TRANSITIONEFFECTS_H
#define TRANSITIONEFFECTS_H

#include "singlechoicepage.h"

class QDomNode;


/*!
	\ingroup Settings
	\brief Allows the user to choose the ScreenSaver.
*/
class TransitionEffects : public SingleChoicePage
{
Q_OBJECT
public:
	TransitionEffects(const QDomNode &conf_node);
	virtual void showPage();
	virtual void cleanUp();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);

private:
	int item_id;
};

#endif // TRANSITIONEFFECTS_H
