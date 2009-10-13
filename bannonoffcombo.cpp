#include "bannonoffcombo.h"

#include <QLabel>

#define BUTONOFFCOMBO_ICON_DIM_Y 60
#define BANONOFFCOMBO_BUT_DIM 60

#define BUTONOFFCOMBO_ICON_DIM_MAX_X 120
#define BUTONOFFCOMBO_ICON_DIM_MIN_X 80


BannOnOffCombo::BannOnOffCombo(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width-BANONOFFCOMBO_BUT_DIM, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(BUT2, 0, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(TEXT, 0, BANONOFFCOMBO_BUT_DIM, banner_width, banner_height-BANONOFFCOMBO_BUT_DIM);
	addItem(ICON, BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X, BUTONOFFCOMBO_ICON_DIM_Y);
	addItem(TEXT2, BANONOFFCOMBO_BUT_DIM, 0, banner_width - 2*BANONOFFCOMBO_BUT_DIM - BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
	nascondi(TEXT2);
}

void BannOnOffCombo::changeStatus(ComboStatus st)
{
	banner::SetIcons(2, status_icon[st]);

	switch (st)
	{
	case CENTER:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X,BUTONOFFCOMBO_ICON_DIM_Y);
		nascondi(TEXT2);
		break;
	case SX:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM+BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, banner_width-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	case DX:
		BannerIcon->setGeometry(banner_width-BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, banner_width-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	default:
		qWarning("STATUS UNKNOWN on BannOnOffCombo::changeStatus");
	}
}

void BannOnOffCombo::SetIcons(QString sxIcon, QString dxIcon, QString centerIcon, QString centerSxIcon, QString centerDxIcon)
{
	status_icon[CENTER] = centerIcon;
	status_icon[SX] = centerSxIcon;
	status_icon[DX] = centerDxIcon;
	banner::SetIcons(sxIcon, dxIcon, QString(), centerIcon);
}
