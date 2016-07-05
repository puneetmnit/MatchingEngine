/**@file	MatchingEngine/src/OrderBook.h
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 00:47:30 2016 \n
 * 	Last Update:	Sun Jul  3 00:47:30 2016
 */
#ifndef _MATCHINGENGINE_SRC_ORDERBOOK_H__
#define _MATCHINGENGINE_SRC_ORDERBOOK_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include "OrderUtils.h"


struct Order;
using OrderListT = std::vector<Order>;
using OrderBookCacheValueT = std::list<Order>; ///> \todo : there should be locable object also associated with it
using OrderBookCacheT = std::unordered_map<std::string, OrderBookCacheValueT>;
using OrderBookCacheIterT = OrderBookCacheT::iterator;

using ResponseCallbackT = std::function<void(int)>;

enum class OrderType { BUY, SELL };

/**
 * \brief A simple Order structure. Kindly note that it even doesn't have a price point
 */

struct Order
{
    std::string trader_;
    int quantity_;
    std::string ticker_;
    OrderType type_; ///< can be either BUY or SELL
    int order_id_;

    bool operator==(const Order& that) {
        return ( trader_ == that.trader_
                && quantity_  == that.quantity_
                && ticker_ == that.ticker_
                && type_ == that.type_);
    }

};

/**
 * \brief A class to maintain all the orders across the system.
 *
 * OrderBook works on the Order structure. Currently, it doesn't care about the price and time of the orders.
 * This doesn't guarantee to match an earlier order first i.e. FCFS is not followed.
 */

class OrderBook
{
public:
    //return false if not a valid order
    bool addOrder(Order order);   ///< use to add new orders

    /** \brief set a callback which will be executed asynchronously on matching of any order
     *
     * \param callback the callback to call. This should be equivalent to a function returning void and accepting an integer (for order_id)
     */ 
    void setResponseCallback(const ResponseCallbackT& callback) {
        callback_ = callback;
    }

    friend OrderListT orderutils::getBuyOrders(const OrderBook& orderbook) ;
    friend OrderListT orderutils::getSellOrders( const OrderBook& orderbook) ;

    struct OrderBookCache {
    public:
        OrderBookCache();

        /**
         * \brief copy the pending orders to a vector of orders, for easy traversal
         */
        OrderListT flattenCache() const;

        void insert(Order order) ;
        
        /**
         * \brief Returns the bounds of the range that includes all the elements of the range [first,last) with key equivalent to order.ticker_.
         */
        std::pair<OrderBookCacheValueT::iterator, OrderBookCacheValueT::iterator> equal_range(const Order& order);

    private:
        /**
         * \brief initialize the cache with all the valid stocks, so that the cache keys will never change throughout the porgram
         */
        void initCache();

    private:
        OrderBookCacheT orders_;
    }; 

private:
    OrderBookCache buyOrders_;
    OrderBookCache sellOrders_;

    ResponseCallbackT callback_;

};

#endif // _MATCHINGENGINE_SRC_ORDERBOOK_H__»
