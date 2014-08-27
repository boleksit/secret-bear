#ifndef CRC_H
#define CRC_H
#include "mainwindow.h"
#include <QMainWindow>

class crc : public QMainWindow
{
    Q_OBJECT
public:
    explicit crc(QWidget *parent = 0);
    unsigned int calculate();
    unsigned long swap(unsigned long, char);
    void init();

signals:

public slots:

};

#endif // CRC_H
