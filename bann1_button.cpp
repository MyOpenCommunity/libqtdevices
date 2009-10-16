#include "bann1_button.h"
#include "sottomenu.h"
#include "titlelabel.h" // TextOnImageLabel
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "icondispatcher.h" //icons_cache
#include "fontmanager.h" //FontManager

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
	banner(parent)
{
	right_button = new BtButton(this);
	right_button->setGeometry(banner_width - BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);

	text = new QLabel(this);
	text->setGeometry(0, BANPULS_BUT_DIM, banner_width, banner_height - BANPULS_BUT_DIM);
	text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	text->setFont(bt_global::font->get(FontManager::TEXT));

	center_icon = new QLabel(this);
	center_icon->setGeometry(BANPULS_BUT_DIM, 0, BANPULS_ICON_DIM_X, BANPULS_ICON_DIM_Y);
}

void BannSinglePuls::initBanner(const QString &right, const QString &center, const QString &banner_text)
{
	loadIcons(right, center);
	text->setText(banner_text);
}

void BannSinglePuls::loadIcons(const QString &right, const QString &center)
{
	right_button->setImage(right);
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
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
	// This banner uses a dx button but shows it in central position.
	addItem(BUT1, (banner_width - BANPULS_BUT_DIM) / 2, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnDx::bannOnDx(QWidget *parent, QString icon, Page *page) : banner(parent)
{
	addItem(BUT1, banner_width - BUT_DIM, (banner_height-BUT_DIM)/2, BUT_DIM ,BUT_DIM);
	addItem(TEXT, 0, 0 ,banner_width-BUT_DIM, banner_height);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnSx::bannOnSx(QWidget *parent, QString icon) : banner(parent)
{
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


bannTextOnImage::bannTextOnImage(QWidget *parent, const QString &text) : banner(parent)
{
	label = new TextOnImageLabel(this, text);
	label->setBackgroundImage(bt_global::skin->getImage("empty_icon"));
	label->setGeometry(BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(BUT1, banner_width - BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height - BUT_DIM);
	SetIcons(bt_global::skin->getImage("forward"), 1);
}

void bannTextOnImage::setInternalText(const QString &text)
{
	label->setInternalText(text);
}
