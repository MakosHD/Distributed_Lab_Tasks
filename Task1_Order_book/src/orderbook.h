#pragma once
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <atomic>
#include "users.h"
#include <functional>

namespace OB {
    class OrderBook;
    struct s_order;
    enum e_order_type : int;
    enum e_currency : int;
    enum e_order_status : int;
    enum e_table_type : int;
};

namespace U {
    class Users;
    struct s_user;
}
enum OB::e_table_type : int {
    tbl_buy,
    tbl_sell,
    tbl_completed
};

enum OB::e_order_type : int {
    buy,
    sell
};

enum OB::e_currency : int{
    UAH,
    USD
};

enum OB::e_order_status : int {
    pending,
    completed,
    canceled
};

struct OB::s_order {
    int64_t id;
    int64_t owner_id;
    std::string owner_name;
    int64_t initial_amount; //amount of UAH we want to buy/sell WITH SCALE!
    int64_t remaining_amount; //amount of UAH we still want ot buy/sell WITH SCALE!
    int64_t price; // the amount of USD we want to give/get per UAH WITH SCALE!
    e_order_type side; // buy or sell
    e_order_status status;
};

class OB::OrderBook {
public:
    OrderBook(U::Users& users);
    ~OrderBook();
    //returns ID of order (-1 means you are a failure! >:) )
    int64_t place_order(const int64_t owner_id, const int64_t initial_amount, const int64_t price, const e_order_type side);
    int64_t get_owner_id(const int64_t id);
    std::string get_owner_name(const int64_t id);
    int64_t get_initial_amount(const int64_t id);
    int64_t get_remaining_amount(const int64_t id);
    int64_t get_price(const int64_t id);
    e_order_type get_side(const int64_t id);
    e_order_status get_status(const int64_t id);
    bool cancel_order(const int64_t id);
    bool delete_order(const int64_t id);    
    int64_t get_price_scale();
    std::vector<s_order> get_orders_snapshot(e_table_type type);
    size_t add_change_listener(std::function<void()> callback);
    void remove_change_listener(size_t id);


private:

    std::mutex mutex;
    std::mutex listeners_mutex;
    std::condition_variable condition_v;

    //SHOULD BE UNDER LOCK!!!!
    s_order* find_order(const int64_t id);

    //SHOULD BE UNDER LOCK!!!!
    std::deque<s_order>::iterator find_order_iterator(const int64_t id);

    //SHOULD BE UNDER LOCK!!!!
    std::vector<s_order*>::iterator find_order_iterator(const s_order* pointer, std::vector<s_order*> &vector);

    //SHOULD BE UNDER LOCK!!!!
    //returns id of order
    int64_t add_order_to_system(const int64_t owner_id, const int64_t initial_amount, const int64_t price, const e_order_type side);
    U::Users& users;
    std::deque<s_order> all_orders;
    std::vector<s_order*> buy_orders;
    bool buy_orders_sorted = true;
    std::vector<s_order*> sell_orders;
    bool sell_orders_sorted = true;
    std::vector<s_order*> completed_orders;
    int number_of_orders = 0;

    std::thread worker;
    std::atomic<bool> stop = false;
    void worker_thread();

    int64_t PRICE_SCALE = 1000000;
    
    std::vector<std::pair<size_t,std::function<void()>>> listeners;
    int number_of_listeners = 0;

    void notify_chage();
};