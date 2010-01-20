#include "bann2_buttons.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getBostikName
#include "titlelabel.h"

#include <QWidget>
#include <QLabel>
#include <QVariant> // used for setProperty
#include <QBoxLayout>


#define BAN2BUT_BUT_DIM 60
#define BUTONOFF_ICON_DIM_X 120
#define BUTONOFF_ICON_DIM_Y 60
#define BANONOFF_BUT_DIM 60
#define BUTONOFF2SCR_ICON_DIM_X 80
#define BUTONOFF2SCR_ICON_DIM_Y 60
#define BANONOFF2SCR_BUT_DIM 60



Bann2LinkedPages::Bann2LinkedPages(QWidget *parent) :
	BannerNew(parent)
{
}

void Bann2LinkedPages::connectLeftButton(Page *p)
{
	connectButtonToPage(left_button, p);
}

void Bann2LinkedPages::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}



BannOnOffNew::BannOnOffNew(QWidget *parent) :
	Bann2LinkedPages(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	center_icon = new TextOnImageLabel;

	QGridLayout *grid = new QGridLayout;
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	grid->addWidget(left_button, 0, 0);
	grid->setColumnStretch(0, 1);
	grid->addWidget(center_icon, 0, 1, Qt::AlignCenter);
	grid->addWidget(right_button, 0, 2);
	grid->setColumnStretch(2, 1);

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->setSpacing(0);
	vbox->addLayout(grid);
	vbox->addWidget(text);
}

void BannOnOffNew::initBanner(const QString &l, const QString &c, const QString &r,
	const QString &banner_text)
{
	center = c;

	initButton(left_button, l);
	initButton(right_button, r);
	center_icon->setBackgroundImage(c);
	text->setText(banner_text);
}

void BannOnOffNew::setBannerText(const QString &str)
{
	text->setText(str);
}

void BannOnOffNew::setInternalText(const QString &text)
{
	center_icon->setInternalText(text);
}


BannOnOffState::BannOnOffState(QWidget *parent) :
	BannOnOffNew(parent)
{
}

void BannOnOffState::initBanner(const QString &left, const QString &center, const QString &right,
	States init_state, const QString &banner_text)
{
	BannOnOffNew::initBanner(left, center, right, banner_text);
	setState(init_state);
}

void BannOnOffState::setState(States new_state)
{
	center_icon->setBackgroundImage(getBostikName(center, new_state == ON ? "on" : "off"));
}


Bann2Buttons::Bann2Buttons(QWidget *parent) :
	Bann2LinkedPages(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::TEXT));

	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, 0);
	l->setColumnStretch(0, 1);
	l->addWidget(text, 0, 1);
	l->setColumnStretch(1, 2);
	l->addWidget(right_button, 0, 2);
	l->setColumnStretch(2, 1);
}

void Bann2Buttons::initBanner(const QString &left, const QString &right, const QString &banner_text,
	FontManager::Type font_type)
{
	initButton(left_button, left);
	initButton(right_button, right);
	text->setText(banner_text);
	QFont central_font = bt_global::font->get(font_type);

	text->setFont(central_font);
}

void Bann2Buttons::setCentralText(const QString &t)
{
	text->setText(t);
}


BannOpenClose::BannOpenClose(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_icon, 1, Qt::AlignHCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
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


BannOnOff2Labels::BannOnOff2Labels(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;
	center_text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERTEXT));

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_text, 1, Qt::AlignCenter);
	hbox->addWidget(center_icon, 0, Qt::AlignCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannOnOff2Labels::initBanner(const QString &left, const QString &_center, const QString &right,
		States init_state, const QString &banner_text, const QString &second_text)
{
	left_button->setImage(left);
	center = _center;
	right_button->setImage(right);

	text->setText(banner_text);
	center_text->setText(second_text);
	setState(init_state);
}

void BannOnOff2Labels::setCentralText(const QString &str)
{
	center_text->setText(str);
}

void BannOnOff2Labels::setCentralTextSecondaryColor(bool secondary)
{
	center_text->setProperty("SecondFgColor", secondary);
}

void BannOnOff2Labels::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
		break;
	case OFF:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "off")));
		break;
	}
}


Bann2CentralButtons::Bann2CentralButtons(QWidget *parent) : BannerNew(parent)
{
	center_left = new BtButton;
	center_right = new BtButton;

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	QGridLayout *l = new QGridLayout;
	l->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TOUCHX
	l->setSpacing(5);
#else
	l->setSpacing(0);
#endif
	l->setColumnStretch(0, 1);
	l->addWidget(center_left, 0, 1, Qt::AlignTop);
	l->addWidget(center_right, 0, 2, Qt::AlignTop);
	l->setColumnStretch(3, 1);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addLayout(l);
	layout->addWidget(text);
}

void Bann2CentralButtons::initBanner(const QString &left, const QString &right, const QString &banner_text)
{
	center_left->setImage(left);
	center_right->setImage(right);
	if (banner_text.isEmpty())
	{
		// just to avoid surprises...
		text->disconnect();
		text->deleteLater();
	}
	else
		text->setText(banner_text);
}


bann2But::bann2But(QWidget *parent) : banner(parent)
{
	banner_height = BAN2BUT_BUT_DIM;
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

