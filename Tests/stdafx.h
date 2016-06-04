// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>

#include <boost/algorithm/string/replace.hpp>

#define BOOST_TEST_INCLUDED
#pragma warning (disable: 4702)
#include <boost/test/unit_test.hpp>

#include <SFML\Graphics.hpp>

namespace tt = boost::test_tools;
static const auto epsilon = tt::tolerance(0.001f);
bool IsEqual(float a, float b);
void VerifyVector2f(const sf::Vector2f & vector, const sf::Vector2f & expectedVector);
void VerifyColor(const sf::Color & color, const sf::Color & myColor);
// TODO: reference additional headers your program requires here
