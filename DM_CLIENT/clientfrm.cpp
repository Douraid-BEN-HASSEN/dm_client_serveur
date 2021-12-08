#include "clientfrm.h"
#include "ui_clientfrm.h"

// constructeur
clientFRM::clientFRM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::clientFRM)
{
    ui->setupUi(this);
    this->user = "undefined";
    this->conversationPopUp = nullptr;
    this->toTXT = nullptr;
    connect(ui->nouvelleConversationBTN, SIGNAL(clicked()), this, SLOT(nouvelleConversation()));
    connect(this, SIGNAL(doReceiveMessage(QString, QString, QString)), this, SLOT(receptionMessage(QString, QString, QString)));
}

//  +--------+
//  | SLOT |
//  +--------+
void clientFRM::nouvelleConversation()
{
    // Construction du formulaire de nouvelle conversation
    this->conversationPopUp = new QDialog(this);
    QGroupBox *conversationGB = new QGroupBox("Nouvelle conversation", this->conversationPopUp);
    conversationGB->setTitle("Nouvelle conversation");

    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;

    this->toTXT = new QLineEdit("");
    vbox->addWidget(toTXT);

    QPushButton *validerBTN = new QPushButton("Valider");
    connect(validerBTN, SIGNAL(clicked()), this, SLOT(nouvelleConversationSubmit()));
    QPushButton *cancelBTN = new QPushButton("Annuler");
    connect(cancelBTN, SIGNAL(clicked()), conversationPopUp, SLOT(close()));

    hbox->addWidget(validerBTN);
    hbox->addWidget(cancelBTN);

    vbox->addLayout(hbox);

    conversationGB->setLayout(vbox);

    QVBoxLayout *mainvBox = new QVBoxLayout;
    mainvBox->addWidget(conversationGB);
    this->conversationPopUp->setLayout(mainvBox);

    this->conversationPopUp->setWindowTitle("Nouvelle conversation");
    this->conversationPopUp->show();
}

void clientFRM::nouvelleConversationSubmit()
{
    // Ajout d'une nouvelle conversation :
    if (!this->mConversations.contains(this->toTXT->text())) { // check si déjà conversion existante avec cet utilisateur... pour éviter les doublon
        this->mConversations[this->toTXT->text()] = new CMessagerie(this, this->user, this->toTXT->text()); // nouveau obj messagerie
        connect(this->mConversations[this->toTXT->text()], SIGNAL(sendingMessage(QString, QString)), this, SLOT(envoiMessage(QString, QString)));
        ui->conversationTW->addTab(this->mConversations[this->toTXT->text()], this->toTXT->text()); // nouvel onglet
    }

    this->conversationPopUp->close();
}

void clientFRM::envoiMessage(QString pTo, QString pContent)
{
    // emet un signal à la fenetre principale
    emit this->doSendMessage(pTo, pContent);
}

void clientFRM::receptionMessage(QString pFrom, QString pContent, QString pDatetime)
{
    if (!this->mConversations.contains(pFrom)) {
        this->mConversations[pFrom] = new CMessagerie(this, this->user, pFrom);
        connect(this->mConversations[pFrom], SIGNAL(sendingMessage(QString, QString)), this, SLOT(envoiMessage(QString, QString)));
        ui->conversationTW->addTab(this->mConversations[pFrom], pFrom);
    }
    // emet un signal de reception d'un nouveau message au Widget CMessagerie
    emit this->mConversations[pFrom]->incomingMessage(pContent, pDatetime);
}

//  +-----------+
//  | SETTER |
//  +-----------+
void clientFRM::setUser(QString pUser)
{
    this->user = pUser;
}

// destructeur
clientFRM::~clientFRM()
{
    if (this->conversationPopUp != nullptr) {
        delete this->conversationPopUp;
    }

    delete ui;
}
