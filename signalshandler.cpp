#include "signalshandler.h"

#include <QSocketNotifier>
#include <QDebug>

#include <signal.h>

#include <sys/types.h> // socketpair
#include <sys/socket.h> // socketpair

int SignalsHandler::signalfd[2];
long SignalsHandler::parent_pid = 0;

SignalsHandler *installSignalsHandler()
{
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_flags |= SA_RESTART;
	sa.sa_handler = SignalsHandler::signalHandler;
	if (sigaction(SIGUSR2, &sa, 0) != 0)
		qWarning() << "Error on installing the handler for the SIGUSR2 signal";
	if (sigaction(SIGTERM, &sa, 0) != 0)
		qWarning() << "Error on installing the handler for the SIGTERM signal";

	return new SignalsHandler;
}

SignalsHandler::SignalsHandler()
{
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalfd))
		qWarning() << "Cannot create SignalsHandler socketpair";
	parent_pid = getpid();

	sn_signal = new QSocketNotifier(signalfd[1], QSocketNotifier::Read, this);
	sn_signal->setEnabled(true);
	connect(sn_signal, SIGNAL(activated(int)), SLOT(handleSignal()));
	connect(this, SIGNAL(signalReceived(int)), SLOT(handleSignal2(int)));
}

SignalsHandler::~SignalsHandler()
{
	delete sn_signal;
}

#include <iostream>
void SignalsHandler::handleSignal2(int signal_number)
{
	if (signal_number == SIGUSR2)
	{
		qDebug("Received signal SIGUSR2");
		std::cerr << "Received signal SIGUSR2";
//		emit systemTimeChanged();
	}
	else if (signal_number == SIGTERM)
	{
		qDebug("Terminating on SIGTERM");
		std::cerr << "Received signal SIGTERM";
		abort();
	}
}

void SignalsHandler::handleSignal()
{
	sn_signal->setEnabled(false);
	char signal_number;
	::read(signalfd[1], &signal_number, sizeof(signal_number));

	qDebug() << "Handling signal" << int(signal_number);
	emit signalReceived(int(signal_number));

	sn_signal->setEnabled(true);
}

void SignalsHandler::signalHandler(int signal_number)
{
	// ignore signals sent to childs; this handles the
	// time interval between fork() and exec() when using QProcess
	if (getpid() != parent_pid)
		return;

	char tmp = signal_number;
	::write(signalfd[0], &tmp, sizeof(tmp)); // write something, in order to "activate" the notifier
}

