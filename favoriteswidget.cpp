#include "favoriteswidget.h"
#include "xml_functions.h"
#include "bannerfactory.h"
#include "banner.h"

#include <QVBoxLayout>
#include <QLabel>


FavoritesWidget::FavoritesWidget(const QDomNode &config_node)
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	loadItems(config_node);
}

void FavoritesWidget::loadItems(const QDomNode &config_node)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	QLabel *title = new QLabel(getTextChild(config_node, "descr"));

	title->setAlignment(Qt::AlignHCenter);

	l->addWidget(title);

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
			l->addWidget(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on favorites page!", id);
		}
	}

	l->addStretch(1);
}

QSize FavoritesWidget::minimumSizeHint() const
{
	return QSize(237, 335);
}
