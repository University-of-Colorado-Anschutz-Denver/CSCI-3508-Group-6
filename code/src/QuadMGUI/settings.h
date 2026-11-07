#pragma once

#include <QWidget>
#include <QStorageInfo>
#include <QSettings>
#include <QDir>

namespace Ui {
class Settings;
}

class Settings : public QWidget {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();


private:
    Ui::Settings *ui;

    void populateDrives();
    void loadSettings();

    signals:
        void settingsSave();

    private slots:
        void saveSettings();
};