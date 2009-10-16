#include "bann2_buttons.h"
#include "btbutton.h"
#include "fontmanager.h" // FontManager
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getBostikName

#include <QWidget>
#include <QLabel>


#define BAN2BUT_BUT_DIM 60
#define BUTONOFF_ICON_DIM_X 120
#define BUTONOFF_ICON_DIM_Y 60
#define BANONOFF_BUT_DIM 60
#define BUTONOFF2SCR_ICON_DIM_X 80
#define BUTONOFF2SCR_ICON_DIM_Y 60
#define BANONOFF2SCR_BUT_DIM 60



BannOnOffState::BannOnOffState(QWidget *parent) :
	banner(parent)
{
	sx_button = new BtButton(this);
	sx_button->setGeometry(0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	dx_button = new BtButton(this);
	dx_button->setGeometry(banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	text = new QLabel(this);
	text->setGeometry(0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	text->setFont(bt_global::font->get(FontManager::TEXT));

	center_icon = new QLabel(this);
	center_icon->setGeometry(BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}

void BannOnOffState::initBanner(const QString &left, const QString &center, const QString &right,
	States init_state, const QString &banner_text)
{
	loadIcons(left, center, right);
	setState(init_state);
	text->setText(banner_text);
}

void BannOnOffState::loadIcons(const QString &l, const QString &c, const QString &r)
{
	left = l;
	center = c;
	right = r;

	sx_button->setImage(left);
	dx_button->setImage(right);
}

void BannOnOffState::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "on")));
		break;
	case OFF:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "on")));
		break;
	}
}


BannOpenClose::BannOpenClose(QWidget *parent) :
	banner(parent)
{
	left_button = new BtButton(this);
	left_button->setGeometry(0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	right_button = new BtButton(this);
	right_button->setGeometry(banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	text = new QLabel(this);
	text->setGeometry(0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	text->setFont(bt_global::font->get(FontManager::TEXT));

	center_icon = new QLabel(this);
	center_icon->setGeometry(BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}

void BannOpenClose::initBanner(QString left, QString center, QString right, QString lr_alternate,
	States starting_state, QString banner_text)
{
	loadIcons(left, center, right, lr_alternate);
	setState(starting_state);
	text->setText(banner_text);
}

void BannOpenClose::loadIcons(QString _left, QString _center, QString _right, QString _alternate)
{
	right = _right;
	left =_left;
	center = _center;
	alternate = _alternate;
}

void BannOpenClose::setState(States new_state)
{
	switch (new_state)
	{
	case STOP:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
		left_button->setImage(left);
		right_button->setImage(right);
		break;
	case OPENING:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "o")));
		right_button->setImage(alternate);
		left_button->setImage(left);
		break;
	case CLOSING:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "c")));
		right_button->setImage(right);
		left_button->setImage(alternate);
		break;
	}
}


bann2But::bann2But(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0,(banner_height - BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN2BUT_BUT_DIM ,(banner_height - BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(TEXT, BAN2BUT_BUT_DIM, 0, banner_width - 2 * BAN2BUT_BUT_DIM, banner_height);
}


bann2ButLab::bann2ButLab(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN2BUT_BUT_DIM, 0, BAN2BUT_BUT_DIM, BAN2BUT_BUT_DIM);
	addItem(TEXT, BAN2BUT_BUT_DIM , 0,banner_width - 2 * BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(TEXT2, 0, BAN2BUT_BUT_DIM-5, banner_width , 25);
}

void bann2ButLab::setAutoRepeat()
{
	sxButton->setAutoRepeat(true);
	dxButton->setAutoRepeat(true);
}


QSize bann2ButLab::sizeHint() const
{
	return banner::sizeHint() + QSize(0, 20);
}



bannOnOff::bannOnOff(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);
	addItem(BUT2, 0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);
	addItem(TEXT, 0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	addItem(ICON, BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}


bannOnOff2scr::bannOnOff2scr(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BANONOFF2SCR_BUT_DIM , BANONOFF2SCR_BUT_DIM);
	addItem(BUT2, banner_width-BANONOFF2SCR_BUT_DIM, 0, BANONOFF2SCR_BUT_DIM, BANONOFF2SCR_BUT_DIM);
	addItem(TEXT, 0, BANONOFF2SCR_BUT_DIM, banner_width, banner_height-BANONOFF2SCR_BUT_DIM);
	addItem(ICON, banner_width-BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X, 0, BUTONOFF2SCR_ICON_DIM_X, BUTONOFF2SCR_ICON_DIM_Y);
	addItem(TEXT2, BANONOFF2SCR_BUT_DIM, 0, banner_width-2*BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X, BUTONOFF2SCR_ICON_DIM_Y);
}

