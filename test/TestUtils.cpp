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
#include <map>
#include <random>
#include "../src/OrderUtils.h"

std::map<std::string,int> testUtils::getOpenPosition(const OrderBook& orderbook) 
{
    std::map<std::string, int> result;

    const auto& buy_orders = orderutils::getBuyOrders(orderbook);

    for_each(buy_orders.begin(), buy_orders.end(), [&result](const auto& order) {
           result[order.ticker_] += order.quantity_;
            });

    const auto& sell_orders = orderutils::getSellOrders(orderbook);
    for_each(sell_orders.begin(), sell_orders.end(), [&result](const auto& order) {
            result[order.ticker_] -= order.quantity_;
            });

    return result;
} 

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


