#include "mainfrm.h"
#include "ui_mainfrm.h"
#include <iostream>

// constructeur
mainFRM::mainFRM(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainFRM)
{

    ui->setupUi(this);

    // init des variables
    this->_host = QHostAddress::LocalHost;
    this->_port = 5555;

    this->mClientFRM = nullptr;
    this->socket = new QTcpSocket();

    // connect menu
    QList<QAction*> menuActions = ui->menuParam_tre->actions();
    menuActions[0]->setText("Changer HOST (" + this->_host.toString() + ")");
    connect(menuActions[0], SIGNAL(triggered()), this, SLOT(showSettingWindowHost()));
    menuActions[1]->setText("Changer PORT (" + QString::number(this->_port) + ")");
    connect(menuActions[1], SIGNAL(triggered()), this, SLOT(showSettingWindowPort()));

    // connect socket
    connect(this->socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(this->socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(this->socket, SIGNAL(readyRead()),this, SLOT(readyToRead()));

    // connect connexionBTN
    connect(ui->connexionBTN, SIGNAL(clicked()), this, SLOT(doLogin()));

    // connect registerBTN
    connect(ui->registerBTN, SIGNAL(clicked()), this, SLOT(doRegister()));

}

//  +--------+
//  | SLOT |
//  +--------+
void mainFRM::connected()
{
    this->afficherMsg("Connecté au serveur");
}

void mainFRM::disconnected()
{
    this->afficherMsg("Déconnecté du serveur");
    if (this->mClientFRM != nullptr) {
        this->mClientFRM->close();
    }

}

void mainFRM::readyToRead()
{
    // traitement reponse
    QTcpSocket *sock = (QTcpSocket*)sender();
    if (sock->bytesAvailable() > 0) parseJson(sock->readAll());

    // traitement requette
    while (this->requests.count() > 0) {
        QJsonObject requestData = this->requests.first();

        /**
          pour chaque requette :

         REGISTER :
            Si state = ok
            Alors : -> afficher "Utilisateur inscrit"
            Sinon : -> afficher "Utilisateur pas inscrit"
            Fin si
         ---------
         ANSWERLOGIN :
            Si state = ok
            Alors :    -> enregistrer utilisateur dans mClientFRM
                            -> afficher formulaire client
            Sinon : -> afficher "Identifiant incorrect !"
            Fin si
        ---------
         RECEIVE :
            -> emettre un  signal doReceiveMessage

          */
        if (requestData.value("action").toString() == "register") {
            if (requestData.value("state").toString() == "ok") this->afficherMsg("Utilisateur inscrit");
            else this->afficherMsg("Utilisateur pas inscrit !");
        } else if (requestData.value("action").toString() == "answerlogin") {
            if (requestData.value("state").toString() == "ok") {
                this->mClientFRM = new clientFRM();
                this->mClientFRM->setUser(ui->loginTXT->text());
                connect(this->mClientFRM, SIGNAL(doSendMessage(QString, QString)), this, SLOT(doSend(QString, QString)));
                this->mClientFRM->show();
                this->hide();
            } else this->afficherMsg("Identifiant incorrect !");
        } else if (requestData.value("action").toString() == "login") {
            if (requestData.value("state").toString() == "ok") this->afficherMsg("Inscription réussie !");
            else this->afficherMsg("Inscription fail !");
        } else if (requestData.value("action").toString() == "receive") {
            emit this->mClientFRM->doReceiveMessage(
                            requestData.value("from").toString(),
                            requestData.value("content").toString(),
                            requestData.value("datetime").toString()
                        );
        }

        this->requests.pop_front();
    }

}

void mainFRM::doLogin()
{
    if(socket->state() != QAbstractSocket::ConnectedState)
    {
        this->socket->connectToHost(this->_host, this->_port);
        this->socket->waitForConnected();
    }

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        QString passwordHASH(QCryptographicHash::hash(ui->passwordTXT->text().toUtf8(), QCryptographicHash::Sha3_512).toHex());
        QString data = QString("{\"action\":\"login\",\"login\":\"%1\",\"password\":\"%2\"}")
                .arg(ui->loginTXT->text())
                .arg(passwordHASH);

        this->socket->write(data.toUtf8());
    } else {
        this->afficherMsg("Impossible de se connecter au serveur");
    }

}

void mainFRM::doRegister()
{

    // check si !connected
    if(socket->state() != QAbstractSocket::ConnectedState)
        {
            this->socket->connectToHost(this->_host, this->_port);
            this->socket->waitForConnected();
        }

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        if (ui->register_passwordTXT->text() == ui->register_passwordBISTXT->text()) {
            QString passwordHASH(QCryptographicHash::hash(ui->register_passwordTXT->text().toUtf8(), QCryptographicHash::Sha3_512).toHex());
            QString data = QString("{\"action\":\"register\",\"login\":\"%1\",\"password\":\"%2\"}")
                    .arg(ui->register_loginTXT->text())
                    .arg(passwordHASH);
            this->socket->write(data.toUtf8());
        } else this->afficherMsg("Les mots de passes sont différents");

    } else {
        this->afficherMsg("Impossible de se connecter au serveur");
    }

}

void mainFRM::doSend(QString pTo, QString pContent)
{
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        QString data = QString("{\"action\":\"send\",\"to\":[\"%1\"],\"content\":\"%2\"}").arg(pTo, pContent);

        this->socket->write(data.toUtf8());
    } else {
        this->afficherMsg("Non connecté au serveur");
    }
}

void mainFRM::showSettingWindowHost()
{
    this->settingFRM = new CSetting(this, "host");
    connect(this->settingFRM, SIGNAL(submitForm(QString, QString)), this, SLOT(setSetting(QString, QString)));
    this->settingFRM->show();
}

void mainFRM::showSettingWindowPort()
{
    this->settingFRM = new CSetting(this, "port");
    connect(this->settingFRM, SIGNAL(submitForm(QString, QString)), this, SLOT(setSetting(QString, QString)));
    this->settingFRM->show();
}

void mainFRM::setSetting(QString pSetting, QString pValue)
{
    if(pSetting == "host") this->_host = QHostAddress(pValue);
    else this->_port = pValue.toInt();

    // update des valeurs dans le menu
    QList<QAction*> menuActions = ui->menuParam_tre->actions();
    menuActions[0]->setText("Changer HOST (" + this->_host.toString() + ")");
    menuActions[1]->setText("Changer PORT (" + QString::number(this->_port) + ")");

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        this->socket->disconnect();
        this->socket->connectToHost(this->_host, this->_port);
        this->socket->waitForConnected();

        if (socket->state() != QAbstractSocket::ConnectedState) this->afficherMsg("Impossible de se connecter au serveur");
    }

}

//  +----------------------+
//  | FCT UTILITAIRE |
//  +----------------------+
// transforme un QByteArray en  QJsonDocument et l'enregistre dans les requettes a traiter
void mainFRM::parseJson(QByteArray pData)
{
    QString requette("");
    for (int it=0; it<pData.count(); it++) {
        requette += pData[it];
        if (pData[it] == '}') {
           this->requests << QJsonDocument :: fromJson(requette.toUtf8()).object();
            requette = "";
        }
    }
}

// permet d'afficher des fenetre de message
void mainFRM::afficherMsg(QString pMsg) {
    QMessageBox msgBox;
     msgBox.setText(pMsg);
     msgBox.exec();
}

// desctructeur
mainFRM::~mainFRM()
{
    if (this->socket != nullptr) {
        delete this->socket;
    }

    delete ui;
}
