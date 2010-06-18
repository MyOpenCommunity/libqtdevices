/*
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "generic_functions.h"
#include "bttime.h"

#include <openmsg.h>

#include <QtTest/QtTest>

void TestLightingDevice::initLightingDevice(LightingDevice *d)
{
	if (d)
	{
		dev = d;
		cleanup_required = false;
	}
	else
		dev = new LightingDevice(LIGHT_DEVICE_WHERE, PULL);
}

void TestLightingDevice::cleanupLightingDevice()
{
	if (cleanup_required)
		delete dev;
}

void TestLightingDevice::cleanup()
{
	cleanupLightingDevice();
}

void TestLightingDevice::init()
{
	initLightingDevice();
	cleanBuffers();
}

void TestLightingDevice::sendTurnOn()
{
	dev->turnOn();
	client_command->flush();
	QString cmd = QString("*1*1*") + dev->where + "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req = QString("*#1*") + dev->where + "##";
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::sendFixedTiming()
{
	dev->fixedTiming(7);
	client_command->flush();
	QString cmd = QString("*1*%1*%2##").arg(18).arg(dev->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendVariableTiming()
{
	dev->variableTiming(100, 22, 50);
	client_command->flush();
	QString cmd = QString("*#1*%1*#2*%2*%3*%4##").arg(dev->where).arg(100).arg(22).arg(50);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendRequestVariableTiming()
{
	dev->requestVariableTiming();
	client_request->flush();
	QString req = QString("*#1*%1*2##").arg(dev->where);
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::receiveLightOnOff()
{
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	t.check(QString("*1*1*%1##").arg(dev->where), true);
	t.check(QString("*1*0*%1##").arg(dev->where), false);
}

void TestLightingDevice::setParams(QString w, PullMode m)
{
	dev->where = w;
	dev->state.mode = m;
	dev->state.advanced = m == PULL_UNKNOWN ? PULL_ADVANCED_UNKNOWN : PULL_NOT_ADVANCED;
}

void TestLightingDevice::setParams(QString w, PullMode m, AdvancedMode a)
{
	dev->where = w;
	dev->state.mode = m;
	dev->state.advanced = a;
}

QString TestLightingDevice::getRequestStatusFrame()
{
	return QString("*#1*%1##").arg(dev->where);
}

// test device mode
void TestLightingDevice::checkPullUnknown()
{
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.checkSignals(global_on, 0);
	t.checkSignals(env_off, 0);
}

void TestLightingDevice::sendPullRequestIfNeeded()
{
	if (dev->delayed_request.isActive())
		dev->delayedStatusRequest();
}

void TestLightingDevice::receiveLightOnOffPull()
{
	setParams(LIGHT_DEVICE_WHERE, PULL);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknown()
{
	setParams(LIGHT_DEVICE_WHERE, PULL_UNKNOWN);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknown2()
{
	setParams(LIGHT_DEVICE_WHERE, PULL_UNKNOWN);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	t.checkSignals(global_on, 0);
	sendPullRequestIfNeeded();
	client_request->flush();
	QCOMPARE(server->frameRequest(), getRequestStatusFrame());
}


void TestLightingDevice::receiveLightOnOffPullExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, PULL);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknownExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, PULL_UNKNOWN);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknownExt2()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, PULL_UNKNOWN);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);
	t.checkSignals(env_off, 0);
	sendPullRequestIfNeeded();
	client_request->flush();
	QCOMPARE(server->frameRequest(), getRequestStatusFrame());
}

void TestLightingDevice::receiveLightOnOffNotPull()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.check(global_on, true);
	t.checkSignals(env_off, 0);
}

void TestLightingDevice::receiveLightOnOffNotPullExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, NOT_PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.check(global_on, true);
	t.check(env_off, false);
}

void TestLightingDevice::receiveLightOnOffNotPullAdvUnknown()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED_UNKNOWN);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";

	t.check(global_on, true);

	QCOMPARE(dev->delayed_request.isActive(), false);
}

void TestLightingDevice::receiveFixedTiming()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString frame = "*1*11*0##";

	t.check(frame, true);
}

void TestLightingDevice::receiveGlobalDimmer100OnOffNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_NOT_ADVANCED);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1#1*0##";
	QString global_off = "*1*0#1*0##";

	t.checkSignals(global_on, 0);
	t.checkSignals(global_off, 0);
}

void TestLightingDevice::receiveGlobalDimmer100OnOffNonPullAdvanced()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1#1*0##";
	QString global_off = "*1*0#1*0##";

	t.check(global_on, true);
	t.check(global_off, false);
}

void TestLightingDevice::receiveGlobalDimmer100OnOffPull()
{
	setParams(LIGHT_DEVICE_WHERE, PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1#1*0##";
	QString global_off = "*1*0#1*0##";

	t.checkSignals(global_on, 0);
	t.checkSignals(global_off, 0);
}

void TestLightingDevice::receiveVariableTiming()
{
	DeviceTester t(dev, LightingDevice::DIM_VARIABLE_TIMING);
	BtTime tm(1, 3, 55);
	QString timing = QString("*#1*%1*2*%2*%3*%4##").arg(dev->where)
		.arg(1).arg(3).arg(55);

	t.check(timing, tm);
}

void TestLightingDevice::receiveInvalidVariableTiming()
{
	DeviceTester t(dev, LightingDevice::DIM_VARIABLE_TIMING);
	QString timing = QString("*#1*%1*2*%2*%3*%4##").arg(dev->where)
		.arg(255).arg(255).arg(255);

	t.checkSignals(timing, 0);
}


void TestDimmerDevice::init()
{
	initDimmer();
	cleanBuffers();
}

void TestDimmerDevice::cleanup()
{
	cleanupDimmer();
}

void TestDimmerDevice::initDimmer(DimmerDevice *d)
{
	if (d)
	{
		dimmer = d;
		cleanup_required = false;
	}
	else
		dimmer = new DimmerDevice(LIGHT_DEVICE_WHERE, PULL);

	initLightingDevice(dimmer);
}

void TestDimmerDevice::cleanupDimmer()
{
	cleanupLightingDevice();
	if (cleanup_required)
		delete dimmer;
}

void TestDimmerDevice::sendDimmerDecreaseLevel()
{
	dimmer->decreaseLevel();
	client_command->flush();
	QString cmd = QString("*1*31*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmerDevice::sendDimmerIncreaseLevel()
{
	dimmer->increaseLevel();
	client_command->flush();
	QString cmd = QString("*1*30*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmerDevice::receiveLightOnRequestLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DEVICE_ON);
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	QString global_on = QString("*1*1*0##");
	QString global_off = QString("*1*0*0##");
	QString light_on = QString("*1*1*%1##").arg(dimmer->where);
	QString light_off = QString("*1*0*%1##").arg(dimmer->where);

	t.check(global_on, true);
	QCOMPARE(dimmer->delayed_request.isActive(), true);

	t.check(global_off, false);
	QCOMPARE(dimmer->delayed_request.isActive(), false);

	t.check(light_on, true);
	QCOMPARE(dimmer->delayed_request.isActive(), false);

	t.check(light_off, false);
	QCOMPARE(dimmer->delayed_request.isActive(), false);
}

void TestDimmerDevice::receiveDimmerLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*1*%1*%2##").arg(9).arg(dimmer->where);
	t.check(frame, 9);
}

void TestDimmerDevice::receiveDimmerLevel2()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	DeviceTester on(dimmer, LightingDevice::DIM_DEVICE_ON, DeviceTester::MULTIPLE_VALUES);

	QString frame = QString("*1*%1*%2##").arg(1).arg(dimmer->where);
	t.checkSignals(frame, 1);
	on.check(frame, true);
}

void TestDimmerDevice::receiveDimmerProblem()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_PROBLEM);
	QString frame = QString("*1*%1*%2##").arg(19).arg(dimmer->where);
	t.check(frame, true);
}

void TestDimmerDevice::receiveGlobalIncrementLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*1*30*0##");

	// pull unknown
	t.checkSignals(frame, 0);

	// not pull
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	dimmer->status = true;
	dimmer->level = 50;
	t.check(frame, 8);

	// not pull, off
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	dimmer->status = false;
	dimmer->level = 50;
	t.check(frame, 7);
	QCOMPARE(dimmer->status, true);
}

void TestDimmerDevice::receiveGlobalDecrementLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*1*31*0##");

	// pull unknown
	t.checkSignals(frame, 0);

	// not pull
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL);
	dimmer->status = true;
	dimmer->level = 50;
	t.check(frame, 6);
}

void TestDimmerDevice::receiveGlobalDimmer100SetlevelNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_NOT_ADVANCED);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString set_level = "*#1*0*1*134*50##";

	t.checkSignals(set_level, 0);
}

void TestDimmerDevice::receiveGlobalDimmer100SetlevelNonPullAdvanced()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString set_level = "*#1*0*1*134*50##";

	t.check(set_level, 5);
}

void TestDimmerDevice::receiveGlobalDimmer100SetlevelPull()
{
	setParams(LIGHT_DEVICE_WHERE, PULL);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString set_level = "*#1*0*1*134*50##";

	t.checkSignals(set_level, 0);
}

void TestDimmerDevice::receiveGlobalDimmer100IncDecNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_NOT_ADVANCED);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString inc_level = "*1*30#20#1*0##";
	QString dec_level = "*1*31#30#1*0##";

	t.checkSignals(inc_level, 0);
	t.checkSignals(dec_level, 0);
}

void TestDimmerDevice::receiveGlobalDimmer100IncDecNonPullAdvanced()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString inc_3 = "*1*30#3#1*0##";
	QString inc_level = "*1*30#20#1*0##";
	QString dec_level = "*1*31#30#1*0##";

	dimmer->status = true;
	dimmer->level = 31;

	t.check(inc_3, 5);
	t.check(inc_3, 6);
	t.check(inc_level, 8);
	t.check(dec_level, 5);
}

void TestDimmerDevice::receiveGlobalDimmer100IncDecPull()
{
	setParams(LIGHT_DEVICE_WHERE, PULL);
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString inc_level = "*1*30#20#1*0##";
	QString dec_level = "*1*31#30#1*0##";

	t.checkSignals(inc_level, 0);
	t.checkSignals(dec_level, 0);
}



void TestDimmer100Device::init()
{
	dimmer100 = new Dimmer100Device(LIGHT_DEVICE_WHERE, PULL);
	initDimmer(dimmer100);
	cleanBuffers();
}

void TestDimmer100Device::cleanup()
{
	cleanupDimmer();
	delete dimmer100;
}

void TestDimmer100Device::sendDimmer100DecreaseLevel()
{
	dimmer100->decreaseLevel100(50, 10);
	client_command->flush();
	QString cmd = QString("*1*31#%1#%2*%3##").arg(50).arg(10).arg(dimmer100->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer100Device::sendDimmer100IncreaseLevel()
{
	dimmer100->increaseLevel100(10, 150);
	client_command->flush();
	QString cmd = QString("*1*30#%1#%2*%3##").arg(10).arg(150).arg(dimmer100->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer100Device::sendRequestDimmer100Status()
{
	dimmer100->requestDimmer100Status();
	client_request->flush();
	QString req = QString("*#1*%1*1##").arg(dimmer100->where);
	QCOMPARE(server->frameRequest(), req);
}

void TestDimmer100Device::receiveDimmer100Status()
{
	DeviceTester tl(dimmer100, LightingDevice::DIM_DIMMER100_LEVEL, DeviceTester::MULTIPLE_VALUES);
	DeviceTester ts(dimmer100, LightingDevice::DIM_DIMMER100_SPEED, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#1*%1*1*%2*%3##").arg(dimmer100->where).arg(134).arg(50);

	tl.check(frame, 34);
	ts.check(frame, 50);
}

void TestDimmer100Device::receiveDimmer100StatusLevel0()
{
	DeviceTester t(dimmer100, LightingDevice::DIM_DEVICE_ON);

	QString frame = QString("*#1*%1*1*%2*%3##").arg(dimmer100->where).arg(100).arg(50);
	t.check(frame, false);
}

QString TestDimmer100Device::getRequestStatusFrame()
{
	return QString("*#1*%1*1##").arg(dimmer100->where);
}

void TestDimmer100Device::receiveGlobalIncrementLevel100()
{
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER100_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*1*30#20#255*0##");

	// pull unknown
	t.checkSignals(frame, 0);

	// not pull
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	dimmer100->status = true;
	dimmer100->level = 50;
	t.check(frame, 70);

	// not pull, off
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	dimmer100->status = false;
	dimmer100->level = 50;
	t.check(frame, 50);
	QCOMPARE(dimmer100->status, true);
}

void TestDimmer100Device::receiveGlobalDecrementLevel100()
{
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER100_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*1*31#20#255*0##");

	// pull unknown
	t.checkSignals(frame, 0);

	// not pull
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	dimmer100->status = true;
	dimmer100->level = 50;
	t.check(frame, 30);
}

void TestDimmer100Device::receiveGlobalDimmer100OnOffNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dimmer100, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1#1*0##";
	QString global_off = "*1*0#1*0##";

	t.check(global_on, true);
	t.check(global_off, false);
}

void TestDimmer100Device::receiveGlobalDimmer100SetlevelNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER100_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString set_level = "*#1*0*1*134*50##";

	t.check(set_level, 34);
}

void TestDimmer100Device::receiveGlobalDimmer100IncDecNonPullBase()
{
	setParams(LIGHT_DEVICE_WHERE, NOT_PULL, PULL_ADVANCED);
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER100_LEVEL, DeviceTester::MULTIPLE_VALUES);
	QString inc_level = "*1*30#20#1*0##";
	QString dec_level = "*1*31#30#1*0##";

	dimmer100->status = true;
	dimmer100->level = 50;

	t.check(inc_level, 70);
	t.check(dec_level, 40);
}

// the tests below make sense for superclasses, since they test the ability
// of some lighting/dimmer10 devices to correctly interpret dimmer100 frames
void TestDimmer100Device::receiveGlobalDimmer100OnOffNonPullAdvanced()
{
}

void TestDimmer100Device::receiveGlobalDimmer100SetlevelNonPullAdvanced()
{
}

void TestDimmer100Device::receiveGlobalDimmer100IncDecNonPullAdvanced()
{
}
