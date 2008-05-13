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
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

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
