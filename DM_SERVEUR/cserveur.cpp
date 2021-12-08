#include "cserveur.h"
#include <iostream>
#include <string>

CServeur::CServeur(QObject *p) : QObject(p)
{
    this->_serveur = new QTcpServer();
    this->initServer();
}

void CServeur::initServer()
{
    /**
      [1] check paths
      [2] check vars
      [3] lancer le serveur
      */
    // init fichier/dossier
    QDir dossier(QDir::currentPath());
    if (!QDir(dossier.currentPath() + "/logs").exists()){
        if(dossier.mkdir("logs")) qDebug() << "Dossier 'logs' crée";
        else qDebug() << "Erreur à la création du dossier 'logs'";
    }
    if (!QDir(dossier.currentPath() + "/users").exists()){
        if(dossier.mkdir("users")) qDebug() << "Dossier 'users' crée";
        else qDebug() << "Erreur à la création du dossier 'users'";
    }

    // settings
    QSettings settings(QDir(dossier.currentPath() + "/TP_CLIENT_SERVEUR").dirName(), "config");
    if(settings.value("host",  -1) == -1) {
        std::string host;
        std::cout << "Saisir HOST (defaut=0.0.0.0) : ";
        if (std::getline(std::cin, host)){
            if (host == "") host = "0.0.0.0";
            settings.setValue("host", QString(host.c_str()));
        }
    } else {
        std::cout << "Souhaitez-vous changer HOST (" << settings.value("host",  -1).toString().toStdString() << ") o/n ? ";
        std::string reponse;
        if (std::getline(std::cin, reponse)){
            QString rep(reponse.c_str());
            if (rep.toLower() == "o") {
                std::string host;
                std::cout << "Saisir HOST (defaut=0.0.0.0) : ";
                if (std::getline(std::cin, host)){
                    if (host == "") host = "0.0.0.0";
                    settings.setValue("host", QString(host.c_str()));
                }
            }
        }
    }
    if(settings.value("port",  -1) == -1) {
        std::string port;
        std::cout << "Saisir PORT (defaut=5555) : ";
        if (std::getline(std::cin, port)){
            if (port == "") port = QString("5555").toStdString();
            settings.setValue("port", QString(port.c_str()));
        }
    } else {
        std::cout << "Souhaitez-vous changer PORT (" << settings.value("port",  -1).toString().toStdString() << ") o/n ? ";
        std::string reponse;
        if (std::getline(std::cin, reponse)){
            QString rep(reponse.c_str());
            if (rep.toLower() == "o") {
                std::string port;
                std::cout << "Saisir PORT (defaut=5555) : ";
                if (std::getline(std::cin, port)){
                    if (port == "") port = QString("5555").toStdString();
                    settings.setValue("port", QString(port.c_str()));
                }
            }
        }
    }

    this->_host = QHostAddress(settings.value("host",  QHostAddress::Any).toString());
    this->_port = settings.value("port",  5555).toInt();

    if (!_serveur->listen(this->_host, this->_port))
    {
        qDebug() << "Impossible de lancer le serveur";
    } else
    {
        qDebug() << "Serveur lancé";
        QObject::connect(this->_serveur, SIGNAL(newConnection()), this, SLOT(nouveauClient()));
        #if defined _WIN32
            system("cls");
        #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
            system("clear");
        #elif defined (__APPLE__)
            system("clear");
        #endif
        std::cout << "SERVEUR (" << this->_host.toString().toStdString() << ":" << this->_port << ")" << std::endl;
    }

}

void CServeur::parseJson(QByteArray pData)
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

Errors CServeur::validateData(QString pAction,  QJsonObject pData)
{
    if (pAction == "action") {
        if (pData.value("action") == QJsonValue::Undefined) {
            return Errors::MISSING_ACTION;
        }
    } else if (pAction == "login") {
        if (pData.value("login") == QJsonValue::Undefined) {
            return Errors::MISSING_LOGIN;
        } else if  (pData.value("login") == QJsonValue::Undefined) {
            return Errors::MISSING_PASSWORD;
        }
    } else if (pAction == "register") {
        if (pData.value("login") == QJsonValue::Undefined) {
            return Errors::MISSING_LOGIN;
        } else if  (pData.value("login") == QJsonValue::Undefined) {
            return Errors::MISSING_PASSWORD;
        }
    } else if (pAction == "send") {
        if (pData.value("to") == QJsonValue::Undefined) {
            return Errors::MISSING_TO;
        } else if (!pData.value("to").isArray()) {
                return Errors::INVALID_TO;
        } else if  (pData.value("content") == QJsonValue::Undefined) {
            return Errors::MISSING_CONTENT;
        }
    }
    return Errors::NO_ERROR;
}

