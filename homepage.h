/****************************************************************
**
** Definizione della classe della pagina principale
**
****************************************************************/

#ifndef BT_HOMEPAGE
#define BT_HOMEPAGE

#include "pagecontainer.h"

#include <QString>

class timeScript;
class TemperatureViewer;

class QDomNode;


/*!
  \class homePage
  \brief This is the class used to implement the main page from which access to all
  the subtree of the various bticino functions.

  It can be composed of a date field, a time field, up to three temperatures, a number of BtButton and a special command. 
  \author Davide
  \date lug 2005
*/

class homePage : public SectionPageContainer
{
Q_OBJECT
public:
	homePage(const QDomNode &config_node);
	void inizializza();

	virtual PageType pageType();

private:
	TemperatureViewer *temp_viewer;
	void loadItems(const QDomNode &config_node);
};


#endif // BT_HOMEPAGE
