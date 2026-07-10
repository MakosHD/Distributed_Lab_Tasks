#include "ordertablemodel.h"

OrderTableModel::OrderTableModel(OB::OrderBook& orderbook,
    OB::e_table_type type,
    QObject* parent
) : QAbstractTableModel(parent), orderbook(orderbook), type(type) {
    listener_id = orderbook.add_change_listener([this]() {
        QMetaObject::invokeMethod(this, &OrderTableModel::refresh, Qt::QueuedConnection);
        });
    average_value = 0;
    refresh();
};

OrderTableModel::~OrderTableModel() {
    orderbook.remove_change_listener(listener_id);
};

int OrderTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(orders.size());
}

int OrderTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return Col_Count;
}

QVariant OrderTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};
    if (index.row() < 0 || index.row() >= static_cast<int>(orders.size()))
        return {};
    
    const OB::s_order& order = orders[index.row()];

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()){
        case Col_Owner:
            return QVariant(Qt::AlignLeft || Qt::AlignVCenter);
        default:
            return QVariant(Qt::AlignRight || Qt::AlignVCenter);
        }
    }
    if (role != Qt::DisplayRole)
        return {};
    const double scale = static_cast<double>(orderbook.get_price_scale());

    switch (index.column()) {
    case Col_Owner:
        return QString::fromStdString(order.owner_name);
    case Col_InitialAmount:
        return order.initial_amount / scale;
    case Col_RemainingAmount:
        return order.remaining_amount / scale;
    case Col_Price:
        return order.price / scale;
    case Col_Side:
        return order.side == OB::buy ? tr("Buy") : tr("Sell");
    case Col_Status:
        switch (order.status) {
        case OB::e_order_status::pending:
            return tr("Pending");
        case OB::e_order_status::canceled:
            return tr("Canceled");
        case OB::e_order_status::completed:
            return tr("Completed");
        }
        return {};
    default:
        return {};
    }
};

QVariant OrderTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QAbstractTableModel::headerData(section, orientation, role);
    switch (section) {
    case Col_Owner:
        return tr("Owner");
    case Col_InitialAmount:
        return tr("Initial");
    case Col_RemainingAmount:
        return tr("Remaining");
    case Col_Price:
        return tr("Price");
    case Col_Side:
        return tr("Side");
    case Col_Status:
        return tr("Status");
    default:
        return {};
    }
}
double OrderTableModel::get_average_value() {
    return average_value;
}

void OrderTableModel::refresh() {
    beginResetModel();
    orders = orderbook.get_orders_snapshot(type);
    double sum_of_all_prices = 0;
    if (type == OB::e_table_type::tbl_completed) {
        endResetModel();
        return;
    }
    int64_t price_scale = orderbook.get_price_scale();
    for (auto &order: orders) {
        sum_of_all_prices += double(order.price) / price_scale;
    }
    average_value = sum_of_all_prices / orders.size();
    endResetModel();
}