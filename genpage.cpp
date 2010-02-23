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


#include "genpage.h"
#include "main.h"

#include <QWidget>
#include <QPixmap>
#include <QFile>


genPage::genPage(QWidget *parent, unsigned char tipo,const char *img, unsigned int f)
	: QLabel(parent)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
	switch (tipo)
	{
	case RED:
		setStyleSheet("QLabel {color:#FF0000;}");
		break;
	case BLUE:
		setStyleSheet("QLabel {color:#0000FF;}");
		break;
	case GREEN:
		setStyleSheet("QLabel {color:#00FF00;}");
		break;
	case IMAGE:
		if (QFile::exists(img))
			setPixmap(QPixmap(img));
		break;
	}
}
