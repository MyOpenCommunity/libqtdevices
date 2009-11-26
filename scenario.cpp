#include "scenario.h"
#include "bann_scenario.h"
#include "scenevocond.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "btmain.h" // bt_global::btmain
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" //SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

namespace
{
#ifdef CONFIG_BTOUCH
	QList<scenEvo_cond*> loadConditions(const QDomNode &config_node)
	{
		// NOTE: the ownership of scenEvo_cond objects is taken by the object that
		// store the list of conditions.
		// TODO: we can have at maximum 1 condH and 1 condDevice, remove lists
		bool has_next = getElement(config_node, "condDevice/value").text().toInt();
		QList<scenEvo_cond*> l;
		foreach (const QDomNode &cond, getChildren(config_node, "condH"))
		{
			if (getTextChild(cond, "value").toInt())
			{
				scenEvo_cond_h *c = new scenEvo_cond_h(cond, has_next);
				QObject::connect(bt_global::btmain, SIGNAL(resettimer()), c, SLOT(setupTimer()));
				l.append(c);
			}
		}

		foreach (const QDomNode &cond, getChildren(config_node, "condDevice"))
		{
			if (getTextChild(cond, "value").toInt())
			{
				scenEvo_cond_d *c = new scenEvo_cond_d(cond);
				l.append(c);
			}
		}
		return l;
	}
#else
	QList<scenEvo_cond*> loadConditions(const QDomNode &config_node)
	{
		// NOTE: the ownership of scenEvo_cond objects is taken by the object that
		// store the list of conditions.
		// TODO: we can have at maximum 1 condH and 1 condDevice, remove lists
		bool has_next = getElement(config_node, "scen/device/status").text().toInt();
		QList<scenEvo_cond*> l;
		// parse time condition
		QDomNode cond = getElement(config_node, "scen/time");
		if (getTextChild(cond, "status").toInt())
		{
			scenEvo_cond_h *c = new scenEvo_cond_h(cond, has_next);
			QObject::connect(bt_global::btmain, SIGNAL(resettimer()), c, SLOT(setupTimer()));
			l.append(c);
		}

		/*
		  TODO: enabling device condition means rewriting conf parsing!
		foreach (const QDomNode &cond, getChildren(config_node, "condDevice"))
		{
			if (getTextChild(cond, "value").toInt())
			{
				scenEvo_cond_d *c = new scenEvo_cond_d(cond);
				l.append(c);
			}
		}
		*/
		return l;
	}
#endif
}


Scenario::Scenario(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
	section_id = getTextChild(config_node, "id").toInt();
}

int Scenario::sectionId()
{
	return section_id;
}

banner *Scenario::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");

	QString what = getTextChild(item_node, "what");
	if (!what.isEmpty())
		where = what + "*" + where;

	// DELETE
//		QString img1 = IMG_PATH + getTextChild(item_node, "cimg1");
//		QString img2 = IMG_PATH + getTextChild(item_node, "cimg2");
//		QString img3 = IMG_PATH + getTextChild(item_node, "cimg3");
//		QString img4 = IMG_PATH + getTextChild(item_node, "cimg4");

	banner *b = 0;
	switch (id)
	{
	case SCENARIO:
		// DELETE
		//b = new bannScenario(0, where, img1);
		b = new BannSimpleScenario(0, item_node);
		break;
	case MOD_SCENARI:
	{
		QString img5 = IMG_PATH + getTextChild(item_node, "cimg5");
		QString img6 = IMG_PATH + getTextChild(item_node, "cimg6");
		QString img7 = IMG_PATH + getTextChild(item_node, "cimg7");
		// DELETE
		//b = new gesModScen(0, where, img1, img2, img3, img4, img5, img6, img7);
		b = new ModifyScenario(0, item_node);
		break;
	}
#ifdef CONFIG_BTOUCH
	case SCENARIO_EVOLUTO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		b = new scenEvo(0, loadConditions(item_node), getElement(item_node, "action/open").text(),
			getTextChild(item_node, "enable").toInt());
	}
		break;
	case SCENARIO_SCHEDULATO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		QList<QString> names, img, descr, icon_names;
		names << "unable" << "disable" << "start" << "stop";
		icon_names << bt_global::skin->getImage("enable_scen") << bt_global::skin->getImage("disable_scen") <<
			bt_global::skin->getImage("start") << bt_global::skin->getImage("stop");

		for (int i = 0; i < names.size(); ++i)
		{
			QDomNode n = getChildWithName(item_node, names[i]);
			if (!n.isNull())
			{
				int v = getTextChild(n, "value").toInt();
				// TODO: this is just a quick fix to use the skin manager instead of "cimg" tags
				//  Remove when scenSched banner is rewritten
				img.append(v ? icon_names[i] : QString());
				descr.append(v ? getTextChild(n, "open") : QString());
			}
			else
			{
				qWarning() << "Unable to find node" << names[i] << "on scenario Schedulato configuration";
				img.append(QString());
				descr.append(QString());
			}
		}
		b = new scenSched(0, img[0], img[1], img[2], img[3], descr[0], descr[1], descr[2], descr[3]);
		break;
	}
	case PPT_SCE:
		b = new PPTSce(0, where, getTextChild(item_node, "cid").toInt());
		break;
	}
#else
	case SCENARIO_SCHEDULATO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		QList<QString> names, img, descr, icon_names;
		names << "attiva" << "disattiva" << "start" << "stop";
		icon_names << bt_global::skin->getImage("enable_scen") << bt_global::skin->getImage("disable_scen") <<
			bt_global::skin->getImage("start") << bt_global::skin->getImage("stop");
		for (int i = 0; i < names.size(); ++i)
		{
			// look for a node called where{attiva,disattiva,start,stop} to be similar to previous code
			QDomElement where = getElement(item_node, QString("schedscen/where") + names[i]);
			qDebug() << "where: " << where.nodeName();
			if (!where.isNull())
			{
				img.append(icon_names[i]);
				// build frame open to send.
				// TODO: this is to reuse scenSched in its current form, it can be changed when
				//  scenSched is rewritten
				QDomElement what = getElement(item_node, QString("schedscen/what") + names[i]);
				qDebug() << "what: " << what.nodeName();
				QString frame = QString("*15*%1*%2##").arg(what.toElement().text()).arg(where.toElement().text());
				descr.append(frame);
			}
			else
			{
				img.append(QString());
				descr.append(QString());
			}
		}
		b = new scenSched(0, img[0], img[1], img[2], img[3], descr[0], descr[1], descr[2], descr[3]);
		break;
	}
	case SCENARIO_EVOLUTO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		b = new scenEvo(0, loadConditions(item_node), getElement(item_node, "scen/action/open").text(),
			getTextChild(item_node, "scen/status").toInt());
	}
	}
#endif

	if (b)
	{
		b->setText(getTextChild(item_node, "descr"));
		b->setId(id);
		b->Draw();
	}
	return b;
}

void Scenario::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on scenario page!", id);
		}
	}
}

