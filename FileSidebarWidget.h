#ifndef FILESIDEBARWIDGET_H
#define FILESIDEBARWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QDir>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QIcon>

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    explicit CustomFileSystemModel(QObject *parent = nullptr) : QFileSystemModel(parent) {}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DecorationRole && index.column() == 0) {
            QFileInfo info = fileInfo(index);
            QIcon icon;
            if (info.isDir()) {
                icon = QIcon(":/images/icons/directory.png");
            } else if (info.isFile()) {
                QString suffix = info.suffix().toLower();
                if (QStringList{"png", "jpg", "jpeg", "gif", "bmp"}.contains(suffix)) {
                    icon = QIcon(":/images/icons/fileImg.png");
                } else if (QStringList{"bin", "o", "exe", "dll", "so"}.contains(suffix)) {
                    icon = QIcon(":/images/icons/fileBin.png");
                } else {
                    icon = QIcon(":/images/icons/fileText.png");
                }
            }
            return icon.pixmap(16, 16);  // Avoid selection-based tinting
        }
        return QFileSystemModel::data(index, role);
    }
};

class FileSidebarWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileSidebarWidget(QWidget *parent = nullptr) : QWidget(parent) {
        fileModel = new CustomFileSystemModel(this);
        fileModel->setRootPath(QDir::currentPath());
        fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

        treeView = new QTreeView(this);
        treeView->setModel(fileModel);
        treeView->setRootIndex(fileModel->index(QDir::currentPath()));

        treeView->setHeaderHidden(true);
        treeView->setColumnHidden(1, true);
        treeView->setColumnHidden(2, true);
        treeView->setColumnHidden(3, true);
        treeView->setAnimated(false);
        treeView->setIndentation(20);
        treeView->setSortingEnabled(false);
        treeView->setFocusPolicy(Qt::NoFocus);

        treeView->setStyleSheet(
            "QTreeView {"
            "    background-color: #21252b;"
            "    border: none;"
            "    color: #d3d3d3;"
            "}"
            "QTreeView::item {"
            "    padding: 2px;"
            "    color: #d3d3d3;"
            "    border: none;"
            "    outline: none;"
            "}"
            "QTreeView::item:selected {"
            "    background-color: #4d78cc;"
            "    color: white;"
            "}"
            );

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(treeView);
        setLayout(layout);

        connect(treeView, &QTreeView::doubleClicked, this, &FileSidebarWidget::onFileSelected);
    }

signals:
    void fileSelected(const QString &filePath);

private slots:
    void onFileSelected(const QModelIndex &index) {
        QString path = fileModel->filePath(index);
        if (QFileInfo(path).isFile()) emit fileSelected(path);
    }

private:
    QTreeView *treeView;
    CustomFileSystemModel *fileModel;
};

#endif // FILESIDEBARWIDGET_H
