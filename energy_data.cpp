/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "energy_data.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "devices_cache.h" // bt_global::devices_cache
#include "btmain.h" // bt_global::btmain
#include "energy_rates.h"
#include "bann_energy.h" // BannEnergyInterface
#include "navigation_bar.h"
#include "bann1_button.h" // BannSinglePuls
#include "btbutton.h"
#include "energy_management.h" // isBuilt, isRateEditDisplayed
#include "energy_graph.h"
#include "main.h" // getPageNodeFromChildNode

#include <QHash>
#include <QDomNode>
#include <QLocale>
#include <QDebug>
#include <QVBoxLayout>

namespace
{
	bool tryConvert(const QDomNode &n, const QString &node_path, int *value)
	{
		Q_ASSERT_X(value, "tryConvert", "value must be a valid pointer!");
		QDomElement e = getElement(n, node_path);
		bool ok;
		*value = e.text().toInt(&ok);
		if (!ok)
			qDebug() << "tryConvert failed converting node: " << node_path << "Text was: " << e.text();
		return ok;
	}

	int getDecimals(const QDomNode &n)
	{
		int tmp;
		if (tryConvert(n, "rate/n_decimal_view", &tmp))
			return tmp;
		else
			return 0;
	}

	// The language used for the floating point number
	static QLocale loc(QLocale::Italian);
}

EnergyData::EnergyData(const QDomNode &config_node)
{
	loadTypes(config_node, !EnergyManagement::isRateEditDisplayed());
	connect(bt_global::btmain, SIGNAL(resettimer()), SLOT(systemTimeChanged()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateDayTimer()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateInterfaces()));
	day_timer.setSingleShot(true);

	if (interfaces.count() == 1)
		connect(interfaces[0], SIGNAL(Closed()), SLOT(Closed()));
}

int EnergyData::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : ENERGY_DATA;
}

void EnergyData::updateDayTimer()
{
	const int msecs_in_a_day = 86400000;
	day_timer.stop();
	// msecsTo() below returns always negative
	int next_day_msecs = msecs_in_a_day + QTime::currentTime().msecsTo(QTime(0, 0, 0));
	day_timer.start(next_day_msecs);
}

void EnergyData::updateInterfaces()
{
	foreach(EnergyInterface *en_int, interfaces)
		en_int->systemTimeChanged();
}

void EnergyData::systemTimeChanged()
{
	updateDayTimer();
	updateInterfaces();
}

void EnergyData::showPage()
{
	if  (interfaces.count() == 1)
		interfaces[0]->showPage();
	else
		BannerPage::showPage();
}

void EnergyData::loadTypes(const QDomNode &config_node, bool edit_rates)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	EnergyRates::energy_rates.loadRates();
#ifdef CONFIG_BTOUCH
	QList<QDomNode> families = getChildren(config_node, "energy_type");
#else
	QList<QDomNode> families = getChildren(config_node, "item");
