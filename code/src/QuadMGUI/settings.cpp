#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Settings)
{
    ui->setupUi(this);

    setWindowTitle("QUADM Settings");
    resize(500, 350);

    populateDrives();
    ui->themeComboBox->clear();
    ui->themeComboBox->addItem("Dark", "Dark");
    ui->themeComboBox->addItem("Light", "Light");

    loadSettings();

    connect(ui->saveButton, &QPushButton::clicked,
            this, &Settings::saveSettings);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::populateDrives()
{

    ui->defaultDriveComboBox->clear();
    for (const QStorageInfo &drive : QStorageInfo::mountedVolumes()) {
        QString driveInfo;
        if (drive.isReady()) {
            if (drive.displayName().isEmpty() || drive.displayName() == "Local Disk" || drive.displayName() == drive.rootPath()) {
                driveInfo = QString("%1").arg(drive.rootPath());
            }
            else {
                driveInfo = QString("%1 (%2)").arg(drive.displayName()).arg(drive.rootPath());
            }
            ui->defaultDriveComboBox->addItem(driveInfo, drive.rootPath());
        }
    }
}

void Settings::loadSettings()
{
    QSettings settings("QUADM", "QUADM");

    QString defaultDrive =
        settings.value("defaultDrive", QDir::rootPath()).toString();

    int index = ui->defaultDriveComboBox->findData(defaultDrive);

    if (index != -1)
        ui->defaultDriveComboBox->setCurrentIndex(index);

    QString theme =settings.value("theme", "Dark").toString();

    int themeIndex = ui->themeComboBox->findData(theme);

    if (themeIndex != -1)
        ui->themeComboBox->setCurrentIndex(themeIndex);
}

void Settings::saveSettings()
{
    QSettings settings("QUADM", "QUADM");

    QString selectedDrive = ui->defaultDriveComboBox->currentData().toString();

    
    QString selectedTheme = ui->themeComboBox->currentData().toString();

    settings.setValue("defaultDrive", selectedDrive);
    settings.setValue("theme", selectedTheme);
    settings.sync();

    close();
    emit settingsSave();
}