#ifndef CSERVEUR_H
#define CSERVEUR_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCryptographicHash>
#include <QtXml>
#include <QFile>
#include <QMap>
#include <QDate>
#include <QTime>

#include <QDebug>
enum Errors {
    MISSING_ACTION,
    MISSING_LOGIN,
    MISSING_PASSWORD,
    MISSING_TO,
    MISSING_CONTENT,
    INVALID_TO,
    WRONG_CREDS,
    INTERNAL_ERROR,
    ACCOUNT_ALREADY,
    NO_ERROR
};

class CServeur: public QObject
{
    Q_OBJECT
public:
    CServeur(QObject* = nullptr);
    ~CServeur();

private:
    QTcpServer *_serveur;
    QHostAddress _host;
    int _port;
    QList<QTcpSocket*> mListSockets;
    QMap<QString, QList<QString>> pendingMsg;

    bool initServer();

    Errors doLogin(QString, QString);
    Errors doRegister(QString, QString);
    Errors doSendMessage(QString, QJsonArray, QString);
    void messageToSend(QString, QList<QString>);
    Errors validateData(QString, QJsonObject);
    void parseJson(QByteArray);
    QList<QJsonObject> requests;

public slots:
    void nouveauClient();
    void readyToRead();

    void logout(); // deconnexion propre du client

};

#endif // CSERVEUR_H
