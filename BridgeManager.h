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
        QString fileContent;
        bool isBinary = false;

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            file.close();
            setReadOnly(false);

            static const QRegularExpression regex("[^\x20-\x7E\x09\x0A\x0D]");
            isBinary = regex.match(fileData).hasMatch();

            if (isBinary) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(nullptr, "Binary File", "The file appears to be binary. Do you want to open it in hexadecimal format?", QMessageBox::Yes | QMessageBox::No);

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

            QString sanitizedContent = QString("%1").arg(fileData);
            sanitizedContent.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'").replace("\n", "\\n").replace("\r", "\\r");
            const QString js = QString("if (window.editor) window.editor.setValue('%1');").arg(sanitizedContent);
            m_view->page()->runJavaScript(js);

        } else {
            qDebug() << "Failed to open file:" << filePath;
            fileContent = "";
        }
    }

    void setTheme(const QString &theme) {
        const QString js = QString("monaco.editor.setTheme('%1');").arg(theme);
        m_view->page()->runJavaScript(js);
    }

    void setLanguage(const QString &language) {
        const QString js = QString("if (window.setLanguage) setLanguage('%1');").arg(language);
        m_view->page()->runJavaScript(js);
    }

    void setReadOnly(const bool YN) {
        const QString js = QString("if (window.editor) window.editor.updateOptions({ readOnly: %1 });").arg(YN);
        m_view->page()->runJavaScript(js);
    }

    void retrieveText() {
        m_view->page()->runJavaScript("window.editor ? window.editor.getValue() : ''", [this](const QVariant &result) {emit textRetrieved(result.toString());});
    }

private:
    QWebEngineView* m_view;
    QWebChannel* m_channel;
};

#endif // BRIDGEMANAGER_H
