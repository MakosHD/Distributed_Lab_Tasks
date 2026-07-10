#pragma once
#include <QMainWindow>
#include <QSoundEffect>
#include "users.h"
#include "orderbook.h"
#include "ordertablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { 
    class MainWindow; 
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow { 
    Q_OBJECT
public:
    MainWindow(U::Users &users, int64_t user_id, OB::OrderBook &orderbook, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void balance_change_changed();
    void change_ballance_button_clicked();
    void one_of_input_order_edits_changed();
    void place_order();
    void cancel_order();


private:
    Ui::MainWindow *ui;
    U::Users &users;
    OB::OrderBook &orderbook;
    int64_t user_id;
    int64_t current_order_id = -1;
    size_t change_listener_id;
    void update_status_labels();
    QString format_amount(double value);
    QSoundEffect sound;

    OrderTableModel *buy_model = new OrderTableModel(orderbook, OB::tbl_buy, this);
    OrderTableModel *sell_model = new OrderTableModel(orderbook, OB::tbl_sell, this);
    OrderTableModel *completed_model = new OrderTableModel(orderbook, OB::tbl_completed, this); 
};