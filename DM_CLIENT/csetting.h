#ifndef CSETTING_H
#define CSETTING_H

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QMainWindow>

class CSetting: public QDialog
{
    Q_OBJECT
private:
    QString setting; // correcspond au a modifier
    QVBoxLayout *vBox;
    QHBoxLayout *hBox;
    QLineEdit *dataTXR;
    QPushButton *submitBTN;
    QPushButton *cancelBTN;

public:
    CSetting(QMainWindow*, QString);
    ~CSetting();

public slots:
    // demande la soumission du formulaire
    void submitFrm();

signals:
    // emet un signal avec le nom et la valeur
    void submitForm(QString, QString);

};

#endif // CSETTING_H
