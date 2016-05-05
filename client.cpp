#include <vector>
#include <cmath>
#include "client.h"

using namespace std;

Client::Client()
{
	tcpSocket = new QTcpSocket(this);
    qDebug() << QString::fromUtf8("QTcpSocket object instanciation");
    
    windowGestion();

    //Signals
    QObject::connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));
    QObject::connect(connectButton, SIGNAL(clicked()), this, SLOT(connection()));
	QObject::connect(disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectClient()));
	QObject::connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    QObject::connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    QObject::connect(isRobot, SIGNAL(stateChanged(int)), this, SLOT(checkRobot()));

    setWindowTitle(tr("Client sensor"));
    
	for (unsigned int i = 0 ; i < 3 ; i++)
       infos[i] = 0;
}
Client::~Client()
{
    qDebug() << QString::fromUtf8("close the socket");
    tcpSocket->close();
}

void Client::connection()
{
	information->append(tr("<em> Try to connect... </em>"));
	connectButton->setEnabled(false);
	
    // we cancel previous connection
    tcpSocket->abort();
    qDebug() << QString::fromUtf8("Connection to server");
    tcpSocket->connectToHost(ipServerValue->text(), portValue->text().toInt());
    information->append(tr("<em> Connected </em>"));
	disconnectButton->setEnabled(true);
}

void Client::disconnectClient()
{
	tcpSocket->disconnectFromHost();	
	information->append(tr("<em> Disconnected </em>"));
	connectButton->setEnabled(true);
    //disconnectButton->setEnabled(false);
}

void Client::dataReceived()
{
    QByteArray message;
    QPoint sensorPosition;
    QString frame = "";
	QString correction = "";
	QString nbCycle = "";
	
    if (tcpSocket->bytesAvailable() < 0)
        return;
    message = tcpSocket->readAll();
	

    frame.append(message);
    if(!isRobot->isChecked())
        information->append(frame);
    else
    {
        QPoint goal(Xvalue->text().toInt(),Yvalue->text().toInt());
        sensorPosition =  decodeXML(frame);
        moveTo(sensorPosition,goal); // Here, we know the information about cycle and correction X/Y

        correction.append("Correction : X = ");
        correction.append(QString::number(infos[1], 'f', 2));
        correction.append(" ; Y = ");
        correction.append(QString::number(infos[2], 'f', 2));
        information->append(correction);

        nbCycle.append("Number of cycle(s) : ");
        nbCycle.append(QString::number(infos[0], 'f', 0));
        information->append(nbCycle);

        qDebug() << QString::fromUtf8("Data received from server");
        qDebug() << QString::fromUtf8("Bytes received : ") << message.size();
        qDebug() << QString::fromUtf8("Message from server : ") << message;
    }

}

void Client::send()
{
    QString XMLframe = "";
    QByteArray message;
	double positionX, positionY, positionZ;

    if(isRobot->isChecked())
    {
        information->append(tr("Robot can't send data to the sensor. Please wait for the data from the sensor."));
    }
    else
    {
        information->append(tr("<em> Sending...</em>"));
        positionX = Xvalue->text().toDouble();
        positionY = Yvalue->text().toDouble();
        positionZ = Zvalue->text().toDouble();
        qDebug() << "encode data";
        XMLframe = formatXML(positionX,positionY,positionZ);

        message.append(XMLframe);

        tcpSocket->write(message);
    }
}

void Client::checkRobot()
{
    if (isRobot->isChecked())
    {
        setWindowTitle(tr("Client robot"));
        label_sensor->setText(tr("Goal position :"));
        Xvalue->setText("100");
        Yvalue->setText("200");
    }
    else
        label_sensor->setText(tr("Sensor position :"));

}

void Client::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            information->append(tr("<em>ERROR : Can't find the server. Check the Ip and the port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            information->append(tr("<em>ERROR : The server refused the connection. Check if the server is on. Also, check the IP and the port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            information->append(tr("<em>ERROR : The server closed the connection.</em>"));
            break;
        default:
            information->append(tr("<em>ERROR : ") + tcpSocket->errorString() + tr("</em>"));
    }
}


