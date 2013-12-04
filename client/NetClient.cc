/*
 FILNAMN: 		NetClient.cc
 PROGRAMMERARE:	hanel742, eriek984, jened502, tobgr602, niker917, davha227
 SKAPAD DATUM:	2013-11-19
 BESKRIVNING:
 */

#include "NetClient.h"
#include "../gui/gui.h"
#include <QTest>

using namespace std;

NetClient::NetClient(QString username, QString inAddress, Gui* myGui, QObject *parent) : QObject(parent){
    
    guiPointer = myGui;
    name=username;
    address=inAddress;
}


void NetClient::start(){
    TcpSocket = new QTcpSocket(this);
    
    connect(TcpSocket,SIGNAL(connected()),this,SLOT(connected()));
    connect(TcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(TcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    
    
    qDebug() << "connecting...";
    
    TcpSocket->connectToHost(QHostAddress(address),quint16(1234));
    
    if(!TcpSocket->waitForConnected(1000)){
        guiPointer->noConnection();
        qDebug() << "Error: " << TcpSocket->errorString();
    }
}


//------Slots---------

void NetClient::connected(){
    QByteArray array = "/initiate";
    array += 0x1F; //unit separator
    array += name;
    
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
    guiPointer->connected();
}

void NetClient::disconnected(){
    qDebug() << "disconnected";
    
}

void NetClient::readyRead(){
    //Recieve commando
    QByteArray Data = TcpSocket->readAll();
    
    // Separate the command from the operand
    QByteArray compare;
    compare += 0x1F;
    
    int i = Data.indexOf(compare);
    
    QString commandName = Data.left(i);
    QString inData = Data.mid(i);
    
    // Check which command that's supposed to run
    if (commandName == "/reinitiate") {
        guiPointer->userNameTaken();
    }
    
    else if (commandName == "/history") {
        QVector<QString> history;
        while(inData.size()>1){
            
            // Get from
            i = inData.indexOf(compare);
            QString from = inData.left(i);
            inData = inData.mid(i);
            history.push_back(from);
            
            // Get to
            i = inData.indexOf(compare);
            QString to = inData.left(i);
            inData = inData.mid(i);
            history.push_back(to);
            
            // Get message
            i = inData.indexOf(compare);
            QString contents = inData.left(i);
            inData = inData.mid(i);
            history.push_back(contents);
            
            //Get time
            i = inData.indexOf(compare);
            QString time = inData.left(i);
            inData = inData.mid(i);
            history.push_back(time);
            
        }
    }
    
    else if (commandName == "/message") {
        
        // Get from
        i = inData.indexOf(compare);
        QString from = inData.left(i);
        inData = inData.mid(i);
        
        // Get to
        i = inData.indexOf(compare);
        QString to = inData.left(i);
        inData = inData.mid(i);
        
        // Get message
        i = inData.indexOf(compare);
        QString contents = inData.left(i);
        inData = inData.mid(i);
        
        // Get time
        QString dateTime = inData;
        
        guiPointer->receiveMessage(from, to, contents, dateTime);
    }
    else{
        throw logic_error("Unknown command, fatal error");
    }
    
}

void NetClient::sendMessage(QString from, QString to, QString message){
    QByteArray array = "/message";
    array += 0x1F; //unit separator
    array += from;
    array += 0x1F;
    array += to;
    array += 0x1F;
    array += message;
    
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
}

void NetClient::setName(QString inName) {
    name=inName;
}


