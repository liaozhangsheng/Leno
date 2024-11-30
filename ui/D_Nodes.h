#pragma once

#include "ElaTreeView.h"
#include <QObject>

class D_NodesModel;
class D_Nodes : public QObject {
    Q_OBJECT

public:
    explicit D_Nodes(QObject* parent = nullptr);
    ~D_Nodes() = default;

private:
    ElaTreeView *view_;
};