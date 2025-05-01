#ifndef FILESIDEBARWIDGET_H
#define FILESIDEBARWIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QDir>
#include <QHeaderView>

class FileSidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileSidebarWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Initialize the file system model
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::currentPath());
        fileModel->setFilter(QDir::AllEntries | QDir::NoDot);

        // Initialize the tree view
        treeView = new QTreeView(this);
        treeView->setModel(fileModel);
        treeView->setRootIndex(fileModel->index(QDir::currentPath()));
        
        // Configure tree view appearance
        treeView->setHeaderHidden(true);
        treeView->setColumnHidden(1, true); // Hide Size
        treeView->setColumnHidden(2, true); // Hide Type
        treeView->setColumnHidden(3, true); // Hide Date Modified
        treeView->setAnimated(true);
        treeView->setIndentation(20);
        treeView->setSortingEnabled(true);

        // Set up layout
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(treeView);
        setLayout(layout);
    }

    ~FileSidebarWidget() = default;

private:
    QTreeView *treeView;
    QFileSystemModel *fileModel;
};

#endif // FILESIDEBARWIDGET_H