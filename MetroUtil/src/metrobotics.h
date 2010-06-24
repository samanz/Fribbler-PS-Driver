#ifndef METROBOTICS
#define METROBOTICS
/**
 * \mainpage MetroUtil Documentation
 *
 * \section  main_preface Preface
 *
 *     This is still very much a work in progress.
 *
 * \section  main_intro Introduction
 *
 *     This library was created for the MetroBotics project at CUNY.  Its purpose is to provide a
 *     framework of commonly used classes and functions that aid in the development of artificial
 *     intelligence and robotics applications.
 *
 * \section  main_install Installing the Library
 *     <ol>
 *       <li>
 *         Run \c make to build the library.
 *       </li>
 *       <li>
 *         Run \c make \c install to install the headers and library files into the \em
 *         MetroUtil/include and \em MetroUtil/lib directories.
 *       </li>
 *     </ol>
 *
 * \section  main_usage Using the Library
 *     <ol>
 *       <li>
 *         Include the \link metrobotics.h \endlink header file into your source code.
 *             \code
 *                 #include "metrobotics.h"
 *             \endcode
 *       </li>
 *       <li>
 *         All classes and functions in the library exist within the \link metrobotics \endlink
 *         namespace.
 *             \code
 *                 using namespace metrobotics;
 *             \endcode
 *       </li>
 *       <li>
 *         Link the \em libMetrobotics.a library file into your project.
 *             \code
 *                 g++ foo.cpp -IMetroUtil/include -LMetroUtil/lib -lMetrobotics
 *             \endcode
 *       </li>
 *     </ol>
 *
 * \section main_references References
 *     <ul>
 *       <li>
 *         <a href="http://bots.sci.brooklyn.cuny.edu/metrobotics/">Metrobotics Home Page</a>
 *       </li>
 *       <li>
 *         <a href="http://github.com/Arkainium/MetroUtil">MetroUtil Git Repository</a>
 *       </li>
 *     </ul>
 *
 * \section main_maintainers Maintainers
 *     <ul>
 *       <li>
 *         Mark Manashirov <mark.manashirov@gmail.com>
 *       </li>
 *     </ul>
 *
 * \section main_acknowledgements Acknowledgements
 */


/**
 * \file    "metrobotics.h"
 *
 * \brief   The all-inclusive header file for this utility library.
 *
 * \details It is sufficient to include just this one header file into your source code
 *          to gain access to all of the library's constituent classes and functions.
 */

// [Simply include everything!]
#include "Communication/Serial.h"
#include "Communication/PosixSerial.h"
#include "Math/RealPredicate.h"
#include "Math/RealEquality.h"
#include "Math/RealLessThan.h"
#include "Math/VectorN.h"
#include "Math/RealVectorN.h"
#include "Math/Lerp.h"
#include "Timer/Timer.h"
#include "Timer/PosixTimer.h"

/**
 * \namespace  metrobotics
 *
 * \brief      Contains all of the library's classes and functions.
 *
 * \details    The namespace under which all of the library's classes and functions are defined.
 */
namespace metrobotics
{
	// [Nothing is actually defined in this file.]
}

#endif
