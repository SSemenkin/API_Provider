#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>

class RequestSender : public QObject
{
    Q_OBJECT
public:
    explicit RequestSender(const QStringList &data,
                           const QString &url,
                           const QString &json_body,
                           QObject *parent = nullptr);

    virtual ~RequestSender();
    void stopRequests();


signals:
    void progress(float percent);
    void log(const QString &);
    void error(const QString &error);
private:
    void startRequests();

    bool isStopRequested() const;

    QByteArray prepareBody(const QString &args) const;

private:
    QScopedPointer<QNetworkAccessManager> s_manager;

    QStringList m_data;
    QString m_json;
    QNetworkRequest m_request;
    mutable QMutex m_mutex;

    bool m_isStop {false};

};

class CustomThread : public QThread
{
    Q_OBJECT
public:
    explicit CustomThread(QObject *parent = nullptr) : QThread(parent){
        qDebug() << Q_FUNC_INFO;
    }
    ~CustomThread() {
        qDebug() << Q_FUNC_INFO;
    }
};

#endif // REQUESTSENDER_H
