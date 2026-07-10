#include <iostream>
#include <QApplication>
#include <QThread>
#include <QLoggingCategory>
#include "users.h"
#include "orderbook.h"
#include "MainWindow.h"
#include "consolereader.h"

int main(int argc, char** argv) {   
    #ifdef ORDERBOOK_THREAD_WAIT
    std::cout << "WARNING: Hey! You build this program with ORDERBOOK_THREAD_WAIT macro. It means that each orders will be processed with " << ORDERBOOK_THREAD_WAIT << " seconds delay" << std::endl;
    #endif
    QLoggingCategory::setFilterRules(
        "qt.multimedia.ffmpeg=false"
    );
    QApplication app(argc, argv);

    U::Users users;
    OB::OrderBook orderbook(users);

    // int64_t user_id = users.add_user("Toothless", 200000000, 200000000);
    // int64_t user_id2 = users.add_user("Stormfly", 200000000, 200000000);

    auto create_window = [&](const QString& name) {

        int64_t user_id = users.add_user(name.toStdString());

        MainWindow* window = new MainWindow(users, user_id, orderbook);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
    };

    QThread console_thread;
    ConsoleReader reader;
    reader.moveToThread(&console_thread);

    QObject::connect(&reader, &ConsoleReader::name_entered, &app, create_window);

    QObject::connect(&console_thread, &QThread::started, &reader, &ConsoleReader::run);
    //QObject::connect(&app, &QApplication::aboutToQuit, [](){std::exit(0);});
    console_thread.start();

    int exit_code = app.exec();

    console_thread.quit();
    console_thread.wait();

    return exit_code;
}
