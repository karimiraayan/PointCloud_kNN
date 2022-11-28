#ifndef SAVESETTINGS_H
#define SAVESETTINGS_H

#include <QDir>
#include <QSettings>
#include <QVector>
#include <QList>
#include <QCheckBox>
#include <QWidget>
#include <QLineEdit>
#include <QDebug>



void savesettings(QVector<QLineEdit*> offsets,QVector<QLineEdit*> sizes,QList<QCheckBox *> checkedtorestore);

void restoresettings(QVector<QLineEdit*> offsets,QVector<QLineEdit*> sizes,QList<QCheckBox *> checkedtorestore);

#endif // SAVESETTINGS_H
