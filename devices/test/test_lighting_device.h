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


#ifndef TEST_LIGHTING_DEVICE_H
#define TEST_LIGHTING_DEVICE_H

#include "test_device.h"

#include <lighting_device.h>

const QString LIGHT_DEVICE_WHERE = "0313";
const QString LIGHT_ADDR_EXTENSION = "#4#12";
const QString LIGHT_ADDR_EXTENSION_2 = "#4#01";

class LightingDevice;

class TestLightingDevice : public TestDevice
{
Q_OBJECT
protected:
	void initLightingDevice(LightingDevice *d = 0);
	void cleanupLightingDevice();
	void setParams(QString w, PullMode m, AdvancedMode a);
	void setParams(QString w, PullMode m);
	virtual QString getRequestStatusFrame();
	virtual void sendPullRequestIfNeeded();

private slots:
	void init();
	void cleanup();

	void sendTurnOn();
	void sendRequestStatus();
	void sendFixedTiming();
	void sendVariableTiming();
	void sendRequestVariableTiming();

	void receiveLightOnOff();
	void receiveLightOnOffPull();
	void receiveLightOnOffNotPull();

	void receiveLightOnOffUnknown();
	void receiveLightOnOffUnknown2();
	void receiveLightOnOffPullExt();
	void receiveLightOnOffNotPullExt();
	void receiveLightOnOffNotPullAdvUnknown();
	void receiveLightOnOffUnknownExt2();
	void receiveLightOnOffUnknownExt();
	void receiveFixedTiming();
	void receiveVariableTiming();
	void receiveInvalidVariableTiming();

	void receiveGlobalDimmer100OnOffNonPullBase();
	void receiveGlobalDimmer100OnOffNonPullAdvanced();
	void receiveGlobalDimmer100OnOffPull();

	void testAdvancedDetection2();

private:
	void checkPullUnknown();

	bool cleanup_required;
	LightingDevice *dev;
};

class TestDimmerDevice : public TestLightingDevice
{
Q_OBJECT
protected:
	void initDimmer(DimmerDevice *d = 0);
	void cleanupDimmer();

	virtual void sendPullRequestIfNeeded();

private slots:
	void init();
	void cleanup();

	void sendDimmerIncreaseLevel();
	void sendDimmerDecreaseLevel();

	void receiveLightOnRequestLevel();
	void receiveDimmerLevel();
	void receiveDimmerLevel2();
	void receiveDimmerProblem();

	void receiveDimmer100WriteLevel();
	void receiveDimmer100Level();

	void receiveGlobalIncrementLevel();
	void receiveGlobalDecrementLevel();

	void receiveGlobalDimmer100SetlevelNonPullBase();
	void receiveGlobalDimmer100SetlevelNonPullAdvanced();
	void receiveGlobalDimmer100SetlevelPull();
	void receiveGlobalDimmer100IncDecNonPullBase();
	void receiveGlobalDimmer100IncDecNonPullAdvanced();
	void receiveGlobalDimmer100IncDecPull();

	void testAdvancedDetection();
	void testAdvancedDetection2();
	void testRequestLevel();

private:
	bool cleanup_required;
	DimmerDevice *dimmer;
};

class TestDimmer100Device : public TestDimmerDevice
{
Q_OBJECT
protected:
	virtual QString getRequestStatusFrame();

private slots:
	void init();
	void cleanup();

	void sendDimmer100IncreaseLevel();
	void sendDimmer100DecreaseLevel();
	void sendRequestDimmer100Status();

	void receiveDimmer100Status();
	void receiveDimmer100StatusLevel0();

	void receiveGlobalIncrementLevel100();
	void receiveGlobalDecrementLevel100();

	void testRequestLevel100();

	// disable some tests from superclasses
	void receiveGlobalDimmer100OnOffNonPullAdvanced();
	void receiveGlobalDimmer100SetlevelNonPullAdvanced();
	void receiveGlobalDimmer100IncDecNonPullAdvanced();
	void testAdvancedDetection();
	void testAdvancedDetection2();

	// override some more tests from superclasses
	void receiveGlobalDimmer100OnOffNonPullBase();
	void receiveGlobalDimmer100SetlevelNonPullBase();
	void receiveGlobalDimmer100IncDecNonPullBase();

private:
	Dimmer100Device *dimmer100;
};
#endif // TEST_LIGHTING_DEVICE_H
