#ifndef MEDS_DISPENSARY_H
#define MEDS_DISPENSARY_H

#include <QWidget>
#include "motor.h"
#include "gpio_sensor.h"

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
    motor *m_dca;
    motor *m_dcb;
    gpio_sensor *sr1;
    gpio_sensor *sr2;
};

#endif // MEDS_DISPENSARY_H
