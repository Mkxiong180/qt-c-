#ifndef NETGAME_H
#define NETGAME_H

#include "Board.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include<QPushButton>
/*
    1) 执红方还是黑方，这个信息有服务器发出，客户端接收
    第一个字节固定是1，第二个字节是1，或者0，1表示接收方走红旗，0表示走黑棋
    2）点击信息
    第一个字节固定是2，第二个字节是row，第三个字节是col，第四个字节是点击的棋子id
*/
class NetGame : public Board
{
    Q_OBJECT
public:
    NetGame(bool server);
    ~NetGame();

    QTcpServer* _server;
    QTcpSocket* _socket;
    QTcpServer* mp_TCPServer;
    QTcpSocket* mp_TCPSocket;

    QTextEdit* qte;
    QTextEdit* messagebox;
    QPushButton* qpb;
    void click(int id, int row, int col);

public slots:
    void slotNewConnection();
    void slotRecv();
    /*客户端*/
     void on_m_connectServerBtn_clicked();

     void on_pushButton_2_clicked();

     void ClientRecvData();

    /*服务端*/
     void OnBtnInitSocket();
     void OnBtnSendData();
     void ServerReadData();
     //void readdata();
     void ServerNewConnection();
     void sServerDisConnection();
};

#endif // NETGAME_H
