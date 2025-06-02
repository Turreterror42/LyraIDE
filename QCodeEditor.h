#ifndef QCODEEDITOR_H
#define QCODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPlainTextEdit>
#include <QWidget>

class QMonacoEditor;

class LineNumberArea : public QWidget {
public:
    LineNumberArea(QMonacoEditor *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMonacoEditor *codeEditor;
};

class QMonacoEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    QMonacoEditor(QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();
    void setupStyle();
private:
    LineNumberArea *lineNumberArea;
};

#endif // QCODEEDITOR_H
