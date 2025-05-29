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
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QApplication>

class NoTintIconDelegate : public QStyledItemDelegate {
public:
    explicit NoTintIconDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        painter->save();

        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, QColor(77, 120, 204, 255));
            painter->setPen(opt.palette.highlightedText().color());
        } else {
            painter->fillRect(opt.rect, QColor(33, 37, 43, 255));
            painter->setPen(opt.palette.text().color());
        }

        QString text = index.data(Qt::DisplayRole).toString();
        QRect textRect = opt.rect.adjusted(20, 0, 0, 0);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, text);

        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(opt.decorationSize);
            painter->drawPixmap(opt.rect.left() + 2, opt.rect.top() + (opt.rect.height() - opt.decorationSize.height()) / 2, pixmap);
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setWidth(size.width() + 20);
        return size;
    }
};

class FileSidebarWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileSidebarWidget(QWidget *parent = nullptr) : QWidget(parent) {
        fileModel = new QFileSystemModel(this);
        fileModel->setRootPath(QDir::currentPath());
        fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

        treeView = new QTreeView(this);
        treeView->setModel(fileModel);
        treeView->setRootIndex(fileModel->index(QDir::currentPath()));
        treeView->setItemDelegate(new NoTintIconDelegate(this));

        treeView->setHeaderHidden(true);
        treeView->setColumnHidden(1, true);
        treeView->setColumnHidden(2, true);
        treeView->setColumnHidden(3, true);
        treeView->setAnimated(false);
        treeView->setIndentation(20);
        treeView->setSortingEnabled(false);
        treeView->setFocusPolicy(Qt::NoFocus);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(treeView);
        setLayout(layout);

        QObject::connect(treeView, &QTreeView::doubleClicked, this, &FileSidebarWidget::onFileSelected);
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
    QFileSystemModel *fileModel;
};

#endif // FILESIDEBARWIDGET_H
