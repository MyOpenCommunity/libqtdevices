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


#ifndef  GENERIC_PAGES_H
#define GENERIC_PAGES_H

#include <QLabel>

/*!
  \class genPage
  \brief This class genertes a page as described in \a pageType. 

  \date lug 2005
*/

class genPage : public QLabel
{
Q_OBJECT
public:
	genPage(QWidget *parent= NULL, unsigned char tipo=0, const char *img = NULL,unsigned int f =  0);
	enum pageType
	{
		RED = 0,  /*! \enum generates a \a red page */
		GREEN = 1,  /*! \enum generates a \a green page */
		BLUE = 2,  /*! \enum generates a \a blue page */
		IMAGE = 3,  /*! \enum generates a page which shows an image whose fileName is the forth argument of the constructor */
		NONE = 4
	};
};

#endif //GENERIC_PAGES_H
