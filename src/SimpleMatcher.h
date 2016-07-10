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
    void operator()(Order order, OrderBookCache& myStore, OrderBookCache& otherSide, const ResponseCallbackT& callback);

    ~SimpleMatcher();

private:
    void async_reply(int order_id, const ResponseCallbackT& callback);

private:
    //OrderBookCache buyOrders_;
    //OrderBookCache sellOrders_;

    std::vector<std::thread> replies_; ///< to track all the replies sent
    std::mutex replyLock_;



};


#endif //MATCHINGENGINE_SIMPLEMATCHER_H
