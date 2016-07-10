/**@file	MatchingEngine/test/TestUtils.cpp
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Wed Jul  6 15:28:04 2016 \n
 * 	Last Update:	Wed Jul  6 15:28:04 2016
 */

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/
#include "TestUtils.h"

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include "../src/OrderUtils.h"

std::string testUtils::Random::getRandomString(int size)
{
    std::uniform_int_distribution<int> dist(0,25);

    auto pos = dist(generator);

    std::string str;
    while(size-- > 0) {
        str += std::string(1, 'A' + pos);
    }

    return str;

} 

int testUtils::Random::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min,max);

    return dist(generator);
} 

std::string testUtils::Random::getRandomTicker() 
{
    return getRandomString(1);
} 


