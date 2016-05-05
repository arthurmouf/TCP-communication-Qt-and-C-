#include "server.h"

Server::Server()
{
    //Creation and disposition of the widget on the window
    serverState = new QLabel;
    quitButton = new QPushButton(tr("Quit"));
    QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(serverState);
    layout->addWidget(quitButton);
    setLayout(layout);

    setWindowTitle(tr("Server"));
    qDebug() << QString::fromUtf8("QTcpServer object instanciation");
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, 6008)) // Connection to any adress on the port 6008
    {
        serverState->setText(tr("Server is not started. Reason :<br />") + server->errorString());
    }
    else
    {
        serverState->setText(tr("Server is listening on port <strong>") + QString::number(server->serverPort()) + tr("</strong>.<br />Client can now connect."));
        QObject::connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }
}

void Server::newConnection()
{
    QTcpSocket *nouveauClient = server->nextPendingConnection();
    clients << nouveauClient;

    QObject::connect(nouveauClient,SIGNAL(readyRead()), this, SLOT(dataReceived()));
    QObject::connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(clientDeconnection()));
}

void Server::clientDeconnection()
{
    //We find out which client is deconnected
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0)
        return;

    clients.removeOne(socket);
    socket->deleteLater();
}

void Server::dataReceived()
{
    qDebug() << "A client sent something..";
    QByteArray message;

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    if (socket == 0)
    {
        return;
    }

    message = socket->readAll();

    //Send the packet to the clients
    for(int i = 0 ; i < clients.size();i++)
        clients[i]->write(message);
}
