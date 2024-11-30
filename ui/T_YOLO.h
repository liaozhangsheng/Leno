#pragma once

#include <QWidget>
#include <QtNodes/DataFlowGraphModel>


class T_YOLO : public QWidget {
    Q_OBJECT
public:
    explicit T_YOLO(QWidget *parent = nullptr);
    ~T_YOLO();

signals:

private:
    QtNodes::DataFlowGraphModel *dataFlowGraphModel_{nullptr};
};
