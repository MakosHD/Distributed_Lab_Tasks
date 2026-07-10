#include "users.h"

using namespace U;

int64_t Users::add_user(const std::string& name, int64_t UAH, int64_t USD) {
    std::lock_guard<std::mutex> lock(mutex);
    users.emplace_back(s_user{number_of_users++,name,UAH,USD});
    return users.back().id;
}

// int64_t Users::add_user(const s_user &user) {
//     std::lock_guard<std::mutex> lock(mutex);
//     users.emplace_back(std::move(user));
//     return users.back().id;
// }

bool Users::user_exists(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = find_user_iterator(id);
    if (it == users.end())
        return false;
    return true;
}

bool Users::delete_user(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = find_user_iterator(id);
    if (it == users.end())
        return false;

    if (!it->orders.empty())
        return false;

    users.erase(it);
    return true;
}

bool Users::change_ballance(const int64_t id, OB::e_currency currency, int64_t amount) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return false;
    if (currency == OB::e_currency::UAH) {
        user->UAH += amount;
        return true;
    } else if  (currency == OB::e_currency::USD) {
        user->USD += amount;
        return true;
    }
    return false;
}

bool Users::add_order(const int64_t id, const int64_t order_id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return false;
    user->orders.push_back(order_id);
    return true;
}

bool Users::remove_order(const int64_t id, const int64_t order_id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return false;
    auto it = std::find_if(user->orders.begin(), user->orders.end(), [order_id](const int64_t id) {
        return id == order_id;
    }); 
    if (it == user->orders.end())
        return false;
    user->orders.erase(it);
    return true;
}

std::string Users::get_name(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return "NOT_FOUND";
    return user->name;
}

std::vector<int64_t> Users::get_orders(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return std::vector<int64_t>{};
    return user->orders;
}

 int64_t Users::get_balance(const int64_t id, OB::e_currency currency) {
    std::lock_guard<std::mutex> lock(mutex);
    s_user* user = find_user(id);
    if (!user)
        return 0;
    if (currency == OB::e_currency::UAH) {
        return user->UAH;
    } else if (currency == OB::e_currency::USD) {
        return user->USD;
    }
    return 0;
}

std::deque<s_user>::iterator Users::find_user_iterator(const int64_t id) {
    return std::find_if(users.begin(), users.end(), [id](s_user& user) {
        return user.id == id;
    }); 
}

s_user* Users::find_user(int64_t id) {
    auto it = find_user_iterator(id); 
     if (it == users.end())
        return nullptr;
    return &(*it);
}