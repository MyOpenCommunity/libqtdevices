#include "page.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"

#include <QStackedWidget>
#include <QVBoxLayout>

#include <assert.h>

static const char *IMG_BACK = IMG_PATH "arrlf.png";


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
QStackedWidget *Page::main_window = 0;


Page::Page(QWidget *parent) : QWidget(parent)
{
	// pages with parent have a special meaning (for example, sound diffusion)
	// so they must not handled here
	if (main_window && !parent)
		main_window->addWidget(this);
}

void Page::inizializza()
{
}

void Page::setMainWindow(QStackedWidget *window)
{
	main_window = window;
}

void Page::showPage()
{
	main_window->setCurrentWidget(this);
}

void Page::sendFrame(QString frame) const
{
	assert(client_comandi && "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void Page::sendInit(QString frame) const
{
	assert(client_richieste && "Client richieste not set!");
	QByteArray buf = frame.toAscii();
	client_richieste->ApriInviaFrameChiudi(buf.constData());
}

void Page::setClients(Client *command, Client *request)
{
	client_comandi = command;
	client_richieste = request;
}


PageLayout::PageLayout(QWidget *parent) : Page(parent)
{
	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 0);
	main_layout->setSpacing(0);
}

void PageLayout::addBackButton()
{
	BtButton *back_btn = new BtButton;
	back_btn->setImage(IMG_BACK);
	connect(back_btn, SIGNAL(clicked()), this, SIGNAL(Closed()));
	main_layout->addStretch();
	main_layout->addWidget(back_btn, 0, Qt::AlignLeft);
	main_layout->addSpacing(10);
}

