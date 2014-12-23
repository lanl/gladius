/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "appl-factory.h"

using namespace gladius::appl;

/**
 * Names of the launchers that are supported.
 */
const std::string AppLFactory::sORTEName  = "orte";
const std::string AppLFactory::sSRUNName  = "srun";
const std::string AppLFactory::sAPRUNName = "aprun";
