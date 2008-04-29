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
 * \version $Id: propertymap_xml.cpp,v 1.1 2008/04/16 08:53:12 cvs Exp $
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 */

/*#*
 *#* $Log: propertymap_xml.cpp,v $
 *#* Revision 1.1  2008/04/16 08:53:12  cvs
 *#* Agresta:
 *#*  # versione data da Develer il 10 Aprile 2008 e consegnata a Mac per iniziare lo sviluppo dello Stop & Go
 *#*
 *#* Revision 1.1  2006/10/25 14:23:16  bernie
 *#* Merge too many PropertyMap enhancements to list here.
 *#*
 *#* Revision 1.3  2005/09/19 16:36:05  chad
 *#* Fix doxygen autobrief
 *#*
 *#* Revision 1.2  2005/09/16 15:44:11  chad
 *#* Change keys creation policy, if the key is empty the key is not created
 *#*
 *#* Revision 1.1  2005/06/22 16:11:18  rasky
 *#* Add supporto for loading a property map from a XML file
 *#*
 *#* Revision 1.1  2005/04/12 02:00:44  bernie
 *#* Import in DevLib.
 *#*/

#include "propertymap_xml.h"
#include "propertymap.h"
#include <string>

#include <qdom.h>
#include <qfile.h>
#include <qstring.h>


static void traverse(PropertyMap &pmap, QDomElement elt, QString path = QString())
{
	path += "/" + elt.tagName();

	QDomNode child = elt.firstChild();
	while (!child.isNull())
	{
		if (child.isElement())
			traverse(pmap, child.toElement(), path);
		else if (child.isText())
		{
			QString txt = child.toText().data();

			std::string path_debbug = path.ascii();
			std::string txt_debug = txt.ascii();

			// If the node contains no text the key is not created...
			if (!txt.isEmpty())
				pmap.set(path.ascii()+1, txt.ascii());
		}
		child = child.nextSibling();
	}
}

bool propertyMapLoadXML(PropertyMap &pmap, const std::string &filename,
                        std::string *err_msg, int *err_line, int *err_col)
{
	QFile f(filename.c_str());

	if (!f.open(IO_ReadOnly))
		return false;

	QDomDocument xmldoc;
	QString msg;
	if (!xmldoc.setContent(f.readAll(), false, &msg, err_line, err_col))
	{
		if (err_msg)
			*err_msg = msg.ascii();
		return false;
	}
	f.close();

	if (xmldoc.documentElement().isNull())
		return true;

	traverse(pmap, xmldoc.documentElement());
	return true;
}
