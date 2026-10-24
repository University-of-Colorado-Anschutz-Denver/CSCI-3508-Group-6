### Header
```
// tests
    QString currentPath;
    QStringList pathHistory;

    void loadRootFolders();
    void loadDirectory(const QString &path);
```

### Class implementations
```

//tests
void MainWindow::loadRootFolders()
{
    ui->rootListWidget->clear();

    // Use your home directory as the test root.
    QDir home(QDir::homePath());

    QFileIconProvider iconProvider;

    // List folders inside the home directory.
    QFileInfoList folders = home.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
        );

    for (const QFileInfo &folder : folders) {
        auto *item = new QListWidgetItem(
            iconProvider.icon(folder),
            folder.fileName()
            );

        // Store the actual directory path.
        item->setData(Qt::UserRole, folder.absoluteFilePath());

        ui->rootListWidget->addItem(item);
    }

    // Clicking a root folder loads its contents.
    connect(ui->rootListWidget,
            &QListWidget::itemClicked,
            this,
            [this](QListWidgetItem *item) {
                loadDirectory(item->data(Qt::UserRole).toString());
            });
}

void MainWindow::loadDirectory(const QString &path)
{
    QDir dir(path);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << path;
        return;
    }

    // Save the current directory for Back navigation.
    if (!currentPath.isEmpty() && currentPath != path) {
        pathHistory.append(currentPath);
    }

    currentPath = dir.absolutePath();

    ui->contentsListWidget->clear();
    ui->currentPathLabel->setText(currentPath);

    QFileIconProvider iconProvider;

    // Show folders first, then files.
    QFileInfoList entries = dir.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot,
        QDir::DirsFirst | QDir::Name
        );

    for (const QFileInfo &entry : entries) {
        auto *item = new QListWidgetItem(
            iconProvider.icon(entry),
            entry.fileName()
            );

        // Keep the full path attached to the list item.
        item->setData(Qt::UserRole, entry.absoluteFilePath());

        ui->contentsListWidget->addItem(item);
    }

    qDebug() << "Loaded directory:" << currentPath
             << "| Items:" << entries.size();
}

```

### main window Constructor
```
//________________________________________________________________________________________
    // Populate the Root panel and load the initial directory.
    loadRootFolders();
    loadDirectory(QDir::homePath());

    // Double-clicking a folder navigates into it.
    connect(ui->contentsListWidget,
            &QListWidget::itemDoubleClicked,
            this,
            [this](QListWidgetItem *item) {
                QString path = item->data(Qt::UserRole).toString();
                QFileInfo info(path);

                if (info.isDir()) {
                    loadDirectory(path);
                } else {
                    qDebug() << "Selected file:" << path;
                }
            });

    // Back button navigates to the previous directory.
    connect(ui->backButton, &QPushButton::clicked,
            this, [this]() {
                if (!pathHistory.isEmpty()) {
                    QString previousPath = pathHistory.takeLast();

                    // Avoid recording the current path again.
                    currentPath.clear();
                    loadDirectory(previousPath);
                }
            });
    //_________________________________________________________________________________________________________________________
```