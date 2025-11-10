#ifndef MENUPARTIDADIALOG_H
#define MENUPARTIDADIALOG_H

#include <QDialog>

class QPushButton;

class MenuPartidaDialog : public QDialog
{
    Q_OBJECT
public:
    enum Resultado { Nueva = QDialog::Accepted, Cargar = QDialog::Rejected };
    explicit MenuPartidaDialog(QWidget* parent = nullptr);

private:
    QPushButton* btnNueva = nullptr;
    QPushButton* btnCargar = nullptr;
};

#endif // MENUPARTIDADIALOG_H
