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

#include "protobuf.h"
#include "proute.pb.h"

#include <QFile>
#include <QDir>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QDateTime>

#include <math.h>

protobuf::protobuf(QObject *parent) :
    QObject(parent)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void protobuf::start()
{
}

void protobuf::handle_to_bpb(QString src_file, QString name)
{
    QXmlQuery points;
    QStringList lats, lons;
    QString output;
    QByteArray data;
    qint64 rand_id;
    qint32 ecosystem;

    qDebug("Source: %s", src_file.toLatin1().constData());

    QFile src(src_file);
    src.open(QFile::ReadOnly);

    if(src_file.contains(tr(".gpx")))
    {
        points.setFocus(&src);
        points.setQuery(tr("declare default element namespace \"http://www.topografix.com/GPX/1/1\";//trkpt/@lat/string()"));
        if(points.isValid())
            points.evaluateTo(&lats);

        points.setQuery(tr("declare default element namespace \"http://www.topografix.com/GPX/1/1\";//trkpt/@lon/string()"));
        if(points.isValid())
            points.evaluateTo(&lons);
    }
    else if(src_file.contains(tr(".kml")))
    {
        QString coords_raw, coords;

        points.setFocus(&src);
        points.setQuery(tr("declare default element namespace \"http://www.opengis.net/kml/2.2\";//coordinates/string()"));
        if(points.isValid())
            points.evaluateTo(&coords_raw);

        coords = coords_raw.simplified();

        QStringList each_pt = coords.split(tr(" "));
        for(int cnt = 0; cnt < each_pt.length(); cnt++)
        {
            lons.append(each_pt.at(cnt).split(tr(",")).at(0));
            lats.append(each_pt.at(cnt).split(tr(",")).at(1));
        }
    }

    src.close();

    data::PbPlannedRoute proute;
    proute.mutable_name()->set_text(name.toLatin1().constData());

    rand_id = (qint64)qrand() << 32 | qrand();
    proute.mutable_route_id()->set_value(rand_id);

    double initial_lat, initial_lon, lat, lon;
    int x, y;
    for(int cnt = 0; cnt < lats.length(); cnt++)
    {
        if(cnt == 0)
        {
            initial_lat = lats.at(cnt).toDouble();
            initial_lon = lons.at(cnt).toDouble();

            data::PbLocation *start_location = proute.start_location().New();
            start_location->set_latitude(initial_lat);
            start_location->set_longitude(initial_lon);
            proute.set_allocated_start_location(start_location);
        }

        lat = lats.at(cnt).toDouble();
        lon = lons.at(cnt).toDouble();

        y = (int)((lat - initial_lat) * 6378137. * acos(-1)/180.);
        x = (int)(6378137. * (lon - initial_lon) * (acos(-1)/180) * cos(initial_lat * acos(-1) / 180.));

        data::PbRoutePoint *point = proute.add_point();
        point->set_x_offset(x);
        point->set_y_offset(y);
        point->set_z_offset(0);
    }

    data.clear();
    data.append(proute.SerializeAsString().data(), proute.ByteSize());

    QString bpb_dir =  QString(tr("%1/%2").arg(QFileInfo(src).absolutePath()).arg(QFileInfo(src).completeBaseName()));
    if(!QDir().mkdir(bpb_dir))
    {
        if(!QDir(bpb_dir).exists())
        {
            emit to_bpb_failure();
            return;
        }
    }

    output.clear();
    output = QString(tr("%1/PROUTE.BPB").arg(bpb_dir));
    QFile proute_file(output);
    proute_file.open(QFile::WriteOnly);
    proute_file.write(data);
    proute_file.close();

    data::PbIdentifier id;
    ecosystem = (qint64)qrand() << 32 | qrand();
    id.set_ecosystem_id(ecosystem);

    QDateTime cur_datetime = QDateTime::currentDateTimeUtc();

    data::PbSystemDateTime *time_date = id.mutable_created();
    data::PbDate *date = time_date->mutable_date();
    data::PbTime *time = time_date->mutable_time();

    date->set_day(cur_datetime.date().day());
    date->set_month(cur_datetime.date().month());
    date->set_year(cur_datetime.date().year());
    time->set_hour(cur_datetime.time().hour());
    time->set_minute(cur_datetime.time().minute());
    time->set_seconds(cur_datetime.time().second());
    time->set_millis(0);

    time_date->set_trusted(false);

    time_date = id.mutable_last_modified();
    date = time_date->mutable_date();
    time = time_date->mutable_time();

    date->set_day(cur_datetime.date().day());
    date->set_month(cur_datetime.date().month());
    date->set_year(cur_datetime.date().year());
    time->set_hour(cur_datetime.time().hour());
    time->set_minute(cur_datetime.time().minute());
    time->set_seconds(cur_datetime.time().second());
    time->set_millis(0);

    time_date->set_trusted(false);

    data.clear();
    data.append(id.SerializeAsString().data(), id.ByteSize());

    output.clear();
    output = QString(tr("%1/ID.BPB").arg(bpb_dir));
    QFile id_file(output);
    id_file.open(QFile::WriteOnly);
    id_file.write(data);
    id_file.close();

    data::PbTrainingSessionTarget tst;
    data::PbOneLineText *tst_name = tst.mutable_name();
    tst_name->set_text(name.toLatin1().constData());

    data::PbExerciseTarget *target = tst.add_exercise_target();
    target->set_target_type(data::PbExerciseTarget::EXERCISE_TARGET_TYPE_ROUTE);

    data::PbSportIdentifier *sport_id = target->mutable_sport_id();
    sport_id->set_value(1);

    data::PbPhases *phases = target->mutable_phases();

    data::PbRouteId *route_id = target->mutable_route();
    route_id->set_value(rand_id);

    data.clear();
    data.append(tst.SerializeAsString().data(), tst.ByteSize());

    output.clear();
    output = QString(tr("%1/TST.BPB").arg(bpb_dir));
    QFile tst_file(output);
    tst_file.open(QFile::WriteOnly);
    tst_file.write(data);
    tst_file.close();

    emit to_bpb_done();
}

