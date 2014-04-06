#ifndef MEDS_DISPENSARY_H
#define MEDS_DISPENSARY_H

#include <QtWidgets>
#include "motor.h"
#include "gpio_sensor.h"
#include "dispensary_fsm_sm.h"

namespace Ui {
class Meds_dispensary;
}

class Meds_dispensary : public QWidget
{
    Q_OBJECT

public:
    explicit Meds_dispensary(QWidget *parent = 0);
    ~Meds_dispensary();
    void set_motor_a_speed(int);
    void set_motor_b_speed(int);
    void set_status_label(QString);

private:
    Ui::Meds_dispensary *ui;
    motor *m_dca;
    motor *m_dcb;
    gpio_sensor *sr1;
    gpio_sensor *sr2;
    dispensary_fsmContext dispense_fsm;

};

#endif // MEDS_DISPENSARY_H
