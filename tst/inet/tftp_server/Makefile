#
# @section License
#
# The MIT License (MIT)
#
# Copyright (c) 2014-2017, Erik Moqvist
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# This file is part of the Simba project.
#

NAME = tftp_server_suite
TYPE = suite
BOARD ?= linux

CDEFS += \
	CONFIG_START_FILESYSTEM=1 \
	CONFIG_FAT16=1 \
	CONFIG_SPIFFS=1 \
	CONFIG_THRD_ENV=1 \
	CONFIG_MODULE_INIT_LOG=1

SRC += socket_stub.c
SYNC_SRC = \
	event.c
INET_SRC = \
	inet.c \
	tftp_server.c
FILESYSTEMS_SRC = fat16.c spiffs.c
SPIFFS_SRC = \
	3pp/spiffs-0.3.5/src/spiffs_nucleus.c \
	3pp/spiffs-0.3.5/src/spiffs_gc.c \
	3pp/spiffs-0.3.5/src/spiffs_hydrogen.c \
	3pp/spiffs-0.3.5/src/spiffs_cache.c \
	3pp/spiffs-0.3.5/src/spiffs_check.c

include $(SIMBA_ROOT)/make/app.mk
