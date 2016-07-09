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
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include "../src/OrderBook.h"
#include "../src/OrderUtils.h"

namespace testUtils {

    template<typename KeyT, typename ValueT>
    auto& operator<<(std::ostream& os, const std::map<KeyT, ValueT>& m)
    {
        for_each(m.begin(), m.end(), [&os](const auto& p){
            os << p.first << " --> " << p.second << std::endl;
        });
        return os;
    }

    template<typename MapT>
    bool map_equals(const MapT& left, const MapT& right) {
       /* for(auto l : left) {
            std::cout << l.first << " : " << l.second << ",";
            if (right.find(l.first) != right.end()) {
                std::cout << right.find(l.first)->second;
            }
            else {
                std::cout << "null";
            }
            std::cout << std::endl;
        }
        std::cout << "left: " << left.size() << "v/s right: " << right.size() << std::endl;
        */

        return left.size() == right.size() &&
             std::equal(left.begin(), left.end(), right.begin());
        }

    template<typename T>
    auto getQuantity(const T& container) {
        return std::accumulate(std::begin(container), std::end(container), 0, [](auto sum, auto order) {
                    return sum + order.quantity_;
                } );
    } 

    template<typename T>
    auto getQuantity(const T& container, std::string ticker) {
        return std::accumulate(std::begin(container), std::end(container), 0, [&ticker](auto sum, auto order) {
                    return ticker == order.ticker_ ? sum + order.quantity_ : sum;
                } );
    } 

    template<typename T>
    auto getQuantity(const T& container, OrderType type) {
        return std::accumulate(std::begin(container), std::end(container), 0, [&type](auto sum, auto order) {
                    return type == order.type_ ? sum + order.quantity_ : sum;
                } );
    } 

    template<typename T>
    auto getCount(const T& container, std::string ticker) {
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
        const auto& buy_orders = orderutils::getBuyOrders(orderbook);
        return getCount(buy_orders, ticker);
    } 

    inline auto getSellOrderSize(const OrderBook& orderbook, std::string ticker) {
        const auto& sell_orders = orderutils::getSellOrders(orderbook);
        return getCount(sell_orders, ticker);
    } 

    inline auto getTotalBuyQuantity(const OrderBook& orderbook) {
        const auto& buy_orders = orderutils::getBuyOrders(orderbook);
        return getQuantity(buy_orders);
    }
    inline auto getTotalBuyQuantity(const OrderBook& orderbook, std::string ticker) {
        const auto& buy_orders = orderutils::getBuyOrders(orderbook);
        return getQuantity(buy_orders, ticker);
    }

    inline auto getTotalSellQuantity(const OrderBook& orderbook) {
        const auto& sell_orders = orderutils::getSellOrders(orderbook);
        return getQuantity(sell_orders);
    }
    inline auto getTotalSellQuantity(const OrderBook& orderbook, std::string ticker) {
        const auto& sell_orders = orderutils::getSellOrders(orderbook);
        return getQuantity(sell_orders, ticker);
    }

    std::map<std::string,int> getOpenPosition(const OrderBook& orderbook) ;

    class Random {
    public:
        Random(): seed(std::chrono::system_clock::now().time_since_epoch().count()), generator(seed) {} 

        std::string getRandomString(int size=1);
         

        int getRandomInt(int min=1, int max=10);
         

        std::string getRandomTicker() ; 


    private:
        unsigned seed;
        std::default_random_engine generator;
    }; 

}// namespace testUtils

#endif // _MATCHINGENGINE_TEST_TESTUTILS_H__
