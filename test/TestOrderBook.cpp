/**@file	MatchingEngine/test/TestOrderBook.cpp
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Fri Jul  1 21:16:02 2016 \n
 * 	Last Update:	Fri Jul  1 21:16:02 2016
 */

/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/
#define BOOST_TEST_MODULE TestOrderBook
#include "../src/OrderBook.h"
#include <boost/test/unit_test.hpp>

/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/

BOOST_AUTO_TEST_CASE(test_start)
{
    OrderBook ob;
    BOOST_CHECK_NO_THROW(ob.start());
}

