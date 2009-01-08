/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** loads.cpp
 **
 **
 **
 ****************************************************************/

#include "loads.h"
#include "global.h" // BTouch
#include "generic_functions.h" // createMsgOpen
#include "xml_functions.h" // getChildren, getTextChild

#include <assert.h>


bannLoads::bannLoads(sottoMenu *parent, QString indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannLoads::gestFrame(char*)
{

}

void bannLoads::Attiva()
{
	BTouch->sendFrame(createMsgOpen("3", "2", getAddress()));
}


Loads::Loads(QDomNode config_node)
{
	loadItems(config_node);
}

void Loads::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		assert(id == CARICO && "Type of item not handled on loads page!");
		QString where = getTextChild(item, "where");
		QString img = IMG_PATH + getTextChild(item, "cimg1");

		banner *b = new bannLoads(this, where, img);
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
	}
}
