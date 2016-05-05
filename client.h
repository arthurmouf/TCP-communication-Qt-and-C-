#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>
#include <QDialog>

#define PORT_SERVEUR 6008

class Client : public QWidget
{
    Q_OBJECT

public:
    Client();
    ~Client();

    void windowGestion();
	QString formatXML(double X, double Y, double Z);
    QPoint decodeXML(QString frame);
    void moveTo(QPoint robotPosition, QPoint goal);
    bool checkPointValidity(QPoint point);

private :
    //Graphic
    QLabel *clientState;

    QPushButton *sendButton;
    QPushButton *connectButton;
    QPushButton *disconnectButton;

    QVBoxLayout *globalInformation;
    QVBoxLayout *infoServer;
    QVBoxLayout *infoSensor;

    QHBoxLayout *serverAndSensor;
    QHBoxLayout *infoIpServer;
    QHBoxLayout *infoPortServer;
    QHBoxLayout *connectionButtons;
    QHBoxLayout *labelCoord;
    QHBoxLayout *infoPositionSensor;

    QGroupBox *serverBox;
    QGroupBox *sensorBox;

    QLabel *label_coordXYZ;
    QLabel *label_coordX;
    QLabel *label_coordY;
    QLabel *label_coordZ;
    QLabel *label_sensor;
    QLabel *label_ipServer;
    QLabel *label_port;

    QTextEdit *information;
	
    QLineEdit *Xvalue;
    QLineEdit *Yvalue;
    QLineEdit *Zvalue;
    QLineEdit *ipServerValue;
    QLineEdit *portValue;

    QCheckBox *isRobot;
    //other
    QTcpSocket *tcpSocket;
	double infos[3];

private slots :

    void dataReceived();
    void send();
    void checkRobot();
    void connection();
	void disconnectClient();
	void socketError(QAbstractSocket::SocketError error);

};

#endif // CLIENT_H
