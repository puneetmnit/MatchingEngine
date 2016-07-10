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
#include <thread>
#include <list>
#include <unordered_map>
#include <vector>

#include <boost/flyweight.hpp>

#include "OrderBookCache.h"


struct Order;

using ResponseCallbackT = std::function<void(int)>;

/**
 * \brief A simple Order structure. Kindly note that it even doesn't have a price point
 */

struct Order
{
public:
    enum class OrderType { BUY, SELL };

    Order(std::string trader, int quantity, std::string ticker, OrderType type)
            : trader_(trader), quantity_(quantity), ticker_(ticker), type_(type), order_id_(count_.fetch_add(1))
    {}

    bool operator==(const Order& that) {
        return ( trader_ == that.trader_
                && quantity_  == that.quantity_
                && ticker_ == that.ticker_
                && type_ == that.type_);
    }

    bool isValid()
    {
        //1. ticker must be between "A" and "Z" (both inclusive)
        //2. quantity must be an integer greater than 0
        return quantity_ > 0
               && ticker_.get().size() == 1
               && ticker_.get().at(0) >= 'A'
               && ticker_.get().at(0) <= 'Z';
    }

public:
    std::string trader_;
    int quantity_;
    //std::string ticker_;
    //remove the flyweight, if this makes the code a little slower
    boost::flyweight<std::string> ticker_; ///< using flyweight as there is very small number of tickers as compared to number of orders active at a given time.

    OrderType type_;
    int order_id_;
    
private:
    static std::atomic<int> count_; ///< to set unique order_ids. This is assuming that unique order ids are not needed across server instances.

};


/**
 * \brief A class to maintain all the orders across the system.
 *
 * OrderBook works on the Order structure. Currently, it doesn't care about the price and time of the orders.
 * Matching algorithm is abstracted in MatchingPolicy.
 */

template <typename MatchingPolicy, typename CachingPolicy>
class OrderBook
{
public:
    /***
     * \brief to add new order
     *  \ return false if not a valid order.
     */
    bool addOrder(Order order)
    {
        if (!order.isValid())
            return false;

        if (Order::OrderType::BUY == order.type_) {
            // try to match with sell
            matcher_(std::move(order), buyOrders_, sellOrders_, callback_);
        }
        else {
            matcher_(std::move(order), sellOrders_, buyOrders_, callback_);
        }

        return true;
    }


    /** \brief set a callback which will be executed asynchronously on matching of any order
     *
     * \param callback the callback to call. This should be equivalent to a function returning void and accepting an integer (for order_id)
     */ 
    void setResponseCallback(const ResponseCallbackT& callback) {
        callback_ = callback;
    }

    /// \brief helper function to get a copy of pending buy orders
    /// It doesn't make any changes in the orderbook.
    auto& getBuyOrders() const {
        return buyOrders_;
    }

    /// \brief helper function to get a copy of pending buy orders
    /// It doesn't make any changes in the orderbook.
    auto& getSellOrders() const {
        return sellOrders_;
    } ;



private:
    //OrderBookCache buyOrders_;
    //OrderBookCache sellOrders_;
    CachingPolicy buyOrders_;
    CachingPolicy sellOrders_;

    ResponseCallbackT callback_;
    MatchingPolicy matcher_;    ///< algorithm to match the orders

};


#endif // _MATCHINGENGINE_SRC_ORDERBOOK_H__»
