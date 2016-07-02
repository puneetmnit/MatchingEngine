/**@file	MatchingEngine/Main.cpp
 * @author	puneet
 * @version	704
 * @date
 * 	Created:	Fri Jul  1 18:57:56 2016 \n
 * 	Last Update:	Fri Jul  1 18:57:56 2016
 */

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/
#include <thread>
#include "OrderBook.h"

/*===========================================================================*/
/*===============================[ includes ]===============================*/
/*===========================================================================*/


int main()
{

    OrderBook ob;
    std::thread startThread(&OrderBook::start, &ob);

    //theoretically the above thread will never return
    startThread.join();

    return 0;
}

