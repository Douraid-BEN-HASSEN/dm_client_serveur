#include "csetting.h"

// constructeur
CSetting::CSetting(QMainWindow *parent, QString pSetting) : QDialog(parent)
{
    this->setting = pSetting;

    // construction du formulaire
    this->vBox = new QVBoxLayout(this);
    this->hBox = new QHBoxLayout(this);
    this->dataTXR = new QLineEdit(this);
    this->submitBTN = new QPushButton(this);
    this->submitBTN->setText("OK");
    this->cancelBTN = new QPushButton(this);
    this->cancelBTN->setText("Annuler");

    if (pSetting == "host"){
        this->dataTXR->setPlaceholderText("Saisir HOST");
        this->setWindowTitle("Changer HOST");
    }
    else  {
        this->dataTXR->setPlaceholderText("Saisir PORT");
        this->setWindowTitle("Changer PORT");
    }

    this->vBox->addWidget(this->dataTXR);
    this->hBox->addWidget(this->submitBTN);
    this->hBox->addWidget(this->cancelBTN);
    this->vBox->addLayout(this->hBox);

    // connect
    connect(this->submitBTN, SIGNAL(clicked()), this, SLOT(submitFrm()));
    connect(this->cancelBTN, SIGNAL(clicked()), this, SLOT(close()));
}

// +--------+
// | SLOT |
// +--------+
void CSetting::submitFrm() {
    emit this->submitForm(this->setting, this->dataTXR->text());
    this->close();
}

// desctructeur
CSetting::~CSetting()
{
    delete this->dataTXR;
    delete this->submitBTN;
    delete this->cancelBTN;
}
