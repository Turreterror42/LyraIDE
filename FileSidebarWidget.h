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

class FileSidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileSidebarWidget(QWidget *parent = nullptr) : QWidget(parent) {
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::currentPath());
        fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

        treeView = new QTreeView(this);
        treeView->setModel(fileModel);
        treeView->setRootIndex(fileModel->index(QDir::currentPath()));

        treeView->setHeaderHidden(true);
        treeView->setColumnHidden(1, true);
        treeView->setColumnHidden(2, true);
        treeView->setColumnHidden(3, true);
        treeView->setAnimated(true);
        treeView->setIndentation(20);
        treeView->setSortingEnabled(true);

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
        QFileInfo info(path);
        if (info.isFile()) {
            emit fileSelected(path);
        }
    }

private:
    QTreeView *treeView;
    QFileSystemModel *fileModel;
};

#endif // FILESIDEBARWIDGET_H