void CServeur::nouveauClient()
{
    QTcpSocket *socket = _serveur->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(logout()));
}

void CServeur::readyToRead()
{
    // traitement reponse
    QTcpSocket *sock = (QTcpSocket*)sender();
    if (sock->bytesAvailable() > 0) parseJson(sock->readAll());

    // traitement requette
    while (this->requests.count() > 0) {
        QJsonObject requestData = this->requests.first();
        Errors validForm = this->validateData("action", requestData);
        qDebug() << "Requette " << requestData.value("action").toString();
        /**
            pour chaque requette :

            REGISTER :
                Si donnee reçues sont valides
                Alors : Si utilisateur inscrit
                             Alors : -> repondre "ok"
                             Sinon : -> repondre "ko"
                             Fin si
                Sinon : -> repondre "ko"
                Fin si
            -----
            LOGIN :
                Si donnee reçues sont valides
                Alors :
                    Si IDs = ok
                    Alors :     Si messages en attente
                                    Alors : -> envoyer les messages
                                    Fin si
                                    -> repondre "ok"
                    Sinon : -> repondre "ko"
                    Fin si
                Sinon : -> repondre "ko"
                Fin si
            -----
            SEND :
                Si donnee reçues sont valides
                Alors : Si message envoye
                             Alors : -> repondre "ok"
                             Sinon : -> repondre "ko"
                             Fin si
                Sinon : -> repondre "ko"
                Fin si
            -----

          */
        if(validForm == Errors::NO_ERROR) {
           QString action = requestData.value("action").toString();
            if (action == "register") {
                validForm = this->validateData("register", requestData);
                if (validForm == Errors::NO_ERROR) {
                    if (this->doRegister(requestData.value("login").toString(), requestData.value("password").toString()) == Errors::NO_ERROR) {
                        sock->write("{\"action\":\"register\",\"state\":\"ok\"}");
                    } else sock->write("{\"action\":\"register\",\"state\":\"ko\"}");
                }
            } else if (action == "login")
            {
                validForm = this->validateData("login", requestData);
                if (validForm == Errors::NO_ERROR)
                {
                    if (this->doLogin(requestData.value("login").toString(), requestData.value("password").toString()) == Errors::NO_ERROR)
                    {
                        sock->setProperty("user", requestData.value("login").toString());
                        this->mListSockets << sock;
                        sock->write("{\"action\":\"answerlogin\",\"state\":\"ok\"}");
                        this->messageToSend(sock->property("user").toString(), this->pendingMsg[sock->property("user").toString()]);
                        this->pendingMsg.remove(sock->property("user").toString());
                     } else sock->write("{\"action\":\"answerlogin\",\"state\":\"ko\"}");
                }
            } else if (action == "send" && sock->property("user").isValid()) {
                validForm = this->validateData("send", requestData);
                if (validForm == Errors::NO_ERROR)
                {
                    //boucle pour trouver socket/user
                    if (this->doSendMessage(sock->property("user").toString(), requestData.value("to").toArray(), requestData.value("content").toString()) == Errors::NO_ERROR) {
                        sock->write("{\"action\":\"send\",\"state\":\"ok\"}");
                    } else sock->write("{\"action\":\"send\",\"state\":\"ko\"}");
                }
            }
        } else sock->write("{\"state\":\"ko\"}");

        this->requests.pop_front();
    }

}

