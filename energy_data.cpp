#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "generic_functions.h" // setCfgValue
#include "devices_cache.h" // bt_global::devices_cache
#include "energy_device.h" // EnergyDevice
#include "btmain.h" // bt_global::btmain
#include "energy_rates.h"
#include "bann_energy.h" // bannEnergyInterface
#include "bannercontent.h"
#include "navigation_bar.h"
#include "bannercontent.h"
#include "bann1_button.h" // BannSinglePuls
#include "btbutton.h"

#include <QVBoxLayout>
#include <QDomNode>
#include <QLocale>
#include <QDebug>

#include <assert.h>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


EnergyData::EnergyData(const QDomNode &config_node, bool edit_rates)
{
	loadTypes(config_node, edit_rates);
	connect(bt_global::btmain, SIGNAL(resettimer()), SLOT(systemTimeChanged()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateDayTimer()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateInterfaces()));
	day_timer.setSingleShot(true);

	if (interfaces.count() == 1)
		connect(interfaces[0], SIGNAL(Closed()), SLOT(Closed()));
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
	bt_global::energy_rates.loadRates();

	QList<QDomNode> families = getChildren(config_node, "energy_type");

	// display the button to edit rates if more than one family
	if (edit_rates && families.count() > 1)
	{
		NavigationBar *nav = new NavigationBar("currency_exchange");
		buildPage(new BannerContent, nav);

		Page *costs = new EnergyCost;

		connect(this, SIGNAL(forwardClick()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
	}
	else
		buildPage();

	SkinContext context(getTextChild(config_node, "cid").toInt());
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

		// TODO energy propagate rate changes to banner/pages

		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}


EnergyCost::EnergyCost()
{
	buildPage();

	bt_global::energy_rates.loadRates();

	QMap<int, EditEnergyCost *> costs;
	foreach (int rate_id, bt_global::energy_rates.allRateId())
	{
		const EnergyRate &rate = bt_global::energy_rates.getRate(rate_id);

		if (!costs.contains(rate.mode))
		{
			// create family cost page
			costs[rate.mode] = new EditEnergyCost;

			// create banner
			BannSinglePuls *b = new BannSinglePuls(0);

			b->initBanner(bt_global::skin->getImage("right"),
				      bt_global::skin->getImage("ok"),
				      rate.description);
			b->connectRightButton(costs[rate.mode]);

			page_content->appendBanner(b);

			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}

		costs[rate.mode]->addRate(rate_id);
	}
}


EditEnergyCost::EditEnergyCost()
{
	buildPage();
}

void EditEnergyCost::addRate(int rate_id)
{
	BannEnergyCost *b = new BannEnergyCost(rate_id, bt_global::skin->getImage("minus"),
					       bt_global::skin->getImage("plus"));

	page_content->appendBanner(b);
}


bool EnergyInterface::is_currency_view = false;

EnergyInterface::EnergyInterface(const QDomNode &config_node, bool edit_rates, bool parent_skipped)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("currency_exchange"));
	buildPage(new BannerContent, nav_bar);

	if (edit_rates)
	{
		Page *costs = new EnergyCost;

		connect(this, SIGNAL(forwardClick()), costs, SLOT(showPage()));
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));
	}
	else
		nav_bar->forward_button->setVisible(false);

	is_any_interface_enabled = false;
	loadItems(config_node, nav_bar);

	// only skip page if parent was not skipped
	if (parent_skipped)
		next_page = NULL;
	if (next_page)
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void EnergyInterface::loadItems(const QDomNode &config_node, NavigationBar *nav_bar)
{
	assert(bt_global::skin->hasContext() && "Skin context not set!");
	QString energy_type = getTextChild(config_node, "descr");
	bool show_currency_button = false;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int mode = getTextChild(item, "mode").toInt();
		QString measure = getTextChild(item, "measure");

		bool is_currency_enabled = getElement(item, "rate/ab").text().toInt();
		int rate_id = is_currency_enabled ? getElement(item, "rate/rate_id").text().toInt() : -1;

		// check if any of the interfaces have currency enabled
		show_currency_button |= is_currency_enabled;

		bannEnergyInterface *b = new bannEnergyInterface(this, rate_id, mode == 1);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		QString addr = getTextChild(item, "address");
		next_page = new EnergyView(measure, energy_type, addr, mode, rate_id);
		b->connectDxButton(next_page);
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		b->setInternalText("---");
		b->setUnitMeasure(measure);

		views.append(next_page);

		device *dev = bt_global::devices_cache[get_device_key("18", addr)];
		connect(dev, SIGNAL(status_changed(const StatusList &)), b, SLOT(status_changed(const StatusList &)));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
		b->Draw();
	}

	if (show_currency_button)
		is_any_interface_enabled = show_currency_button;

	if (page_content->bannerCount() > 1)
		next_page = NULL;
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
		bannEnergyInterface *b = static_cast<bannEnergyInterface*>(page_content->getBanner(i));
		b->updateText();
	}
}

void EnergyInterface::showPage()
{
	// restore visualization of raw data if we aren't enabled
	if (isCurrencyView() && !is_any_interface_enabled)
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
