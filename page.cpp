#include "page.h"
#include "main.h"
#include "openclient.h"

#include <assert.h>

// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;


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

