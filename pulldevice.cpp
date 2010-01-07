#include "pulldevice.h"

#include <openmsg.h>

#include <QPair>
#include <QDebug>
#include <QString>

/*
 * Split a where into a+pf part and address extension.
 */
QPair<QString, QString> splitWhere(const QString &w)
{
	int idx = w.indexOf("#");
	if (idx < 0)
		return qMakePair(w.left(idx), QString());
	else
		return qMakePair(w.left(idx), w.mid(idx));
}

/*
 * Return the environment part from a+pf string
 */
QString getEnvironment(const QString &w)
{
	if (w.length() == 2)
		return w.mid(0, 1);
	else if (w.length() == 4)
		return w.mid(0, 2);
	else if (w.length() == 3)
	{
		Q_ASSERT_X(w == "100", "getEnvironment", "Environment with legth 3 must be 100 only");
		return "10";
	}
	else if (w.length() == 1)
		return w;

	return QString();
}


AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where)
{
	if (msg_where == dev_where)
		return P2P;

	// frame where (input)
	QPair<QString, QString> in = splitWhere(msg_where);
	// device where (our)
	QPair<QString, QString> our = splitWhere(dev_where);

	if (!(in.second == "#3" && our.second.isEmpty()))
		if (!(in.second.isEmpty()) && (in.second != our.second))
			return NOT_MINE;

	// here we don't need to care about extension anymore
	// general address
	if (in.first == "0")
		return GLOBAL;

	// environment address. The first part must be "00", "100" or numbers 1 to 9
	// use toInt() to remove differences between "00" "0" and so on.
	if (in.first == "00" || in.first == "100" || in.first.length() == 1)
		if (getEnvironment(our.first).toInt() == getEnvironment(in.first).toInt())
			return ENVIRONMENT;

	return NOT_MINE;
}

enum
{
	INVALID_STATE = -1,
	VARIABLE_TIMING_STATE = 1234,   // something different from all other 'what' of automation system (ie 0 to 18
	                                // for normal frames and 100 to 200 for dimmer100 level frames).
};

PullStateManager::PullStateManager(PullMode m)
{
	mode = m;
	status = INVALID_STATE;
	status_requested = false;
}

PullMode PullStateManager::getPullMode()
{
	return mode;
}

bool PullStateManager::moreFrameNeeded(OpenMsg &msg, bool is_environment)
{
	// PullStateManager will be used for automation and lighting only.
	// I'll handle all 'what' combinations here, split to a different function or class when needed
	// We need to look for write environment commands
	int new_state = msg.what();
	bool measure_frame = (is_environment && msg.IsWriteFrame()) || (!is_environment && msg.IsMeasureFrame());
	if (measure_frame)
	{
		// dimmer 100 status
		if (msg.what() == 1)
			new_state = msg.whatArgN(0) - 100;
		// variable temporization
		// this is the trickiest one. Remember that devices don't send status updates if they are light on
		// by a general var timing frame. We have two cases:
		// - if the frame is environment and we are ON, we can't decide anything; we need to skip this frame
		// - if the frame is PP, we can use it to test device status
		else
		{
			// use a dirty trick/ugly hack/beard trick
			// avoid requesting status if we are 'on' by making what == status
			if (status > 0)
				new_state = status;
			else
				new_state = VARIABLE_TIMING_STATE;
		}
	}

	if (is_environment)
	{
		if (status == INVALID_STATE || status != new_state)
		{
			status_requested = true;
			return true;
		}
	}
	else
	{
		// We can decide the mode only if we have seen an environment frame previously.
		// If we just get PP frames, we can't decide the mode!
		if (status_requested && status != INVALID_STATE)
		{
			if (status == new_state)
				mode = PULL;
			else
				mode = NOT_PULL;
		}
		else
		{
			status = new_state;
			status_requested = false;
		}
	}

	return false;
}

PullDevice::PullDevice(QString who, QString where, PullMode m) :
	device(who, where),
	state(m)
{
}

void PullDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() == msg.who())
		manageFrame(msg);
}

void PullDevice::manageFrame(OpenMsg &msg)
{
	switch (checkAddressIsForMe(QString::fromStdString(msg.whereFull()), where))
	{
	case NOT_MINE:
		return;
	case GLOBAL:
	case ENVIRONMENT:
		if (state.getPullMode() == PULL_UNKNOWN)
		{
			if (state.moreFrameNeeded(msg, true))
				requestPullStatus();
			return;
		}
		else if (state.getPullMode() == PULL)
			return;
		// when NOT_PULL we must parse the frame and emit status_changed() signal
		break;
	default:
		if (state.getPullMode() == PULL_UNKNOWN)
			state.moreFrameNeeded(msg, false);
		break;
	}

	StatusList sl;
	parseFrame(msg, &sl);
	emit status_changed(sl);
}

