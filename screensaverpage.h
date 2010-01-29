#ifndef SCREENSAVERPAGE_H
#define SCREENSAVERPAGE_H

#include "singlechoicepage.h"
#include "gridcontent.h" // GridContent

#include <QDir>

class BtButton;
class QLabel;

class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage();
	virtual void showPage();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);
};

class SlideshowImageContent;

/**
 * Select images to show during the slideshow.
 */
class SlideshowSelectionPage : public Page
{
Q_OBJECT
public:
	typedef SlideshowImageContent ContentType;

	SlideshowSelectionPage(const QString &start_path);

public slots:
	void browseUp();

private:
	void showFiles();
	void refreshContent();
	QDir current_dir;
	int level;
	QString checked_icon, unchecked_icon, photo_icon;

private slots:
	void enterDirectory(QString dir);
};


/**
 * Show currently selected images for slideshow with option to remove them. Also select slideshow preferences.
 */
class ImageRemovalPage : public Page
{
Q_OBJECT
public:
	typedef SlideshowImageContent ContentType;

	ImageRemovalPage();

private:
	void loadImages();
};


/**
 * Select slideshow settings: add images, remove all, delay between images.
 */
class SlideshowSettings : public QWidget
{
Q_OBJECT
public:
	SlideshowSettings();

signals:
	void clearAllImages();
	void addMoreImages();
};


/**
 * Display items in a grid with 2 columns.
 */
class SlideshowImageContent : public GridContent
{
Q_OBJECT
public:
	SlideshowImageContent(QWidget *parent=0);
	void addItem(QWidget *item);
	void clearContent();
	void showContent();

public slots:
	void nextItem();
	void prevItem();

protected:
	virtual void drawContent();

private:
	QList<QWidget *> items;
};


/**
 * Slideshow content item to display a directory (directory button, check button, label below).
 */
class SlideshowItemDir : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param path Returned when directoryToggled() is emitted.
	 */
	SlideshowItemDir(const QString &path, const QString &checked_icon, const QString &unchecked_icon, const QString &main_icon);

private:
	BtButton *dir_button, *check_button;
	QLabel *text;
	QString dir_path;

private slots:
	void checked(bool check);
	void dirButtonClicked();

signals:
	void browseDirectory(QString);
	void directoryToggled(bool, QString);
};


/**
 * Slideshow content item to display a thumbnail (thumbnail label, check button, label below)
 */
class SlideshowItemImage : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param filename Returned when fileToggled() is emitted.
	 * \param working_dir Used internally to load the image with path working_dir + filename
	 */
	SlideshowItemImage(const QString &filename, const QString &working_dir, const QString &checked_icon, const QString &unchecked_icon);

private:
	BtButton *check_button;
	QLabel *thumbnail, *text;
	QString file_name;

private slots:
	void checked(bool check);

signals:
	void fileToggled(bool, QString);
};

#endif // SCREENSAVERPAGE_H
