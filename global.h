/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Valdambrini Gianni <aleister@develer.com>
 *
 * This file is a container for global stuff. If you really need a global
 * object, add here the extern declaration, if you have a global function (or
 * another type of declaration), put it into another file.
 *
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "btmain.h"
#include "device_cache.h"

/// A pointer to the main class of BTouch.
extern BtMain *BTouch;

/// Devices' cache
extern device_cache btouch_device_cache;

#endif // GLOBAL_H
