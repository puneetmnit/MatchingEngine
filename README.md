#Matching Engine
This application will contain an order store and a matching engine. 
The order store is meant to record only the open orders (unmatched orders). 
The matching engine will match the orders based on the interest of buyers and sellers. 
When an order is placed, the matching engine will match the order against the order(s) in the order store and notify the trader if matched.

Assumptions:
1. Price is not considered.
2. Orders can be filled in random order, and not necessarily in FCFS manner
3. No Order IDs
4. No logging

Dependencies:
Clang with c++14 support
cmake : version 3.5 or more
boost : 1.59
UTF : boost unit test framework
