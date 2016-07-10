//
// Created by puneet on 10/07/16.
//

#include "OrderBookCache.h"
#include <array>
#include <numeric>
#include "OrderBook.h"

OrderBookCache::OrderBookCache()
{
    initCache();
}

void OrderBookCache::initCache()
{
    // the only valid tickers are "A" to "Z"
    std::array<char,26> chars;
    std::iota(begin(chars), end(chars), 0);

    for(auto c : chars) {
        orders_.emplace(std::string(1,'A'+c),std::make_pair(std::make_unique<std::mutex>(), OrderBookCacheValueT()));
    }
}

OrderListT OrderBookCache::flattenCache() const
{
    OrderListT orders;
    for(const auto& value : orders_) {
        std::lock_guard<std::mutex> guard(*(const_cast<OrderBookCacheT::value_type&>(value).second.first));
        std::copy(value.second.second.begin(), value.second.second.end(), std::back_inserter(orders));
    }
    return orders;
}

void OrderBookCache::insert(Order order)
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
