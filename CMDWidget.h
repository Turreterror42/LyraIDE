#ifndef CMDWIDGET_H
#define CMDWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProcess>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QScrollBar>
#include <QPalette>
#include <QRegularExpression>
#include <QMouseEvent>

class CMDWidget : public QTextEdit {
    Q_OBJECT

public:
    CMDWidget(QWidget *parent = nullptr) : QTextEdit(parent) {
        // Set fixed-width font for terminal-like appearance
        setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        setUndoRedoEnabled(false);
        setAcceptRichText(false);
        setCursorWidth(2);

        // Initialize QProcess for cmd.exe
        process = new QProcess(this);
        process->setProgram("cmd.exe");
        process->setProcessChannelMode(QProcess::MergedChannels);

        // Set environment to use UTF-8 encoding
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process->setProcessEnvironment(env);

        // Start cmd.exe and switch to UTF-8 codepage
        process->start();
        process->write("chcp 65001\n"); // Set console to UTF-8 to avoid encoding issues

        // Connect process output to readOutput slot
        connect(process, &QProcess::readyRead, this, &CMDWidget::readOutput);
    }

    void setDarkTheme(bool dark) {
        if (dark) {
            setStyleSheet("QTextEdit { background-color: #1e1e1e; color: white; }");
        } else {
            setStyleSheet("QTextEdit { background-color: white; color: black; }");
        }
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        // Prevent editing or moving cursor before the prompt
        if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Left) {
            QTextCursor cursor = textCursor();
            if (cursor.position() <= lastPromptPosition) {
                return; // Block backspace or left arrow before prompt
            }
        }

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            QString command = getCurrentCommand();
            // Clear the current command line
            QTextCursor cursor = textCursor();
            cursor.setPosition(lastPromptPosition);
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            // Send command to process
            process->write((command + "\n").toUtf8());
            lastPromptPosition = document()->characterCount() - 1; // Update prompt position
            ensureCursorVisible();
            return;
        }

        QTextEdit::keyPressEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        QTextCursor cursor = cursorForPosition(event->pos());
        if (cursor.position() < lastPromptPosition) {
            // Move cursor to the end if click is before the prompt
            moveCursor(QTextCursor::End);
        } else {
            QTextEdit::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        QTextCursor cursor = cursorForPosition(event->pos());
        if (cursor.position() < lastPromptPosition) {
            // Prevent cursor movement before the prompt
            return;
        }
        QTextEdit::mouseMoveEvent(event);
    }

private slots:
    void readOutput() {
        // Read and decode process output as UTF-8
        QByteArray data = process->readAll();
        QString text = QString::fromUtf8(data);
        moveCursor(QTextCursor::End);
        insertPlainText(text);
        lastPromptPosition = document()->characterCount() - 1; // Update prompt position
        ensureCursorVisible();
    }

private:
    QProcess *process;
    int lastPromptPosition = 0;

    QString getCurrentCommand() {
        QTextDocument *doc = document();
        QTextCursor promptCursor(doc);
        promptCursor.setPosition(lastPromptPosition);
        promptCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        QString command = promptCursor.selectedText();
        return command.trimmed(); // Remove whitespace or newline
    }
};

#endif //CMDWIDGET_H
