#include "bannonoffcombo.h"
#include "main.h"

#include <QLabel>

#define BUTONOFFCOMBO_ICON_DIM_Y 60
#define BANONOFFCOMBO_BUT_DIM 60

#define BUTONOFFCOMBO_ICON_DIM_MAX_X 120
#define BUTONOFFCOMBO_ICON_DIM_MIN_X 80


BannOnOffCombo::BannOnOffCombo(QWidget *parent, const char *name) : banner(parent, name)
{
	addItem(BUT1, MAX_WIDTH-BANONOFFCOMBO_BUT_DIM, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(BUT2, 0, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(TEXT, 0, BANONOFFCOMBO_BUT_DIM, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BANONOFFCOMBO_BUT_DIM);
	addItem(ICON, BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X, BUTONOFFCOMBO_ICON_DIM_Y);
	addItem(TEXT2, BANONOFFCOMBO_BUT_DIM, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
	nascondi(TEXT2);
}

void BannOnOffCombo::changeStatus(ComboStatus st)
{
	QByteArray buf = status_icon[st].toAscii();
	banner::SetIcons(2, buf.constData());

	switch (st)
	{
	case CENTER:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X,BUTONOFFCOMBO_ICON_DIM_Y);
		nascondi(TEXT2);
		break;
	case SX:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM+BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	case DX:
		BannerIcon->setGeometry(MAX_WIDTH-BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	default:
		qWarning("STATUS UNKNOWN on BannOnOffCombo::changeStatus");
	}
}

void BannOnOffCombo::SetIcons(const char *sxIcon , const char *dxIcon, const char *centerIcon, const char *centerSxIcon, const char *centerDxIcon)
{
	status_icon[CENTER] = centerIcon;
	status_icon[SX] = centerSxIcon;
	status_icon[DX] = centerDxIcon;
	banner::SetIcons(sxIcon, dxIcon, NULL, centerIcon);
}
