#include "v800main.h"
#include "ui_v800main.h"

#include <QFileDialog>
#include <QStandardPaths>

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

void v800main::on_to_bpb_clicked()
{
    QString src_file = QFileDialog::getOpenFileName(this, tr("Open Route"), QStandardPaths::displayName(QStandardPaths::DocumentsLocation), tr("Route Files (*.gpx *.kml)"));

    if(!src_file.isEmpty())
        emit to_bpb(src_file);
}

void v800main::on_to_kml_clicked()
{
    QString src_file = QFileDialog::getOpenFileName(this, tr("Open Route"), QStandardPaths::displayName(QStandardPaths::DocumentsLocation), tr("Route Files (*.bpb)"));

    if(!src_file.isEmpty())
        emit to_kml(src_file);
}
