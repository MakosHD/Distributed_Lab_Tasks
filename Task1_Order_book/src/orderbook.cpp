#include "orderbook.h"

using namespace OB;

OrderBook::OrderBook(U::Users& users): users(users) {
    worker = std::thread(&OrderBook::worker_thread, this);
};

OrderBook::~OrderBook() {
    stop = true;
    condition_v.notify_all();
    worker.join();
}

int64_t OrderBook::place_order(const int64_t owner_id, const int64_t initial_amount, const int64_t price, const e_order_type side) {
    if (initial_amount <= 0 || price < 0)
        return -1;
    if (side == buy && users.get_balance(owner_id,USD) < ((__int128_t)initial_amount*price) / PRICE_SCALE ) {
        return -1;
    } 
    if (side == sell && users.get_balance(owner_id,UAH) < initial_amount) {
        return -1;
    } 
    int64_t id = -1;
    {
        std::lock_guard<std::mutex> lock(mutex);
        id = add_order_to_system(owner_id, initial_amount, price, side);
    }
    if (id != -1)
        notify_chage();
    return id;
};

int64_t OrderBook::get_owner_id(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return -1;
    return order->owner_id; 
};

std::string OrderBook::get_owner_name(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return std::string{};
    return order->owner_name; 
};

int64_t OrderBook::get_initial_amount(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return -1;
    return order->initial_amount;
}

int64_t OrderBook::get_remaining_amount(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return -1;
    return order->remaining_amount;
}

int64_t OrderBook::get_price(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return -1;
    return order->price;
}

e_order_type OrderBook::get_side(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return buy;
    return order->side;
}

e_order_status OrderBook::get_status(const int64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    s_order* order = find_order(id);
    if (!order)
        return canceled;
    return order->status;
}

bool OrderBook::cancel_order(const int64_t id) {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        s_order* order = find_order(id);
        if (!order)
            return false;
        std::vector<s_order*> *vector;
        switch (order->status) {
        case canceled:
            return false;
        case e_order_status::completed:
            return false;
        case pending:
            if (order->side == buy)
                vector = &buy_orders;
            else
                vector = &sell_orders;
            break;
        default:
            return false;
            break;
        }
        auto order_iterator = find_order_iterator(order, *vector);
        if (order_iterator == vector->end())
            return false;
        vector->erase(order_iterator);
        order->status=canceled;
        completed_orders.push_back(order);
        changed = true;
    }
    if (changed)
        notify_chage();
    return true;
}

bool OrderBook::delete_order(const int64_t id) {
        bool changed = false;
        {
        std::lock_guard<std::mutex> lock(mutex);
        auto order_iterator = find_order_iterator(id);
        if (order_iterator == all_orders.end())
            return false;
        std::vector<s_order*> *vector;
        if (order_iterator->status == canceled || order_iterator->status == e_order_status::completed) {
            vector = &completed_orders;
        } else {
            if (order_iterator->side == buy)
                vector = &buy_orders;
            else
                vector = &sell_orders;
        }
        auto order_iterator_in_vector = find_order_iterator(&(*order_iterator), *vector);
        if (order_iterator_in_vector == vector->end())
            return false;
        vector->erase(order_iterator_in_vector);
        all_orders.erase(order_iterator);
        changed = true;
    }
    if (changed)
        notify_chage();
    return true;
}


std::deque<s_order>::iterator OrderBook::find_order_iterator(const int64_t id) {
    return std::find_if(all_orders.begin(), all_orders.end(), [id](s_order& order) {
        return order.id == id;
    }); 
}

std::vector<s_order*>::iterator OrderBook::find_order_iterator(const s_order* pointer, std::vector<s_order*> &vector) {
    return std::find_if(vector.begin(), vector.end(), [pointer](s_order* order) {
        return order == pointer;
    });     
}

s_order* OrderBook::find_order(const int64_t id) {
    auto it = find_order_iterator(id); 
     if (it == all_orders.end())
        return nullptr;
    return &(*it);
}

int64_t OrderBook::add_order_to_system(const int64_t owner_id, const int64_t initial_amount, const int64_t price, const e_order_type side) {
    auto name = users.get_name(owner_id);
    if (!users.user_exists(owner_id))
        return -1;
    all_orders.emplace_back(s_order{
        .id = number_of_orders++, 
        .owner_id = owner_id, 
        .owner_name = users.get_name(owner_id),
        .initial_amount = initial_amount, 
        .remaining_amount = initial_amount,
        .price = price, 
        .side = side, 
        .status = pending
    });
    s_order &order = all_orders.back();
    if (side == buy) {
        buy_orders.push_back(&order);
        buy_orders_sorted = false;
    }
    else {
        sell_orders.push_back(&order);
        sell_orders_sorted = false;
    }
    condition_v.notify_one();
    return order.id;
}
int64_t OrderBook::get_price_scale() {
    return PRICE_SCALE;
}

