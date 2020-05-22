#ifndef PROYECTO_H
#define PROYECTO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class proyecto; }
QT_END_NAMESPACE

class proyecto : public QMainWindow
{
    Q_OBJECT

public:
    proyecto(QWidget *parent = nullptr);
    ~proyecto();

private slots:
    void on_pushButton_clicked();

    void on_lcdvol_overflow();

private:
    Ui::proyecto *ui;
};
#endif // PROYECTO_H
