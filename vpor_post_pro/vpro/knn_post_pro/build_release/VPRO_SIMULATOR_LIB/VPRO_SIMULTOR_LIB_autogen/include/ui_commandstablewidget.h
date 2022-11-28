/********************************************************************************
** Form generated from reading UI file 'commandstablewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMANDSTABLEWIDGET_H
#define UI_COMMANDSTABLEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_commandstablewidget
{
public:

    void setupUi(QWidget *commandstablewidget)
    {
        if (commandstablewidget->objectName().isEmpty())
            commandstablewidget->setObjectName(QString::fromUtf8("commandstablewidget"));
        commandstablewidget->resize(547, 374);

        retranslateUi(commandstablewidget);

        QMetaObject::connectSlotsByName(commandstablewidget);
    } // setupUi

    void retranslateUi(QWidget *commandstablewidget)
    {
        commandstablewidget->setWindowTitle(QCoreApplication::translate("commandstablewidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class commandstablewidget: public Ui_commandstablewidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMANDSTABLEWIDGET_H
