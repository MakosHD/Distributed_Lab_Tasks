#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(U::Users &users, int64_t user_id, OB::OrderBook &orderbook, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), users(users), user_id(user_id), orderbook(orderbook), sound(this) {
        ui->setupUi(this);

        this->setWindowTitle(QString("Orderbook of a little cove on Berk (") + QString::fromStdString(users.get_name(user_id)) + ")");

        ui->tableView_buy->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_sell->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_completed->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_buy->setModel(buy_model);
        ui->tableView_sell->setModel(sell_model);
        ui->tableView_completed->setModel(completed_model);

        ui->lineEdit_change_balance->setValidator(new QDoubleValidator(-9999,9999,6));
        ui->lineEdit_transaction_uah->setValidator(new QDoubleValidator(-9999,9999,6));
        ui->lineEdit_transaction_usd->setValidator(new QDoubleValidator(-9999,9999,6));

        ui->label_order_is_on_market->hide();
        ui->pushButton_cancel_order->hide();

        connect(ui->lineEdit_change_balance,&QLineEdit::textChanged, this, &MainWindow::balance_change_changed);
        connect(ui->pushButton_change_balance,&QPushButton::clicked, this, &MainWindow::change_ballance_button_clicked);
        connect(ui->comboBox_transcation, &QComboBox::currentIndexChanged, this, &MainWindow::one_of_input_order_edits_changed);
        connect(ui->lineEdit_transaction_uah, &QLineEdit::textChanged, this, &MainWindow::one_of_input_order_edits_changed);
        connect(ui->lineEdit_transaction_usd, &QLineEdit::textChanged, this, &MainWindow::one_of_input_order_edits_changed);
        connect(ui->pushButton_transaction,&QPushButton::clicked, this, &MainWindow::place_order);
        connect(ui->pushButton_cancel_order,&QPushButton::clicked, this, &MainWindow::cancel_order);

        change_listener_id = orderbook.add_change_listener([this]() {
            QMetaObject::invokeMethod(this, &MainWindow::update_status_labels, Qt::QueuedConnection);
        });
        update_status_labels();

        orderbook.place_order(user_id,10000000, 24000, OB::e_order_type::buy);

    }

MainWindow::~MainWindow() {
    orderbook.remove_change_listener(change_listener_id);
    delete ui;
}

QString MainWindow::format_amount(double value) {
    QString string = QString::number(value, 'f', 6);

    while (string.contains('.') && string.endsWith('0')) {
        string.chop(1);

        if (string.endsWith('.'))
            string.chop(1);
    }
    
    return string;
}

void MainWindow::update_status_labels() {
    const int64_t scale = orderbook.get_price_scale();
    const double uah = double(users.get_balance(user_id, OB::UAH)) / scale;
    const double usd = double(users.get_balance(user_id, OB::USD)) / scale;
    ui->label_balance_UAH->setText(format_amount(uah) + " UAH");
    ui->label_balance_USD->setText(format_amount(usd) + " USD");

    if (current_order_id == -1) {
        ui->label_your_order_completed->hide();
    } else if (orderbook.get_status(current_order_id) == OB::e_order_status::completed) {
        ui->label_your_order_completed->setText("Congratulations! Your last order was finished!");
        ui->label_your_order_completed->show();
    } else if (orderbook.get_status(current_order_id) == OB::e_order_status::canceled) {
        ui->label_your_order_completed->setText("Awwwrrrr, but okay, I understand, not everyone wants to take risks");
        ui->label_your_order_completed->show();
    } else {
        ui->label_your_order_completed->hide();
    }
    ui->label_buy_sell_average->setText(QString("average: ") + format_amount(buy_model->get_average_value()) + " / " + format_amount(sell_model->get_average_value()));

    if (orderbook.get_status(current_order_id) != OB::e_order_status::pending) {
        ui->_LABEL_I_want_to->show();
        ui->comboBox_transcation->show();
        ui->lineEdit_transaction_uah->show();
        ui->_LABEL_UAH->show();
        ui->_LABEL_at->show();
        ui->lineEdit_transaction_usd->show();
        ui->_LABEL_USD->show();
        ui->_LABEL_per_UAH->show();
        ui->pushButton_transaction->show();
        ui->label_order_is_on_market->hide();
        ui->pushButton_cancel_order->hide();
    }
}

void MainWindow::balance_change_changed() {
    if (ui->lineEdit_change_balance->text().isEmpty()) {
        ui->pushButton_change_balance->setDisabled(true);
    } else {
        ui->pushButton_change_balance->setDisabled(false);
    }
}

