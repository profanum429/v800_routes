#ifndef V800MAIN_H
#define V800MAIN_H

#include <QWidget>

namespace Ui {
class v800main;
}

class v800main : public QWidget
{
    Q_OBJECT

public:
    explicit v800main(QWidget *parent = 0);
    ~v800main();

signals:
    void to_bpb(QString src_file);
    void to_kml(QString src_file);

private slots:
    void on_to_bpb_clicked();

    void on_to_kml_clicked();

private:
    Ui::v800main *ui;
};

#endif // V800MAIN_H
