#pragma once
#include <QAbstractTableModel>
#include <vector>
#include "orderbook.h"

class OrderTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column {
        Col_Owner,
        Col_InitialAmount,
        Col_RemainingAmount,
        Col_Price,
        Col_Side,
        Col_Status,
        Col_Count
    };

    explicit OrderTableModel(OB::OrderBook& orderbook,OB::e_table_type type, QObject* parent = nullptr);
    ~OrderTableModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;
    double get_average_value();
public slots:
    void refresh();

private:
    OB::OrderBook& orderbook;
    OB::e_table_type type;
    double average_value;
    std::vector<OB::s_order> orders;
    size_t listener_id;
};
