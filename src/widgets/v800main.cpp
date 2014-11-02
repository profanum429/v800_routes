/*
    Copyright 2014 Christian Weber

    This file is part of V800 Routes.

    V800 Routes is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    V800 Downloader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with V800 Routes.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "v800main.h"
#include "ui_v800main.h"
#include "protobuf.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QDir>
#include <QThread>
#include <QProgressDialog>
#include <QMessageBox>

v800main::v800main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::v800main)
{
    QThread *protobuf_thread = new QThread;
    protobuf_parser = new protobuf();
    protobuf_parser->moveToThread(protobuf_thread);

    connect(this, SIGNAL(to_bpb(QString,QString)), protobuf_parser, SLOT(handle_to_bpb(QString,QString)));
    connect(this, SIGNAL(to_kml(QString)), protobuf_parser, SLOT(handle_to_kml(QString)));

    connect(protobuf_parser, SIGNAL(to_bpb_done()), this, SLOT(handle_bpb_done()));
    connect(protobuf_parser, SIGNAL(to_bpb_failure()), this, SLOT(handle_bpb_failure()));
    connect(protobuf_parser, SIGNAL(to_kml_done()), this, SLOT(handle_kml_done()));
    connect(protobuf_parser, SIGNAL(to_kml_failure()), this, SLOT(handle_kml_failure()));

    connect(protobuf_thread, SIGNAL(started()), protobuf_parser, SLOT(start()));
    protobuf_thread->start();

    ui->setupUi(this);
}

v800main::~v800main()
{
    delete ui;
}

void v800main::handle_bpb_done()
{
    conv_progress->done(0);

    QString msg_text(QString(tr("Done converting to BPB")));

    QMessageBox done;
    done.setWindowTitle(tr("V800 Routes"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();
}

void v800main::handle_bpb_failure()
{
    conv_progress->done(0);

    QString msg_text(QString(tr("Failed to convert to BPB")));

    QMessageBox done;
    done.setWindowTitle(tr("V800 Routes"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();
}

void v800main::handle_kml_done()
{
    conv_progress->done(0);

    QString msg_text(QString(tr("Done converting to KML")));

    QMessageBox done;
    done.setWindowTitle(tr("V800 Routes"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();
}

void v800main::handle_kml_failure()
{
    conv_progress->done(0);

    QString msg_text(QString(tr("Failed to convert to KML")));

    QMessageBox done;
    done.setWindowTitle(tr("V800 Routes"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();
}

void v800main::on_to_bpb_clicked()
{
    bool name_ok;
    QString name;

    QString src_file = QFileDialog::getOpenFileName(this, tr("Open Route"), QDir::homePath(), tr("Route Files (*.gpx *.kml)"));
    name = QInputDialog::getText(this, tr("Route Name"), tr("Route Name:"), QLineEdit::Normal, tr(""), &name_ok);

    if(!src_file.isEmpty() && !name.isEmpty() && name_ok)
    {
        conv_progress = new QProgressDialog(tr("Converting to BPB..."), tr("Cancel"), 0, 1, this);
        conv_progress->setCancelButton(0);
        conv_progress->setWindowModality(Qt::WindowModal);
        conv_progress->setValue(0);
        conv_progress->setWindowTitle(tr("V800 Routes"));
        conv_progress->show();

        emit to_bpb(src_file, name);
    }
}

void v800main::on_to_kml_clicked()
{
    QString src_file = QFileDialog::getOpenFileName(this, tr("Open Route"), QDir::homePath(), tr("Route Files (*.bpb)"));

    if(!src_file.isEmpty())
    {
        conv_progress = new QProgressDialog(tr("Converting to KML..."), tr("Cancel"), 0, 1, this);
        conv_progress->setCancelButton(0);
        conv_progress->setWindowModality(Qt::WindowModal);
        conv_progress->setValue(0);
        conv_progress->setWindowTitle(tr("V800 Routes"));
        conv_progress->show();

        emit to_kml(src_file);
    }
}
