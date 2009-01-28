#include "page.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"

#include <QVBoxLayout>

#include <assert.h>

static const char *IMG_BACK = IMG_PATH "arrlf.png";


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;


Page::Page(QWidget *parent) : QWidget(parent)
{
	main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 5, 0, 0);
	main_layout->setSpacing(0);
	setLayout(main_layout);
}

void Page::inizializza()
{
}

void Page::showPage()
{
	showFullScreen();
	setFixedSize(MAX_WIDTH, MAX_HEIGHT);
}

void Page::sendFrame(QString frame)
{
	assert(client_comandi && "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void Page::sendInit(QString frame)
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

void Page::addBackButton()
{
	BtButton *back_btn = new BtButton;
	back_btn->setImage(IMG_BACK);
	connect(back_btn, SIGNAL(clicked()), this, SIGNAL(Closed()));
	main_layout->addStretch();
	main_layout->addWidget(back_btn, 0, Qt::AlignLeft);
	main_layout->addSpacing(10);
}
