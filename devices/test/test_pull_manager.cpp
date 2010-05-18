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

bool TestPullManager::parseFrames(const char *frame, PullStateManager *psm, bool is_environment)
{
	OpenMsg msg(frame);
	bool res = psm->moreFrameNeeded(msg, is_environment);
	// assume the device sent the delayed status request
	if (res)
		psm->status_requested = res;
	return res;
}

void TestPullManager::testSimpleLight_on()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1*0##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*1*0##", &psm2, true);
	parseFrames("*1*0*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_off()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off, light off
	parseFrames("*1*1*11##", &psm, false);
	parseFrames("*1*0*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off, light on
	parseFrames("*1*1*11##", &psm2, false);
	parseFrames("*1*0*0##", &psm2, true);
	parseFrames("*1*1*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_on_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light off, general on 100, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1#1*0##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), true);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light off, general on 100, light on
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*1#1*0##", &psm2, true);
	parseFrames("*1*0*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_off_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*1*11##", &psm, false);
	parseFrames("*1*0#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), true);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off 100, light on
	parseFrames("*1*1*11##", &psm2, false);
	parseFrames("*1*0#1*0##", &psm2, true);
	parseFrames("*1*1*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_setLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*2*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_setLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*#1*0*#1*110*255####", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_increaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general increaseLevel, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*30*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_decreaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general decreaseLevel, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*31*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_increaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general increaseLevel 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*30#1#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_decreaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general decreaseLevel 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*31#1#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_timing()
{
	// PULL device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*11*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*11*0##", &psm2, true);
	parseFrames("*1*1*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testSimpleLight_variabletiming()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN, &LightingDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm2, true);
	parseFrames("*1*1*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), true);
}

void TestPullManager::testDimmer_on()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1*0##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*1*0##", &psm2, true);
	parseFrames("*1*0*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_off()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off, light off
	parseFrames("*1*3*11##", &psm, false);
	parseFrames("*1*0*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off, light on
	parseFrames("*1*3*11##", &psm2, false);
	parseFrames("*1*0*0##", &psm2, true);
	parseFrames("*1*3*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_on_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light off, general on 100, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1#1*0##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), true);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light off, general on 100, light on
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*1#1*0##", &psm2, true);
	parseFrames("*1*0*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_off_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*3*11##", &psm, false);
	parseFrames("*1*0#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), true);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light on
	parseFrames("*1*3*11##", &psm2, false);
	parseFrames("*1*0#1*0##", &psm2, true);
	parseFrames("*1*3*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_setLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*2*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*2*0##", &psm2, true);
	parseFrames("*1*2*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_setLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*#1*0*#1*110*255####", &psm, true);
	parseFrames("*1*2*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), true);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general off 100, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*#1*0*#1*110*255####", &psm2, true);
	parseFrames("*1*0*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_increaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general increaseLevel, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*30*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general increaseLevel, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*30*0##", &psm2, true);
	parseFrames("*1*3*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_decreaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general decreaseLevel, light off
	parseFrames("*1*2*11##", &psm, false);
	parseFrames("*1*31*0##", &psm, true);
	parseFrames("*1*2*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general decreaseLevel, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*31*0##", &psm2, true);
	parseFrames("*1*4*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_increaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general increaseLevel 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*30#1#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general increaseLevel 100, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*30#1#1*0##", &psm2, true);
	parseFrames("*1*2*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), true);
}

void TestPullManager::testDimmer_decreaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general decreaseLevel 100, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*31#1#1*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general decreaseLevel 100, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*31#1#1*0##", &psm2, true);
	parseFrames("*1*3*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), true);
}

void TestPullManager::testDimmer_timing()
{
	// PULL device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*11*0##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*1*11*0##", &psm2, true);
	parseFrames("*1*3*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_variabletiming()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN, &DimmerDevice::isFrameHandled);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm, true);
	parseFrames("*1*0*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL_UNKNOWN);
	QCOMPARE(psm.isAdvanced(), false);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general timing, light off
	parseFrames("*1*0*11##", &psm2, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm2, true);
	parseFrames("*1*2*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer100_on()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN);
	// light off, general on, light on
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*1*0##", &psm, true);
	parseFrames("*#1*11*1*110*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN);
	// light off, general on, light on
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*1*0##", &psm2, true);
	parseFrames("*#1*11*1*100*255###", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_off()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off, light off
	parseFrames("*#1*11*1*110*255##", &psm, false);
	parseFrames("*1*0*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	// PULL device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general off, light on
	parseFrames("*#1*11*1*110*255###", &psm2, false);
	parseFrames("*1*0*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_on_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN);
	// light off, general on 100, light on
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*1#1*0##", &psm, true);
	parseFrames("*#1*11*1*110*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light off, general on 100, light on
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*1#1*0##", &psm2, true);
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_off_100()
{
	// NOT_PULL device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off 100, light off
	parseFrames("*#1*11*1*110*255##", &psm, false);
	parseFrames("*1*0#1*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general off 100, light on
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	parseFrames("*1*0#1*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_setLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off 100, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*2*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general off 100, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*2*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_setLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off 100, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*#1*0*#1*110*255####", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general off 100, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*#1*0*#1*110*255####", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_increaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general increaseLevel, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*30*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general increaseLevel, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*30*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_decreaseLevel()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general decreaseLevel, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*31*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general decreaseLevel, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*31*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_increaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general increaseLevel 100, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*30#1#1*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general increaseLevel 100, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*30#1#1*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_decreaseLevel_100()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general decreaseLevel 100, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*31#1#1*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// PULL_UNKNOWN device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general decreaseLevel 100, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*31#1#1*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_timing()
{
	// PULL device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general timing, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*1*11*0##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general timing, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*1*11*0##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testDimmer100_variabletiming()
{
	// PULL_UNKNOWN device
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general timing, light off
	parseFrames("*#1*11*1*100*255##", &psm, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm, true);
	parseFrames("*#1*11*1*100*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);

	// NOT_PULL device
	PullStateManager psm2(PULL_UNKNOWN);
	// light on, general timing, light off
	parseFrames("*#1*11*1*100*255##", &psm2, false);
	parseFrames("*#1*0*#2*1*2*3##", &psm2, true);
	parseFrames("*#1*11*1*110*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), NOT_PULL);
}

void TestPullManager::testSimpleLight()
{
	PullStateManager psm(PULL_UNKNOWN);
	// light on, general off, light on
	parseFrames("*1*1*11##", &psm, false);
	parseFrames("*1*0*1##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testSimpleLight2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// light off, general on, light on
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*1*1##", &psm, true);
	parseFrames("*1*1*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testDimmer()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer off, general level 3, dimmer level 3
	parseFrames("*1*0*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testDimmer2()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level 7, general level 3, dimmer level 3
	parseFrames("*1*7*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*3*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testDimmer3()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level 7, general level 3, dimmer level 7
	parseFrames("*1*7*11##", &psm, false);
	parseFrames("*1*3*1##", &psm, true);
	parseFrames("*1*7*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), PULL);
	QCOMPARE(psm.isAdvanced(), false);
}

void TestPullManager::testDimmer_increaseLevel_old()
{
	PullStateManager psm(PULL_UNKNOWN);
	// NOT_PULL device
	// dimmer level 5, general level up, dimmer level 6
	parseFrames("*1*5*11##", &psm, false);
	parseFrames("*1*30*1##", &psm, true);
	parseFrames("*1*6*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	PullStateManager psm2(PULL_UNKNOWN);
	// PULL device
	// dimmer level 5, general level up, dimmer level 5
	parseFrames("*1*5*11##", &psm2, false);
	parseFrames("*1*30*1##", &psm2, true);
	parseFrames("*1*5*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
}

void TestPullManager::testDimmer_decreaseLevel_old()
{
	PullStateManager psm(PULL_UNKNOWN);
	// NOT_PULL device
	// dimmer level 5, general level down, dimmer level 4
	parseFrames("*1*5*11##", &psm, false);
	parseFrames("*1*31*1##", &psm, true);
	parseFrames("*1*4*11##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);
	QCOMPARE(psm.isAdvanced(), false);

	PullStateManager psm2(PULL_UNKNOWN);
	// PULL device
	// dimmer level 5, general level down, dimmer level 5
	parseFrames("*1*5*11##", &psm2, false);
	parseFrames("*1*31*1##", &psm2, true);
	parseFrames("*1*5*11##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
	QCOMPARE(psm2.isAdvanced(), false);
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

void TestPullManager::testDimmer100_increaseLevel_old()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level100 40, general increase level100 by 5, dimmer level100 45
	parseFrames("*#1*12*1*140*255##", &psm, false);
	parseFrames("*1*30#5#255*1##", &psm, true);
	parseFrames("*#1*12*1*145*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	PullStateManager psm2(PULL_UNKNOWN);
	// dimmer level100 40, general increase level100 by 5, dimmer level100 40
	parseFrames("*#1*12*1*140*255##", &psm2, false);
	parseFrames("*1*30#5#255*1##", &psm2, true);
	parseFrames("*#1*12*1*140*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_decreaseLevel_old()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level100 40, general decrease level100 by 5, dimmer level100 35
	parseFrames("*#1*12*1*140*255##", &psm, false);
	parseFrames("*1*31#5#255*1##", &psm, true);
	parseFrames("*#1*12*1*135*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	PullStateManager psm2(PULL_UNKNOWN);
	// dimmer level100 40, general decrease level100 by 5, dimmer level100 40
	parseFrames("*#1*12*1*140*255##", &psm2, false);
	parseFrames("*1*31#5#255*1##", &psm2, true);
	parseFrames("*#1*12*1*140*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
}

void TestPullManager::testDimmer100_receiveIncreaseLevel10()
{
	PullStateManager psm(PULL_UNKNOWN);
	// dimmer level100 4, general increase level, dimmer level100 10
	parseFrames("*#1*12*1*104*255##", &psm, false);
	parseFrames("*1*31*1##", &psm, true);
	parseFrames("*#1*12*1*110*255##", &psm, false);
	QCOMPARE(psm.getPullMode(), NOT_PULL);

	PullStateManager psm2(PULL_UNKNOWN);
	// dimmer level100 4, general increase level, dimmer level100 10
	parseFrames("*#1*12*1*104*255##", &psm2, false);
	parseFrames("*1*31*1##", &psm2, true);
	parseFrames("*#1*12*1*104*255##", &psm2, false);
	QCOMPARE(psm2.getPullMode(), PULL);
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
