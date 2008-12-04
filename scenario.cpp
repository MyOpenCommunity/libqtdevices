#include "scenario.h"
#include "bann_scenario.h"
#include "scenevocond.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Scenario::Scenario(QWidget *parent, QDomNode config_node) : sottoMenu(parent)
{
	loadItems(config_node);
}

void Scenario::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "where");

		QString what = getTextChild(item, "what");
		if (!what.isEmpty())
			where = what + "*" + where;

		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");

		banner *b;
		switch (id)
		{
		case SCENARIO:
			b = new bannScenario(this, where, img1);
			break;

		case MOD_SCENARI:
		{
			QString img5 = IMG_PATH + getTextChild(item, "cimg5");
			QString img6 = IMG_PATH + getTextChild(item, "cimg6");
			QString img7 = IMG_PATH + getTextChild(item, "cimg7");
			b = new gesModScen(this, where, img1, img2, img3, img4, img5, img6, img7);
			break;
		}
		case SCENARIO_EVOLUTO:
			b = new scenEvo(this, loadConditions(item), img1, img2, img3, img4,
				getElement(item, "action/open").text(), getTextChild(item, "enable").toInt());
			break;

		case SCENARIO_SCHEDULATO:
		{
			QList<QString> names, img, descr;
			names << "unable" << "disable" << "start" << "stop";

			for (int i = 0; i < names.size(); ++i)
			{
				QDomNode n = getChildWithName(item, names[i]);
				if (!n.isNull())
				{
					int v = getTextChild(n, "value").toInt();
					img.append(v ? getTextChild(n, "cimg1") : QString());
					descr.append(v ? getTextChild(n, "open") : QString());
				}
				else
				{
					qWarning() << "Unable to find node" << names[i] << "on scenario Schedulato configuration";
					img.append(QString());
					descr.append(QString());
				}
			}
			b = new scenSched(this, img[0], img[1], img[2], img[3], descr[0], descr[1], descr[2], descr[3]);
			break;
		}
		default:
			assert(!"Type of item not handled on scenario page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
	}
}

QList<scenEvo_cond*> Scenario::loadConditions(QDomNode config_node)
{
	// Note: the ownership of scenEvo_cond objects is taken by scenEvo instance.
	QList<scenEvo_cond*> l;
	QDomNode cond;
	foreach (cond, getChildren(config_node, "condH"))
	{
		scenEvo_cond_h *c = new scenEvo_cond_h(0);
		if (int v = getTextChild(cond, "value").toInt())
		{
			c->setVal(v);
			c->set_h(getTextChild(cond, "hour"));
			c->set_m(getTextChild(cond, "minute"));
			for (int i = 1; i <= 4; ++i)
			{
				QString img = getTextChild(cond, "cimg" + QString::number(i));
				if (!img.isNull())
					c->setImg(i - 1,  IMG_PATH + img);
			}
			connect(this, SIGNAL(resettimer()), c, SLOT(setupTimer()));
			c->SetIcons();
			l.append(c);
		}
	}

	foreach (cond, getChildren(config_node, "condDevice"))
	{
		scenEvo_cond_d *c = new scenEvo_cond_d(0);
		if (int v = getTextChild(cond, "value").toInt())
		{
			c->setVal(v);
			c->set_descr(getTextChild(cond, "descr"));
			c->set_where(getTextChild(cond, "where"));
			c->set_trigger(getTextChild(cond, "trigger"));
			for (int i = 1; i <= 5; ++i)
			{
				QString img = getTextChild(cond, "cimg" + QString::number(i));
				if (!img.isNull())
					c->setImg(i - 1,  IMG_PATH + img);
			}
			c->SetIcons();
			l.append(c);
		}
	}
	return l;
}
