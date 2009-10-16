#ifndef CONTENT_WIDGET_H
#define CONTENT_WIDGET_H

#include <QWidget>
#include <QList>

class banner;


/**
 * The ContentWidget class manage a list of banner usually put inside a Page.
 */
class ContentWidget : public QWidget
{
Q_OBJECT
public:
	ContentWidget(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);

public slots:
	virtual void upClick();
	virtual void downClick();
	virtual void forwardClick() {} // Do nothing by default

protected:
	virtual void paintEvent(QPaintEvent *e);

private:
	// The index of the current banner
	int current_index;
	// The maximum number of banners per page
	int max_banner;
	QList<banner*> banner_list;
};

#endif // CONTENT_WIDGET_H
