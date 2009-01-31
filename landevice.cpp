#include "landevice.h"

#include <openmsg.h>

#include <QDebug>
#include <QStringList>

#include <assert.h>


LanDevice::LanDevice() : device(QString("13"), QString(""))
{
}

void LanDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = DIM_STATUS;
	sendFrame(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
}

void LanDevice::sendRequest(int what)
{
	sendInit(QString("*#%1**%2##").arg(who).arg(what));
}

void LanDevice::requestStatus()
{
	sendRequest(DIM_STATUS);
}

void LanDevice::requestIp()
{
	sendRequest(DIM_IP);
}

void LanDevice::requestNetmask()
{
	sendRequest(DIM_NETMASK);
}

void LanDevice::requestMacAddress()
{
	sendRequest(DIM_MACADDR);
}

void LanDevice::requestGateway()
{
	sendRequest(DIM_GATEWAY);
}

void LanDevice::requestDNS1()
{
	sendRequest(DIM_DNS1);
}

void LanDevice::requestDNS2()
{
	sendRequest(DIM_DNS2);
}

void LanDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who())
		return;

	int what = msg.what();
	int what_args = msg.whatArgCnt();

	QHash<int, QVariant> status_list;
	QVariant v;

	if (what == DIM_MACADDR || what == DIM_IP || what == DIM_NETMASK ||
		what == DIM_GATEWAY || what == DIM_DNS1 || what == DIM_DNS2 ||
		what == DIM_STATUS)
	{
		qDebug("LanDevice::frame_rx_handler -> frame read:%s", frame);
		switch (what)
		{
		case DIM_IP:
		{
			assert(what_args == 4); // IPv4 is composed by 4 parts
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i));
			qDebug() << "Indirizzo ip" << parts.join(".");
			v.setValue(parts.join("."));
			break;
		}
		case DIM_MACADDR:
		{
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i));
			qDebug() << "Mac address" << parts.join(".");
			v.setValue(parts.join("."));
			break;
		}
		case DIM_NETMASK:
		{
			assert(what_args == 4); // IPv4 netmask is composed by 4 parts
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i));
			qDebug() << "Netmask" << parts.join(".");
			v.setValue(parts.join("."));
			break;
		}
		}

		status_list[what] = v;
		emit status_changed(status_list);
	}
}
