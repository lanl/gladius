#
# Copyright (c) 2014-2015 Los Alamos National Security, LLC
#                         All rights reserved.
#
# This file is part of the Gladius project. See the LICENSE.txt file at the
# top-level directory of this distribution.
#

# A convenience makefile for building Gladius. Useful for me, but may not be
# that useful to most.

SHELL = /bin/bash
.PHONY: all clean vclean run runh runv install

GLADIUS_PREFIX := ${HOME}/local/gladius
LLDB_HOME      := ${HOME}/local/lldb
LMON_HOME      := ${HOME}/local/lmon
MRNET_HOME     := ${HOME}/local/mrnet
BUILD_LOC      := build
TEST_EXEC      := ${GLADIUS_PREFIX}/bin/gladius
APP_LAUNCHER   := orte

all:
	@ \
	if [[ ! -d ${BUILD_LOC} ]]; then \
		mkdir ${BUILD_LOC} && \
		cd ${BUILD_LOC} && \
		../configure \
		--prefix=${GLADIUS_PREFIX} \
		--with-lmon=${LMON_HOME} \
		--with-mrnet=${MRNET_HOME} && \
		make -j2 && \
		cd -; \
	fi

install:
	make -C ${BUILD_LOC} install

clean:
	make -C ${BUILD_LOC} clean

docs:
	make -C ${BUILD_LOC} docs

vclean:
	rm -rf ${BUILD_LOC}

run:
	@ \
	make all && \
	make install && \
	export LD_LIBRARY_PATH=${LMON_HOME}/lib:${LD_LIBRARY_PATH} && \
	export GLADIUS_APP_LAUNCHER=${APP_LAUNCHER} && \
	${TEST_EXEC}

runv:
	@ \
	make all && \
	make install && \
	export LD_LIBRARY_PATH=${LMON_HOME}/lib:${LD_LIBRARY_PATH} && \
	export GLADIUS_APP_LAUNCHER=${APP_LAUNCHER} && \
	valgrind \
	--leak-check=full \
	--trace-children=yes \
	--track-origins=yes \
	--log-file=ValgrindOutput.txt ${TEST_EXEC}

runh:
	@ \
	make all && \
	make install && \
	export LD_LIBRARY_PATH=${LMON_HOME}/lib:${LD_LIBRARY_PATH} && \
	export GLADIUS_APP_LAUNCHER=${APP_LAUNCHER} && \
	valgrind \
	--tool=helgrind \
	--log-file=HelgrindOutput.txt ${TEST_EXEC}
