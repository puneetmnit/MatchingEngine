//
// Created by puneet on 09/07/16.
//

#ifndef MATCHINGENGINE_SIMPLEMATCHER_H
#define MATCHINGENGINE_SIMPLEMATCHER_H

#include <thread>
#include <vector>

#include "OrderBook.h"


/**
 * \brief A simple FCFS matcher. Because of the FCFS policy, it might be not be as performant as some random matcher.
 */
class SimpleMatcher {
public:
    /**
     * \brief the core function to do the orders' matching
     *
     *  It tries to match the new order with all the existing orders in FCFS fashion.
     *  If any order get completely filled, it removes it from the cache, and then call the callback for each such match
     *  If the new order doesn't get fully filled, the unmatched quantity gets added to the cache.
     *
     * \param order in Order to match
     * \param myStore in cache where order should go
     * \param otherSide in the other side cache, where it tries to find the matches
     * \callback in callback to call on matching
     */
    template<typename CacheT>
    void operator()(Order order, CacheT& myStore, CacheT& otherSide, const ResponseCallbackT& callback)
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

    ~SimpleMatcher();

private:
    void async_reply(int order_id, const ResponseCallbackT& callback);

private:
    std::vector<std::thread> replies_; ///< to track all the replies sent
    std::mutex replyLock_;



};


#endif //MATCHINGENGINE_SIMPLEMATCHER_H
