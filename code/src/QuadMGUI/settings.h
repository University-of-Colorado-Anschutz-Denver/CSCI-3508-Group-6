#pragma once
#include <QWidget>

class Settings : public QWidget {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("New Window");
        resize(400, 300);}
};