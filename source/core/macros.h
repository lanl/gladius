/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/exception.h"
#include "core/colors.h"

/**
 * Convenience macro used for stringification.
 */
#define GLADIUS_STRINGIFY(x) #x
#define GLADIUS_TOSTRING(x)  GLADIUS_STRINGIFY(x)

/**
 * Convenience macro used to silence warnings about unused variables.
 */
#define GLADIUS_UNUSED(x)                                                      \
do {                                                                           \
    (void)(x);                                                                 \
} while (0)

/**
 * Convenience macro for throwing an out of resource exception.
 */
#define GLADIUS_THROW_OOR()                                                    \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "Out of Resources"                                               \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing an exception with a given string.
 */
#define GLADIUS_THROW(msg)                                                     \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              std::string(msg)                                                 \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing an invalid argument exception.
 */
#define GLADIUS_THROW_INVLD_ARG()                                              \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "Invalid argument detected"                                      \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing a call failure message.
 */
#define GLADIUS_THROW_CALL_FAILED(msg)                                         \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "The following call failed: '" + std::string(msg) + "'."         \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing a call failure message with a numerical status
 * code.
 */
#define GLADIUS_THROW_CALL_FAILED_RC(msg, rc)                                  \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "The following call failed: '" + std::string(msg) +              \
              "' with status code " + std::to_string(rc) + "."                 \
          );                                                                   \
} while (0)

/**
 * Convenience macro for printing out warning messages.
 */
#define GLADIUS_WARN(msg)                                                      \
do {                                                                           \
    using namespace gladius::core;                                             \
    std::cerr << colors::color().ansiBeginColor(colors::YELLOW)                \
              << "[" PACKAGE_NAME " WARNING @ "                                \
              << __FILE__ << ": " << __LINE__ << "]: "                         \
              << colors::color().ansiEndColor()                                \
              << std::string(msg) << std::endl;                                \
} while (0)

/**
 * Convenience macro for printing out messages to cerr;
 */
#define GLADIUS_CERR                                                           \
    std::cerr << gladius::core::colors::color().ansiBeginColor(                \
                     gladius::core::colors::RED                                \
                 )                                                             \
              << "[" PACKAGE_NAME "] "                                         \
              << gladius::core::colors::color().ansiEndColor()

/**
 * Convenience macro for printing out warning messages to cerr;
 */
#define GLADIUS_CERR_WARN                                                      \
    std::cerr << gladius::core::colors::color().ansiBeginColor(                \
                     gladius::core::colors::YELLOW                             \
                 )                                                             \
              << "[" PACKAGE_NAME "] "                                         \
              << gladius::core::colors::color().ansiEndColor()

/**
 * Convenience macro for printing out status messages to cout;
 */
#define GLADIUS_COUT_STAT                                                      \
    std::cout << gladius::core::colors::color().ansiBeginColor(                \
                     gladius::core::colors::GREEN                              \
                 )                                                             \
              << "[" PACKAGE_NAME "] "                                         \
              << gladius::core::colors::color().ansiEndColor()

/**
 * Convenience macro for printing out component-specific status messages to
 * cout; The caller is able to customize (w00t) the output for their needs.
 */
#define GLADIUS_COMP_COUT(compName, compNameColorCode)                         \
    std::cout << compNameColorCode                                             \
              << "[" + std::string(compName) + "] "                            \
              << gladius::core::colors::color().ansiEndColor()
