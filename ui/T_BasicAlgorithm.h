#pragma once

#include <QWidget>
#include <QtNodes/DataFlowGraphModel>


class T_BasicAlgorithm : public QWidget {
    Q_OBJECT
public:
    explicit T_BasicAlgorithm(QWidget *parent = nullptr);
    ~T_BasicAlgorithm();

signals:

private:
    QtNodes::DataFlowGraphModel *dataFlowGraphModel_{nullptr};
};
