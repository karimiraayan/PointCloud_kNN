#ifndef RFTABLEVIEW_H
#define RFTABLEVIEW_H


#include <QWidget>
#include <QDebug>
#include <QStandardItemModel>
#include <QLabel>
#include <QTableView>
#include <QAbstractTableModel>

class RfTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RfTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override; // for editing
    int basevalue=10;
    int *base= &basevalue;
    void setRegisterFile(uint8_t *ref);

    static const int COLS = 4;
    static const int ROWS = 1024 / COLS;

private:
    uint8_t *rf;
    bool gotRfRef;
//    QString m_gridData[ROWS][COLS];  //holds text entered into QTableView

signals:
    void editCompleted(const QString &);
};



namespace Ui {
class RfTableView;
}

class RfTableView : public QTableView
{
    Q_OBJECT
public:
    explicit RfTableView(QWidget *parent = nullptr);
    ~RfTableView();
    RfTableModel *modelData;

public slots:
    void setRegisterFile(int lane, uint8_t *ref);
    void selectRegisterFile(int lane);

private:
    QVector<uint8_t* > rf_ref;
    int lane;

};


#endif // RFTABLEVIEW_H
