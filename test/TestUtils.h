/**@file	/Users/puneet/Documents/technical/projects/MatchingEngine/test/TestUtils.h
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 11:19:50 2016 \n
 * 	Last Update:	Sun Jul  3 11:19:50 2016
 */
#ifndef _MATCHINGENGINE_TEST_TESTUTILS_H__
#define _MATCHINGENGINE_TEST_TESTUTILS_H__

#include <algorithm>
#include <iterator>
#include <numeric>
#include "../src/OrderUtils.h"

namespace testUtils {

    template<typename T>
    auto getQuantity(T&& container, std::string ticker) {
        return std::accumulate(std::begin(container), std::end(container), 0, [&ticker](auto sum, auto order) {
                    return ticker == order.ticker_ ? sum + order.quantity_ : sum;
                } );
    } 

    template<typename T>
    auto getCount(T&& container, std::string ticker) {
        return std::count_if(std::begin(container), std::end(container), [&ticker](auto order) {
                    return ticker == order.ticker_ ;
                } );
    } 

    int getBuyOrderSize(const OrderBook& orderbook) { 
        return orderutils::getBuyOrders(orderbook).size(); 
    }

    int getSellOrderSize(const OrderBook& orderbook) { 
        return orderutils::getSellOrders(orderbook).size(); 
    }

    int getBuyOrderSize(const OrderBook& orderbook, std::string ticker) {
        auto buy_orders = orderutils::getBuyOrders(orderbook);
        return getCount(std::move(buy_orders), ticker);
    } 

    int getSellOrderSize(const OrderBook& orderbook, std::string ticker) {
        auto sell_orders = orderutils::getSellOrders(orderbook);
        return getCount(std::move(sell_orders), ticker);
    } 

    int getTotalBuyQuantity(const OrderBook& orderbook, std::string ticker) {
        auto buy_orders = orderutils::getBuyOrders(orderbook);
        return getQuantity(std::move(buy_orders), ticker);
    }

    int getTotalSellQuantity(const OrderBook& orderbook, std::string ticker) {
        auto sell_orders = orderutils::getSellOrders(orderbook);
        return getQuantity(std::move(sell_orders), ticker);
    }
}// namespace testUtils

#endif // _MATCHINGENGINE_TEST_TESTUTILS_H__
