#ifndef MEDS_DISPENSARY_H
#define MEDS_DISPENSARY_H

#include <QWidget>

namespace Ui {
class Meds_dispensary;
}

class Meds_dispensary : public QWidget
{
    Q_OBJECT

public:
    explicit Meds_dispensary(QWidget *parent = 0);
    ~Meds_dispensary();

private:
    Ui::Meds_dispensary *ui;
};

#endif // MEDS_DISPENSARY_H
