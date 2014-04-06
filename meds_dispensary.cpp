#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"
#include "dispensary_fsm_sm.h"
#include <QFontDatabase>
#include <QtCore/qdebug.h>

Meds_dispensary::Meds_dispensary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Meds_dispensary),
    dispense_fsm(*this)
 {

    ui->setupUi(this);
    m_dca = new motor(e_motor_id::e_motor_a);
    m_dcb = new motor(e_motor_id::e_motor_b);
    sr1 = new gpio_sensor("P9_25", "P9_39");
    sr2 = new gpio_sensor("P9_27", "P9_40");
    dispense_fsm.enterStartState();

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

void Meds_dispensary::set_motor_a_speed(int speed) {
    m_dca->motor_run(speed);
}

void Meds_dispensary::set_motor_b_speed(int speed) {
    m_dcb->motor_run(speed);
}

void Meds_dispensary::set_status_label(QString info) {
    ui->status_label->setText(info);
}
