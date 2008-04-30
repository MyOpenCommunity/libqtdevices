/****************************************************************
**
** Develer S.r.l
**
**fontmanager.cpp
**
**implementazione della classe di gestione della configurazione dei font
**
****************************************************************/

#include "fontmanager.h"
#include "main.h" // MY_FILE_CFG_FONT
#include <qdom.h>
#include <qfile.h>
#include <stdlib.h>

/**
 * \brief inizializzazione del membro statico
 */
FontManager* FontManager::m_pinstance = 0;

/**
 * \brief implementazione del singleton
 */
FontManager* FontManager::instance ( )
{
	if ( m_pinstance == 0 )
	{
		m_pinstance = new FontManager;
	}
    return m_pinstance;
}

/**
 * \brief costruttore di default
*/
FontManager::FontManager( )
{
	loadFonts();
}

//#define PRINT_ON_CONSOLE
/**
 * \brief colleziona gli elementi di un font (id, family, size...) 
 *          e quando la descrizione e' completa salva il font nella mappa
 */
int FontManager::addElement( int nElement, FontInfo & fi, const char * tagName,  const char * text )
{
	static int testCounter = 0;
	
#ifdef PRINT_ON_CONSOLE 
	printf ("\t%s\t%s\n", tagName ,text  );
#endif
	
	if ( strcasecmp ( tagName, XMLfieldNames[ eId ] ) == 0 )
	{
		fi.id = atoi( text );
		testCounter ++;
	}
	else if ( strcasecmp ( tagName, XMLfieldNames[ eFamily ] ) == 0 )
	{
		strncpy ( fi._family, text, maxFamilyLen );
		testCounter ++;
	}
	else if ( strcasecmp ( tagName,  XMLfieldNames[ eSize ] ) == 0 )
	{
		fi.size = atoi( text );
		testCounter ++;
	}
	else if ( strcasecmp ( tagName,  XMLfieldNames[ eWeight ] ) == 0 )
	{
		fi.weight = (QFont::Weight) atoi( text );
		testCounter ++;
	}
	else if ( strcasecmp ( tagName,  XMLfieldNames[ eDescr ] ) == 0 )
	{
		testCounter ++;
	}

	if ( testCounter != nElement )
	{
		return -1;
	}

	if ( testCounter == nFontInfoFields )
	{
#ifdef PRINT_ON_CONSOLE 
		printf ("added %d %s %d %d\n", fi.id, fi._family, fi.size, fi.weight );
#endif
		m_map.insert( (eFontID) fi.id, fi);
		testCounter = 0;
	}
	return 0;
}

/**
 * \brief legge il file XML contenente le dichiarazioni dei font
 * 
 * \return 0= tutto ok, -1 file non leggibile, -2 xml invalido, -3 manca qualche elemento
 */
int FontManager::loadFonts()
{
	QDomDocument doc( "mydocument" );
	QFile file( MY_FILE_CFG_FONT );
	if ( !file.open( IO_ReadOnly ) )
		return -1;
	if ( !doc.setContent( &file ) ) 
	{
		file.close();
		return -2;
	}
	file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
	QDomElement docElem = doc.documentElement();

	QDomNode n = docElem.firstChild();
	while( !n.isNull() ) 
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if( !e.isNull() )
		{
			// printf ("%s",  e.tagName().ascii() );
			int nElement = 0;
			FontInfo fi;
			for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
			{
				if ( n.isElement() ) 
				{
					nElement ++;
					QDomElement e = n.toElement();
					int err = addElement( nElement , fi, e.tagName().ascii() , e.text().ascii() );
					if (  err  != 0 )
					{
						printf ("%d %s %d",  err, __FILE__, __LINE__);
						// todo log errore
						return -3;
					}
				}
			}
		}
		n = n.nextSibling();
	}
	return 0;
}

/**
 * \brief funzione pubblica che imposta l'oggetto font passato in base al fontID richiesto
 *  in caso di font ID non esistente setta un font di default
 * 
 * \return 0=tutto bene, -1=font non esistente nel file di configurazione
 */
int FontManager::getFont( eFontID fontID, QFont & font)
{
	FontInfo fi;
	int ret = getFontInfo ( fontID, fi );
	font.setFamily( fi.family() );
	font.setPointSize ( fi.size );
	font.setWeight ( fi.weight );
	return ret;
}

/**
 * \brief funzione privata che imposta in una struttura interna il font relativo al fontID richiesto
 *  in caso di font ID non esistente setta un font di default
 * 
 * \return 0=tutto bene, -1=font non esistente nel file di configurazione
 */
int FontManager::getFontInfo( eFontID fontID, FontInfo & fi)
{
	QMap <eFontID, FontInfo>::iterator it = m_map.find( fontID );
	if ( it != m_map.end() )
	{
		fi = *it;
		return 0;
	}
	strcpy ( fi._family, "arial" ); // default font
	fi.size= 24;
	fi.weight = QFont::Normal ;
	return -1;
}

