#include "cmessagerie.h"

// constructeur
CMessagerie::CMessagerie(QWidget *parent, QString pUser, QString pTo) : QWidget(parent)
{
    this->sender = pUser;
    this->receiver = pTo;

    // construction du widget
    this->vBox_1 = new QVBoxLayout(this);
    this->messageLW = new QListWidget (this);
    this->vBox_1->addWidget(this->messageLW);

    this->hBox_1 = new QHBoxLayout(this);

    this->messageTXT = new QLineEdit(this);
    this->envoyerBTN = new QPushButton("Envoyer", this);
    connect(this->envoyerBTN, SIGNAL(clicked()), this, SLOT(submitMessage()));
    this->exportPdfBTN = new QPushButton("Exporter en PDF", this);
    connect(this->exportPdfBTN, SIGNAL(clicked()), this, SLOT(exportPdf()));

    this->hBox_1->addWidget(this->messageTXT);
    this->hBox_1->addWidget(this->envoyerBTN);
    this->hBox_1->addWidget(this->exportPdfBTN);

    this->vBox_1->addLayout(this->hBox_1);
}

//  +--------+
//  | SLOT |
//  +--------+
void CMessagerie::submitMessage()
{
    this->messageLW->addItem(QDate::currentDate().toString("dd/MM/yyyy") + " " + QTime::currentTime().toString("hh:mm") + " : -> " + this->messageTXT->text());
    this->messageLW->setProperty(QString(this->messageLW->count()).toStdString().c_str(), this->sender); // nomme l'emetteur du message (pour le traitement pdf)
    emit this->sendingMessage(this->receiver, this->messageTXT->text()); // emet un signal d'envoi de msg au conteneur parent
    this->messageTXT->setText("");
}

void CMessagerie::exportPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Choisir un nom", QDir::homePath(), "Pdf Files (*.pdf)");
    if(fileName.count() == 0) return;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    painter.begin(&printer);
    painter.drawText(10, 10, QString("Conversation avec " + this->receiver));

    // écriture des messages dans le pdf
    int posY = 40;
    for (int it=0; it<this->messageLW->count(); it++) {
        if (it % 20 == 0 && it > 0) printer.newPage(); // 20 messages par page max
        // condition pour nommer le message
        if (this->messageLW->property(QString(it).toStdString().c_str()) == this->sender) painter.drawText(10, posY, "[" + this->sender + "] " + this->messageLW->item(it)->text());
        else painter.drawText(10, posY, "[" + this->receiver + "] " + this->messageLW->item(it)->text());
        posY += 20;
    }

    painter.end();

    QMessageBox msgBox;
     msgBox.setText("Fichier enregistré");
     msgBox.exec();
}

void CMessagerie::incomingMessage(QString pContent, QString pDatetime)
{
    this->messageLW->addItem(pDatetime + " : <-  " + pContent); // ajoute le msg reçu dans la liste
}

// destructeur
CMessagerie::~CMessagerie()
{
    delete this->messageLW;
    delete this->envoyerBTN;
    delete this->exportPdfBTN;
    delete this->messageTXT;
    delete this->vBox_1;
    delete  this->hBox_1;
}
