#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QScopedPointer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->browseButton, &QPushButton::clicked, this, &MainWindow::browse);
    connect(ui->sendRequestsButton, &QPushButton::clicked, this, &MainWindow::startSendingRequests);
    connect(ui->stopRequestsButton, &QPushButton::clicked, this, &MainWindow::stopSendingRequests);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::browse()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Select file", m_filepath, "CSV, delimeter \";\"(*.csv)");

    if (!filepath.isEmpty()) {
        m_filepath = filepath;
        ui->path->setText(m_filepath);
    }
}

QStringList
MainWindow::getFileContent() const
{
    QFile file(m_filepath);
    if (!file.exists()) {
        ui->logs->append("File " + m_filepath + " not exists.");
        return QStringList();
    }

    if (!file.open(QIODevice::ReadOnly)) {
        ui->logs->append("File " + m_filepath + " cannot be open.\n" + file.errorString());
        return QStringList();
    }

    QStringList result = QString(file.readAll()).split("\r\n", Qt::SkipEmptyParts);
    file.close();
    return result;
}

void
MainWindow::startSendingRequests()
{
    static QFile logFile("log.txt");
    ui->logs->setText("");
    ui->errors->setText("");
    m_requestSender.reset(new RequestSender(getFileContent(), ui->url->text(), ui->body->toPlainText()));
    connect(this, &MainWindow::stop, m_requestSender.data(), &RequestSender::stopRequests, Qt::DirectConnection);
    connect(m_requestSender.data(), &RequestSender::progress, ui->progressBar, &QProgressBar::setValue);
    connect(m_requestSender.data(), &RequestSender::log, ui->logs, &QTextBrowser::append);
    connect(m_requestSender.data(), &RequestSender::error, ui->errors, &QTextBrowser::append);
    connect(m_requestSender.data(), &RequestSender::finished, this, &MainWindow::showSuccessPercent);
    ui->stopRequestsButton->setEnabled(true);

    if (not logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        ui->errors->append("Cannot open log file" + logFile.errorString());
    } else {
        auto write_str = [](QString s) -> void {
            logFile.write(s.replace("<br>", "\n").toUtf8());
            logFile.flush();
        };
        connect(m_requestSender.data(), &RequestSender::log, this, write_str);
        connect(m_requestSender.data(), &RequestSender::error, this, write_str);
    }
}

void
MainWindow::stopSendingRequests()
{
    emit stop();
}

void
MainWindow::showSuccessPercent(float value)
{
    QMessageBox::information(this, "Success Percent", QString("Success percent is %1").arg(value));
}
