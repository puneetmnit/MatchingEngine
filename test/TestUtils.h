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

namespace testUtils {
    int getBuyOrderSize(const OrderBook& orderbook) { 
        return orderbook.getBuyOrders.size(); 
    }

    int getSellOrderSize(const OrderBook& orderbook) { 
        return orderbook.getSellOrders.size(); 
    }

    int getBuyOrderSize(const OrderBook& orderbook, string stock) {
        auto buy_orders = orderbook.getBuyOrders();
        return getCount(std::move(buy_orders), stock):
    } 

    int getSellOrderSize(const OrderBook& orderbook, string stock) {
        auto sell_orders = orderbook.getSellOrders();
        return getCount(std::move(sell_orders), stock):
    } 

    int getTotalBuyQuantity(const OrderBook& orderbook, string stock) {
        auto buy_orders = orderbook.getBuyOrders();
        return getQuantity(std::move(buy_orders), stock):
    }

    int getTotalSellQuantity(const OrderBook& orderbook, string stock) {
        auto sell_orders = orderbook.getSellOrders();
        return getQuantity(std::move(sell_orders), stock):
    }

    template<typename T>
    auto getQuantity(T&& container, string stock) {
        return std::accumulate(std::begin(container), std::end(container), 0, [](auto sum, auto order) {
                    return stock == order.getStock() ? sum + order.getQuantity() : sum;
                } );
    } 

    template<typename T>
    auto getCount(T&& container, string stock) {
        return std::count_if(std::begin(container), std::end(container), 0, [](auto order) {
                    return stock == order.getStock() ;
                } );
    } 

}// namespace testUtils

#endif // _MATCHINGENGINE_TEST_TESTUTILS_H__
