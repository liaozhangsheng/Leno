#include "d_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    this->_init_window();
    this->_init_content();

    this->moveToCenter();
}

void MainWindow::_init_window()
{
    this->setWindowTitle("CV Nodes");
    this->setWindowIcon(QIcon(":/favicon.svg"));
    this->resize(1080, 720);
    this->setUserInfoCardVisible(false);
}

void MainWindow::_init_content()
{
    this->homePage_ = new T_Home(this);
    this->addPageNode("首页", this->homePage_, ElaIconType::House);
    this->basicAlgorithm_ = new T_BasicAlgorithm(this);
    this->addPageNode("基本处理", this->basicAlgorithm_, ElaIconType::HexagonImage);
    this->yolo_ = new T_YOLO(this);
    this->addPageNode("YOLO", this->yolo_, ElaIconType::BullseyeArrow);
    this->custom_ = new T_Custom(this);
    this->addPageNode("自定义", this->custom_, ElaIconType::PenNib);

    this->addFooterNode("关于", nullptr, this->aboutKey_, 0, ElaIconType::User);

    this->addFooterNode("设置", nullptr, this->settingKey_, 0, ElaIconType::GearComplex);
}
