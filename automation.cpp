#include "automation.h"
#include "xml_functions.h"
#include "actuators.h"
#include "automatismi.h"

#include <QDebug>


Automation::Automation(QWidget *parent, QDomNode config_node) : sottoMenu(parent)
{
	QList<QDomNode> items = getChildren(config_node, "item");

	for (int i = 0; i < items.size(); ++i)
	{
		int id = getTextChild(items[i], "id").toInt();
		QString where = getTextChild(items[i], "where");
		QString img1 = IMG_PATH + getTextChild(items[i], "cimg1");
		QString img2 = IMG_PATH + getTextChild(items[i], "cimg2");
		QString img3 = IMG_PATH + getTextChild(items[i], "cimg3");
		QString img4 = IMG_PATH + getTextChild(items[i], "cimg4");
		QString time = getTextChild(items[i], "time");

		banner *b;
		switch (id)
		{
		case ATTUAT_AUTOM_INT_SIC:
			b = new attuatAutomIntSic(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_AUTOM_INT:
			b = new attuatAutomInt(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_AUTOM:
			b = new attuatAutom(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_VCT_SERR:
			b = new attuatPuls(this, where, img1, img2, VCT_SERR);
			break;
		case GR_ATTUAT_INT:
	  		{
			QList<QString> addresses;
			QList<QDomNode> elements = getChildren(items[i], "element");
			for (int j = 0; j < elements.size(); ++j)
				addresses.append(getTextChild(elements[j], "where"));
				
			b = new grAttuatInt(this, addresses, img1, img2, img3);
			break;
			}
		case AUTOM_CANC_ATTUAT_ILL:
			b = new automCancAttuatIll(this, where, img1, img2, time);
			break;
		case AUTOM_CANC_ATTUAT_VC:
			b = new automCancAttuatVC(this, where, img1, img2);
			break;
		case ATTUAT_AUTOM_PULS:
			b = new attuatPuls(this, where, img1, img2, AUTOMAZ);
			break;
		default:
			assert(!"Type of item not handled on automation page!");
		}

		b->setText(getTextChild(items[i], "descr"));
		b->setId(id);
		// TODO: gestire il serNum come fa sottoMenu::addItemU!!
		elencoBanner.append(b);
	}
}