#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

  QApplication a(argc, argv);
  MainWindow w;
  a.setApplicationName("Fishy Tracking");
  a.setOrganizationName("Benjamin Gallois");
  a.setOrganizationDomain("http://www.benjamin-gallois.fr");
  w.setWindowIcon(QIcon(":/icon.png")); 
  w.show();
  a.exec();
}
