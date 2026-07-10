#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <vector>
#include <algorithm>
#include "orderbook.h"

namespace OB {
    enum e_currency : int;
    class OrderBook;
}

namespace U {
    class Users;
    struct s_user;
}

struct U::s_user {
    int64_t id;
    std::string name;
    int64_t UAH;
    int64_t USD;
    std::vector<int64_t> orders;
 };
 
class U::Users {
    friend class OB::OrderBook;
public:
    //Returns ID of a new user
    int64_t add_user(const std::string&, int64_t UAH = 0, int64_t USD = 0);
    //int64_t add_user(const s_user &user);
    bool user_exists(const int64_t id);
    //CONSIDER USING IT WITH PRICE_SCALE!!! i.e ammount you want to add/remove * orderbook.scale
    bool change_ballance(const int64_t id, OB::e_currency currency, int64_t amount);
    bool delete_user(const int64_t id);
    std::string get_name(const int64_t id);
    std::vector<int64_t> get_orders(const int64_t id);
    int64_t get_balance(const int64_t id, OB::e_currency currency);


private:
    std::mutex mutex;
    std::deque<s_user> users;
    bool add_order(const int64_t id, const int64_t order_id);
    bool remove_order(const int64_t id, const int64_t order_id);
    //SHOULD BE UNDER LOCK!!!!
    s_user* find_user(const int64_t id);
    //SHOULD BE UNDER LOCK!!!!
    std::deque<s_user>::iterator find_user_iterator(const int64_t id);
    int number_of_users = 1;

};