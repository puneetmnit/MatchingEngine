/**@file	MatchingEngine/src/OrderBook.h
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 00:47:30 2016 \n
 * 	Last Update:	Sun Jul  3 00:47:30 2016
 */
#ifndef _MATCHINGENGINE_SRC_ORDERBOOK_H__
#define _MATCHINGENGINE_SRC_ORDERBOOK_H__

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

#include <boost/flyweight.hpp>

#include "OrderUtils.h"


struct Order;
using OrderListT = std::vector<Order>;
using OrderBookCacheValueT = std::list<Order>; ///> \todo : there should be locable object also associated with it
using OrderBookCacheT = std::unordered_map<std::string, std::pair<std::unique_ptr<std::mutex>,OrderBookCacheValueT>>;
using OrderBookCacheIterT = OrderBookCacheT::iterator;

using ResponseCallbackT = std::function<void(int)>;

enum class OrderType { BUY, SELL };

/**
 * \brief A simple Order structure. Kindly note that it even doesn't have a price point
 */

struct Order
{
public:
    Order(std::string trader, int quantity, std::string ticker, OrderType type) : trader_(trader), quantity_(quantity), ticker_(ticker), type_(type), order_id_(count.fetch_add(1)) {}

    //Order(const Order& order) : trader_(order.trader_), quantity_(order.quantity_.load()), ticker_(order.ticker_), type_(order.type_), order_id_(order.order_id_)
    //{}

    //Order(Order&& order) noexcept : trader_(std::move(order.trader_)), quantity_(order.quantity_.load()), ticker_(std::move(order.ticker_)), type_(std::move(order.type_)), order_id_(std::move(order.order_id_))
    //{}

    //no assignment operators
    //Order& operator=(const Order&) = delete;
    //Order& operator=(Order&&) = delete;

    

   bool operator==(const Order& that) {
        return ( trader_ == that.trader_
                && quantity_  == that.quantity_
                && ticker_ == that.ticker_
                && type_ == that.type_);
    }

public:
    std::string trader_;
    int quantity_;
    //std::atomic<int> quantity_;
    boost::flyweight<std::string> ticker_; ///< using flyweight as there is very small number of tickers as compared to number of orders active at a given time.

    OrderType type_; ///< can be either BUY or SELL
    int order_id_;
    
private:
        static std::atomic<int> count;

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
        //std::atomic<OrderBookCacheValueT::pointer> locked_element_;

    private:
        /**
         * \brief initialize the cache with all the valid stocks, so that the cache keys will never change throughout the porgram
         */
        void initCache();


    public:
        OrderBookCacheT orders_;
    }; 

private:
    OrderBookCache buyOrders_;
    OrderBookCache sellOrders_;

    ResponseCallbackT callback_;

};

#endif // _MATCHINGENGINE_SRC_ORDERBOOK_H__»
