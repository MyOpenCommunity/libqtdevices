#include "iconwindow.h"
#include "skinmanager.h"

#include <QHBoxLayout>
#include <QLabel>

IconWindow::IconWindow(const QString &icon, const QString &style)
{
	if (!style.isNull())
		setStyleSheet(style);

	QHBoxLayout *l = new QHBoxLayout(this);

	QLabel *label = new QLabel;
	label->setPixmap(bt_global::skin->getImage(icon));

	l->addWidget(label, 0, Qt::AlignCenter);
}
