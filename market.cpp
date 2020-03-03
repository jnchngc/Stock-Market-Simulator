// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6
//  p2a.cpp
//  proj2a
//
//  Created by Chung on 2020/2/13.
//  Copyright © 2020 Chung. All rights reserved.
//

#include <stdio.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <queue>
//#include "xcode_redirect.hpp"
#include "P2random.h"
#include <algorithm>
#include <functional>
#include <limits>

using namespace std;

class RunningMedian {
private:
    //Values greater than the median sorted so that smallest is on top
    std::priority_queue<int, std::vector<int>, std::greater<int> > upper;
    //Values smaller than the median sorted so that greatest is on top
    std::priority_queue<int, std::vector<int>, std::less<int> > lower;
    
public:
    RunningMedian(){
        //Seed the queues
        upper.push(std::numeric_limits<int>::max());
        lower.push (std::numeric_limits<int>::min());
    }
    void push(int val){
        //Add number to the property queue
        
        //If number is greater than the least upper number, it is an upper number
        if(val>=upper.top())
            upper.push(val);
        else //Otherwise it is a lower number
            lower.push(val);
        
        //Rebalance
        //If the queues sizes differ by 2, they need to be rebalanced so that they
        //differ by 1.
        if(upper.size()-lower.size()==2){ //Upper queue is larger
            lower.push(upper.top());        //Move value from upper to lower
            upper.pop();                    //Remove same value from upper
        } else if(lower.size()-upper.size()==2){ //Lower queue is larger
            upper.push(lower.top());               //Move value from lower to upper
            lower.pop();                           //Remove same value
        }
    }
    
    int getMedian() const {
        if(upper.size()==lower.size())               //Upper and lower are same size
            return(upper.top()+lower.top())/((int)2.0);  //so median is average of least upper and greatest lower
        else if(upper.size()>lower.size())           //Upper size greater
            return upper.top();
        else                                         //Lower size greater
            return lower.top();
    }
};

enum TimeStatus {NO_TRADES, CAN_BUY, COMPLETED, POTENTIAL};

class TimeTraveler {
private:
    int sell_price = 0;
    int sell_ts = 0;
    
    int buy_price = 0;
    int buy_ts = 0;
    
    int completed_profit = 0;
    
    int potential_sell_price = 0;
    int potential_sell_ts = 0;
    
    
    friend class Stock_market;
};

class Order {
private:
    //int timestamp;
    int t_id;
    //int s_id;
    //char buy_sell;
    int price_limit;
    mutable int quantity;
    
    int sequence;
    
    friend class Stock_market;
    friend class Stock;
public:
    Order(int t_id_in, int pl_in, int q_in, int seq_in) : t_id(t_id_in), price_limit(pl_in), quantity(q_in), sequence(seq_in) {}
    
    void dec_quantity (int transaction_quantity) const {
        quantity -= transaction_quantity;
    }
    int get_price () const {
        return price_limit;
    }
    
    int get_sequence () const {
        return sequence;
    }
};


struct seller_q_comparator { // used when new buyer comes in; look for lowest seller
    bool operator() (const Order& lhs, const Order& rhs) {
        if (lhs.get_price() == rhs.get_price()) {
            return (lhs.get_sequence() > rhs.get_sequence());
        }
        else
            return (lhs.get_price() > rhs.get_price());
    }
};

struct buyer_q_comparator { // used when new seller comes in; look for highest buyer
    bool operator() (const Order& lhs, const Order& rhs) {
        if (lhs.get_price() == rhs.get_price()) {
            return (lhs.get_sequence() > rhs.get_sequence());
        }
        else
            return (lhs.get_price() < rhs.get_price());
    }
};

class Stock {
private:
    priority_queue<Order, vector<Order>, seller_q_comparator> seller_queue;
    priority_queue<Order, vector<Order>, buyer_q_comparator> buyer_queue;
    
    bool traded_on = false;
    
    //int median_price = 0;
    
    //vector<int> transactions;
    
    RunningMedian run_med;
    
    // time traveler stuff
    TimeStatus status = TimeStatus::NO_TRADES;
    TimeTraveler tt;
    
    
    friend class Stock_market;
public:
    /*
    void calculate_median () {
        if (traded_on) {
            if (transactions.size() % 2 == 0) {
                //sort(transactions.begin(), transactions.end());
                median_price = (transactions[transactions.size()/2] + transactions[(transactions.size()/2)-1])/2;
            }
            else {
                //sort(transactions.begin(), transactions.end());
                median_price = transactions[transactions.size()/2];
            }
        }
    }
     */
};

class Trader {
private:
    int num_bought = 0;
    int num_sold = 0;
    int net = 0;
    
    friend class Stock_market;
};

