/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** carico.cpp
 **
 **
 **
 ****************************************************************/

#include "carico.h"
#include "global.h" // BTouch
#include "generic_functions.h" // createMsgOpen
#include "xml_functions.h" // getChildren, getTextChild

#include <assert.h>


bannCarico::bannCarico(sottoMenu *parent, QString indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannCarico::gestFrame(char*)
{

}

void bannCarico::Attiva()
{
	BTouch->sendFrame(createMsgOpen("3", "2", getAddress()));
}

void bannCarico::inizializza(bool forza)
{
}


Carico::Carico(QDomNode config_node)
{
	loadItems(config_node);
}

void Carico::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		assert(id == CARICO && "Type of item not handled on carico page!");
		QString where = getTextChild(item, "where");
		QString img = IMG_PATH + getTextChild(item, "cimg1");

		banner *b = new bannCarico(this, where, img);
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
	}
}
