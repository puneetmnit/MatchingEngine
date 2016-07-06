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
#include "../src/OrderBook.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/timer/timer.hpp>

#include "TestUtils.h"
#include "../src/OrderUtils.h"


/*===========================================================================*/
/*===============================[ include ]===============================*/
/*===========================================================================*/

BOOST_AUTO_TEST_SUITE(TestConcurrent)
using namespace testUtils;
namespace   {
void addOrders(OrderBook& ob, OrderListT::iterator itr_begin, OrderListT::iterator itr_end)
{
    std::for_each(itr_begin, itr_end, [&ob](auto order){
                ob.addOrder(order);
            } );
}  

}// namespace  

namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE(test_add_orders, bdata::xrange(2))
{
    std::cout << "TEST: test_add_orders:\n";
    constexpr const unsigned num_orders(1.000000);
    const unsigned num_threads = 1;//std::thread::hardware_concurrency();
    std::cout << "threads: " << num_threads << std::endl;
    const unsigned partition_size = num_orders / num_threads;

    OrderListT orders;
    orders.reserve(num_orders);
    
    //create and add orders in a vector
    testUtils::Random rand_gen;
    for(unsigned i=0; i<num_orders; ++i) {
        orders.emplace_back(rand_gen.getRandomString(1), rand_gen.getRandomInt(), rand_gen.getRandomTicker(),static_cast<OrderType>(sample));
    }
    std::cout << "num of orders: " << orders.size()<< std::endl;

    //find total quantity to compare later
    auto exp_total_qty = getQuantity(orders);

    //add iterators to use by different thread in a vector
    std::vector<OrderListT::iterator> iterators;
    iterators.reserve(num_threads+1);

    auto begin = orders.begin();
    iterators.push_back(begin);
    while(distance(begin, orders.end()) >= partition_size) {
        advance(begin, partition_size);
        iterators.push_back(begin);
    };
    iterators.push_back(orders.end());


    // create order book
    OrderBook ob;

    //add orders via multiple threads
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    auto start_itr = iterators[0];

    { 
        boost::timer::auto_cpu_timer timer;
        for (size_t i=1; i<iterators.size(); ++i) {
            threads.emplace_back(addOrders, std::ref(ob), start_itr, iterators[i]);
            start_itr = iterators[i];
        }

        //wait for completion
        std::for_each(threads.begin(), threads.end(), [](auto& t) {
                t.join();
            });
    }

    //check the total number of open orders
    auto actual_total_qty = testUtils::getTotalBuyQuantity(ob) + testUtils::getTotalSellQuantity(ob);

    BOOST_REQUIRE_EQUAL(actual_total_qty, exp_total_qty);

}

BOOST_AUTO_TEST_CASE(test_match_orders)
{
    std::cout << "TEST: test_match_orders:\n";

    constexpr const unsigned num_orders{1000000};
    const unsigned num_threads = 1;//std::thread::hardware_concurrency();
    std::cout << "threads: " << num_threads << std::endl;
    const unsigned partition_size = num_orders / num_threads;

    OrderListT orders;
    orders.reserve(num_orders);
    
    //create and add orders in a vector
    testUtils::Random rand_gen;
    for(unsigned i=0; i<num_orders; ++i) {
        orders.emplace_back(rand_gen.getRandomString(1), rand_gen.getRandomInt(), rand_gen.getRandomTicker(),static_cast<OrderType>(rand_gen.getRandomInt(0,1)));
    }
    std::cout << "num of orders: " << orders.size()<< std::endl;

    //caculating buy-sell
    //find total quantity to compare later
    std::map<std::string, int> exp_total_qty;
    for(auto& order : orders) {
        auto res = exp_total_qty.find(order.ticker_);
        if (res != exp_total_qty.end()) {
            res->second += (OrderType::BUY == order.type_ ? order.quantity_ : (-1 * order.quantity_));
        }
        else {  //not found
            exp_total_qty[order.ticker_] = (OrderType::BUY == order.type_ ? order.quantity_ : (-1 * order.quantity_));
        } 
    } 

    //add iterators to use by different threads in a vector
    std::vector<OrderListT::iterator> iterators;
    iterators.reserve(num_threads+1);

    auto begin = orders.begin();
    iterators.push_back(begin);
    auto count = partition_size;
    while(distance(begin, orders.end()) > partition_size) {
        count += partition_size;
        advance(begin, partition_size);
        iterators.push_back(begin);
    };
    iterators.push_back(orders.end());

    // create order book
    OrderBook ob;

    //add orders via multiple threads
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    auto start_itr = iterators[0];

    { 
        boost::timer::auto_cpu_timer timer;
        for (size_t i=1; i<iterators.size(); ++i) {
            threads.emplace_back(addOrders, std::ref(ob), start_itr, iterators[i]);
            start_itr = iterators[i];
        }

        //wait for completion
        std::for_each(threads.begin(), threads.end(), [](auto& t) {
                t.join();
            });
    }

    //find the open position
    auto actual_total_qty = testUtils::getOpenPosition(ob);

    BOOST_REQUIRE(testUtils::map_equals(exp_total_qty, actual_total_qty));


}
BOOST_AUTO_TEST_SUITE_END()
