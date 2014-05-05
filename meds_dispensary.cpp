#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"
#include "dispensary_fsm_sm.h"
#include <QFontDatabase>
#include <QProgressBar>
#include <QtCore/qdebug.h>
#include <QTime>

Meds_dispensary::Meds_dispensary(QWidget *parent) :
    QWidget(parent),
    dispense_fsm(*this),
    ui(new Ui::Meds_dispensary)
 {

    ui->setupUi(this);
    m_dca = new motor(e_motor_id::e_motor_a);
    m_dca->set_motor_rotation_speed(248);
    m_dcb = new motor(e_motor_id::e_motor_b);
    m_dcb->set_motor_rotation_speed(10);
    sr1 = new gpio_sensor("P9_25", "P9_39");
    sr2 = new gpio_sensor("P9_27", "P9_40");
    keypad_sw1 = new gpio_keypad("P8_39", "SW_1");
    keypad_a = new gpio_keypad("P8_32", "A");
    keypad_b = new gpio_keypad("P8_33", "B");
    keypad_sw2 = new gpio_keypad("P8_34", "SW_2");
    keypad_sw3 = new gpio_keypad("P8_35", "SW_3");
    keypad_sw4 = new gpio_keypad("P8_38", "SW_4");
    keypad_sw5 = new gpio_keypad("P8_37", "SW_5");

    QTimer::singleShot(1000, this, SLOT(dispense_start()));
    last_disc_sensor_reading = sr2->sensor_read();
    lowest_reading = last_disc_sensor_reading;
    highest_reading = last_disc_sensor_reading;
    formulary_rpm = 0.0;
    found_index = false;

//    QFontDatabase fonts;
//    qDebug() << "fonts" << fonts.families();

}

Meds_dispensary::~Meds_dispensary() {
    m_dca->motor_run(0);
    m_dcb->motor_run(0);
    delete sr1;
    delete sr2;
    delete m_dcb;
    delete m_dca;
    delete ui;
}

void Meds_dispensary::dispense_start(void) {
    dispense_fsm.enterStartState();
}

void Meds_dispensary::set_up_dispensary_motor(void) {
    m_dca->motor_run(0);
}

const static int motor_calibration_percent=50;

void Meds_dispensary::set_up_formulary_motor(void) {

    m_dcb->motor_run(0);
    /* Check the hardware, first the position sensor on the formulary wheel */
    sr2->set_read_interval(50);
    connect(sr2, SIGNAL(adc_signal(int)), this, SLOT(disc_sensor_proc(int)));
    sr2->set_bounds(120,2000);
    sr2->set_in_or_out(false);                      /* Send signal if out of 120->2000 bounds */
    m_dcb->motor_run(motor_calibration_percent);    /* Start the motor running */
    last_elapsed = 0;
    qDebug() << "Started motor speed check";
    ui->sensor_reading->setRange(0, 5);
}

void Meds_dispensary::set_status_label(QString info) {
    ui->status_label->setText(info);
    ui->status_label->repaint();
}
void Meds_dispensary::set_arena_to_safe_pos(){
    m_dcb->motor_run(30);
    while(m_dcb->position() < 90);
    m_dcb->motor_run(-30);
    while(m_dcb->position() != 45);
    m_dcb->motor_stop();
}

void Meds_dispensary::sweep_limbs(int rotate_degrees) {
    m_dca->motor_run(20);
    while(m_dca->position() < rotate_degrees);
    m_dca->motor_stop();
}

void Meds_dispensary::disc_sensor_proc(int trigger_value) {
    unsigned int this_reading = trigger_value;

//    qDebug() << "Current sensor reading = " << this_reading;
    if (this_reading < (120)) {    /* sensor sees something different */
        qDebug() << "Saw sensor_change = " << this_reading;
        if (!found_index) {                         /* Found the index for the first time */
            found_index = true;
            index_time.start();
            rev_count = 0;
            ui->sensor_reading->setValue(rev_count);
        } else {
            int this_elapsed = index_time.elapsed();
            qDebug() << "elapsed_time = " << this_elapsed;
            qDebug() << "Sensor high = " << highest_reading << "Sensor low = " << lowest_reading << "Sensor last = " << last_disc_sensor_reading;
            if ((this_elapsed - last_elapsed) > 500) {  /* Avoid duplicate readings of index strip */
                ++rev_count;
                ui->sensor_reading->setValue(rev_count);
                qDebug() << "rev_count = " << rev_count;
                formulary_rpm = (float)rev_count/(((float)this_elapsed / 1000 / 60)) /
                            ((float)motor_calibration_percent/100.0); /* milliseconds to minutes */
                if (rev_count > 4) {
                    qDebug() << "Current formulary disc RPM = " << formulary_rpm;
                    m_dcb->motor_stop();
                    sr2->set_read_interval(-1); /* This stops the Index polling */
                } /* endif */
                last_elapsed = this_elapsed;
            } /* endif */
        } /* endif */
    } /* endif */
    last_disc_sensor_reading = this_reading;
} /* disc_sensor_read */
