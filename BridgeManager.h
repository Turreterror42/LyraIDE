#ifndef BRIDGEMANAGER_H
#define BRIDGEMANAGER_H

#include <QObject>
#include <QString>
#include <QWebEngineView>
#include <QWebChannel>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>

class BridgeManager : public QObject
{
    Q_OBJECT
public:
    explicit BridgeManager(QWebEngineView* view, QObject *parent = nullptr)
        : QObject(parent), m_view(view)
    {
        m_channel = new QWebChannel(this);
        m_channel->registerObject("bridge", this);
        m_view->page()->setWebChannel(m_channel);
    }

signals:
    void textRetrieved(const QString &text);

public slots:
    // Called from C++ to load text into Monaco
    void loadText(const QString &text) {
        // Escape text for JavaScript safely
        QString escaped = QString::fromUtf8(QJsonDocument::fromVariant(QVariant(text)).toJson(QJsonDocument::Compact));
        m_view->page()->runJavaScript("window.editor && window.editor.setValue(" + escaped + ");");
    }

    void setTheme(const QString &theme) {
        m_view->page()->runJavaScript("monaco.editor.setTheme('" + theme + "');");
    }

    void setLanguage(const QString &language) {
        if (!language.isEmpty()) m_view->page()->runJavaScript(QString("if (window.setLanguage) setLanguage('%1');").arg(language));
    }

    void retrieveText() {
        m_view->page()->runJavaScript("window.editor ? window.editor.getValue() : ''", [this](const QVariant &result) {emit textRetrieved(result.toString());});
    }

private:
    QWebEngineView* m_view;
    QWebChannel* m_channel;
};

#endif // BRIDGEMANAGER_H
