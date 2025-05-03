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

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            fileContent = in.readAll();
            file.close();
        } else {
            qDebug() << "Failed to open file:" << filePath;
            fileContent = "";
        }

        QString sanitizedContent = fileContent;
        sanitizedContent.replace("\\", "\\\\").replace("\"", "\\\"").replace("'", "\\'").replace("\n", "\\n").replace("\r", "\\r");
        const QString js = QString("if (window.editor) window.editor.setValue('%1');").arg(sanitizedContent);
        m_view->page()->runJavaScript(js);
    }

    void setTheme(const QString &theme) {
        const QString js = QString("monaco.editor.setTheme('%1');").arg(theme);
        m_view->page()->runJavaScript(js);
    }

    void setLanguage(const QString &language) {
        const QString js = QString("if (window.setLanguage) setLanguage('%1');").arg(language);
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
