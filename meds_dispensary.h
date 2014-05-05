#ifndef MEDS_DISPENSARY_H
#define MEDS_DISPENSARY_H

#include <QtWidgets>
#include "motor.h"
#include "gpio_sensor.h"
#include "dispensary_fsm_sm.h"
#include "gpio_keypad.h"

namespace Ui {
class Meds_dispensary;
}

class Meds_dispensary : public QWidget
{
    Q_OBJECT

public:
    explicit Meds_dispensary(QWidget *parent = 0);
    ~Meds_dispensary();
    void set_up_dispensary_motor(void);
    void set_up_formulary_motor(void);
    void set_status_label(QString);
    void set_arena_to_safe_pos();
    void sweep_limbs(int rotate_degrees);
    dispensary_fsmContext dispense_fsm;

private:
    Ui::Meds_dispensary *ui;
    motor *m_dca;
    motor *m_dcb;
    gpio_sensor *sr1;
    gpio_sensor *sr2;
    gpio_keypad *keypad_sw1;
    gpio_keypad *keypad_a;
    gpio_keypad *keypad_b;
    gpio_keypad *keypad_sw2;
    gpio_keypad *keypad_sw3;
    gpio_keypad *keypad_sw4;
    gpio_keypad *keypad_sw5;

    unsigned int last_disc_sensor_reading;
    int last_elapsed;
    unsigned int lowest_reading;
    unsigned int highest_reading;
    float formulary_rpm;
    bool found_index;
    QTime index_time;
    unsigned int rev_count;

private slots:
    void dispense_start(void);
    void disc_sensor_proc(int trigger_value);

};

#endif // MEDS_DISPENSARY_H
