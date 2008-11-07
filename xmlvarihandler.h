/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**xmlvarihandler.h
**
**definizioni per classe per il parsing SAX di extra.xml e stack_open.xml
**
****************************************************************/

#ifndef XMLVARIHANDLER_H
#define XMLVARIHANDLER_H

#include <qxml.h>
#include <QString>


/*!
  \class xmlcfghandler 
  \brief This class parses the stack open configuration file and reads logFile and verbosity level.

  Using QT SAX handler for xml parsing, this class discoveres is the logFile to be used (whose pointer is passed as a second argument of the constructor) and the initial verbosity level (the first argument is a *int which return this value).

  \author Davide
  \date lug 2005
*/

class xmlcfghandler : public QXmlDefaultHandler
{
public:
	xmlcfghandler(int*, char**);
	~xmlcfghandler();

	bool startDocument();
	bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
	bool characters(const QString & ch);
	bool endElement(const QString&, const QString&, const QString&);

private:
	QString CurTagL1,CurTagL2,CurTagL3,CurTagL4,CurTagL5,CurTagL6,CurTagL7;
	bool ok;
	char** logFile;
	int* verbosity;
};
#endif 
