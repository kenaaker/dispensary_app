#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"
#include <formulary_fsm_sm.h>
#include <QFontDatabase>
#include <QProgressBar>
#include <QtCore/qdebug.h>
#include <QTime>
#include <algorithm>

Meds_dispensary::Meds_dispensary(QWidget *parent) :
    QWidget(parent),
    formulary_fsm(*this),
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
    first_pulse = true;
    current_slot = 0;

    if (!cmd_server.listen(QHostAddress::Any, 45046)) {
       abort();
    } /* endif */
    connect(&cmd_server, SIGNAL(command_received(QString &)), this, SLOT(command_proc(QString &)));

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
    formulary_fsm.enterStartState();
    formulary_fsm.setDebugFlag(true);
    formulary_fsm.init_formulary();
}

void Meds_dispensary::set_up_dispensary_motor(void) {
    m_dca->motor_run(0);
}

const static int motor_calibration_percent=30;

void Meds_dispensary::set_up_formulary_motor(void) {

    m_dcb->motor_run(0);
    for (int i = 0; i<num_bottles+2; ++i) {
        pulse_times[i] = 0;
    }
    pulse_count = 0;
    mean_elapsed_sector_time = -1;                /* 'ware, hackery */
    /* Check the hardware, first the position sensor on the formulary wheel */
    sr2->set_read_interval(75);
    connect(sr2, SIGNAL(adc_signal(int)), this, SLOT(disc_sensor_proc(int)));
    sr2->set_bounds(00,150);
    sr2->set_in_or_out(false);                      /* Send signal if out of 0->100 bounds */
    pulse_time.start();
    m_dcb->motor_run(motor_calibration_percent);    /* Start the motor running */
    connect(m_dcb, SIGNAL(movement_done(int)), this, SLOT(movement_done_proc(int)));
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

bool Meds_dispensary::stable_sensor_pulses() {

    bool return_value = false;
    if ((pulse_count == 0) || (pulse_count % (num_bottles + 2) != 0)) {
        return return_value;
    } else {
        int deltas[num_bottles+1];
        for (int i=0; i<num_bottles+1; ++i) {
            int this_delta = pulse_times[i+1] - pulse_times[i];
            deltas[i] = this_delta;
        } /* endfor */
        /* First lets get an average and std deviation of the pulse times */
        /* There should be one less delta, to allow for dropping out the double marker */
        std::vector<double> pts;
        for (int i = 0; i < (num_bottles+1); ++i) {
            if (deltas[i] > 500) {
                pts.push_back(deltas[i]);
//                qDebug() << "pts[" << pts.size()-1 << "] = " << pts[pts.size()-1];
            } /* endif */
        } /* endfor */
        double sum = std::accumulate(pts.begin(), pts.end(), 0.0);
        double mean = sum / pts.size();

        std::vector<double> pts_diff(pts.size());
        std::transform(pts.begin(), pts.end(), pts_diff.begin(),
                       std::bind2nd(std::minus<double>(), mean));
        double sq_sum = std::inner_product(pts_diff.begin(), pts_diff.end(), pts_diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / pts.size());
//        qDebug() << "Stats of pulse times mean =" << mean << "std deviation =" << stdev;
        return_value = stdev < 500.0;
        if (return_value) {
            /* Now calculate RPM */
            /* Make the rpm a little low to allows the sector markers to establish precedence */
            formulary_rpm = (double)(1.0/((double)mean/1000.0 * num_bottles) * 60) /((double)motor_calibration_percent / 100.0) * 0.85;
            mean_elapsed_sector_time = round(mean);
            qDebug() << "Calculated RPM is " <<  formulary_rpm << "Mean sector elapsed time is " << mean_elapsed_sector_time;
            m_dcb->set_motor_rotation_speed(formulary_rpm);
        } /* endif */
        return return_value;
    } /* endif */
}

/* Finish setting up disk characterization parms like RPM and current position */
void Meds_dispensary::disk_index_setup() {

    int deltas[num_bottles+1];
    for (int i=0; i<num_bottles+1; ++i) {
        int this_delta = pulse_times[i+1] - pulse_times[i];
        deltas[i] = this_delta;
    } /* endfor */

    /* Figure out where the index mark is by looking for two pulses that are close together in time */
    m_dcb->motor_stop(); /* Stop the motor while we figure this out */
    /* Find index of minimal delta */
    int min_elem_value = 32767;
    int min_elem = 0;
    for (int i=0; i<num_bottles; ++i) {
//        qDebug() << "deltas[" << i << "] = " << deltas[i];
        if (deltas[i] < min_elem_value) {
            min_elem = i;
            min_elem_value = deltas[i];
        } /* endif */
    } /* endfor */
//    qDebug() << "min_elem=" << min_elem << "min_elem_value=" << min_elem_value;
    current_slot = ((pulse_count-2-min_elem) % num_bottles);
    qDebug() << "Current position is " << current_slot << "slots past index";
    m_dcb->set_motor_position((360/num_bottles)*current_slot);
    connect(keypad_sw5, SIGNAL(button_pushed(int)), this, SLOT(move_minus_1(int)));
    connect(keypad_sw3, SIGNAL(button_pushed(int)), this, SLOT(move_plus_1(int)));
    connect(keypad_sw1, SIGNAL(button_pushed(int)), this, SLOT(move_home(int)));
    connect(keypad_sw4, SIGNAL(button_pushed(int)), this, SLOT(move_dispense(int)));
}

void Meds_dispensary::move_home(int on_off) {
    if (on_off != '0') {
        formulary_fsm.move_cmd();
        move_disk(0);
    } /* endif */
}

void Meds_dispensary::move_dispense(int on_off) {
    if (on_off != '0') {
        formulary_fsm.move_cmd();
        m_dcb->motor_goto_pos(current_slot * (360/(num_bottles)) + ((360/num_bottles)/2), wheel_speed);
    } /* endif */
}

void Meds_dispensary::move_minus_1(int on_off) {
    if (on_off != '0') {
        move_disk_relative(-1);
    } /* endif */
}

void Meds_dispensary::move_plus_1(int on_off) {
    if (on_off != '0') {
        move_disk_relative(1);
    } /* endif */
}

void Meds_dispensary::disc_sensor_proc(int trigger_value) {
//    qDebug() << "Saw sensor_change = " << trigger_value;
    int8_t current_motor_speed = m_dcb->motor_get_speed();
    last_disc_sensor_reading = trigger_value;
    int pulse_time_at = modulo(pulse_count, (num_bottles+2));
    int prev_pulse_time_at = modulo((pulse_count-1), (num_bottles+2));
    unsigned int pulse_e_time = pulse_time.elapsed();
    unsigned int prev_pulse_e_time = pulse_times[prev_pulse_time_at];
    bool first_pulse_detected = false;

//    qDebug() << "disc_sensor_proc pulse_time at = " << pulse_time_at << "prev at" << prev_pulse_time_at <<
//                "prev_pulse_e_time" << prev_pulse_e_time << "pulse_e_time" << pulse_e_time << "delta" << (pulse_e_time - prev_pulse_e_time);

    pulse_times[pulse_time_at] = pulse_time.elapsed();
    m_dcb->motor_pause();
    /* Try to allow for two markers at the index area */
    int curr_pos = modulo(m_dcb->position(),360);
//    qDebug() << "disc_sensor_proc current position" << curr_pos;
    if ((curr_pos < 30) || (curr_pos > 330)) { /* Within general position */
        unsigned int sector_pulse_elapsed_time;
//        qDebug() << "disc_sensor_proc IN zone for double pulse";
        if (mean_elapsed_sector_time < 0) { /* We are still in setup */
            sector_pulse_elapsed_time = 500;
        } else {
            sector_pulse_elapsed_time = mean_elapsed_sector_time/2;
        } /* endif */
        if ((pulse_e_time - prev_pulse_e_time) < sector_pulse_elapsed_time) {
            /* Only adjust at second of two marks */
//            qDebug() << "disc_sensor_proc detected 2nd pulse of double pulse";
            if (current_motor_speed < 0) {
                current_slot = modulo((current_slot-1),num_bottles);
            } else {
                current_slot = modulo((current_slot+1),num_bottles);
            } /* endif */
        } else {
            first_pulse_detected = true;
//            qDebug() << "disc_sensor_proc detected first pulse of double pulse";
        } /* endif */
    } else {
        if (current_motor_speed < 0) {
            current_slot = modulo((current_slot-1),num_bottles);
        } else {
            current_slot = modulo((current_slot+1),num_bottles);
        } /* endif */
    } /* endif */
//    qDebug() << "disc_sensor_proc after current_slot =" << current_slot;
    /* Do not reset the position during setup or after the first pulse is detected */
    if ((mean_elapsed_sector_time > 0) || (first_pulse_detected)) {
        m_dcb->set_motor_position(modulo(current_slot*(360/num_bottles), 360));
    } /* endif */
//    qDebug() << "pulse_times["<< pulse_count << "]=" << pulse_times[pulse_count % (num_bottles+2)];
    ++pulse_count;
    formulary_fsm.sensor_pulse();
    ui->sensor_reading->setValue(modulo(pulse_count, (num_bottles)));
    m_dcb->motor_resume();
} /* disc_sensor_read */

void Meds_dispensary::move_disk(unsigned int slot_num) {
//    qDebug() << "starting move to slot = " << slot_num;
    ui->status_label->setText(QString("Moving to slot " + QString::number(slot_num)));
    destination_slot = slot_num;
    m_dcb->motor_goto_pos(slot_num*(360/(num_bottles)), wheel_speed);
}

void Meds_dispensary::move_disk_relative(int offset) {

    int working_slot = modulo((offset + current_slot), num_bottles);
//    qDebug() << "starting move to slot = " << working_slot;
    formulary_fsm.move_cmd();
    move_disk(working_slot);
}

void Meds_dispensary::movement_done_proc(int) {
//    qDebug() << "finished move to slot = " << destination_slot;
    formulary_fsm.movement_done();
} /* disc_sensor_read */

void Meds_dispensary::command_proc(QString &cmd) {
    qDebug() << "Got new command =\"" << cmd << "\"";
    if (cmd == "counter_clockwise_1") {
        move_minus_1(1);
    }
    if (cmd == "clockwise_1") {
        move_plus_1(1);
    }
    if (cmd == "home") {
        move_home(1);
    }
    if (cmd == "dispense") {
        move_dispense(1);
    }
} /* command proc */
