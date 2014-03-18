#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"

Meds_dispensary::Meds_dispensary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Meds_dispensary)
{
    ui->setupUi(this);
    m_dca = new motor(e_motor_id::e_motor_a);
    m_dcb = new motor(e_motor_id::e_motor_b);
    sr1 = new gpio_sensor("P9_25", "P9_39");
    sr2 = new gpio_sensor("P9_27", "P9_40");
}

Meds_dispensary::~Meds_dispensary()
{
    delete sr1;
    delete sr2;
    delete m_dcb;
    delete m_dca;
    delete ui;
}
