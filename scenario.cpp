#include "scenario.h"
#include "bann_scenario.h"
#include "scenevocond.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "btmain.h" // bt_global::btmain

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Scenario::Scenario(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Scenario::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
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
			// DELETE
			//b = new bannScenario(this, where, img1);
			b = new BannSimpleScenario(this, item);
			break;

		case PPT_SCE:
			b = new PPTSce(this, where, getTextChild(item, "cid").toInt());
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
					img.append(v ? IMG_PATH + getTextChild(n, "cimg1") : QString());
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
			qFatal("Type of item not handled on scenario page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}

QList<scenEvo_cond*> Scenario::loadConditions(const QDomNode &config_node)
{
	// Note: the ownership of scenEvo_cond objects is taken by scenEvo instance.
	QList<scenEvo_cond*> l;
	foreach (const QDomNode &cond, getChildren(config_node, "condH"))
	{
		scenEvo_cond_h *c = new scenEvo_cond_h(getTextChild(cond, "hour"), getTextChild(cond, "minute"));
		if (int v = getTextChild(cond, "value").toInt())
		{
			c->setConditionType(v);
			for (int i = 1; i <= 4; ++i)
			{
				QString img = getTextChild(cond, "cimg" + QString::number(i));
				if (!img.isEmpty())
					c->setImg(i - 1,  IMG_PATH + img);
			}
			connect(bt_global::btmain, SIGNAL(resettimer()), c, SLOT(setupTimer()));
			c->SetIcons();
			l.append(c);
		}
	}

	foreach (const QDomNode &cond, getChildren(config_node, "condDevice"))
	{
		scenEvo_cond_d *c = new scenEvo_cond_d;
		if (int v = getTextChild(cond, "value").toInt())
		{
			c->setConditionType(v);
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
