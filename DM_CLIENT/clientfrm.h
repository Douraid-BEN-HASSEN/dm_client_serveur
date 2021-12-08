#ifndef CLIENTFRM_H
#define CLIENTFRM_H

#include <QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDebug>
#include "cmessagerie.h"

namespace Ui {
class clientFRM;
}

/**
 * Classe qui gère l'interface graphique client principale
 */
class clientFRM : public QMainWindow
{
    Q_OBJECT

public:
    explicit clientFRM(QWidget *parent = nullptr);
    ~clientFRM();

private:
    Ui::clientFRM *ui;

    // Utilisateur authentifié
    QString user;
     // Dict contenant tout les Widgets de conversation
    QMap<QString, CMessagerie*> mConversations;
    // Fomulaire de création d'une nouvelle conversation
    QDialog *conversationPopUp;
    // Destinataire
    QLineEdit *toTXT;

public:
    void setUser(QString);

public slots:
    // Slot qui affiche le formulaire de création d'une conversation
    void nouvelleConversation();
    // Slot qui gère le formulaire d'une nouvelle conversation
    void nouvelleConversationSubmit();
    // Slot qui gère l'envoi d'un message
    void envoiMessage(QString, QString);
    // Slot qui gère la reception d'un message
    void receptionMessage(QString, QString, QString);

signals:
    // Signal pour l'envoi des messages
    void doSendMessage(QString, QString);
    // Signal pour les messages entrants
    void doReceiveMessage(QString, QString, QString);

};

#endif // CLIENTFRM_H
