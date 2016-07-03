/**@file	MatchingEngine/src/OrderUtils.cpp
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Sun Jul  3 14:05:31 2016 \n
 * 	Last Update:	Sun Jul  3 14:05:31 2016
 */

/*===========================================================================*/
/*===============================[ include headers ]===============================*/
/*===========================================================================*/

#include "OrderUtils.h"
#include "OrderBook.h"

/*===========================================================================*/
/*===============================[ include headers ]===============================*/
/*===========================================================================*/


OrderListT orderutils::getBuyOrders(const OrderBook& orderbook) 
{
    return orderbook.buyOrders_.flattenCache();

}

OrderListT orderutils::getSellOrders(const OrderBook& orderbook)
{
    return orderbook.sellOrders_.flattenCache();
}



