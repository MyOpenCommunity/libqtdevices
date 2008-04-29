/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Save/load a property map through a XML file (using Qt's QDom).
 *
 * \version $Id: propertymap_xml.h,v 1.1 2008/04/16 08:53:12 cvs Exp $
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

/*#*
 *#* $Log: propertymap_xml.h,v $
 *#* Revision 1.1  2008/04/16 08:53:12  cvs
 *#* Agresta:
 *#*  # versione data da Develer il 10 Aprile 2008 e consegnata a Mac per iniziare lo sviluppo dello Stop & Go
 *#*
 *#* Revision 1.2  2007/09/25 16:00:25  aleph
 *#* Fix comment typo
 *#*
 *#* Revision 1.1  2006/10/25 14:23:16  bernie
 *#* Merge too many PropertyMap enhancements to list here.
 *#*
 *#* Revision 1.2  2005/09/19 16:36:05  chad
 *#* Fix doxygen autobrief
 *#*
 *#* Revision 1.1  2005/06/22 16:11:18  rasky
 *#* Add supporto for loading a property map from a XML file
 *#*
 *#* Revision 1.1  2005/04/12 02:00:44  bernie
 *#* Import in DevLib.
 *#*/

#ifndef DEVLIB_PROPERTYMAP_XML_H
#define DEVLIB_PROPERTYMAP_XML_H

#include <string>

class PropertyMap;

/**
 * Load the PropertyMap \a pmap from the file \a filename, using
 * the XML format. If specified, \a err_msg, \a err_line and \a err_col
 * will provide detailed information in case of parsing error.
 *
 * \return true if everything ok, false in case of error.
 *
 * \note This function does not clear the map before loading the map: the
 * data extracted from the XML file is inserted into the map keeping the
 * previous data.
 */
bool propertyMapLoadXML(PropertyMap &pmap, const std::string &filename,
                        std::string *err_msg = 0, int *err_line = 0, int *err_col = 0);


#endif /* DEVLIB_PROPERTYMAP_XML_H */
