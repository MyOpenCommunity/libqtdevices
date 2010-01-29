#ifndef SCREENSAVERPAGE_H
#define SCREENSAVERPAGE_H

#include "singlechoicepage.h"
#include "gridcontent.h" // GridContent

#include <QDir>
#include <QSet>

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
	void saveSlideshowToFile();
	/*
	 * Check if an item is selected.
	 * By design we don't query the interface but rely only on the QSets below (it may well be that the interface
	 * doesn't hold as many buttons as items in the directory).
	 * Compared to pixmap loading or scaling, this operation is rather cheap; however, use it sparingly since
	 * it involves 3 set lookups for each level of depth of the given path. For example the path:
	 * /media/disk/photos/img001.jpg
	 * will require 12 (3 * 4) lookups if the file and any directory below it are not selected.
	 */
	bool isItemSelected(QString abs_path);
	QDir current_dir;
	int level;
	QString checked_icon, unchecked_icon, photo_icon;
	// removed_images: will be removed from selected_images
	// inserted_images: will be added to selected_images
	// selected_images: contains full path names for files. Full path names for directories if all files and dir
	//                  below are selected.
	QSet<QString> selected_images, removed_images, inserted_images;

private slots:
	void enterDirectory(QString dir);
	void confirmSelection();
	void itemSelected(bool is_selected, QString relative_path);
	void clearCaches();
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
	void setChecked(bool check);

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
	void setChecked(bool check);

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
