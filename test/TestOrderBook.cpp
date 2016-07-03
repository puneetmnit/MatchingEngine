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
#include <iostream>
#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "TestUtils.h"
#include "../src/OrderUtils.h"

/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/

namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE(test_zero_quantity, bdata::xrange(2))
{
    Order order_A_0S{"A", 0, "S", static_cast<OrderType>(sample), sample};

    // create order book
    OrderBook ob;

    //add order
    BOOST_CHECK_EQUAL(false, ob.addOrder(order_A_0S));

}

BOOST_DATA_TEST_CASE(test_valid_ticker, bdata::xrange(2)*bdata::xrange(26), type, ticker)
{
    auto order_type = static_cast<OrderType>(type);
    std::string symbol(1, 'A' + ticker);
    Order order_A{"A",1,symbol,order_type, ticker};

    OrderBook ob;
    BOOST_CHECK_EQUAL(true, ob.addOrder(order_A));
}

BOOST_AUTO_TEST_CASE(test_persist_buy_order)
{
    Order order_A_200S_buy{"A", 200, "S", OrderType::BUY, 1};

    // create order book
    OrderBook ob;

    //add order
    BOOST_REQUIRE_NO_THROW(ob.addOrder(order_A_200S_buy));

    //no sell order
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
    
    //check order book size
    auto buy_book = orderutils::getBuyOrders(ob);
    BOOST_REQUIRE_EQUAL(buy_book.size(), 1);

    //check order book contents
    BOOST_CHECK_EQUAL(true, std::all_of(std::begin(buy_book), std::end(buy_book), [&order_A_200S_buy](auto order) { return order == order_A_200S_buy; }));
}

BOOST_AUTO_TEST_CASE(test_persist_sell_order)
{
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL, 1};

    // create order book
    OrderBook ob;

    //add order
    BOOST_REQUIRE_NO_THROW(ob.addOrder(order_A_200S_sell));

    //no buy order
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    
    //check order book size
    auto sell_book = orderutils::getSellOrders(ob);
    BOOST_REQUIRE_EQUAL(sell_book.size(), 1);

    //check order book contents
    BOOST_REQUIRE_EQUAL(true, std::all_of(std::begin(sell_book), std::end(sell_book), [&order_A_200S_sell](auto order) { return order == order_A_200S_sell; }));
}

BOOST_AUTO_TEST_CASE(test_match_buy)
{
    // create order book
    OrderBook ob;

    std::vector<int> actual_fills;
    std::vector<int> expected_fills;
    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
                    BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "S"), 0);
                    //BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 0);
                    BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "X"), 200);
                    //BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "X"), 1);
                }
            });

    //add sell order
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL, 1};
    ob.addOrder(order_A_200S_sell);
    Order order_A_200X_sell{"A", 200, "X", OrderType::SELL, 2};
    ob.addOrder(order_A_200X_sell);

    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 1);
    

    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY, 3};
    expected_fills.emplace_back(3);
    expected_fills.emplace_back(1);
    ob.addOrder(order_B_200S_buy);
}

BOOST_AUTO_TEST_CASE(test_match_sell)
{
    // create order book
    OrderBook ob;
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;

    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);
                    //BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);
                    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), 200);
                    //BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "X"), 1);
                }
            });


    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY, 1};
    ob.addOrder(order_B_200S_buy);
    Order order_B_200X_buy{"B", 200, "X", OrderType::BUY, 2};
    ob.addOrder(order_B_200X_buy);
    
    BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 1);
    BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
    
    //add sell order
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL, 3};
    expected_fills.emplace_back(3);
    expected_fills.emplace_back(1);
    ob.addOrder(order_A_200S_sell);
}

BOOST_AUTO_TEST_CASE(test_match_one_to_many)
{
    // create order book
    OrderBook ob;
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;
    int total_buy_X;
    int expected_pending_buy_S;

    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    //sell order should have been completely matched
                    BOOST_REQUIRE_EQUAL(testUtils::getSellOrderSize(ob), 0);

                    //remaining buy quantity should be 300
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
    
                }
            });


    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY, 1};
    ob.addOrder(order_B_200S_buy);
    Order order_C_100S_buy{"C", 100, "S", OrderType::BUY, 2};
    ob.addOrder(order_C_100S_buy);
    Order order_D_300S_buy{"D", 300, "S", OrderType::BUY, 3};
    ob.addOrder(order_D_300S_buy);
    Order order_E_200S_buy{"E", 100, "S", OrderType::BUY, 4};
    ob.addOrder(order_E_200S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY, 5};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    expected_pending_buy_S = total_buy_S - matched_qty;

    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL, 6};
    expected_fills.emplace_back(6);
    expected_fills.emplace_back(1);
    expected_fills.emplace_back(2);
    ob.addOrder(order_A_200S_sell);
}

BOOST_AUTO_TEST_CASE(test_partial_match)
{
    // create order book
    OrderBook ob;
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;

    //add buy order
    Order order_B_1000S_buy{"B", 1000, "S", OrderType::BUY, 1};
    ob.addOrder(order_B_1000S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY, 2};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    auto total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    auto expected_pending_buy_S = total_buy_S - matched_qty;

    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    //sell order should have been completely matched, so not added to cache
                    BOOST_REQUIRE_EQUAL(testUtils::getSellOrderSize(ob), 0);

                    //remaining buy quantity should be 300
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
                
                }
            });


    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL, 3};
    expected_fills.emplace_back(3);
    ob.addOrder(order_A_200S_sell);

   
    //add sell order for more than pending qty, so that buy qty becomes 0
    auto pending_sell_S = 100;
    matched_qty = expected_pending_buy_S + pending_sell_S;

    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    //buy order should have been completely matched
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);
                    //BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);

                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
               
                }
            });


    Order order_C_S_sell{"C", matched_qty, "S", OrderType::SELL, 4};
    expected_fills.emplace_back(1);
    ob.addOrder(order_C_S_sell);

    ob.setResponseCallback( [&](int order_id) { 
                BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
                actual_fills.emplace_back(order_id);

                if (actual_fills.size() == expected_fills.size()) {
                    //sell order should have been completely matched
                    BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "S"), 0);
                    //BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 0);
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);
                    //BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob, "S"), 0);

                    //no change for X
                    BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
                }
            });


    //add buy order 
    Order order_D_B_buy{"D", pending_sell_S, "S", OrderType::BUY, 5};
    expected_fills.emplace_back(4);
    expected_fills.emplace_back(5);
    ob.addOrder(order_D_B_buy);

}

