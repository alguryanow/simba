#
# @file kernel/kernel.mk
# @version 1.0
#
# @section License
# Copyright (C) 2014-2015, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Simba project.
#


INC += $(SIMBA)/src/kernel
INC += $(SIMBA)/src/kernel/ports/$(ARCH)/$(TOOLCHAIN)

KERNEL_SRC ?= chan.c core.c fs.c inet.c log.c queue.c sem.c shell.c \
	      sock.c spin.c std.c sys.c thrd.c time.c timer.c

SRC += $(KERNEL_SRC:%=$(SIMBA)/src/kernel/%)
