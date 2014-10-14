#
# Copyright (c) 2014      Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

SHELL = /bin/bash
.PHONY: all clean vclean run

LLDB_HOME := ${HOME}/local/lldb
BUILD_LOC := build
TEST_EXEC := ./source/gladius/gladius

all:
	@ \
	if [[ ! -d ${BUILD_LOC} ]]; then \
		mkdir ${BUILD_LOC} && \
		cd ${BUILD_LOC} && cmake ../ -DGLADIUS_LLDB_HOME=${LLDB_HOME} && \
		cd -; \
	fi; \
	cd ${BUILD_LOC} && make -j4

clean:
	make -C ${BUILD_LOC} clean

vclean:
	rm -rf ${BUILD_LOC}

run:
	@ \
	make all && cd ${BUILD_LOC} && ${TEST_EXEC};