class Stock_market {
private:
    bool verbose_on;
    bool median_on;
    bool trader_info_on;
    bool tt_on;
    string input_mode;
    unsigned int num_traders;
    unsigned int num_stocks;
    vector<Stock> stocks;
    int orders_processed = 0;
    vector<Trader> traders;
    
    stringstream ss;
    // for PR mode
    int seed;
    int num_orders;
    int arrival_rate;
    
public:
    Stock_market() : verbose_on(false), median_on(false), trader_info_on(false), tt_on(false),
    num_traders(0), num_stocks(0) {}
    // call in main
    void get_options(int argc, char** argv) {
        int option_index = 0, option = 0;
        
        // Don't display getopt error messages about options
        opterr = false;
        
        /*
         
         TODO: Add the remaining elements into the longOpts array.
         
         */
        // use getopt to find command line options
        struct option longOpts[] = {{ "verbose", no_argument, nullptr, 'v' },
            { "median", no_argument, nullptr, 'm'},
            { "trader_info", no_argument, nullptr, 'i'},
            { "time_travelers", no_argument, nullptr, 't'},
            { nullptr, 0, nullptr, '\0' }};
        
        /*
         
         TODO: Add the remaining chars to the option string in
         the while loop conditional (currently contains "p:h").
         Options with required_argument (print) need a colon after the
         char, options with no_argument do not (help).
         
         */
        string out_mode;

        while ((option = getopt_long(argc, argv, "vmit", longOpts, &option_index)) != -1) {
            switch (option) {
                case 'v':
                    verbose_on = true;
                    break;
                case 'm':
                    median_on = true;
                    break;
                case 'i':
                    trader_info_on = true;
                    break;
                case 't':
                    tt_on = true;
                    break;
                default:
                    cerr << "Invalid command line argument(s)\n";
                    exit(1);
                    
            }
        }
        
    }
    
    // call in main
    void set_up () {
        string temp;
        getline(cin, temp); // skip past the comment line
        cin >> temp; // skip past "Mode: "
        cin >> input_mode;
        
        cin >> temp; // skip past "NUM_TRADERS:"
        cin >> num_traders;
        cin >> temp; // skip past "NUM_STOCKS:"
        cin >> num_stocks;
        
        if (input_mode == "PR") {
            string temp2;
            cin >> temp2; // skip past "RANDOM_SEED"
            cin >> seed;
            cin >> temp2; // skip past "NUMBER_OF_ORDERS"
            cin >> num_orders;
            cin >> temp2; // skip past "ARRIVAL_RATE"
            cin >> arrival_rate;
            
            P2random::PR_init(ss, (unsigned int)seed, num_traders, num_stocks, (unsigned int)num_orders, (unsigned int)arrival_rate);
        }
        
        //cout << input_mode << " " << num_traders << " " << num_stocks << endl;
        
        stocks = vector<Stock>(num_stocks);
        traders = vector<Trader>(num_traders);
    }
    
    void get_the_int(string&a, string&b, string&c, string&d, int& trader,int& stock,int& price,int& quantity) {
        a.erase(0, 1);
        b.erase(0, 1);
        c.erase(0, 1);
        d.erase(0, 1);
        
        trader = stoi(a);
        stock = stoi(b);
        price = stoi(c);
        quantity = stoi(d);
    }
    
    void print_transaction (int buyer_numer, int seller_num, int shares_purchased, int stock_num, int price) {
        cout << "Trader " << buyer_numer << " purchased " << shares_purchased << " shares of Stock " << stock_num << " from Trader " << seller_num << " for $" << price << "/share\n";
    }
    
    void print_median (int stock_num, int time, int median_price) {
        cout << "Median match price of Stock " << stock_num << " at time " << time << " is $" << median_price << "\n";
    }
    
    void print_trader_info () {
        cout << "---Trader Info---\n";
        for (unsigned int i = 0; i < num_traders; i++) {
            cout << "Trader " << i << " bought " << traders[i].num_bought << " and sold " << traders[i].num_sold << " for a net transfer of $" << traders[i].net << "\n";
        }
    }
    
    void tt_info() {
        cout << "---Time Travelers---\n";
        for (unsigned int i = 0; i < num_stocks; i++) {
            if (stocks[i].status == TimeStatus::COMPLETED || stocks[i].status == TimeStatus::POTENTIAL) {
                cout << "A time traveler would buy shares of Stock " << i << " at time " << stocks[i].tt.sell_ts << " for $" << stocks[i].tt.sell_price << " and sell these shares at time " << stocks[i].tt.buy_ts << " for $" << stocks[i].tt.buy_price << "\n";
            }
            else {
                cout << "A time traveler could not make a profit on shares of Stock " << i << "\n";
            }
        }
    }
    
