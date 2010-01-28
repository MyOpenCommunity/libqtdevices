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
	QDir current_dir;
	int level;
	QString checked_icon, unchecked_icon, forward_icon;
};


class ImageRemovalPage : public Page
{
Q_OBJECT
public:
	typedef SlideshowImageContent ContentType;

	ImageRemovalPage();

private:
	void loadImages();
};


class SlideshowSettings : public QWidget
{
Q_OBJECT
public:
	SlideshowSettings();

signals:
	void clearAllImages();
	void addMoreImages();
};

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



class SlideshowItemDir : public QWidget
{
Q_OBJECT
public:
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

#endif // SCREENSAVERPAGE_H
