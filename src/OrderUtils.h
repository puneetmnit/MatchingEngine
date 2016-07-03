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

class OrderBook;
struct Order;

using OrderListT = std::vector<Order> ;

namespace orderutils {

    /** \brief helper function to get the buy side open orders from a supplied order book.
     *
     * It simply returs a list of Orders. i.e. a c++ std::vector, not sorted in any particular order.
     * These orders are clones of the existing open orders from the given order book.
     */
    OrderListT getBuyOrders(const OrderBook& orderbook) ;

    /** \brief helper function to get the buy side open orders from a supplied order book.
     *
     * It simply returs a list of Orders. i.e. a c++ std::vector, not sorted in any particular order.
     * These orders are clones of the existing open orders from the given order book.
     */
    OrderListT getSellOrders( const OrderBook& orderbook) ;


}// namespace orderutils

#endif // _MATCHINGENGINE_SRC_ORDERUTILS_H__