    bool tradable (int stock_num) {
        if (!stocks[(unsigned int)stock_num].seller_queue.empty() && !stocks[(unsigned int)stock_num].buyer_queue.empty()) {
            if (stocks[(unsigned int)stock_num].buyer_queue.top().price_limit >= stocks[(unsigned int)stock_num].seller_queue.top().price_limit) {
                return true;
            }
        }
        return false;
    }
    
    void make_trade (int stock_num) {
        auto& buy_order = stocks[(unsigned int)stock_num].buyer_queue.top();
        auto& sell_order = stocks[(unsigned int)stock_num].seller_queue.top();
        
        int actual_quantity = (buy_order.quantity < sell_order.quantity) ? buy_order.quantity : sell_order.quantity;
        
        int actual_price = (buy_order.sequence < sell_order.sequence) ? buy_order.price_limit : sell_order.price_limit;
        
        orders_processed++;
        
        if (median_on) {
            // median and trader info stuff
            stocks[(unsigned int)stock_num].traded_on = true;
            //stocks[(unsigned int)stock_num].transactions.push_back(actual_price);
            //////////
            
            stocks[(unsigned int)stock_num].run_med.push(actual_price);
        }
        
        if (trader_info_on) {
            auto& k = traders[(unsigned int)buy_order.t_id];
            k.num_bought += actual_quantity;
            k.net -= actual_quantity*actual_price;
            
            auto& i = traders[(unsigned int)sell_order.t_id];
            i.num_sold += actual_quantity;
            i.net += actual_quantity*actual_price;
        }
        
        // print verbose message
        if (verbose_on) {
            print_transaction(buy_order.t_id, sell_order.t_id, actual_quantity, stock_num, actual_price);
        }
        
        //decrement both orders' quantities
        buy_order.quantity -= actual_quantity;
        sell_order.quantity -= actual_quantity;
        if (buy_order.quantity <= 0) {
            stocks[(unsigned int)stock_num].buyer_queue.pop();
        }
        
        if (sell_order.quantity <= 0)  {
            stocks[(unsigned int)stock_num].seller_queue.pop();
        }
        
    }
    
    
    
