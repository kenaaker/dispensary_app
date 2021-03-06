#ifndef MEDS_DISPENSARY_H
#define MEDS_DISPENSARY_H

#include <QtWidgets>
#include "motor.h"
#include "gpio_sensor.h"
#include <formulary_fsm_sm.h>
#include "gpio_keypad.h"
#include "dispensary_sslsock.h"
#include "zconfservice.h"

namespace Ui {
    class Meds_dispensary;
}

static const int num_bottles = 6;
static const int wheel_speed = 30;

class Meds_dispensary : public QWidget
{
    Q_OBJECT

public:
    explicit Meds_dispensary(QWidget *parent = 0);
    ~Meds_dispensary();
    void set_up_dispensary_motor(void);
    void set_up_formulary_motor(void);
    void set_status_label(QString);
    void set_arena_to_safe_pos(void);
    void sweep_limbs(int rotate_degrees);
    formulary_fsmContext formulary_fsm;
    bool stable_sensor_pulses(void);
    void disk_index_setup(void);
    unsigned int pulse_count;
    void move_disk(unsigned int slot_num);
    void move_disk_relative(int offset);
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

    int current_slot;
    int destination_slot;
    unsigned int last_disc_sensor_reading;
    int last_elapsed;
    unsigned int lowest_reading;
    unsigned int highest_reading;
    float formulary_rpm;
    bool found_index;
    bool first_pulse;
    QTime index_time;
    QTime pulse_time;
    int mean_elapsed_sector_time;
    unsigned int rev_count;
    int pulse_times[num_bottles+2];   /* Allow space to capture one wrap around reading, needed for case where it starts between double mark */

    dispensary_sslsock cmd_server;
    ZConfService *disp_svc;

private slots:
    void move_home(int on_off);
    void move_minus_1(int on_off);
    void move_plus_1(int on_off);
    void move_dispense(int on_off);
    void dispense_start(void);
    void disc_sensor_proc(int trigger_value);
    void movement_done_proc(int);
    void command_proc(QString &cmd);
};


static inline int modulo(int m, int n) {
    return m >= 0 ? m % n : ( n - abs ( m%n ) ) % n;
} /* modulo */

#endif // MEDS_DISPENSARY_H
