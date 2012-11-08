#ifndef SIGNALS_HANDLER_H
#define SIGNALS_HANDLER_H

#include <QObject>

class QSocketNotifier;
class SignalsHandler;


SignalsHandler *installSignalsHandler();


/*!
	\ingroup Core
	\brief Manages the unix signals used on the BTicino hardware.
*/
class SignalsHandler : public QObject
{
	Q_OBJECT
public:
	friend SignalsHandler *installSignalsHandler();
	~SignalsHandler();

	static void signalHandler(int signal_number);

signals:
	void signalReceived(int signal_number);

private slots:
	void handleSignal();
	void handleSignal2(int signal_number);

private:
	SignalsHandler();
	static long parent_pid;
	static int signalfd[2];
	QSocketNotifier *sn_signal;
};


#endif // SIGNALS_HANDLER_H
