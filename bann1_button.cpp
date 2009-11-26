#include "bann1_button.h"
#include "titlelabel.h" // TextOnImageLabel
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "fontmanager.h" //FontManager
#include "generic_functions.h" // getBostikName

#include <QHBoxLayout>
#include <QDebug>

#define BUT_DIM 60
#define BUTONDX_H_SCRITTA 20
#define BANNONICONS_ICON_DIM 60
#define BANON2SCR_TEXT1_DIM_X 100
#define BANON2SCR_TEXT1_DIM_Y 60
#define BUTON2SCR_ICON_DIM_X 40
#define BUTON2SCR_ICON_DIM_Y 60
#define BANNBUT2ICON_TEXT_DIM_X 240
#define BANNBUT2ICON_TEXT_DIM_Y 20
#define BANNBUT2ICON_ICON_DIM_X 60
#define BANNBUT2ICON_ICON_DIM_Y 60

BannSinglePuls::BannSinglePuls(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	connect(right_button, SIGNAL(clicked()), SIGNAL(rightClick()));
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(center_icon, 1, Qt::AlignRight);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannSinglePuls::initBanner(const QString &right, const QString &center, const QString &banner_text)
{
	loadIcons(right, center);
	text->setText(banner_text);
	// always set a text on the label, otherwise the sizeHint() height changes
	if (banner_text.isEmpty())
		text->setText(" ");
}

void BannSinglePuls::loadIcons(const QString &right, const QString &center)
{
	right_button->setImage(right);
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
}

void BannSinglePuls::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}



BannCenteredButton::BannCenteredButton(QWidget *parent) :
	BannerNew(parent)
{
	center_button = new BtButton;
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(center_button, 0, Qt::AlignTop);
}

void BannCenteredButton::initBanner(const QString &center)
{
	center_button->setImage(center);
}


BannOn2Labels::BannOn2Labels(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::TEXT));
	left_icon = new QLabel;
	right_icon = new QLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(center_text, 1, Qt::AlignCenter);
	hbox->addWidget(left_icon);
	hbox->addWidget(right_icon);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannOn2Labels::initBanner(const QString &right, const QString &_right_icon, const QString &_left_icon,
		const QString &banner_text, const QString &_center_text)
{
	right_button->setImage(right);
	center_right = _right_icon;
	center_left = _left_icon;
	text->setText(banner_text);
	center_text->setText(_center_text);
	setState(OFF);
	setElapsedTime(0);
}

void BannOn2Labels::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		right_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_right, "on")));
		break;
	case OFF:
		left_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_left, "0")));
		right_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_right, "off")));
		break;
	}
}

void BannOn2Labels::setElapsedTime(int time)
{
	if (time >= 1 && time <= 8)
		left_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center_left,
			QString::number(time))));
	// TODO: should we also set the light on??
}


BannLeft::BannLeft(QWidget *parent) :
	BannerNew(parent)
{
	// Bannleft does not have the description label
	banner_height = BUT_DIM;

	left_button = new BtButton;
	text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, Qt::AlignLeft);
	l->addWidget(text, 1, Qt::AlignHCenter);
}

void BannLeft::initBanner(const QString &left, const QString &center)
{
	left_button->setImage(left);
	text->setText(center);
}


bannPuls::bannPuls(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width - BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	addItem(ICON,BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(TEXT, 0, BANPULS_BUT_DIM, banner_width, banner_height - BANPULS_BUT_DIM);

	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
	connect(this,SIGNAL(sxPressed()),this,SIGNAL(pressed()));
	connect(this,SIGNAL(sxReleased()),this,SIGNAL(released()));
}


bannSimple::bannSimple(QWidget *parent, QString icon, Page *page) : banner(parent)
{
	banner_height = BUT_DIM;
	// This banner uses a dx button but shows it in central position.
	addItem(BUT1, (banner_width - BANPULS_BUT_DIM) / 2, 0,  BANPULS_BUT_DIM, BANPULS_BUT_DIM);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnDx::bannOnDx(QWidget *parent, QString icon, Page *page) : banner(parent)
{
	banner_height = BUT_DIM;
	addItem(BUT1, banner_width - BUT_DIM, (banner_height - BUT_DIM)/2, BUT_DIM, BUT_DIM);
	addItem(TEXT, 0, 0, banner_width-BUT_DIM, banner_height);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnSx::bannOnSx(QWidget *parent, QString icon) : banner(parent)
{
	banner_height = BUT_DIM;
	addItem(BUT1, 0, (banner_height - BUT_DIM)/2, BUT_DIM, BUT_DIM);
	addItem(TEXT, BUT_DIM, 0, banner_width - BUT_DIM, banner_height);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);
	Draw();
}

BtButton *bannOnSx::getButton()
{
	return sxButton;
}


bannOnIcons::bannOnIcons(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BUT_DIM, BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height-BUT_DIM);
	addItem(ICON, BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(ICON2, 2*BUT_DIM, 0, BUT_DIM, BUT_DIM);
}


bannOn2scr::bannOn2scr(QWidget *parent) : banner(parent)
{
	int x = 0, y = 0;
	addItem(TEXT2, x, y, BANON2SCR_TEXT1_DIM_X, BANON2SCR_TEXT1_DIM_Y);
	x = BANON2SCR_TEXT1_DIM_X;
	addItem(ICON, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(ICON2, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(BUT2, x, y, BUT_DIM, BUT_DIM);
	y = BUT_DIM;
	x = 0;
	addItem(TEXT, x, y, banner_width, banner_height - BUT_DIM);
}


bannBut2Icon::bannBut2Icon(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width - BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(ICON, BUT_DIM, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(ICON2, BUT_DIM + BANNBUT2ICON_ICON_DIM_X, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height - BUT_DIM);
}


bannTextOnImage::bannTextOnImage(QWidget *parent, const QString &text, QString bg_image, QString fwd_image) : banner(parent)
{
	label = new TextOnImageLabel(this, text);
	QString img = bt_global::skin->getImage(bg_image);
	QPixmap *p = bt_global::icons_cache.getIcon(img);
	label->setBackgroundImage(img);
	int left = banner_width - BUT_DIM - p->width();
	label->setGeometry(left, 0,  p->width() ,BANPULS_ICON_DIM_Y);
	addItem(BUT1, banner_width-BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(TEXT, left, BUT_DIM, p->width() , banner_height - BUT_DIM);
	SetIcons(bt_global::skin->getImage(fwd_image), 1);
}

void bannTextOnImage::setInternalText(const QString &text)
{
	label->setInternalText(text);
}
