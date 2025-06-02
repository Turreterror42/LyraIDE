#include "QCodeEditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QFontDatabase>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QFile>

// LineNumberArea

LineNumberArea::LineNumberArea(QMonacoEditor *editor)
    : QWidget(editor), codeEditor(editor) {}

QSize LineNumberArea::sizeHint() const {
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    codeEditor->lineNumberAreaPaintEvent(event);
}

// QMonacoEditor

QMonacoEditor::QMonacoEditor(QWidget *parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &QMonacoEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &QMonacoEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &QMonacoEditor::highlightCurrentLine);

    setContentsMargins(0, 0, 0, 0);
    document()->setDocumentMargin(1);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setupStyle();
}

void QMonacoEditor::setupStyle() {
    QFont font("JetBrains Mono", 11);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.3);
    font.setKerning(true);
    font.setHintingPreference(QFont::PreferFullHinting);
    font.setStyleStrategy(QFont::PreferAntialias);

    setFont(font);
    setWordWrapMode(QTextOption::NoWrap);
    setCursorWidth(2);

    setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: none;
            padding: 0px;
            margin: 0px;
            selection-background-color: #264f78;
            selection-color: white;
        }
    )");
}

int QMonacoEditor::lineNumberAreaWidth() {
    int maxLineCount = qMax(1000000, blockCount());
    int digits = QString::number(maxLineCount).length();
    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void QMonacoEditor::updateLineNumberAreaWidth(int) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QMonacoEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void QMonacoEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QMonacoEditor::highlightCurrentLine() {
    lineNumberArea->update();
    viewport()->update();
}

void QMonacoEditor::paintEvent(QPaintEvent *event) {
    QPlainTextEdit::paintEvent(event);

    QPainter painter(viewport());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    int currentLine = textCursor().blockNumber();
    QColor highlightColor(42, 45, 46);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            if (blockNumber == currentLine) {
                QPen pen(highlightColor);
                pen.setWidth(2);
                painter.setPen(pen);
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(1, top, viewport()->width()-2, fontMetrics().height());
                break;
            }
        }
        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void QMonacoEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(30, 30, 30));

    QTextBlock currentBlock = textCursor().block();
    int currentBlockNumber = currentBlock.blockNumber();

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    QColor highlightColor(30, 30, 30);
    painter.setFont(font());
    painter.setPen(QColor(133, 133, 133));

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            if (blockNumber == currentBlockNumber) {
                painter.fillRect(0, top, lineNumberArea->width() + viewport()->width(), fontMetrics().height(), highlightColor);
            }

            QString number = QString("%1").arg(blockNumber + 1, 4); // ex: "   25"
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight | Qt::AlignVCenter, number + "   ");
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
