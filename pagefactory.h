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


#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

class Page;

/*!
	\ingroup Core
	\brief A factory method that builds and returns the \ref Section page related to \a id.
	\note this method can be used only to build the HOME_PAGE and the SPECIAL_PAGE
*/
Page *getSectionPageFromId(int id);

/*!
	\ingroup Core
	\brief A factory method that builds and returns the \ref Section page related to \a page_id.
*/
Page *getSectionPage(int page_id);

#endif // PAGEFACTORY_H
