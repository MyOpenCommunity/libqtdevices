#ifndef FAVORITESWIDGET_H
#define FAVORITESWIDGET_H

#include <QWidget>

class QDomNode;


// favorites widges, shown on the right, contains a list of banners
class FavoritesWidget : public QWidget
{
Q_OBJECT
public:
	FavoritesWidget();

	void loadItems(const QDomNode &config_node);

protected:
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
};

#endif // FAVORITESWIDGET_H
