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

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "TestUtils.h"
#include "../src/OrderUtils.h"

/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/

BOOST_AUTO_TEST_SUITE(TestSingleThreaded)
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE(test_zero_quantity, bdata::xrange(2))
{
    //Order order_A_0S{"A", 0, "S", static_cast<OrderType>(sample), sample};
    Order order_A_0S{"A", 0, "S", static_cast<OrderType>(sample)};

    // create order book
    OrderBook ob;

    //add order
    BOOST_CHECK_EQUAL(false, ob.addOrder(order_A_0S));

}

BOOST_DATA_TEST_CASE(test_valid_ticker, bdata::xrange(2)*bdata::xrange(26), type, ticker)
{
    auto order_type = static_cast<OrderType>(type);
    std::string stock_ticker(1, ticker+'A');
    Order order_A{"A",1,stock_ticker,order_type};

    OrderBook ob;
    BOOST_CHECK_EQUAL(true, ob.addOrder(order_A));
}

BOOST_AUTO_TEST_CASE(test_persist_buy_order)
{
    Order order_A_200S_buy{"A", 200, "S", OrderType::BUY};

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
    Order order_A_200S_sell{"A", 200, "S", OrderType::SELL};

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
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;

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
    expected_fills.push_back(order_B_200S_buy.order_id_);
    expected_fills.push_back(order_A_200S_sell.order_id_);
    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            BOOST_CHECK_EQUAL(testUtils::getBuyOrderSize(ob), 0);
            BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "S"), 0);
            //BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "S"), 0);
            BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "X"), 200);
            //BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob, "X"), 1);
        }
    });
    ob.addOrder(order_B_200S_buy);
}

BOOST_AUTO_TEST_CASE(test_match_sell)
{
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;

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
    expected_fills.push_back(order_A_200S_sell.order_id_);
    expected_fills.push_back(order_B_200S_buy.order_id_);
    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);
            BOOST_CHECK_EQUAL(testUtils::getSellOrderSize(ob), 0);
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), 200);
        }
    });


    ob.addOrder(order_A_200S_sell);
}

BOOST_AUTO_TEST_CASE(test_match_one_to_many)
{
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;
    int total_buy_X;
    int expected_pending_buy_S;


    // create order book
    OrderBook ob;
    //add buy order
    Order order_B_200S_buy{"B", 200, "S", OrderType::BUY};
    ob.addOrder(order_B_200S_buy);
    Order order_C_100S_buy{"C", 100, "S", OrderType::BUY};
    ob.addOrder(order_C_100S_buy);
    Order order_D_300S_buy{"D", 300, "S", OrderType::BUY};
    ob.addOrder(order_D_300S_buy);
    Order order_E_200S_buy{"E", 100, "S", OrderType::BUY};
    ob.addOrder(order_E_200S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    expected_pending_buy_S = total_buy_S - matched_qty;

    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL};
    expected_fills.push_back(order_A_200S_sell.order_id_);
    expected_fills.push_back(order_B_200S_buy.order_id_);
    expected_fills.push_back(order_C_100S_buy.order_id_);
    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob, expected_pending_buy_S, total_buy_X](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            //sell order should have been completely matched
            BOOST_REQUIRE_EQUAL(testUtils::getSellOrderSize(ob), 0);

            //remaining buy quantity should be 300
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);

        }
    });

    ob.addOrder(order_A_200S_sell);
}

BOOST_AUTO_TEST_CASE(test_partial_match)
{
    std::vector<int> actual_fills;
    std::vector<int> expected_fills;

    // create order book
    OrderBook ob;
    //add buy order
    Order order_B_1000S_buy{"B", 1000, "S", OrderType::BUY};
    ob.addOrder(order_B_1000S_buy);
    Order order_B_200X_buy{"B", 400, "X", OrderType::BUY};
    ob.addOrder(order_B_200X_buy);

    auto total_buy_S = testUtils::getTotalBuyQuantity(ob, "S");
    auto total_buy_X = testUtils::getTotalBuyQuantity(ob, "X");
    
    //add sell order
    auto matched_qty = 400;
    auto expected_pending_buy_S = total_buy_S - matched_qty;

    Order order_A_200S_sell{"A", matched_qty, "S", OrderType::SELL};
    expected_fills.push_back(order_A_200S_sell.order_id_);

    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob, expected_pending_buy_S, total_buy_X](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            //sell order should have been completely matched, so not added to cache
            BOOST_REQUIRE_EQUAL(testUtils::getSellOrderSize(ob), 0);

            //remaining buy quantity should be 300
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), expected_pending_buy_S);
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);

        }
    });

    ob.addOrder(order_A_200S_sell);

   
    //add sell order for more than pending qty, so that buy qty becomes 0
    auto pending_sell_S = 100;
    matched_qty = expected_pending_buy_S + pending_sell_S;


    Order order_C_S_sell{"C", matched_qty, "S", OrderType::SELL};
    expected_fills.push_back(order_B_1000S_buy.order_id_);
    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob, total_buy_X](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            //buy order should have been completely matched
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);

            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);

        }
    });

    ob.addOrder(order_C_S_sell);


    //add buy order 
    Order order_D_B_buy{"D", pending_sell_S, "S", OrderType::BUY};
    expected_fills.push_back(order_C_S_sell.order_id_);
    expected_fills.push_back(order_D_B_buy.order_id_);
    ob.setResponseCallback( [&expected_fills, &actual_fills, &ob, expected_pending_buy_S, total_buy_X](int order_id) {
        BOOST_REQUIRE(std::find(expected_fills.begin(), expected_fills.end(), order_id) != expected_fills.end());
        actual_fills.push_back(order_id);

        if (actual_fills.size() == expected_fills.size()) {
            //sell order should have been completely matched
            BOOST_CHECK_EQUAL(testUtils::getTotalSellQuantity(ob, "S"), 0);
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "S"), 0);

            //no change for X
            BOOST_CHECK_EQUAL(testUtils::getTotalBuyQuantity(ob, "X"), total_buy_X);
        }
    });

    ob.addOrder(order_D_B_buy);

}
BOOST_AUTO_TEST_SUITE_END()
