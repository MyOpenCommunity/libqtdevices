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

#include "test_devicescache.h"
#include "openserver_mock.h"
#include "devices_cache.h"
#include "openclient.h"
#include "probe_device.h"

#include <QtTest>


void TestDevicesCache::init()
{
	client_request->setDelay(0);
	cache = new DevicesCache();
	cache->devicesCreated();
}

void TestDevicesCache::cleanup()
{
	delete cache;
}

void TestDevicesCache::testNonLazyInit()
{
	NonControlledProbeDevice *d = new NonControlledProbeDevice("116", NonControlledProbeDevice::EXTERNAL);
	cache->insert("0", d);

	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);
}

void TestDevicesCache::testNonLazyInitReconnect()
{
	NonControlledProbeDevice *d = new NonControlledProbeDevice("116", NonControlledProbeDevice::EXTERNAL);
	cache->insert("0", d);

	// first connection
	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);

	// reconnect
	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);
}

void TestDevicesCache::testLazyInit()
{
	NonControlledProbeDevice *d = new NonControlledProbeDevice("116", NonControlledProbeDevice::EXTERNAL);
	d->setSupportedInitMode(device::DEFERRED_INIT);
	cache->insert("0", d);

	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString(""));
	QCOMPARE(cache->lazy_update_list.count(), 1);

	cache->checkLazyUpdate(2);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);
}

void TestDevicesCache::testLazyInitReconnect()
{
	NonControlledProbeDevice *d = new NonControlledProbeDevice("116", NonControlledProbeDevice::EXTERNAL);
	d->setSupportedInitMode(device::DEFERRED_INIT);
	cache->insert("0", d);

	// first connection
	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString(""));
	QCOMPARE(cache->lazy_update_list.count(), 1);

	cache->checkLazyUpdate(2);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);

	// reconnect
	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString(""));
	QCOMPARE(cache->lazy_update_list.count(), 1);

	cache->checkLazyUpdate(2);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#4*116*15#1##"));
	QCOMPARE(cache->lazy_update_list.count(), 0);
}

void TestDevicesCache::testNoInit()
{
	NonControlledProbeDevice *d = new NonControlledProbeDevice("116", NonControlledProbeDevice::EXTERNAL);
	d->setSupportedInitMode(device::DISABLED_INIT);
	cache->insert("0", d);

	cache->initOpenserverDevices(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString(""));
	QCOMPARE(cache->lazy_update_list.count(), 0);
}
