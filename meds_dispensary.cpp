#include "meds_dispensary.h"
#include "ui_meds_dispensary.h"

Meds_dispensary::Meds_dispensary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Meds_dispensary)
{
    ui->setupUi(this);
}

Meds_dispensary::~Meds_dispensary()
{
    delete ui;
}