void protobuf::handle_to_kml(QString src_file)
{
    QFile src(src_file);
    src.open(QFile::ReadOnly);

    data::PbPlannedRoute proute;
    proute.ParseFromFileDescriptor(src.handle());
    src.close();

    QString output;
    output = src_file.toLower().replace(tr(".bpb"), tr(".kml"));

    QFile out_file(output);
    out_file.open(QFile::WriteOnly);
    out_file.write(QString(tr("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n<Document>\n<name>Planned Route</name>\n<Style id=\"yellowLineGreenPoly\">\n<LineStyle>\n<color>7f00ffff</color>\n<width>4</width>\n</LineStyle>\n<PolyStyle>\n<color>7f00ff00</color>\n</PolyStyle>\n</Style>\n<Placemark>\n<name>Route</name>\n<styleUrl>#yellowLineGreenPoly</styleUrl>\n<LineString>\n<extrude>1</extrude>\n<tessellate>1</tessellate>\n<altitudeMode>absolute</altitudeMode>\n<coordinates>\n")).toLocal8Bit());

    double p_lat, p_lon, lat_offset, lon_offset;

    for(int cnt = 0; cnt < proute.point_size(); cnt++)
    {
        lat_offset = proute.point(cnt).y_offset() / 6378137.;
        lon_offset = proute.point(cnt).x_offset() / (6378137. * cos(acos(-1)*proute.start_location().latitude()/180.));

        p_lat = proute.start_location().latitude() + (lat_offset * (180/acos(-1)));
        p_lon = proute.start_location().longitude() + (lon_offset * (180/acos(-1)));

        out_file.write(QString(tr("%1,%2,0\n")).arg(p_lon, 0, 'g', 10).arg(p_lat, 0, 'g', 10).toLatin1());
    }

    out_file.write(QString(tr("</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>")).toLatin1());

    out_file.close();

    emit to_kml_done();
}
