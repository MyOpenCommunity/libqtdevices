#include "page.h"
#include "main.h"

Page::Page(QWidget *parent) : QWidget(parent)
{
}

void Page::inizializza()
{
}

void Page::showPage()
{
	showFullScreen();
	setFixedSize(MAX_WIDTH, MAX_HEIGHT);
}
