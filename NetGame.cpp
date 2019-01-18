#include "NetGame.h"
#include <QDebug>
#include <QTextEdit>
#include<QPushButton>
#include <QMessageBox>
NetGame::NetGame(bool server)
{
    _server = NULL;
    _socket = NULL;
    mp_TCPServer=NULL;
    mp_TCPSocket=NULL;
    qte=new QTextEdit("",this);
    qte->setFont(QFont(tr("Consolas"), 14));
    qte->setGeometry(700, 200, 320, 320); //新建QPushButton对象，父窗口为qw
    messagebox=new QTextEdit("",this);
    messagebox->setFont(QFont(tr("Consolas"), 14));
    messagebox->setGeometry(700, 530, 210, 40); //新建QPushButton对象，父窗口为qw
    qpb=new QPushButton("发送",this);
    qpb->setGeometry(920, 530,100, 40);
    if(server)
    {
        mp_TCPServer = new QTcpServer(this);
        mp_TCPServer->listen(QHostAddress::Any, 8888);
        connect(mp_TCPServer, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
        connect(qpb, SIGNAL(clicked()),this, SLOT(OnBtnSendData()));
    }
    else
    {
        mp_TCPSocket = new QTcpSocket();
        mp_TCPSocket->connectToHost(QHostAddress("127.0.0.1"), 8888);
        connect(mp_TCPSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));
        connect(qpb, SIGNAL(clicked()),this, SLOT(on_pushButton_2_clicked()));
    }

    if(server)
    {
        _server = new QTcpServer(this);
        _server->listen(QHostAddress::Any, 9999);
        connect(_server, SIGNAL(newConnection()),
                this, SLOT(slotNewConnection()));
    }
    else
    {
        _socket = new QTcpSocket(this);
        _socket->connectToHost(QHostAddress("127.0.0.1"), 9999);

        connect(_socket, SIGNAL(readyRead()),
                this, SLOT(slotRecv()));
    }
}

NetGame::~NetGame()
{

}

void NetGame::click(int id, int row, int col)
{
    if(_selectid == -1 && id != -1)
    {
        if(_s[id]._red != _bSide)
            return;
    }

    Board::click(id, row, col);

    /* 发送给对方 */
    char buf[4];
    buf[0] = 2;
    buf[1] = 9-row;
    buf[2] = 8-col;
    buf[3] = id;
    _socket->write(buf, 4);
}

void NetGame::slotRecv()
{
    QByteArray ba = _socket->readAll();
    char cmd = ba[0];
    if(cmd == 1)
    {
        // 初始化
        char data = ba[1];
        init(data==1);
    }
    else if(cmd==2)
    {
        int row = ba[1];
        int col = ba[2];
        int id = ba[3];
        Board::click(id, row, col);
    }
}

void NetGame::slotNewConnection()
{
    if(_socket) return;

    _socket = _server->nextPendingConnection();
    connect(_socket, SIGNAL(readyRead()),
            this, SLOT(slotRecv()));

    /* 给对方发送数据 */
    char buf[2];
    buf[0] = 1;
    buf[1] = 0;
    _socket->write(buf, 2);

    init(buf[1]==0);
}

void NetGame::ServerNewConnection()
{
    //获取客户端连接
        mp_TCPSocket = mp_TCPServer->nextPendingConnection();
        if(!mp_TCPSocket)
        {
            QMessageBox::information(this, "QT网络通信", "未正确获取客户端连接！");
            return;
        }
        else
        {
            QMessageBox::information(this, "QT网络通信", "成功接受客户端的连接");
            connect(mp_TCPSocket, SIGNAL(readyRead()), this, SLOT(ServerReadData()));
            //connect(mp_TCPSocket, SIGNAL(disconnected()), this, SLOT(sServerDisConnection()));
        }
}



void NetGame::ServerReadData()
{
    char buffer[1024] = {0};
        mp_TCPSocket->read(buffer, 1024);
        if( strlen(buffer) > 0)
        {
            QString showNsg = buffer;
            qte->setTextColor(QColor(0,0,255));
            qte->append(showNsg);
        }
        else
        {
            QMessageBox::information(this, "QT网络通信", "未正确接收数据");
            return;
        }
}

void NetGame::sServerDisConnection()
{
    QMessageBox::information(this, "QT网络通信", "与客户端的连接断开");
        return;
}

void NetGame::OnBtnSendData()
{
    char sendMsgChar[1024] = {0};
        QString sendMsg = messagebox->toPlainText();
        if(sendMsg.isEmpty())
        {
            QMessageBox::information(this, "QT网络通信", "发送数据为空，请输入数据");
            return;
        }
        strcpy(sendMsgChar, sendMsg.toStdString().c_str());
        if(mp_TCPSocket->isValid())
        {
            qte->setTextColor(QColor(0,220,0));
            qte->append(sendMsg);
            messagebox->setText("");
            int sendRe = mp_TCPSocket->write(sendMsgChar, strlen(sendMsgChar));
            if( -1 == sendRe)
            {
                QMessageBox::information(this, "QT网络通信", "服务端发送数据失败！");
            }
        }
        else
        {
            QMessageBox::information(this, "QT网络通信", "套接字无效！");
        }
}

void NetGame::on_m_connectServerBtn_clicked()
{

        if(!mp_TCPSocket->waitForConnected(30000))
        {
            QMessageBox::information(this, "QT网络通信", "连接服务端失败！");
            return;
        }
         connect(mp_TCPSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));
}

void NetGame::OnBtnInitSocket()
{
    mp_TCPServer = new QTcpServer();
        //int port = ui->m_portLineEdit->text().toInt();
        if(!mp_TCPServer->listen(QHostAddress::Any, 8888))
        {
            QMessageBox::information(this, "QT网络通信", "服务器端监听失败！");
            return;
        }
        else
        {
            QMessageBox::information(this, "QT网络通信", "服务器监听成功！");
        }
        connect(mp_TCPServer, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
}

void NetGame::on_pushButton_2_clicked()
{
    //获取TextEdit控件中的内容
       QString sendMsg = messagebox->toPlainText();
       char sendMsgChar[1024] = {0};
       strcpy(sendMsgChar, sendMsg.toStdString().c_str());
       qte->setTextColor(QColor(0,220,0));
       qte->append(sendMsg);
       messagebox->setText("");
       int sendRe = mp_TCPSocket->write(sendMsgChar, strlen(sendMsgChar));
       if(sendRe == -1)
       {
            QMessageBox::information(this, "QT网络通信", "向服务端发送数据失败！");
            return;
       }
}

void NetGame::ClientRecvData()
{
    //将接收内容存储到字符串中
        char recvMsg[1024] = {0};
        int recvRe = mp_TCPSocket->read(recvMsg, 1024);
        if(recvRe == -1)
        {
            QMessageBox::information(this, "QT网络通信", "接收服务端数据失败！");
            return;
        }
        QString showQstr = recvMsg;
        qte->setTextColor(QColor(0,0,255));
        qte->append(showQstr);
}
