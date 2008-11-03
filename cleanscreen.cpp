#include "cleanscreen.h"
#include "main.h"
#include "fontmanager.h"
#include "btlabel.h"

#include <assert.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qdatetime.h>

static const char *CLEANSCREEN_ICON = IMG_PATH "btnplusp.png";

CleanScreen::CleanScreen(QWidget *parent) : QWidget(parent)
{

	connect(&secs_timer, SIGNAL(timeout()), SLOT(update()));

	connect(&timer, SIGNAL(timeout()), SLOT(timerElapsed()));
	connect(&timer, SIGNAL(timeout()), &secs_timer, SLOT(stop()));
	connect(&timer, SIGNAL(timeout()), SIGNAL((Closed())));
	timer.setSingleShot(true);

	QFont aFont;
	FontManager::instance()->getFont(font_homepage_bottoni_label, aFont);

	time_label = new BtLabel(this);
	time_label->setFont(aFont);
	time_label->setAlignment(Qt::AlignHCenter);
	time_label->setGeometry(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_WIDTH, TIME_LABEL_HEIGHT);

	icon_label = new BtLabel(this);
	QPixmap *icon = icons_library.getIcon(CLEANSCREEN_ICON);
	icon_label->setPixmap(*icon);
	icon_label->setAlignment(Qt::AlignHCenter);
	icon_label->setGeometry(ICON_LABEL_X, ICON_LABEL_Y, ICON_LABEL_WIDTH, ICON_LABEL_HEIGHT);

	QDomNode page_root = getPageNode(IMPOSTAZIONI);
	assert(!page_root.isNull());
	QDomNode cleanscreen_root = getChildWithId(page_root, QRegExp("item\\d{1,2}"), CLEANSCREEN);
	assert(!cleanscreen_root.isNull());
	wait_time_sec = cleanscreen_root.namedItem("time").toElement().text().toInt();
}


void CleanScreen::resetTime()
{
	// update the widget every second
	secs_timer.start(1 * 1000);
	timer.start(wait_time_sec * 1000);
	end_time = time(0) + wait_time_sec;
}

void CleanScreen::showEvent(QShowEvent *e)
{
	resetTime();
}

void CleanScreen::mousePressEvent(QMouseEvent *e)
{
	resetTime();
}

// if we move when the touch panel is pressed
void CleanScreen::mouseMoveEvent(QMouseEvent *e)
{
	resetTime();
}

void CleanScreen::paintEvent(QPaintEvent *e)
{
	// we use QTime only to format the output
	QTime diff;
	diff = diff.addSecs(end_time - time(0));
	if (diff.minute())
		time_label->setText(diff.toString("mm:ss"));
	else
		time_label->setText(diff.toString("ss"));
}