void Client::windowGestion()
{
    globalInformation = new QVBoxLayout;
    infoServer = new QVBoxLayout;
    infoSensor = new QVBoxLayout;

    serverAndSensor = new QHBoxLayout;
    infoIpServer = new QHBoxLayout;
    infoPortServer = new QHBoxLayout;
    connectionButtons = new QHBoxLayout;
    labelCoord = new QHBoxLayout;
    infoPositionSensor = new QHBoxLayout;

    serverBox = new QGroupBox(tr("Server connection"));
    sensorBox = new QGroupBox(tr("Positions"));

    label_coordXYZ = new QLabel(tr("Coordinates :"));
    label_coordX = new QLabel(tr("X"));
    label_coordY = new QLabel(tr("Y"));
    label_coordZ = new QLabel(tr("Z"));
    label_sensor = new QLabel(tr("Sensor position :"));
    label_ipServer = new QLabel(tr("Server IP :"));
    label_port = new QLabel(tr("Port :"));
    clientState = new QLabel;//

    sendButton = new QPushButton(tr("Send"));
    connectButton = new QPushButton(tr("Connect"));
    disconnectButton = new QPushButton(tr("Disconnect"));
	disconnectButton->setEnabled(false);

    information = new QTextEdit;
    Xvalue = new QLineEdit(tr("0"));
    Yvalue = new QLineEdit(tr("0"));
    Zvalue = new QLineEdit(tr("0"));
    ipServerValue = new QLineEdit(tr("127.0.0.1"));
    portValue = new QLineEdit(tr("6008"));

    isRobot = new QCheckBox(tr("Robot"));

    //Part "information about server"
    infoIpServer->addWidget(label_ipServer);
    infoIpServer->addWidget(ipServerValue);
    infoPortServer->addWidget(label_port);
    infoPortServer->addWidget(portValue);

    connectionButtons->addWidget(connectButton);
    connectionButtons->addWidget(disconnectButton);

    infoServer->addLayout(infoIpServer);
    infoServer->addLayout(infoPortServer);
    infoServer->addLayout(connectionButtons);
    infoServer->addWidget(isRobot);

    //Part "information about sensor"
    labelCoord->addWidget(label_coordXYZ);
    labelCoord->addWidget(label_coordX);
    labelCoord->addWidget(label_coordY);
    labelCoord->addWidget(label_coordZ);

    infoPositionSensor->addWidget(label_sensor);
    infoPositionSensor->addWidget(Xvalue);
    infoPositionSensor->addWidget(Yvalue);
    infoPositionSensor->addWidget(Zvalue);

    infoSensor->addLayout(labelCoord);
    infoSensor->addLayout(infoPositionSensor);
    infoSensor->addWidget(sendButton);

    //Gathering
    serverAndSensor->addLayout(infoServer);
    serverAndSensor->addLayout(infoSensor);
    globalInformation->addLayout(serverAndSensor);
    globalInformation->addWidget(information);
    setLayout(globalInformation);
}

/****Frame and robot ****/
QString Client::formatXML(double X, double Y, double Z)
{

    qDebug() << "format XML...";
    QString sendString = "<Sen Type=\"PCext\"><EStr>Hello from sensor!</EStr><DeltaPos";
    sendString.append(" X=\"" + QString::number(X) + "\"");
    sendString.append(" Y=\"" + QString::number(Y) + "\"");
    sendString.append(" Z=\"" + QString::number(Z) + "\"");
    sendString.append(" A=\"" + QString::number(0.0) +"\"");
    sendString.append(" B=\"" + QString::number(0.0) +"\"");
    sendString.append(" C=\"" + QString::number(0.0) +"\"");
    sendString.append("/>");
    sendString.append("<IPOC>" "</IPOC></Sen>");

    qDebug() << sendString.toStdString().c_str();
    return sendString;
}

void Client::moveTo(QPoint robotPosition, QPoint goal)
{
    double distanceX = 0.0f, distanceY = 0.0f, distanceTotal = 0.0f; //mm
    double correctionX = 0.0f, correctionY = 0.0f;
    double signX = 0.0f, signY = 0.0f;
    const double timeCycle = 12.0f; // 12 ms
    double timeTotal = 0.0f;
    double nbCycle = 0.0f;

    bool validity = checkPointValidity(robotPosition);

    if (validity)
    {
        //signs for each correction
        if (robotPosition.x() > goal.x())
            signX = -1;
        else
            signX = 1;

        if (robotPosition.y() > goal.y())
            signY = -1;
        else
            signY = 1;

        distanceX = abs(robotPosition.x()) + abs(goal.x());
        distanceY = abs(robotPosition.y()) + abs(goal.y());

        distanceTotal = sqrt((pow(distanceX, 2) + pow(distanceY, 2))); // Pythagore
        timeTotal = timeCycle * distanceTotal; 	// The robot does 	1mm in 	12ms (max speed value).
        //			For	 	x mm, 	time = (x*12) / 1		(ms)
        nbCycle = timeTotal / timeCycle; //	1 cycle		=	12ms
        //	x cycle		=	(timeTotal * 1) / 12

        if (distanceX > distanceY)
        {
            correctionX = 1.0f;
            correctionY = distanceY / distanceX;
        }
        else if (distanceX < distanceY)
        {
            correctionY = 1.0f;
            correctionX = distanceX / distanceY;
        }
        else
        {
            if (distanceX == 0)
            {
                correctionX = 0.0f;
                correctionY = 0.0f;
            }
            else
            {
                correctionX = 1.0f;
                correctionY = 1.0f;
            }
        }

        correctionX *= signX;
        correctionY *= signY;

        infos[0] = nbCycle;
        infos[1] = correctionX;
        infos[2] = correctionY;
    }
    else
    {
        infos[0] = 0;
        infos[1] = 0;
        infos[2] = 0;
    }
    //return infos;
}

QPoint Client::decodeXML(QString frame)
{
    QPoint position;
    vector<QString> splittedFrame;
    vector<double> value;
    QString item;
    double coord, coordX, coordY;
    QStringList list;
    bool ok;

    list = frame.split("\"");

    foreach(item, list)
            splittedFrame.push_back(item);

    for (unsigned int i = 0 ; i < splittedFrame.size() ; i++)
    {
        item = splittedFrame[i];
        coord = item.toDouble(&ok);

        if(ok)
            value.push_back(coord);
    }

    coordX = value[0];
    coordY = value[1];
    position.setX(coordX);
    position.setY(coordY);

    qDebug() << "position : X = " << position.x() << ", Y =  " << position.y();
    return position;
}

bool Client::checkPointValidity(QPoint point)
{
    bool validity = false;
    const double minX = 0.0f;
    const double maxX = 600.0f;
    const double minY = -400.0f;
    const double maxY = 400.0f;

    if (point.x() > minX && point.x() < maxX)
        validity = true;
    else
        validity = false;

    if (point.y() > minY && point.y() < maxY)
        validity = true;
    else
        validity = false;

    return validity;
}
