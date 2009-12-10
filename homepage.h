/****************************************************************
**
** Definizione della classe della pagina principale
**
****************************************************************/

#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "sectionpage.h"

#include <QString>

class timeScript;
class TemperatureViewer;

class QDomNode;


/*!
  \class HomePage
  \brief This is the class used to implement the main page from which access to all
  the subtree of the various bticino functions.

  It can be composed of a date field, a time field, up to three temperatures, a number of BtButton and a special command. 
  \author Davide
  \date lug 2005
*/

class HomePage : public SectionPage
{
Q_OBJECT
public:
	HomePage(const QDomNode &config_node);
	void inizializza();

	virtual PageType pageType();

public slots:
	void showSectionPage(int page_id);

private:
	TemperatureViewer *temp_viewer;
	void loadItems(const QDomNode &config_node);
};


#endif // HOME_PAGE_H
