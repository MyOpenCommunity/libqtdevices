#include "settings.h"

Settings::Settings(QWidget *parent, QDomNode config_node) : sottoMenu(parent)
{
	loadItems(config_node);
}

void Settings::loadItems(QDomNode config_node)
{
}