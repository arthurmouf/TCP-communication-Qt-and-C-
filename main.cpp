#include <QApplication>
#include "server.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    Server server;
    server.show();
    return a.exec(); // permettra de traiter les signaux
}
