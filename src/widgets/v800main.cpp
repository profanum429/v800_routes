#include "v800main.h"
#include "ui_v800main.h"

v800main::v800main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::v800main)
{
    ui->setupUi(this);
}

v800main::~v800main()
{
    delete ui;
}
