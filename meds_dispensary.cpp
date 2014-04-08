#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"
#include "dispensary_fsm_sm.h"
#include <QFontDatabase>
#include <QtCore/qdebug.h>

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
    QTimer::singleShot(1000, this, SLOT(dispense_start()));

//    QFontDatabase fonts;
//    qDebug() << "fonts" << fonts.families();

}

Meds_dispensary::~Meds_dispensary() {
    delete sr1;
    delete sr2;
    delete m_dcb;
    delete m_dca;
    delete ui;
}

void Meds_dispensary::dispense_start(void) {
    dispense_fsm.enterStartState();
}

void Meds_dispensary::set_motor_a_speed(int speed) {
    m_dca->motor_run(speed);
}

void Meds_dispensary::set_motor_b_speed(int speed) {
    m_dcb->motor_run(speed);
}

void Meds_dispensary::set_status_label(QString info) {
    ui->status_label->setText(info);
    ui->status_label->repaint();
}
void Meds_dispensary::set_arena_to_safe_pos(){
    m_dcb->motor_run(25);
    while(m_dcb->position() < 90);
    m_dcb->motor_run(-20);
    while(m_dcb->position() != 45);
    m_dcb->motor_stop();
}

void Meds_dispensary::sweep_limbs(int rotate_degrees) {
    m_dca->motor_run(20);
    while(m_dca->position() < rotate_degrees);
    m_dca->motor_stop();
}
