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
LMON_HOME := ${HOME}/local/lmon
MRNET_HOME := ${HOME}/local/mrnet
BUILD_LOC := build
TEST_EXEC := ./source/gladius/gladius
CMAKE_FLAGS :=

all:
	@ \
	if [[ ! -d ${BUILD_LOC} ]]; then \
		mkdir ${BUILD_LOC} && \
		cd ${BUILD_LOC} && cmake ../ \
		${CMAKE_FLAGS} \
		-DGLADIUS_LMON_HOME=${LMON_HOME} \
		-DGLADIUS_LLDB_HOME=${LLDB_HOME} \
		-DGLADIUS_MRNET_HOME=${MRNET_HOME} && \
		cd -; \
	fi; \
	cd ${BUILD_LOC} && make -j4

clean:
	make -C ${BUILD_LOC} clean

docs:
	make -C ${BUILD_LOC} docs

vclean:
	rm -rf ${BUILD_LOC}

run:
	@ \
	make all && cd ${BUILD_LOC} && ${TEST_EXEC};

debug:
	make CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=Debug" all
