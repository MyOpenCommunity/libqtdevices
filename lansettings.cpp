#include "lansettings.h"
#include "bannondx.h"

LanSettings::LanSettings(QWidget *parent) :
	sottoMenu(parent)
{
	// create banners
	bannOnDx *b = new bannOnDx(this);
	b->setText("Prova");
	b->SetIcons(ICON_INFO, 1);
	appendBanner(b);


	// create children
	// . riassunto
	// . impostazioni IP
}
