#include <QCoreApplication>
#include "cserveur.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CServeur serveur;

    return a.exec();
}
