#ifndef MAINFRM_H
#define MAINFRM_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QAction>

#include "clientfrm.h"
#include "csetting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainFRM; }
QT_END_NAMESPACE

class mainFRM : public QMainWindow
{
    Q_OBJECT

public:
    mainFRM(QWidget *parent = nullptr);
    ~mainFRM();

private:
    Ui::mainFRM *ui;

    QHostAddress _host;
    int _port;
    QTcpSocket *socket;

    // formulaire client
    clientFRM *mClientFRM;
    // formulaire settings
    CSetting *settingFRM;

    // Liste des requettes reçues
    QList<QJsonObject> requests;
    //-----------------
    // transforme les données reçues et les enregistre
    void parseJson(QByteArray);
    // permet d'afficher des messages graphique
    void afficherMsg(QString);

public slots:
    // connection au serveur
    void connected();
    // deconnection serveur
    void disconnected();
    // recepetion des données
    void readyToRead();
    // authentification au seuveur
    void doLogin();
    // inscription
    void doRegister();
    // envoi de message
    void doSend(QString, QString);

    // affiche settingFRM pour changer host
    void showSettingWindowHost();
    // affiche settingFRM pour changer port
    void showSettingWindowPort();
    // change host ou port
    void setSetting(QString, QString);

};
#endif // MAINFRM_H