void OrderBook::worker_thread() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition_v.wait(lock, [this] {
                if (!buy_orders_sorted || !sell_orders_sorted || stop)
                    return true;
                return false;
            });
            if (stop) {
                condition_v.notify_all();
                return;
            }
            if (!buy_orders_sorted) {
                std::sort(buy_orders.begin(), buy_orders.end(), [](s_order* order_1,s_order* order_2) {
                    if (order_1->price != order_2->price)
                        return order_1->price > order_2->price;
                    return order_1->id < order_2->id; 
                });
                buy_orders_sorted = true;
            }
            if (!sell_orders_sorted) {
                std::sort(sell_orders.begin(), sell_orders.end(), [](s_order* order_1,s_order* order_2) {
                    if (order_1->price != order_2->price)
                        return order_1->price < order_2->price;
                    return order_1->id < order_2->id; 
                });
                sell_orders_sorted = true;
            }
            while (!buy_orders.empty() && !sell_orders.empty()) {
                #ifdef ORDERBOOK_THREAD_WAIT
                mutex.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(ORDERBOOK_THREAD_WAIT));
                mutex.lock();
                #endif
                s_order *best_buy = buy_orders.front();
                s_order *best_sell = sell_orders.front();

                if (best_buy->price < best_sell->price)
                    break;

                int64_t trade_amount = std::min(best_buy->remaining_amount,best_sell->remaining_amount);
                int64_t trade_price = best_buy->price;
                int64_t trade_money = ((__int128_t)trade_amount * trade_price)/ PRICE_SCALE;
                //remove UAH from seller
                users.change_ballance(best_sell->owner_id, UAH, -(trade_amount));
                //remove USD from buyer
                users.change_ballance(best_buy->owner_id, USD, -trade_money);

                //add UAH to buyer
                users.change_ballance(best_buy->owner_id, UAH, trade_amount);

                //add USD to sellet 
                users.change_ballance(best_sell->owner_id, USD, trade_money);


                best_buy->remaining_amount -= trade_amount;
                best_sell->remaining_amount -= trade_amount; 

                if (best_buy->remaining_amount == 0) {
                    best_buy->status = e_order_status::completed;
                    completed_orders.push_back(best_buy);
                    buy_orders.erase(buy_orders.begin());
                }

                if (best_sell->remaining_amount == 0) {
                    best_sell->status = e_order_status::completed;
                    completed_orders.push_back(best_sell);
                    sell_orders.erase(sell_orders.begin());
                }
                #ifdef ORDERBOOK_THREAD_WAIT
                notify_chage();
                #endif
            }

        }
        notify_chage();
    }
}

std::vector<s_order> OrderBook::get_orders_snapshot(e_table_type type) {
    std::lock_guard<std::mutex> lock(mutex);

    const std::vector<s_order*>* source = nullptr;
    switch (type) {
    case e_table_type::tbl_buy:
        source = &buy_orders;
        break;
    case e_table_type::tbl_sell:
        source = &sell_orders;
        break;
    case e_table_type::tbl_completed:
        source = &completed_orders;
        break;
    default:
        source = &buy_orders;
        break;
    }
    
    std::vector<s_order> result;
    result.reserve(source->size());
    for (const s_order* order : *source) {
        result.push_back(s_order{
        .id = order->id,
        .owner_id = order->owner_id, 
        .owner_name = order->owner_name, 
        .initial_amount = order->initial_amount, 
        .remaining_amount = order->remaining_amount, 
        .price = order->price, 
        .side = order->side, 
        .status = order->status,
        });
    }
    return result;
}

size_t OrderBook::add_change_listener(std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(listeners_mutex);
    size_t id = number_of_listeners;
    listeners.emplace_back(number_of_listeners++, std::move(callback));
    return id;
}

void OrderBook::remove_change_listener(size_t id) {
    std::lock_guard<std::mutex> lock(listeners_mutex);
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [id](const auto& it) {return it.first == id;}),listeners.end());
}

void OrderBook::notify_chage() {
    std::lock_guard<std::mutex> lock(listeners_mutex);
    for (auto& [id,cb] :listeners)
        cb();
}