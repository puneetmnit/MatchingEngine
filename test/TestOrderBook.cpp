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

#include <algorithm>
#include <chrono>
#include <numeric>
#include <thread>

#include <boost/test/unit_test.hpp>

#include "TestUtils.h"

/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/

BOOST_AUTO_TEST_CASE(test_persist_buy_order)
{
    Order order_A_200S_buy{"A", 200, "S", OrderType::BUY};

    // create order book
    OrderBook ob;

    //add order
    BOOST_REQUIRE_NO_THROW(ob.addOrder(order_A_200S_buy));

    //no sell order
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    
    //check order book size
    auto buy_book = ob.getBuyOrders();
    BOOST_REQUIRE_EQUAL(buy_book.size(), 1);

    //check order book contents
    BOOST_CHECK_EQUAL(true, std::all_of(std::begin(buy_book), std::end(buy_book), [](auto order) { return order == order_A_200S_buy; }));
}

BOOST_AUTO_TEST_CASE(test_persist_sell_order)
{
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL};

    // create order book
    OrderBook ob;

    //add order
    BOOST_REQUIRE_NO_THROW(ob.addOrder(order_A_200S_sell));

    //no buy order
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    
    //check order book size
    auto sell_book = ob.getSellOrders();
    BOOST_REQUIRE_EQUAL(sell_book.size(), 1);

    //check order book contents
    BOOST_REQUIRE_EQUAL(true, std::all_of(std::begin(sell_book), std::end(sell_book), [](auto order) { return order == order_A_200S_sell; }));
}

BOOST_AUTO_TEST_CASE(test_match_buy)
{
    // create order book
    OrderBook ob;

    //add sell order
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL};
    ob.addOrder(order_A_200S_sell);
    Order order_A_200X_sell{"A", 200, "X", OrderType::SELL};
    ob.addOrder(order_A_200X_sell);

    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 1);
    

    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY};
    ob.addOrder(order_B_200S_buy);

    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //check order book size
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 0);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "X"), 1);
    
}

BOOST_AUTO_TEST_CASE(test_match_sell)
{
    // create order book
    OrderBook ob;

    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY};
    ob.addOrder(order_B_200S_buy);
    Order order_B_200X_buy{"B", 200, "X", OrderType::BUY};
    ob.addOrder(order_B_200X_buy);
    
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 1);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
    
    //add sell order
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL};
    ob.addOrder(order_A_200S_sell);


    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //check order book size
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "X"), 1);
 
}

BOOST_AUTO_TEST_CASE(test_match_one_to_many)
{
    // create order book
    OrderBook ob;

    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY};
    ob.addOrder(order_B_200S_buy);
    Order order_C_100S_buy{"C", 100, "S", OrderType::BUY};
    ob.addOrder(order_C_200S_buy);
    Order order_D_300S_buy{"D", 300, "S", OrderType::BUY};
    ob.addOrder(order_D_200S_buy);
    Order order_E_200S_buy{"E", 100, "S", OrderType::BUY};
    ob.addOrder(order_E_200S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    auto total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    auto expected_pending_buy_S = total_buy_S - matched_qty;

    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL};
    ob.addOrder(order_A_200S_sell);


    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //sell order should have been completely matched
    BOOST_CHECK_EQUAL(ob.getSellOrders.size(), 0);

    //remaining buy quantity should be 300
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
    
}

BOOST_AUTO_TEST_CASE(test_partial_match)
{
    // create order book
    OrderBook ob;

    //add buy order
    Order order_B_200S_buy{"B", 1000, "S", OrderType::BUY};
    ob.addOrder(order_B_1000S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    auto total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    auto expected_pending_buy_S = total_buy_S - matched_qty;

    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL};
    ob.addOrder(order_A_200S_sell);


    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //sell order should have been completely matched
    BOOST_CHECK_EQUAL(ob.getSellOrders.size(), 0);

    //remaining buy quantity should be 300
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
    
    //add sell order for more than pending qty, so that buy qty becomes 0
    auto pending_sell_S = 100;
    matched_qty = expected_pending_buy_S + pending_sell_S;

    Order order_C_S_sell{"C", matched_qty, "S", OrderType::SELL};
    ob.addOrder(order_C_S_sell);


    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //buy order should have been completely matched
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);

    //remaining sell quantity should be pending_sell_S
    BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "S"), pending_sell_S);
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
    
    //add buy order 
    Order order_D_B_buy{"D", pending_sell_S, "S", OrderType::BUY};
    ob.addOrder(order_D_B_buy);


    //shouldn't check the order book size here, as the matching is asynchronous, 
    //and hence, can't assume the order book size at this time
    //Ideally, one should wait for the success message (with time-out) and then check the orderbook's size
    //
    //untill the communication mechanism is setup, sleeping for 1 sec
    
    /// \todo : remove sleep and wait for success message.
    std::this_thread.sleep_for(std::chrono::seconds(1));
    
    //sell order should have been completely matched
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 0);
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);

    //no change for X
    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);

}

/**
 * \todo test cases for filling orders on FCFS basis
*/
