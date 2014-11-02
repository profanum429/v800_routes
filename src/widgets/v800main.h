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

#ifndef V800MAIN_H
#define V800MAIN_H

#include <QWidget>

namespace Ui {
class v800main;
}

class protobuf;
class QProgressDialog;

class v800main : public QWidget
{
    Q_OBJECT

public:
    explicit v800main(QWidget *parent = 0);
    ~v800main();

signals:
    void to_bpb(QString src_file, QString name);
    void to_kml(QString src_file);

private slots:
    void on_to_bpb_clicked();

    void on_to_kml_clicked();

    void handle_bpb_done();
    void handle_bpb_failure();

    void handle_kml_done();
    void handle_kml_failure();

private:
    Ui::v800main *ui;

    protobuf *protobuf_parser;

    QProgressDialog *conv_progress;
};

#endif // V800MAIN_H
