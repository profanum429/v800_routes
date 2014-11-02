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

#ifndef PROTOBUF_H
#define PROTOBUF_H

#include <QObject>

class protobuf : public QObject
{
    Q_OBJECT
public:
    explicit protobuf(QObject *parent = 0);

signals:
    void to_bpb_done();
    void to_bpb_failure();

    void to_kml_done();
    void to_kml_failure();

public slots:
    void start();
    void handle_to_bpb(QString src_file, QString name);
    void handle_to_kml(QString src_file);

};

#endif // PROTOBUF_H
