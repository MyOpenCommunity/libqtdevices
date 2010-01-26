#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display
#include "hardware_functions.h"
#include "singlechoicecontent.h"
#include "navigation_bar.h" // NavigationBar
#include "skinmanager.h" // bt_global::skin

#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>

ScreenSaverPage::ScreenSaverPage()
{
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
	addBanner(tr("Text"), ScreenSaver::TEXT);
	//addBanner(tr("Deform"), ScreenSaver::DEFORM); // the deform is for now unavailable!

	// TODO maybe we want an OK button for touch 10 as well
	if (hardwareType() == TOUCH_X)
	{
		addBanner(tr("Slideshow"), ScreenSaver::SLIDESHOW);
		connect(page_content, SIGNAL(bannerSelected(int)),
			SLOT(confirmSelection()));
	}
}

void ScreenSaverPage::showPage()
{
	setCheckedId(getCurrentId());
	SingleChoicePage::showPage();
}

int ScreenSaverPage::getCurrentId()
{
	return bt_global::display.currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	bt_global::display.setScreenSaver(static_cast<ScreenSaver::Type>(id));
	if (id == ScreenSaver::NONE)
		bt_global::display.setBrightness(BRIGHTNESS_OFF);
}



SlideshowImageContent::SlideshowImageContent(QWidget *parent) :
	GridContent(parent)
{

}

void SlideshowImageContent::addItem(QWidget *item)
{
	items.append(item);
}

void SlideshowImageContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		// compute the page list
		prepareLayout(items, 2);

		// add icons to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(items.at(base + j), j / 2, j % 2);
		}

		l->setRowStretch(l->rowCount(), 1);
	}

	updateLayout(items);
}

void SlideshowImageContent::nextItem()
{
	current_page += 1;
	// wrap around to the first page
	if (current_page >= pageCount())
		current_page = 0;
	drawContent();
}

void SlideshowImageContent::prevItem()
{
	current_page -= 1;
	// wrap around to the last page
	if (current_page < 0)
		current_page = pageCount() - 1;
	drawContent();
}



SlideshowSelectionPage::SlideshowSelectionPage(const QString &start_path) :
	Page(0),
	current_dir(start_path),
	level(0)
{
	NavigationBar *nav_bar = new NavigationBar;
	buildPage(new SlideshowImageContent, nav_bar, "Select photos");
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(nav_bar, SIGNAL(upClick()), page_content, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), page_content, SLOT(nextItem()));
	connect(page_content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	checked_icon = bt_global::skin->getImage("checked");
	unchecked_icon = bt_global::skin->getImage("unchecked");
	forward_icon = bt_global::skin->getImage("forward");

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
	showFiles();
}

void SlideshowSelectionPage::showFiles()
{
	QStringList filters;
	filters << "*.jpg" << "*.png";

	QFileInfoList list = current_dir.entryInfoList(filters);
	foreach (const QFileInfo &fi, list)
	{
		QWidget *w = 0;
		if (fi.isDir())
			w = new QLabel(fi.fileName());

		if (fi.isFile())
		{
			QPixmap tmp(fi.filePath());
			tmp = tmp.scaled(QSize(100, 100), Qt::KeepAspectRatio);
			QLabel *l = new QLabel;
			l->setPixmap(tmp);
			w = l;
		}
		Q_ASSERT_X(w!=0, "SlideshowImageSelection::browseFiles", "w is 0");
		page_content->addItem(w);
	}
}

void SlideshowSelectionPage::browseUp()
{
	if (level > 0)
	{
		--level;
		current_dir.cdUp();
		showFiles();
	}
	else
		emit Closed();
}

