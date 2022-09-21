#include "requestsender.h"
#include <QEventLoop>

RequestSender::RequestSender(const QStringList &data,
                             const QString &url,
                             const QString &json,
                             QObject *parent)
    : QObject{parent}
    , m_data(data)
    , m_json(json.trimmed())
    , m_request(QUrl(url))
{
    m_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    moveToThread(new CustomThread(this));
    s_manager.reset(new QNetworkAccessManager);
    s_manager->moveToThread(thread());
    connect(thread(), &QThread::started, this, &RequestSender::startRequests);
    thread()->start();
}

RequestSender::~RequestSender()
{
    if (!thread()->isFinished()) {
        thread()->quit();
        thread()->wait();
    }
}


void RequestSender::startRequests()
{
    QEventLoop loop;
    connect(s_manager.data(), &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    for (int i = 0; i < m_data.size(); ++i) {
        m_mutex.lock();
        if (isStopRequested()) {
            thread()->quit();
            break;
        }
        m_mutex.unlock();
        QByteArray body = prepareBody(m_data.at(i));
        QNetworkReply *reply = s_manager->post(m_request, body);
        loop.exec();
        QString answer = reply->readAll();
        void (RequestSender::*signal)(const QString &) = &RequestSender::log;

        if (answer.toLower() != "{\"status\":\"ok\"}") {
            answer = "<font color = \"red\">" + answer + "<font>";
            signal = &RequestSender::error;
        }
        emit (this->*signal)(QString("Post to : %1\nBody : %2").arg(m_request.url().toString(), body));
        emit (this->*signal)(answer.toUtf8() + "<br>");
        emit progress(static_cast<float>(i + 1) / static_cast<float>(m_data.size()) * 100.0f);
    }
}

void RequestSender::stopRequests()
{
    QMutexLocker locker(&m_mutex);
    m_isStop = true;
}

bool RequestSender::isStopRequested() const
{
    return m_isStop;
}

QByteArray RequestSender::prepareBody(const QString &arg) const
{
    QStringList args = arg.split(';', Qt::SkipEmptyParts);
    QByteArray result;
    switch (args.size()) {
    case 1:
        result = m_json.arg(args.first()).toUtf8();
        break;
    case 2:
        result = m_json.arg(args.first(), args.at(1)).toUtf8();
        break;
    case 3:
        result = m_json.arg(args.first(), args.at(1), args.at(2)).toUtf8();
        break;
    case 4:
        result = m_json.arg(args.first(), args.at(1), args.at(2), args.at(3)).toUtf8();
        break;
    case 5:
        result = m_json.arg(args.first(), args.at(1), args.at(2), args.at(3), args.at(4)).toUtf8();
        break;
    case 6:
        result = m_json.arg(args.first(), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5)).toUtf8();
        break;
    case 7:
        result = m_json.arg(args.first(), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(6)).toUtf8();
        break;
    default:
        break;
    }
    return result;
}
