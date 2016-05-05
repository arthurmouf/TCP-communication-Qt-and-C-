#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtWidgets>
#include <QDebug>
#include <QtNetwork>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server();

private slots:
    void newConnection();
    void dataReceived();
    void clientDeconnection();

private:
    QLabel *serverState;
    QPushButton *quitButton;

    QTcpServer *server;
    QList<QTcpSocket *> clients;
};

#endif // SERVER_H
