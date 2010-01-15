#ifndef WEBCAM_LIST_H
#define WEBCAM_LIST_H

#include "page.h"
#include "itemlist.h"

class QDomNode;
class QHttp;
class QUrl;
class QString;
class ImageLabel;


/**
 * The page that display a webcam image, plus some controls to reload/change image
 */
class WebcamPage : public Page
{
Q_OBJECT
public:
	WebcamPage();
	void setImage(QUrl url, QString label);

signals:
	void reloadImage();
	void nextImage();
	void prevImage();

private slots:
	void downloadFinished(int id, bool error);

private:
	PageTitleWidget *page_title;
	ImageLabel *image;
	QHttp *http;
	QString description;
};


/**
 * This page shows the list of webcam, taking them from the configuration file.
 */
class WebcamListPage : public Page
{
Q_OBJECT
public:
	WebcamListPage(const QDomNode &config_node);

	typedef ItemList ContentType;

private slots:
	void itemSelected(int index);
	void reloadImage();
	void nextImage();
	void prevImage();

private:
	QList<ItemList::ItemInfo> webcam_list;
	int current_image; // the index of the current webcam image
	WebcamPage *webcam_page;

	void loadWebcams(const QDomNode &config_node);
};

#endif // WEBCAM_LIST_H