#endif

	// display the button to edit rates if more than one family
	if (edit_rates && EnergyRates::energy_rates.hasRates() && families.count() > 1)
	{
		Page *costs = new EnergyCost;

#ifdef LAYOUT_BTOUCH
		NavigationBar *nav = new NavigationBar(bt_global::skin->getImage("currency_exchange"));
		buildPage(new BannerContent, nav, getTextChild(config_node, "descr"));

		connect(this, SIGNAL(forwardClick()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
#else
		QWidget *main = new QWidget;
		QVBoxLayout *l = new QVBoxLayout(main);
		l->setContentsMargins(5, 5, 25, 47);

		BannerContent *content = new BannerContent;
		BtButton *edit_costs = new BtButton(bt_global::skin->getImage("currency_exchange"));

		l->addWidget(content, 1);
		l->addWidget(edit_costs, 0, Qt::AlignRight);

		buildPage(main, content, new NavigationBar, getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);

		connect(edit_costs, SIGNAL(clicked()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
#endif
	}
	else
		buildPage(getTextChild(config_node, "descr"));

	foreach (const QDomNode& type, families)
	{
		SkinContext cont(getTextChild(type, "cid").toInt());
		BannSinglePuls *b = new BannSinglePuls(0);

		b->initBanner(bt_global::skin->getImage("select"), bt_global::skin->getImage("energy_type"),
			      getTextChild(type, "descr"));
		page_content->appendBanner(b);

		EnergyInterface *en_interf = new EnergyInterface(type, edit_rates && families.count() == 1,
								 families.count() == 1);
		interfaces.push_back(en_interf);
		b->connectRightButton(en_interf);

		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());

		if (families.count() > 1)
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		else
			connect(b, SIGNAL(pageClosed()), SIGNAL(Closed()));
	}
}


static const char *family_cost_icons[] = {
	"cost_electricity", "cost_water", "cost_gas", "cost_dhw", "cost_conditioning"
};

static const char *family_descriptions[] = {
	QT_TRANSLATE_NOOP("EditEnergyCost", "Electricity"),
	QT_TRANSLATE_NOOP("EditEnergyCost", "Water"),
	QT_TRANSLATE_NOOP("EditEnergyCost", "Gas"),
	QT_TRANSLATE_NOOP("EditEnergyCost", "DHW"),
	QT_TRANSLATE_NOOP("EditEnergyCost", "Heating/Cooling")
};

EnergyCost::EnergyCost()
{
	buildPage();

	EnergyRates::energy_rates.loadRates();

	QHash<int, EditEnergyCost *> costs;
	foreach (int rate_id, EnergyRates::energy_rates.allRateId())
	{
		const EnergyRate &rate = EnergyRates::energy_rates.getRate(rate_id);

		if (!costs.contains(rate.mode))
		{
			// create family cost page
			next_page = costs[rate.mode] = new EditEnergyCost;

			// create banner
			BannSinglePuls *b = new BannSinglePuls(0);

			b->initBanner(bt_global::skin->getImage("forward"),
				      bt_global::skin->getImage(family_cost_icons[rate.mode - 1]),
				      family_descriptions[rate.mode - 1]);
			b->connectRightButton(costs[rate.mode]);

			page_content->appendBanner(b);

			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}

		costs[rate.mode]->addRate(rate_id);
	}

	if (page_content->bannerCount() > 1)
		next_page = NULL;
	if (next_page)
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void EnergyCost::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}


EditEnergyCost::EditEnergyCost()
{
#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	buildPage(new BannerContent, nav_bar);

	connect(nav_bar, SIGNAL(backClick()), SLOT(resetRates()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(saveRates()));
#else
	QWidget *main = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(main);
	l->setContentsMargins(5, 5, 25, 47);

	BannerContent *content = new BannerContent;
	BtButton *save = new BtButton(bt_global::skin->getImage("ok"));

	l->addWidget(content, 1);
	l->addWidget(save, 0, Qt::AlignRight);

	buildPage(main, content, new NavigationBar);

	connect(this, SIGNAL(Closed()), SLOT(resetRates()));
	connect(save, SIGNAL(clicked()), SLOT(saveRates()));
#endif

	production_count = consumption_count = 0;

}

void EditEnergyCost::addRate(int rate_id)
{
	const EnergyRate &rate = EnergyRates::energy_rates.getRate(rate_id);

#ifdef CONFIG_BTOUCH
	QString descr = rate.is_production ? tr("Production") : tr("Consumption");
	if (rate.is_production)
	{
		production_count += 1;
		if (production_count > 1)
			descr += " " + QString::number(production_count);
	}
	else
	{
		consumption_count += 1;
		if (consumption_count > 1)
			descr += " " + QString::number(consumption_count);
	}
#else
	QString descr = rate.description;
#endif

	BannEnergyCost *b = new BannEnergyCost(rate_id, bt_global::skin->getImage("minus"),
					       bt_global::skin->getImage("plus"),
					       descr);

	page_content->appendBanner(b);
}

void EditEnergyCost::saveRates()
{
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		BannEnergyCost *b = static_cast<BannEnergyCost *>(page_content->getBanner(i));

		b->saveRate();
	}

	emit Closed();
}

void EditEnergyCost::resetRates()
{
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		BannEnergyCost *b = static_cast<BannEnergyCost *>(page_content->getBanner(i));

		b->resetRate();
	}
}


bool EnergyInterface::is_currency_view = false;

EnergyInterface::EnergyInterface(const QDomNode &config_node, bool edit_rates, bool parent_skipped)
{
	BtButton *edit_costs = 0;

#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("currency_exchange"));
	if (edit_rates && EnergyRates::energy_rates.hasRates())
	{
		Page *costs = new EnergyCost;

		connect(this, SIGNAL(forwardClick()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
	}
	else
		nav_bar->forward_button->setVisible(false);

	edit_costs = nav_bar->forward_button;
	buildPage(new BannerContent, nav_bar);

#else
	QWidget *main = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(main);
	l->setContentsMargins(5, 5, 25, 47);

	BannerContent *content = new BannerContent;
	l->addWidget(content, 1);

	if (edit_rates && EnergyRates::energy_rates.hasRates())
	{
		Page *costs = new EnergyCost;
		edit_costs = new BtButton(bt_global::skin->getImage("currency_exchange"));
		l->addWidget(edit_costs, 0, Qt::AlignRight);
		connect(edit_costs, SIGNAL(clicked()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
	}
	buildPage(main, content, new NavigationBar, getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);
#endif

	bool show_currency_button = loadItems(config_node);
	if (edit_costs)
		edit_costs->setVisible(show_currency_button);

	// only skip page if parent was not skipped
	if (parent_skipped)
		next_page = NULL;
	if (next_page)
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

bool EnergyInterface::loadItems(const QDomNode &config_node)
{
	Q_ASSERT_X(bt_global::skin->hasContext() , "EnergyInterface::loadItems", "Skin context not set!");
	QString energy_type = getTextChild(config_node, "descr");
	bool is_any_interface_enabled = false;
	// IMPORTANT: the table instance is shared between all energy interfaces: any
	// signals must be disconnected and reconnected when the interface page is shown
	EnergyTable *table = new EnergyTable;
	EnergyGraph *graph = new EnergyGraph;

#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &item, getChildren(config_node, "item"))
#else
	foreach (const QDomNode &item, getChildren(getPageNodeFromChildNode(config_node, "lnk_pageID"), "item"))
#endif
	{
		int mode = getTextChild(item, "mode").toInt();
		QString measure = getTextChild(item, "measure");

#ifdef CONFIG_BTOUCH
		bool is_currency_enabled = getElement(item, "rate/ab").text().toInt();
		QString where = getTextChild(item, "address");
#else
		bool is_currency_enabled = getElement(item, "rate/enabled").text().toInt();
		QString where = getTextChild(item, "where");
#endif
		int rate_id = is_currency_enabled ? getElement(item, "rate/rate_id").text().toInt() : EnergyRates::INVALID_RATE;
		int decimals = getDecimals(item);

		// check if any of the interfaces have currency enabled
		is_any_interface_enabled |= is_currency_enabled;

		next_page = new EnergyView(measure, energy_type, where, mode, rate_id, decimals, table, graph);

		EnergyDevice *dev = bt_global::add_device_to_cache(new EnergyDevice(where, mode));

		BannEnergyInterface *b = new BannEnergyInterface(rate_id, mode == 1, getTextChild(item, "descr"), dev);
		b->connectRightButton(next_page);
		b->setUnitMeasure(measure);

		views.append(next_page);

		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}

	if (page_content->bannerCount() > 1)
		next_page = NULL;

	return is_any_interface_enabled;
}

void EnergyInterface::systemTimeChanged()
{
	for (int i = 0; i < views.size(); ++i)
		views[i]->systemTimeChanged();
}

bool EnergyInterface::checkTypeForCurrency(const QString &type, const QDomNode &conf)
{
	bool currency_enabled = (getElement(conf, "currency/ab").text().toInt() == 1);
	QString node_path;
	switch (type.toInt())
	{
	case 0: //consumption
		node_path = "cons/ab";
		break;
	case 1: // production
		node_path = "prod/ab";
		break;
	}
	QDomElement node = getElement(conf, node_path);
	if (!node.isNull())
	{
		bool cons_ab = (node.text().toInt() == 1);
		return currency_enabled && cons_ab;
	}
	else
		return false;
}

void EnergyInterface::updateBanners()
{
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		BannEnergyInterface *b = static_cast<BannEnergyInterface*>(page_content->getBanner(i));
		b->updateText();
	}
}

void EnergyInterface::showPage()
{
	// restore visualization of raw data
	if (isCurrencyView())
		toggleCurrencyView();

	updateBanners();
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}

void EnergyInterface::toggleCurrency()
{
	toggleCurrencyView();
	updateBanners();
}

void EnergyInterface::toggleCurrencyView()
{
	is_currency_view = !is_currency_view;
}

bool EnergyInterface::isCurrencyView()
{
	return is_currency_view;
}
