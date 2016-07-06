/**@file	/Users/puneet/Documents/technical/projects/MatchingEngine/src/OrderBook.cpp
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 00:49:33 2016 \n
 * 	Last Update:	Sun Jul  3 00:49:33 2016
 */

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/

#include "OrderBook.h"
#include <iostream>
#include <future>

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/

namespace  {

bool isValid(const Order& order) 
{
    //1. ticker must be between "A" and "Z" (both inclusive)
    //2. quantity must be an integer greater than 0
    return order.quantity_ > 0 
        && order.ticker_.get().size() == 1
        && order.ticker_.get().at(0) >= 'A'
        && order.ticker_.get().at(0) <= 'Z';
}

void async_reply(int order_id, const ResponseCallbackT& callback)
{
    std::async(std::launch::async, callback, order_id);
};

void matchAndAdd(Order order, OrderBook::OrderBookCache& myStore, OrderBook::OrderBookCache& otherSide, const ResponseCallbackT& callback)
{
    // 1. find the match
    auto result = otherSide.equal_range(order);
    
    // 2. adjust the quantities
    if (result.first != result.second) {
        //match found
        auto itr = result.first;

        while (itr != result.second && order.quantity_ >= itr->quantity_) {
            // this quantity check is needed, as there might be some orders, which are still waiting to be removed from the cache. 
            // Remember, we don't allow 0 quantity orders in the order book at the first place
            //if (itr->quantity_ > 0) {
                order.quantity_ -= itr->quantity_;
                itr->quantity_ = 0;

                /// \todo : remove 0 quantity orders from cache
                //remove from cache and send the reply, asynchronously
                //async_remove_and_reply(otherSide, itr); ///< \todo
                async_reply(itr->order_id_, callback);
            //}
            ++itr;
        }

        if (itr != result.second && order.quantity_ > 0) {
            //itr->quantity_ must be greater than order.quantity_ at this point
            if (itr->quantity_ <= order.quantity_) {
                throw std::logic_error("Incorrent order matching.");
            }
            itr->quantity_ -= order.quantity_;
            order.quantity_ = 0;
        }
    }

    if (order.quantity_ == 0) {
        //send success message
        async_reply(order.order_id_, callback);
    }
    else {
        myStore.insert(std::move(order));
    } 
    
}

}// namespace 

std::atomic<int> Order::count{0};
OrderBook::OrderBookCache::OrderBookCache()
{
    initCache();
}

void OrderBook::OrderBookCache::initCache()
{
    orders_ = {
        {"A", OrderBookCacheValueT()},
        {"B", OrderBookCacheValueT()},
        {"C", OrderBookCacheValueT()},
        {"D", OrderBookCacheValueT()},
        {"E", OrderBookCacheValueT()},
        {"F", OrderBookCacheValueT()},
        {"G", OrderBookCacheValueT()},
        {"H", OrderBookCacheValueT()},
        {"I", OrderBookCacheValueT()},
        {"J", OrderBookCacheValueT()},
        {"K", OrderBookCacheValueT()},
        {"L", OrderBookCacheValueT()},
        {"M", OrderBookCacheValueT()},
        {"N", OrderBookCacheValueT()},
        {"O", OrderBookCacheValueT()},
        {"P", OrderBookCacheValueT()},
        {"Q", OrderBookCacheValueT()},
        {"R", OrderBookCacheValueT()},
        {"S", OrderBookCacheValueT()},
        {"T", OrderBookCacheValueT()},
        {"U", OrderBookCacheValueT()},
        {"V", OrderBookCacheValueT()},
        {"W", OrderBookCacheValueT()},
        {"X", OrderBookCacheValueT()},
        {"Y", OrderBookCacheValueT()},
        {"Z", OrderBookCacheValueT()}
    };    
}

OrderListT OrderBook::OrderBookCache::flattenCache() const
{
    OrderListT orders;
    for(auto& value : orders_) {
        for_each(value.second.begin(), value.second.end(), [&orders](auto o) {
                orders.emplace_back(std::move(o));
                });
    } 
    return orders;
}

void OrderBook::OrderBookCache::insert(Order order) 
{
    //no lock needed here, as we are never going to update this cache.
    //We'll just be adding the orders to the vector corresponding to this stock.
    auto itr =  orders_.find(order.ticker_);
    if (itr == orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in it all the time.");
    }


    /// \todo : make it thread safe. lock the vector before modifying it
    itr->second.emplace_back(order);
}

std::pair<OrderBookCacheValueT::iterator, OrderBookCacheValueT::iterator> OrderBook::OrderBookCache::equal_range(const Order& order) 
{
    //return the range of the value i.e. vector, for the stock as the key

    auto itr =  orders_.find(order.ticker_);
    if (itr == orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in it all the time.");
    }

    //skip all with 0 quantities
    return std::make_pair(std::find_if_not(itr->second.begin(), itr->second.end(), [](const auto& element){return element.quantity_ == 0;}), itr->second.end());

} 

bool OrderBook::addOrder(Order order)
{
    if (!isValid(order))
        return false;

    if (OrderType::BUY == order.type_) {
        // try to match with sell
        matchAndAdd(std::move(order), buyOrders_, sellOrders_, callback_);
    }
    else {
        matchAndAdd(std::move(order), sellOrders_, buyOrders_, callback_);
    }

    return true;
}


