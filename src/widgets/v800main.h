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

private:
    Ui::v800main *ui;
};

#endif // V800MAIN_H
