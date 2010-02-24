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


#include "test_pull_manager.h"
#include <lighting_device.h>
#include <openmsg.h>

#include <QtTest/QtTest>

namespace
{
	bool parseFrames(const char *frame, PullStateManager *psm, bool is_environment)
	{
		OpenMsg msg(frame);
		return psm->moreFrameNeeded(msg, is_environment);
	}
}

void TestPullManager::testSimpleLight()
{
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off, light on
	parseFrames("*1*1*11##", &psm, false);
	parseFrames("*1*0*1##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
}

void TestPullManager::testSimpleLight2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1*1##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer off, general level 3, dimmer level 3
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level 7, general level 3, dimmer level 3
	parseFrames("*1*7*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer3()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level 7, general level 3, dimmer level 7
	parseFrames("*1*7*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*7*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
}

void TestPullManager::testDimmer100()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level100 20, general level100 40, dimmer level100 40
	parseFrames("*#1*12*1*120*255##", &psm, false);
	parseFrames("*#1*0*#1*140*255##", &psm, true);
	parseFrames("*#1*12*1*140*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level100 20, general level100 40, dimmer level100 40
	parseFrames("*#1*12*1*100*255##", &psm, false);
	parseFrames("*#1*1*#1*140*255##", &psm, true);
	parseFrames("*#1*12*1*140*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_3()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer on, general level100 40, dimmer level100 40
	parseFrames("*1*1*12##", &psm, false);
	parseFrames("*#1*1*#1*140*255##", &psm, true);
	parseFrames("*#1*12*1*140*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

// From off to on (dimmer)
void TestPullManager::testVariableTiming()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer off, general variable timing, dimmer level 7
	parseFrames("*1*0*12##", &psm, false);
	parseFrames("*#1*0*#2*1*7*4##", &psm, true);
	parseFrames("*1*7*12##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

// When on arrives a variable timing => do nothing
void TestPullManager::testVariableTiming2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level 7, general variable timing
	parseFrames("*1*7*12##", &psm, false);
	// we can't decide, no need to request status
	bool request_status = parseFrames("*#1*0*#2*1*7*4##", &psm, true);
	QCOMPARE(request_status, false);
}

// from off to on (dimmer100)
void TestPullManager::testVariableTiming3()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer off, general variable timing, dimmer level100 40
	parseFrames("*1*0*12##", &psm, false);
	parseFrames("*#1*0*#2*1*7*4##", &psm, true);
	parseFrames("*#1*12*1*140*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
}

// test pull devices with variable temporization
void TestPullManager::testVariableTiming4()
{
	PullStateManager psm(PULL_UNKNOWN);
	// light off, variable timing, environment on, light off
	parseFrames("*1*0*12##", &psm, false);
	parseFrames("*#1*11*2*0*0*0##", &psm, false);
	parseFrames("*1*1*1##", &psm, true);
	parseFrames("*1*0*12##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
}
