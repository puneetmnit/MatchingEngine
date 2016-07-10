/**@file	MatchingEngine/src/OrderUtils.h
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 13:54:57 2016 \n
 * 	Last Update:	Sun Jul  3 13:54:57 2016
 */
#ifndef _MATCHINGENGINE_SRC_ORDERUTILS_H__
#define _MATCHINGENGINE_SRC_ORDERUTILS_H__

#include <vector>
#include "OrderBook.h"


namespace orderutils {

    /** \brief helper function to get the buy side open orders from a supplied order book.
     *
     * It simply returs a list of Orders. i.e. a c++ std::vector, not sorted in any particular order.
     * These orders are clones of the existing open orders from the given order book.
     */
    template<typename OrderBookT>
    auto getBuyOrders(const OrderBookT& orderbook)
    {
        return orderbook.getBuyOrders().flattenCache();
    }


    /** \brief helper function to get the buy side open orders from a supplied order book.
     *
     * It simply returs a list of Orders. i.e. a c++ std::vector, not sorted in any particular order.
     * These orders are clones of the existing open orders from the given order book.
     */
    template<typename OrderBookT>
    auto getSellOrders( const OrderBookT& orderbook)
    {
        return orderbook.getSellOrders().flattenCache();
    }


}// namespace orderutils

#endif // _MATCHINGENGINE_SRC_ORDERUTILS_H__
