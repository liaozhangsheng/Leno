#pragma once

#include <ElaWindow.h>

#include <QMainWindow>

#include "T_BasicAlgorithm.h"
#include "T_Custom.h"
#include "T_Home.h"
#include "T_YOLO.h"


class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    void _init_window();
    void _init_content();

    T_Home* homePage_{nullptr};
    T_BasicAlgorithm* basicAlgorithm_{nullptr};
    T_YOLO* yolo_{nullptr};
    T_Custom* custom_{nullptr};

    QString aboutKey_{""};
    QString settingKey_{""};
};
