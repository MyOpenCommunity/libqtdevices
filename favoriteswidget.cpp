#include "favoriteswidget.h"
#include "xml_functions.h"

#include <QVBoxLayout>
#include <QLabel>


FavoritesWidget::FavoritesWidget(const QDomNode &config_node)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	QLabel *title = new QLabel(getTextChild(config_node, "descr"));

	title->setAlignment(Qt::AlignHCenter);

	QWidget *p = new QWidget;

	// TODO add banners from configuration

	l->addWidget(title);
	l->addWidget(p, 1);
}

QSize FavoritesWidget::minimumSizeHint() const
{
	return QSize(237, 335);
}