    // call in main
    void algorithm () {
        cout << "Processing orders..." << "\n";
        int CURRENT_TIMESTAMP = 0;
        int temp_ts;
        string temp_intent, temp_trader, temp_stock, temp_price, temp_quantity;
        int trader_num, stock_num, price, quantity;
        
        int seq = 0;
        istream & inputStream = (input_mode == "PR") ? ss : cin;
        while (inputStream >> temp_ts >> temp_intent >> temp_trader >> temp_stock >> temp_price >> temp_quantity) {
            get_the_int(temp_trader, temp_stock, temp_price, temp_quantity, trader_num, stock_num, price, quantity);
            // check for timestamp error
            if (temp_ts < 0) {
                cerr << "timestamp is negative\n";
                exit(1);
            }
            if (temp_ts < CURRENT_TIMESTAMP) {
                cerr << "timestamp is decreasing\n";
                exit(1);
            }
            
            // check for trader/stock id errors
            if (trader_num < 0 || (unsigned int)trader_num >= num_traders) {
                cerr << "trader id invald\n";
                exit(1);
            }
            if (stock_num < 0 || (unsigned int)stock_num >= num_stocks) {
                cerr << "stock id invald\n";
                exit(1);
            }
            
            // check for valid price/quantity
            if (price <= 0) {
                cerr << "invalid price\n";
                exit(1);
            }
            
            if (quantity <= 0) {
                cerr << "invalid quantity\n";
                exit(1);
            }
            
            // median stuff
            if (temp_ts != CURRENT_TIMESTAMP) {
                if (median_on) { // do median stuff
                    for (int i = 0; (unsigned int)i < num_stocks; i++) {
                        if (stocks[(unsigned int)i].traded_on) {
                            //stocks[(unsigned int)i].calculate_median();
                            print_median(i, CURRENT_TIMESTAMP, stocks[(unsigned int)i].run_med.getMedian());
                        }
                    }
                }
                CURRENT_TIMESTAMP = temp_ts;
            }
            
            
            if (temp_intent[0] == 'B') { // new buy order
                // time traveler stuff
                if (tt_on) {
                    if (stocks[(unsigned int)stock_num].status == TimeStatus::CAN_BUY) {
                        if (price > stocks[(unsigned int)stock_num].tt.sell_price) {
                            stocks[(unsigned int)stock_num].tt.buy_price = price;
                            stocks[(unsigned int)stock_num].tt.buy_ts = CURRENT_TIMESTAMP;
                            stocks[(unsigned int)stock_num].tt.completed_profit = price - stocks[(unsigned int)stock_num].tt.sell_price;
                            stocks[(unsigned int)stock_num].status = TimeStatus::COMPLETED;
                        }
                    }
                    else if (stocks[(unsigned int)stock_num].status == TimeStatus::COMPLETED) {
                        int new_profit = price - stocks[(unsigned int)stock_num].tt.sell_price;
                        if (new_profit > stocks[(unsigned int)stock_num].tt.completed_profit) {
                            stocks[(unsigned int)stock_num].tt.buy_price = price;
                            stocks[(unsigned int)stock_num].tt.buy_ts = CURRENT_TIMESTAMP;
                            stocks[(unsigned int)stock_num].tt.completed_profit = new_profit;
                        }
                    }
                    else if (stocks[(unsigned int)stock_num].status == TimeStatus::POTENTIAL) {
                        int new_profit = price - stocks[(unsigned int)stock_num].tt.potential_sell_price;
                        if (new_profit > stocks[(unsigned int)stock_num].tt.completed_profit) {
                            stocks[(unsigned int)stock_num].tt.sell_price = stocks[(unsigned int)stock_num].tt.potential_sell_price;
                            stocks[(unsigned int)stock_num].tt.sell_ts = stocks[(unsigned int)stock_num].tt.potential_sell_ts;
                            stocks[(unsigned int)stock_num].tt.buy_price = price;
                            stocks[(unsigned int)stock_num].tt.buy_ts = CURRENT_TIMESTAMP;
                            
                            
                            stocks[(unsigned int)stock_num].tt.completed_profit = new_profit;
                            stocks[(unsigned int)stock_num].status = TimeStatus::COMPLETED;
                        }
                        
                    }
                }
                
                stocks[(unsigned int)stock_num].buyer_queue.push(Order(trader_num, price, quantity, seq));
                
                while(tradable(stock_num)) {
                    make_trade(stock_num);
                }
                
            }
            else if (temp_intent[0] == 'S') { // new sell order
                // time traveler stuff
                if (tt_on) {
                    if (stocks[(unsigned int)stock_num].status == TimeStatus::NO_TRADES) {
                        stocks[(unsigned int)stock_num].status = TimeStatus::CAN_BUY;
                        stocks[(unsigned int)stock_num].tt.sell_price = price;
                        stocks[(unsigned int)stock_num].tt.sell_ts = CURRENT_TIMESTAMP;
                    }
                    else if (stocks[(unsigned int)stock_num].status == TimeStatus::CAN_BUY) {
                        if (price < stocks[(unsigned int)stock_num].tt.sell_price) {
                            stocks[(unsigned int)stock_num].tt.sell_price = price;
                            stocks[(unsigned int)stock_num].tt.sell_ts = CURRENT_TIMESTAMP;
                        }
                    }
                    else if (stocks[(unsigned int)stock_num].status == TimeStatus::COMPLETED) {
                        if (price < stocks[(unsigned int)stock_num].tt.sell_price) {
                            stocks[(unsigned int)stock_num].tt.potential_sell_price = price;
                            stocks[(unsigned int)stock_num].tt.potential_sell_ts = CURRENT_TIMESTAMP;
                            stocks[(unsigned int)stock_num].status = TimeStatus::POTENTIAL;
                        }
                    }
                    else if (stocks[(unsigned int)stock_num].status == TimeStatus::POTENTIAL) {
                        if (price < stocks[(unsigned int)stock_num].tt.potential_sell_price) {
                            stocks[(unsigned int)stock_num].tt.potential_sell_price = price;
                            stocks[(unsigned int)stock_num].tt.potential_sell_ts = CURRENT_TIMESTAMP;
                        }
                    }
                    
                    
                }
                
                stocks[(unsigned int)stock_num].seller_queue.push(Order(trader_num, price, quantity, seq));
                
                while(tradable(stock_num)) {
                    make_trade(stock_num);
                }

            }
            seq++;
        }
        
        
        if (median_on) { // do median stuff
            for (int i = 0; (unsigned int)i < num_stocks; i++) {
                if (stocks[(unsigned int)i].traded_on) {
                    //stocks[(unsigned int)i].calculate_median();
                    print_median(i, CURRENT_TIMESTAMP, stocks[(unsigned int)i].run_med.getMedian());
                }
            }
        }
        
        cout << "---End of Day---" << "\n";
        cout << "Orders Processed: " << orders_processed << "\n";
        
        if (trader_info_on) {
            print_trader_info();
        }
        
        if (tt_on) {
            tt_info();
        }
    }
    
    
    
};

int main(int argc, char** argv) {
    //xcode_redirect(argc, argv);
    // the ios thing
    std::ios_base::sync_with_stdio(false);
    Stock_market test;
    test.get_options(argc, argv);
    test.set_up();
    test.algorithm();
    return 0;
}
