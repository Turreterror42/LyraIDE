#ifndef BRIDGEMANAGER_H
#define BRIDGEMANAGER_H

#include <QObject>
#include <QString>
#include <QWebEngineView>
#include <QWebChannel>
#include <QVariant>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QByteArray>
#include <QTextStream>

class BridgeManager : public QObject
{
    Q_OBJECT
public:
    explicit BridgeManager(QWebEngineView* view, QObject *parent = nullptr) : QObject(parent), m_view(view) {
        m_channel = new QWebChannel(this);
        m_channel->registerObject("bridge", this);
        m_view->page()->setWebChannel(m_channel);
    }

signals:
    void textRetrieved(const QString &text);

public slots:
    void loadText(const QString &filePath) {
        QFile file(filePath);
        bool isBinary = false;

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            file.close();
            setReadOnly(false);

            static const QRegularExpression regex("[^\x20-\x7E\x09\x0A\x0D]");
            isBinary = regex.match(fileData).hasMatch();

            if (isBinary) {
                int reply;
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setWindowTitle("Binary File");
                msgBox.setText("The file appears to be binary. Do you want to open it in hexadecimal format?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);

                msgBox.setWindowFlag(Qt::WindowCloseButtonHint, false);
                msgBox.setWindowModality(Qt::ApplicationModal);

                reply = msgBox.exec();

                if (reply == QMessageBox::Yes) {
                    QString hexContent;
                    int byteCount = 0;
                    setReadOnly(true);

                    for (int i = 0; i < fileData.size(); ++i) {
                        hexContent.append(QString("0x%1 ").arg(static_cast<unsigned char>(fileData[i]), 2, 16, QChar('0')).toLower());

                        byteCount++;

                        if (byteCount == 16) {
                            hexContent.append("\n");
                            byteCount = 0;
                        }
                    }

                    if (hexContent.endsWith(" ")) hexContent.chop(1);
                    fileData = hexContent.toUtf8();
                } else {
                    qDebug() << "User chose not to open the binary file.";
                }
            }

            QString escapedText = QString::fromUtf8(fileData);
            escapedText.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'").replace("\n", "\\n").replace("\r", "\\r");
            m_view->page()->runJavaScript(QString("setEditorText('%1');").arg(escapedText));
        } else {
            qDebug() << "Failed to open file:" << filePath;
        }
    }

    void setContent(const QString &content) {
        QString escapedText = content;
        escapedText.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'").replace("\n", "\\n").replace("\r", "\\r");
        m_view->page()->runJavaScript(QString("setEditorText('%1');").arg(escapedText));
    }

    void setTheme(const QString &theme) {
        m_view->page()->runJavaScript(QString("setEditorTheme('%1');").arg(theme));
    }

    void setLanguage(const QString &language) {
        m_view->page()->runJavaScript(QString("setEditorLanguage('%1');").arg(language));
    }

    void setReadOnly(const bool YN) {
        m_view->page()->runJavaScript(QString("setEditorReadOnly(%1);").arg(YN));
    }

    void retrieveText() {
        m_view->page()->runJavaScript("getEditorText();", [=](const QVariant &result) {
            QString text = result.toString();
            emit textRetrieved(text);
        });
    }

    void executeCommand(const QString &command) {
        m_view->page()->runJavaScript(QString("editor.trigger('', '%1', {});").arg(command));
    }

private:
    QWebEngineView* m_view;
    QWebChannel* m_channel;
};

#endif // BRIDGEMANAGER_H
