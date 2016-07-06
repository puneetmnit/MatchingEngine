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
#include <chrono>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include "../src/OrderUtils.h"

namespace testUtils {

    template<typename MapT>
    bool map_equals(const MapT& left, const MapT& right) {
        return left.size() == right.size()
            && std::equal(left.begin(), left.end(), right.begin());
    } 

    template<typename T>
    auto getQuantity(T&& container) {
        return std::accumulate(std::begin(container), std::end(container), 0, [](auto sum, auto order) {
                    return sum + order.quantity_;
                } );
    } 

    template<typename T>
    auto getQuantity(T&& container, std::string ticker) {
        return std::accumulate(std::begin(container), std::end(container), 0, [&ticker](auto sum, auto order) {
                    return ticker == order.ticker_ ? sum + order.quantity_ : sum;
                } );
    } 

    template<typename T>
    auto getQuantity(T&& container, OrderType type) {
        return std::accumulate(std::begin(container), std::end(container), 0, [&type](auto sum, auto order) {
                    return type == order.type_ ? sum + order.quantity_ : sum;
                } );
    } 

    template<typename T>
    auto getCount(T&& container, std::string ticker) {
        return std::count_if(std::begin(container), std::end(container), [&ticker](auto order) {
                    return ticker == order.ticker_ ;
                } );
    } 

    inline auto getBuyOrderSize(const OrderBook& orderbook) { 
        return orderutils::getBuyOrders(orderbook).size(); 
    }

    inline auto getSellOrderSize(const OrderBook& orderbook) { 
        return orderutils::getSellOrders(orderbook).size(); 
    }

    inline auto getBuyOrderSize(const OrderBook& orderbook, std::string ticker) {
        auto buy_orders = orderutils::getBuyOrders(orderbook);
        return getCount(std::move(buy_orders), ticker);
    } 

    inline auto getSellOrderSize(const OrderBook& orderbook, std::string ticker) {
        auto sell_orders = orderutils::getSellOrders(orderbook);
        return getCount(std::move(sell_orders), ticker);
    } 

    inline auto getTotalBuyQuantity(const OrderBook& orderbook) {
        auto buy_orders = orderutils::getBuyOrders(orderbook);
        return getQuantity(std::move(buy_orders));
    }
    inline auto getTotalBuyQuantity(const OrderBook& orderbook, std::string ticker) {
        auto buy_orders = orderutils::getBuyOrders(orderbook);
        return getQuantity(std::move(buy_orders), ticker);
    }

    inline auto getTotalSellQuantity(const OrderBook& orderbook) {
        auto sell_orders = orderutils::getSellOrders(orderbook);
        return getQuantity(std::move(sell_orders));
    }
    inline auto getTotalSellQuantity(const OrderBook& orderbook, std::string ticker) {
        auto sell_orders = orderutils::getSellOrders(orderbook);
        return getQuantity(std::move(sell_orders), ticker);
    }

    inline auto getOpenPosition(const OrderBook& orderbook) {
        std::map<std::string, int> result;

        auto buy_orders = orderutils::getBuyOrders(orderbook);
        for_each(buy_orders.begin(), buy_orders.end(), [&result](auto& order) {
                    auto res = result.find(order.ticker_);
                    if (res != result.end()) {
                        res->second += order.quantity_;
                    }
                    else {
                        result[order.ticker_] = order.quantity_;
                    }
                });

        auto sell_orders = orderutils::getSellOrders(orderbook);
        for_each(sell_orders.begin(), sell_orders.end(), [&result](auto& order) {
                    auto res = result.find(order.ticker_);
                    if (res != result.end()) {
                        res->second -= order.quantity_;
                    }
                    else {
                        result[order.ticker_] = -1*order.quantity_;
                    }
                });

        return result;
    } 

    class Random {
    public:
        Random(): seed(std::chrono::system_clock::now().time_since_epoch().count()), generator(seed) {} 

        std::string getRandomString(int size=1)
        {
            std::uniform_int_distribution<int> dist(0,25);
            
            auto pos = dist(generator);

            std::string str;
            while(size-- > 0) {
                str += std::string(1, 'A' + pos);
            }

            return str;

        } 

        int getRandomInt(int min=1, int max=10)
        {
            std::uniform_int_distribution<int> dist(min,max);
            
            return dist(generator);
        } 

        std::string getRandomTicker() {
            return getRandomString(1);
        } 


    private:
        unsigned seed;
        std::default_random_engine generator;
    }; 

}// namespace testUtils

#endif // _MATCHINGENGINE_TEST_TESTUTILS_H__
