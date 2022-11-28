#ifndef COMMANDSTABLEWIDGET_H
#define COMMANDSTABLEWIDGET_H

#include <QWidget>
#include "../../../model/commands/CommandVPRO.h"
#include <QMainWindow>
#include <QObject>
#include <QTableWidgetItem>
#include <QVariant>
#include<QTableWidget>
#include <QLayout>


namespace Ui {
class commandstablewidget;
}

class commandstablewidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit commandstablewidget(QWidget *parent = nullptr,std::shared_ptr<CommandVPRO> cmd= nullptr);
    ~commandstablewidget();
    QVector<QVector<QTableWidgetItem*>> tabledata;
public slots:
    void closewindow();
private:
    Ui::commandstablewidget *ui;
    void createtable();
};

#endif // COMMANDSTABLEWIDGET_H
