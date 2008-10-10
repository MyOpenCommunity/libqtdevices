#include "cleanscreen.h"
#include "main.h"

#include <qlabel.h>

#define WAIT_TIME 10

static const char *BG_CLEANSCREEN = IMG_PATH "dwnpage.png";


CleanScreen::CleanScreen(QWidget *parent) : QWidget(parent)
{
	connect(&timer, SIGNAL(timeout()), SIGNAL(Closed()));
	timer.setSingleShot(true);
	// TODO: sistemare con i metodi qt4!
	/* 
	QPixmap bg;
	if (bg.load(BG_CLEANSCREEN))
		setPaletteBackgroundPixmap(bg);
	*/
}

void CleanScreen::showEvent(QShowEvent *e)
{
	timer.start(WAIT_TIME * 1000);
}

void CleanScreen::mousePressEvent(QMouseEvent *e)
{
	timer.start(WAIT_TIME * 1000);
}

