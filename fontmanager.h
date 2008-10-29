/****************************************************************
**
** Develer S.r.l
**
**fontmanager.cpp
**
**definizione classe di gestione della configurazione dei font
**
****************************************************************/


#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QFont>
#include <QMap>
#include <QString>

/**
 * \brief enumerazione dei font usati nel codice BTicino
 * convenzione usata per i nomi: font_nomedelfile_descrizione
 *
*/
#define first_font 1
enum eFontID
{
	font_tastiera_scritta_label =first_font, // 14 QFont::Bold
	font_tastiera_digit_label, // 20 QFont::Bold
	font_tastiera_bottoni_stati, // 16, QFont::Bold)
	font_aux_descr_ambiente, // 24, QFont::Bold)
	font_aux_nome_sorgente, // 24, QFont::Bold)
	font_listbrowser, //  18)
	font_playwindow, //  18);
	font_items_bannertext,  // 14, QFont::Bold)
	font_items_secondarytext, // 18, QFont::Bold)
	font_versio_datiGen, // 24, QFont::Black,TRUE)
	font_versio_datiGenFw, //  14, QFont::Bold)
	font_allarme_descr, // 20, QFont::Bold
	font_banTermo_tempMis, //"Times", 40, QFont::Bold
	font_banTermo_tempImp, //("Times", 25, QFont::Bold)
	font_banTermo_sondoffanti, //("Times", 20, QFont::Bold)
	font_banTermo_sondoffanti2, //("Times", 40, QFont::Bold)
	font_banTermo_testo, // "Times", 20, QFont::Bold)
	font_scenEvoCond_Area2, // 20, QFont::Bold
	font_scenEvoCond_light_status, //  20, QFont::Bold)
	font_scenEvoCond_device_condition_dimming, // 20, QFont::Bold
	font_radio_descrizione_ambiente, //  12, QFont::Bold)
	font_radio_rdsLabel, // 26, QFont::Bold
	font_radio_radioName, // 12, QFont::Bold
	font_radio_progrText, //   20, QFont::Bold
	font_homepage_bottoni_label, //  14, QFont::Bold)
	font_homepage_bottoni_descrizione,  // , 14, QFont::Bold
	font_banner_BannerText, // , 14, QFont::Bold
	font_banner_SecondaryText, // , 18, QFont::Bold
	font_sveglia_scelta, //, 14, QFont::Bold)
	font_videocitof_area1, //, 20, QFont::Bold)
	font_videocitof_area3, //  20, QFont::Bold)
	font_videocitof_area5, //, 20, QFont::Bold)
	font_stopngo_page_title, // 14, QFont::Bold
	font_stopngo_page_labels, // 14, QFont::Bold
	invalid_font
};


/**
 * \brief classe di gestione dei font
 * singleton che legge da file XML i settaggi richiesti per ogni font
 *
*/

static const char XMLfieldNames[][12] = {"id", "family", "size", "weight", "descr" };

class FontManager
{
#ifdef GENERAFONT
public:
#endif
	
	enum eXMLfieldPos {eId, eFamily, eSize, eWeight, eDescr}; //! posizione delle descrizioni in XMLfieldNames

	enum
	{
		nFontInfoFields = 5,   //! numero di campi obligtori da mettere nel file XML, incluso "descr"
		maxFamilyLen = 30, //! massima dimensione del nome del font
	};
	struct FontInfo
	{
		char *family() { return _family; }

		int id;
		char _family[maxFamilyLen];
		int size;
		enum QFont::Weight weight;
	};

	int getFontInfo(eFontID, FontInfo &);

private:
	FontManager();
	FontManager(const FontManager &);
	FontManager &operator=(const FontManager&);
	int loadFonts();

	static FontManager * m_pinstance;
	QMap<eFontID, FontInfo> m_map;
	int addElement(int nElement, FontInfo & fi, QString tagName,  QString text);

public:
	static FontManager * instance();
	int getFont(eFontID, QFont &);
};

#endif // FONTMANAGER_H
