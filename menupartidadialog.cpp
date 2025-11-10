#include "menupartidadialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

MenuPartidaDialog::MenuPartidaDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Tipo de partida");
    setModal(true);
    setFixedSize(300,160);

    auto* layout = new QVBoxLayout(this);
    auto* lbl = new QLabel("¿Qué deseas hacer?");
    layout->addWidget(lbl, 0, Qt::AlignHCenter);

    btnNueva  = new QPushButton("Iniciar partida nueva", this);
    btnCargar = new QPushButton("Cargar partida", this);
    layout->addWidget(btnNueva);
    layout->addWidget(btnCargar);

    connect(btnNueva,  &QPushButton::clicked, this, [this]{ accept();  }); // devuelve Accepted
    connect(btnCargar, &QPushButton::clicked, this, [this]{ reject();  }); // devuelve Rejected
}
