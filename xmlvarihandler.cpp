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


/*******************************************
*
*******************************************/
xmlskinhandler::xmlskinhandler(QColor** ba, QColor** f1, QColor** f2)
{
  bg=ba;
  fg1=f1;
  fg2=f2;
  r=g=b=127;
}

/*******************************************
*
*******************************************/
xmlskinhandler::~xmlskinhandler()
{

}

/*******************************************
*
* Inizio nuovo documento xml
*
*
*******************************************/
bool xmlskinhandler::startDocument()
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

/*******************************************
*
* Letta nuova tag
*
*
*******************************************/
bool xmlskinhandler::startElement( const QString&, const QString&, 
                                   const QString& qName, 
                                   const QXmlAttributes& )
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

/*******************************************
*
* Esco da un livello
*
*
*******************************************/
bool xmlskinhandler::endElement( const QString&, const QString&, const QString& )
{
    if (!CurTagL1.compare("extra"))
    {
        if (CurTagL3.isEmpty())
        {
            if (!CurTagL2.compare("bg"))
                *bg=new QColor(r,g,b);
            else if (!CurTagL2.compare("fg1"))
                *fg1=new QColor(r,g,b);
            else if (!CurTagL2.compare("fg2"))
                *fg2=new QColor(r,g,b);
        }
    }
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

/*******************************************
*
* Letto nuovo valore
*
*
*******************************************/
bool xmlskinhandler::characters( const QString & qValue)
{
      if (!CurTagL1.compare("extra"))
    {
        if ( (!CurTagL2.compare("bg")) || (!CurTagL2.compare("fg1")) || (!CurTagL2.compare("fg2")) )
        {
                if (!CurTagL3.compare("r"))
                    r=qValue.toInt( &ok, 10 );
                else if (!CurTagL3.compare("g"))
                    g=qValue.toInt( &ok, 10 );
                else if (!CurTagL3.compare("b"))
                    b=qValue.toInt( &ok, 10 );
            }           
    }
    return TRUE;
}

//_-------------------_------________-----------------------------------
//-___________-_____-------------____________________

xmlcfghandler::xmlcfghandler(int* v, char* c)
{
    logFile=c;
    verbosity=v;
}

/*******************************************
*
*******************************************/
xmlcfghandler::~xmlcfghandler()
{

}

/*******************************************
*
* Inizio nuovo documento xml
*
*
*******************************************/
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

/*******************************************
*
* Letta nuova tag
*
*
*******************************************/
bool xmlcfghandler::startElement( const QString&, const QString&, 
                                   const QString& qName, 
                                   const QXmlAttributes& )
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

/*******************************************
*
* Esco da un livello
*
*
*******************************************/
bool xmlcfghandler::endElement( const QString&, const QString&, const QString& )
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

/*******************************************
*
* Letto nuovo valore
*
*
*******************************************/
bool xmlcfghandler::characters( const QString & qValue)
{
    if (!CurTagL1.compare("root"))
    {
        if  (!CurTagL2.compare("sw"))
        {
            if (!CurTagL3.compare("BTouch"))
            {
                 if (!CurTagL4.compare("logverbosity"))
                     *verbosity=qValue.toInt( &ok, 10 );
            }
            else if (!CurTagL3.compare("openserver"))
            {
                 if (!CurTagL4.compare("logfile"))
                     strncat(logFile,qValue.ascii(),sizeof(logFile));
            }
        }           
    }
    return TRUE;
}
