//
// Created by puneet on 10/07/16.
//

#ifndef MATCHINGENGINE_ORDERBOOKCACHE_H
#define MATCHINGENGINE_ORDERBOOKCACHE_H

#include <list>
#include <unordered_map>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <utility>

struct Order;
using OrderListT = std::vector<Order>;
using OrderBookCacheValueT = std::list<Order>;
using OrderBookCacheT = std::unordered_map<std::string, std::pair<std::unique_ptr<std::mutex>,OrderBookCacheValueT>>;
using OrderBookCacheIterT = OrderBookCacheT::iterator;

/// \brief Actual order book cache
struct OrderBookCache {
public:
    OrderBookCache();

    /**
     * \brief COPY the pending orders to a vector of orders, for easy traversal
     */
    OrderListT flattenCache() const;

    void insert(Order order) ;

    /**
     * \brief Returns the bounds of the range that includes all the elements of the range [first,last) with key equivalent to order.ticker_.
     */
    //std::pair<OrderBookCacheValueT::iterator, OrderBookCacheValueT::iterator> equal_range(const Order& order);

private:
    /**
     * \brief initializes the cache with all the valid stocks, so that the cache keys will never change throughout the porgram
     */
    void initCache();


public:
    OrderBookCacheT orders_;
};


#endif //MATCHINGENGINE_ORDERBOOKCACHE_H
