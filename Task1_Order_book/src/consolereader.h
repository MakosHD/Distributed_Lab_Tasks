#pragma once
#include <QObject>
#include <iostream>
#include <string>

class ConsoleReader : public QObject {
    Q_OBJECT
public:
    ConsoleReader() = default;

public slots:
    void run() {
        std::string name;
        while (true) {
            std::cout << "Enter a name for a new user(it will open a new window with that user): ";
            if (!std::getline(std::cin, name))
                break;

            if (!name.empty())
                emit name_entered(QString::fromStdString(name));
        }
    }
signals:
    void name_entered(const QString& name);
};