void MainWindow::change_ballance_button_clicked() {
    if (ui->comboBox_change_balance->currentText() == "UAH") {
        users.change_ballance(user_id,OB::e_currency::UAH, ui->lineEdit_change_balance->text().toDouble() * orderbook.get_price_scale());
    } else {
        users.change_ballance(user_id,OB::e_currency::USD, ui->lineEdit_change_balance->text().toDouble() * orderbook.get_price_scale());
    }
    ui->lineEdit_change_balance->clear();
    update_status_labels();
    one_of_input_order_edits_changed();
}

void MainWindow::one_of_input_order_edits_changed() {
    auto uah_text = ui->lineEdit_transaction_uah->text();
    auto usd_text = ui->lineEdit_transaction_usd->text(); 
    if (uah_text.toDouble() < 0) {
        sound.setSource(QUrl("qrc:/sounds/Talking Tom Nuh uh.wav"));
        sound.setVolume(1);
        sound.play();
        ui->lineEdit_transaction_uah->setText(uah_text.remove(0,1));
    }

    if (usd_text.toDouble() < 0) {
        sound.setSource(QUrl("qrc:/sounds/Talking Tom Nuh uh.wav"));
        sound.setVolume(0.5);
        sound.play();
        ui->lineEdit_transaction_usd->setText(usd_text.remove(0,1));
    }

    if (uah_text.isEmpty() || usd_text.isEmpty()) {
        ui->pushButton_transaction->setDisabled(true);
        return;
    }
    if (uah_text.toDouble() == 0 || usd_text.toDouble() == 0) {
        ui->pushButton_transaction->setDisabled(true);
        return;
    }

    

    int64_t initial_amount = uah_text.toDouble() * orderbook.get_price_scale();
    int64_t price = usd_text.toDouble() * orderbook.get_price_scale();
    // std::cout << "START!" << std::endl;
    // std::cout << initial_amount << std::endl;
    // std::cout << price << std::endl;
    // std::cout <<  users.get_balance(user_id,OB::e_currency::USD) << std::endl;
    // std::cout <<  users.get_balance(user_id,OB::e_currency::UAH) << std::endl;
    if (ui->comboBox_transcation->currentText() == "BUY" && users.get_balance(user_id,OB::e_currency::USD) < ((__int128_t)initial_amount*price) / orderbook.get_price_scale() )  {
        ui->pushButton_transaction->setDisabled(true);
        return;
    } else if (ui->comboBox_transcation->currentText() == "SELL" && users.get_balance(user_id, OB::e_currency::UAH) < initial_amount) {
        ui->pushButton_transaction->setDisabled(true);
        return;
    }
    ui->pushButton_transaction->setDisabled(false);
}

void MainWindow::place_order() {
    auto uah_text = ui->lineEdit_transaction_uah->text();
    auto usd_text = ui->lineEdit_transaction_usd->text(); 
    int64_t initial_amount = uah_text.toDouble() * orderbook.get_price_scale();
    int64_t price = usd_text.toDouble() * orderbook.get_price_scale();
    current_order_id = orderbook.place_order(user_id,initial_amount, price, ui->comboBox_transcation->currentText() == "BUY" ? OB::e_order_type::buy : OB::e_order_type::sell);
    if (current_order_id == -1) {
        one_of_input_order_edits_changed();
        return;
    }
    ui->_LABEL_I_want_to->hide();
    ui->comboBox_transcation->hide();
    ui->lineEdit_transaction_uah->hide();
    ui->_LABEL_UAH->hide();
    ui->_LABEL_at->hide();
    ui->lineEdit_transaction_usd->hide();
    ui->_LABEL_USD->hide();
    ui->_LABEL_per_UAH->hide();
    ui->pushButton_transaction->hide();
    ui->label_order_is_on_market->show();
    ui->pushButton_cancel_order->show();
} 

void MainWindow::cancel_order() {
    if (!orderbook.cancel_order(current_order_id))
        return;
    ui->_LABEL_I_want_to->show();
    ui->comboBox_transcation->show();
    ui->lineEdit_transaction_uah->show();
    ui->_LABEL_UAH->show();
    ui->_LABEL_at->show();
    ui->lineEdit_transaction_usd->show();
    ui->_LABEL_USD->show();
    ui->_LABEL_per_UAH->show();
    ui->pushButton_transaction->show();
    ui->label_order_is_on_market->hide();
    ui->pushButton_cancel_order->hide();
    
}