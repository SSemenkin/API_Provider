#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "requestsender.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void stop();

private:
    void browse();
    [[nodiscard]] QStringList getFileContent() const;

    void startSendingRequests();
    void stopSendingRequests();

private:
    Ui::MainWindow *ui;

    QString m_filepath;

    QScopedPointer<RequestSender> m_requestSender {nullptr};
};
#endif // MAINWINDOW_H
