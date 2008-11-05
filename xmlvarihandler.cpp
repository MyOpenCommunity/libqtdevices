/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**xmlvarihandler.cpp
**
**classe per il parsing SAX di extra.xml stack_open.xml
**
****************************************************************/

#include "xmlvarihandler.h"
#include "main.h"

#include <stdio.h>


xmlcfghandler::xmlcfghandler(int* v, char** c)
{
	logFile = c;
	verbosity = v;
}

xmlcfghandler::~xmlcfghandler()
{

}

bool xmlcfghandler::startDocument()
{
	CurTagL1 = "";
	CurTagL2 = "";
	CurTagL3 = "";
	CurTagL4 = "";
	CurTagL5 = "";
	CurTagL6 = "";
	CurTagL7 = "";
	return TRUE;
}

bool xmlcfghandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes&)
{
	if (CurTagL1.isEmpty())
		CurTagL1 = qName;
	else if (CurTagL2.isEmpty())
		CurTagL2 = qName;
	else if (CurTagL3.isEmpty())
		CurTagL3 = qName;
	else if (CurTagL4.isEmpty())
		CurTagL4 = qName;
	else if (CurTagL5.isEmpty())
		CurTagL5 = qName;
	else if (CurTagL6.isEmpty())
		CurTagL6 = qName;
	else if (CurTagL7.isEmpty())
		CurTagL7 = qName;
	return TRUE;
}

bool xmlcfghandler::endElement(const QString&, const QString&, const QString&)
{
	if (!CurTagL7.isEmpty())
		CurTagL7 = "";
	else if (!CurTagL6.isEmpty())
		CurTagL6 = "";
	else if (!CurTagL5.isEmpty())
		CurTagL5 = "";
	else if (!CurTagL4.isEmpty())
		CurTagL4 = "";
	else if (!CurTagL3.isEmpty())
		CurTagL3 = "";
	else if (!CurTagL2.isEmpty())
		CurTagL2 = "";
	else if (!CurTagL1.isEmpty())
		CurTagL1 = "";
	return TRUE;
}

bool xmlcfghandler::characters(const QString & qValue)
{
	if (!CurTagL1.compare("root"))
	{
		if  (!CurTagL2.compare("sw"))
		{
			if (!CurTagL3.compare("BTouch"))
			{
				if (!CurTagL4.compare("logverbosity"))
					*verbosity=qValue.toInt(&ok, 10);
			}
			if (!CurTagL3.compare("logfile"))
			{
				QByteArray buf = qValue.toAscii();
				strncpy(*logFile,buf.constData(),MAX_PATH);
			}
		}
	}
	return TRUE;
}