// gestion de l'inscription d'un utilisateur
Errors CServeur::doLogin(QString pLogin, QString pPassword)
{
    // check si compte existant
    QString hashedLogin = QString(QCryptographicHash::hash(pLogin.toUtf8(), QCryptographicHash::Sha3_512).toHex());
    QDir f(QDir::currentPath() + "/users");
    if (!f.exists(hashedLogin + ".xml")) {
        qDebug() << "Ce compte n'existe pas";
        return Errors::WRONG_CREDS;
    } else {
        // lire fichier
        QDomDocument xmlBOM;
        QFile xml(QDir::currentPath() + "/users/" + hashedLogin + ".xml");
        if (xml.open(QIODevice::ReadOnly ))
        {
            xmlBOM.setContent(&xml);
            xml.close();
            // comparaison creds
           QDomElement root=xmlBOM.documentElement();
           QDomNodeList childNodes=root.childNodes();
           QString login = childNodes.at(0).toElement().firstChild().toText().data();
           QString password = childNodes.at(1).toElement().firstChild().toText().data();
            if (pLogin == login && pPassword == password) {
                return Errors::NO_ERROR;
            } else return Errors::WRONG_CREDS;
        }
            return Errors::INTERNAL_ERROR;
    }
}

// gestion de l'inscription utilisateur
Errors CServeur::doRegister(QString pLogin, QString pPassword)
{
    /* * requestRegister
 * check !exist
 * -> creer compte
 * -> retourner connexion
 * */
    QString hashedLogin = QString(QCryptographicHash::hash(pLogin.toUtf8(), QCryptographicHash::Sha3_512).toHex());
    QDir f(QDir::currentPath() + "/users");

    if (f.exists(hashedLogin + ".xml")) return Errors::ACCOUNT_ALREADY;
    else {
        QDomDocument docXml;
        QDomElement root = docXml.createElement("DATA");
        docXml.appendChild(root);
        QDomElement loginTag = docXml.createElement("LOGIN");
        QDomElement passwordTag = docXml.createElement("PASSWORD");
        root.appendChild(loginTag);
        root.appendChild(passwordTag);
        QDomText loginValue = docXml.createTextNode(pLogin);
        QDomText passwordValue = docXml.createTextNode(pPassword);
        loginTag.appendChild(loginValue);
        passwordTag.appendChild(passwordValue);
        QFile xml(QDir::currentPath() + "/users/" + hashedLogin + ".xml");
       if (xml.open(QFile::WriteOnly | QFile::Truncate)) {
             QTextStream out(&xml);
             out << docXml.toString();
             xml.close();
        } else return Errors::INTERNAL_ERROR;
    }

    return Errors::NO_ERROR;
}

// gestion des messages a envoyer
Errors CServeur::doSendMessage(QString pFrom, QJsonArray pTo, QString pContent)
{
    int index;
    for(int it_1=0; it_1<pTo.count(); it_1++) {
        index = -1;
        for(int it_2=0; it_2<this->mListSockets.length(); it_2++) {
            if (pTo[it_1].toString() == this->mListSockets[it_2]->property("user").toString()) {
                index = it_2;
                break;
            }
        }

        QString data = QString("{\"action\":\"receive\",\"from\":\"%1\",\"content\":\"%2\",\"datetime\":\"%3\" }").arg(pFrom, pContent, QDate::currentDate().toString("dd/MM/yyyy") + " " + QTime::currentTime().toString("hh:mm"));
        if (index > -1) {
            this->mListSockets[index]->write(data.toUtf8()); // connecté
        } else this->pendingMsg[pTo[it_1].toString()].append(data); // déconnecté
    }
    return Errors::NO_ERROR;
}

// / gestion des messages en attente a envoyer
void CServeur::messageToSend(QString pTo, QList<QString> pData)
{
    for (int it_1=0; it_1<this->mListSockets.length(); it_1++) {
        if (this->mListSockets[it_1]->property("user").toString() == pTo) {
            for (int it_2=0; it_2<pData.length(); it_2++) {
                this->mListSockets[it_1]->write(pData[it_2].toUtf8());
            }
        }
    }
}

// deconnection d'un client
void CServeur::logout()
{
    qDebug() << "logout";
    QTcpSocket *socket = (QTcpSocket*)sender();
    // supprimer de la liste
    for (int it=0; it < this->mListSockets.length(); it++) {
        if (this->mListSockets[it]->property("user").toString() == socket->property("user").toString()) {
            this->mListSockets.removeAt(it);
            break;
        }
    }
}

CServeur::~CServeur()
{
    delete this->_serveur;
}
