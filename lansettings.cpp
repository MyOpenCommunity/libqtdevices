#include "lansettings.h"
#include "bannondx.h"

LanSettings::LanSettings(QWidget *parent) :
	sottoMenu(parent)
{
	// create banners
	bannOnDx *b = new bannOnDx(this);
	b->SetTextU("Prova");
	b->SetIcons(ICON_INFO, 1);
	appendBanner(b);

	connect(this, SIGNAL(Closed()), SLOT(hide()));

	// create children
	// . riassunto
	// . impostazioni IP
}
