#pragma once

#include "./LWindow/LWindow.h"

#include <QMainWindow>

class MainWindow : public LWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    void _init_window();
    void _init_content();
};