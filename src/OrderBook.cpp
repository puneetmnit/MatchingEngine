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
#include <numeric>

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
    //auto result = otherSide.equal_range(order);
    
    auto res =  otherSide.orders_.find(order.ticker_);
    if (res == otherSide.orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in the cache at all the time.");
    }

    std::unique_lock<std::mutex> guard(*(res->second.first));
    
    auto result{std::make_pair(res->second.second.begin(), res->second.second.end())};
    // 2. adjust the quantities
    if (result.first != result.second) {
        //match found
        auto itr = result.first;

        while (itr != result.second && order.quantity_ >= itr->quantity_) {
            // Remember, we don't allow 0 quantity orders in the order book at the first place
            //while (otherSide.locked_element_ == &*itr) {
                //this element is locked at the moment. wait for it to get unlocked
                //std::this_thread::yield();
            //}
            // this quantity check is needed, as there might be some orders, which are still waiting to be removed from the cache. 
            //otherSide.locked_element_.exchange(&*itr);
            if (itr->quantity_ > 0) {
                order.quantity_ -= itr->quantity_;
                itr->quantity_ = 0;

                /// \todo : remove 0 quantity orders from cache
                //remove from cache and send the reply, asynchronously
                //async_remove_and_reply(otherSide, itr); ///< \todo
                async_reply(itr->order_id_, callback);
                //remove this node
                itr = res->second.second.erase(itr);
            }
            //otherSide.locked_element_.exchange(nullptr);
            else {
                ++itr;
            } 
        }

        //while (otherSide.locked_element_ == &*itr) {
            //this element is locked at the moment. wait for it to get unlocked
            //std::this_thread::yield();
        //}
        //otherSide.locked_element_.exchange(&*itr);
        if (itr != result.second && order.quantity_ > 0) {
            //itr->quantity_ must be greater than order.quantity_ at this point
            if (itr->quantity_ <= order.quantity_) {
                throw std::logic_error("Incorrent order matching.");
            }
            itr->quantity_ -= order.quantity_;
            order.quantity_ = 0;
        }
        //otherSide.locked_element_.exchange(nullptr);
    }
    guard.unlock();

    if (order.quantity_ == 0) {
        //send success message
        async_reply(order.order_id_, callback);
    }
    else {
        //adding to the end of the list, so no lock needed
        myStore.insert(std::move(order));
    } 
    
}

}// namespace 

std::atomic<int> Order::count{0};
OrderBook::OrderBookCache::OrderBookCache() 
    //: locked_element_(nullptr)
{
    initCache();
}

void OrderBook::OrderBookCache::initCache()
{
    std::array<char,26> chars;
    std::iota(begin(chars), end(chars), 0);

    for(auto c : chars) {
        orders_.emplace(std::string(1,'A'+c),std::make_pair(std::make_unique<std::mutex>(), OrderBookCacheValueT()));
    }
}

OrderListT OrderBook::OrderBookCache::flattenCache() const
{
    OrderListT orders;
    for(auto& value : orders_) {
        std::lock_guard<std::mutex> guard(*(const_cast<OrderBookCacheT::value_type&>(value).second.first));
        for_each(value.second.second.begin(), value.second.second.end(), [&orders](auto o) {
                orders.emplace_back(std::move(o));
                });
    } 
    return orders;
}

void OrderBook::OrderBookCache::insert(Order order) 
{
    //no lock needed here, as we are never going to update this cache.
    //We'll just be adding the orders to the list corresponding to this ticker.
    auto itr =  orders_.find(order.ticker_);
    if (itr == orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in cache at all the time.");
    }

    std::lock_guard<std::mutex> guard(*(itr->second.first));
    itr->second.second.emplace_back(order);
}

std::pair<OrderBookCacheValueT::iterator, OrderBookCacheValueT::iterator> OrderBook::OrderBookCache::equal_range(const Order& order) 
{
    //return the range of the value i.e. vector, for the stock as the key

    auto itr =  orders_.find(order.ticker_);
    if (itr == orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in the cache at all the time.");
    }

    return std::make_pair(itr->second.second.begin(), itr->second.second.end());

    //skip all with 0 quantities
    //return std::make_pair(std::find_if_not(itr->second.begin(), itr->second.end(), [](const auto& element){return element.quantity_ == 0;}), itr->second.end());

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


