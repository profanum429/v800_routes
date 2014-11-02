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
#include <QXmlQuery>
#include <QXmlResultItems>

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

        output = src_file.replace(tr(".gpx"), tr(".bpb"));
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

        output = src_file.replace(tr(".kml"), tr(".bpb"));
    }

    src.close();

    data::PbPlannedRoute proute;
    proute.mutable_name()->set_text(name.toLatin1().constData());
    proute.mutable_route_id()->set_value(1000000);

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

    QByteArray data;
    data.append(proute.SerializeAsString().data(), proute.ByteSize());

    if(!output.isEmpty())
    {
        QFile out_file(output);
        out_file.open(QFile::WriteOnly);
        out_file.write(data);
        out_file.close();
    }
    else
    {
        emit to_bpb_failure();
    }

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
    output = src_file.replace(tr(".bpb"), tr(".kml"));

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
