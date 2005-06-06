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
#include "genericfunz.h"
#include "btmain.h"

#include <qstring.h>

class xmlskinhandler : public QXmlDefaultHandler
{
public:
    xmlskinhandler(QColor**, QColor**, QColor**);
    
    ~xmlskinhandler();

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& , 
                       const QXmlAttributes& );
    bool characters( const QString & ch );
    bool endElement( const QString&, const QString&, const QString& );

private:
    QString CurTagL1,CurTagL2,CurTagL3,CurTagL4,CurTagL5,CurTagL6,CurTagL7;
    bool ok;
    int r,g,b;
    QColor** bg,** fg1, **fg2;
};                   


class xmlcfghandler : public QXmlDefaultHandler
{
public:
    xmlcfghandler(int*, char*);
    
    ~xmlcfghandler();

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& , 
                       const QXmlAttributes& );
    bool characters( const QString & ch );
    bool endElement( const QString&, const QString&, const QString& );

private:
    QString CurTagL1,CurTagL2,CurTagL3,CurTagL4,CurTagL5,CurTagL6,CurTagL7;
    bool ok;
    char* logFile;
    int* verbosity;
};                   
#endif 
