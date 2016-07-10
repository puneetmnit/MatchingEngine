//
// Created by puneet on 09/07/16.
//

#include "SimpleMatcher.h"

void SimpleMatcher::async_reply(int order_id, const ResponseCallbackT& callback)
{
    if (callback) {
        std::lock_guard<std::mutex> guard{replyLock_};
        replies_.emplace_back(callback, order_id);
    }

};

void SimpleMatcher::operator()(Order order, OrderBookCache& myStore, OrderBookCache& otherSide, const ResponseCallbackT& callback)
{
    // 1. find the match
    auto res =  otherSide.orders_.find(order.ticker_);
    if (res == otherSide.orders_.end() ) {
        throw std::runtime_error("Corrupt Order Book Cache. All stock symbols must be present in the cache at all the time.");
    }

    std::unique_lock<std::mutex> guard(*(res->second.first));

    auto& orders = res->second.second;

    // 2. adjust the quantities

    if (!orders.empty()) {
        //match found
        auto itr = orders.begin();

        while (itr != orders.end() //result.second
               && order.quantity_ >= itr->quantity_) {
            // Remember, we don't allow 0 quantity orders in the order book at the first place

            order.quantity_ -= itr->quantity_;
            itr->quantity_ = 0;

            //remove from cache and send the reply, asynchronously
            async_reply(itr->order_id_, callback);
            //remove this node
            orders.erase(itr++);

        }

        if (itr != orders.end()
            && order.quantity_ > 0) {
            if (itr->quantity_ <= order.quantity_) {
                //This should never happen
                throw std::logic_error("Incorrect order matching. All the matchable other side orders must have been matched by this time.");
            }
            itr->quantity_ -= order.quantity_;
            order.quantity_ = 0;
        }
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

SimpleMatcher::~SimpleMatcher()
{
    //wait to all the async-replies to be sent
    for(auto& t : replies_) {
        if (t.joinable()) t.join();
    }
}


