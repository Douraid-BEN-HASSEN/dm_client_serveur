#ifndef CMESSAGERIE_H
#define CMESSAGERIE_H

#include <QObject>
#include <QWidget>
#include <QListWidget >
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QTime>
#include <QPainter>
#include <QPrinter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
/**
 * Classe Widget de messagerie
 */
class CMessagerie : public QWidget
{
    Q_OBJECT
public:
    CMessagerie(QWidget*, QString, QString);
    ~CMessagerie();

private:
    QString sender; // utilisateur qui envoi les messages
    QString receiver; // utilisateur qui reçoi les messages
    // corps du widget
    QListWidget  *messageLW;
    QPushButton *envoyerBTN;
    QPushButton *exportPdfBTN;
    QLineEdit *messageTXT;
    QVBoxLayout *vBox_1;
    QHBoxLayout *hBox_1;

public slots:
    // slot d'envoi d'un message
    void submitMessage();
    // slot qui gère la génération du pdf
    void exportPdf();
    // slot qui gère la recepetion des messages
    void incomingMessage(QString, QString);

signals:
    void sendingMessage(QString, QString); // signal emis à l'envoi d'un message

};

#endif // CMESSAGERIE_H
