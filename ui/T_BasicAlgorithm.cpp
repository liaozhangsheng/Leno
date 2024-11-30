#include <ElaIcon.h>
#include <ElaMenu.h>
#include <ElaMenuBar.h>

#include <QVBoxLayout>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "../src/nodes/ImageLoaderNode.hpp"
#include "../src/nodes/ImageShowNode.hpp"
#include "../src/nodes/VideoLoaderNode.hpp"
#include "../src/nodes/base/ThresholdNode.hpp"
#include "../src/nodes/yolo/YOLONode.hpp"
#include "T_BasicAlgorithm.h"


static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels() {
    auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

    ret->registerModel<YOLONode>("目标检测");
    ret->registerModel<ThresholdNode>("基本处理");
    ret->registerModel<ImageShowNode>("输入输出");
    ret->registerModel<ImageLoaderNode>("输入输出");
    ret->registerModel<VideoLoaderNode>("输入输出");

    return ret;
}

T_BasicAlgorithm::T_BasicAlgorithm(QWidget *parent) : QWidget{parent} {
    this->setWindowTitle("基础算法");

    // 菜单栏
    auto *menuBar = new ElaMenuBar(this);
    menuBar->setFixedHeight(20);

    ElaMenu *iconMenu = menuBar->addMenu(ElaIconType::FileSignature, "文件");
    iconMenu->setMenuItemHeight(30);
    iconMenu->addElaIconAction(ElaIconType::FolderOpen, "打开");
    iconMenu->addElaIconAction(ElaIconType::LaptopFile, "保存");

    // 画布
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry_ = registerDataModels();
    this->dataFlowGraphModel_ = new QtNodes::DataFlowGraphModel(registry_);
    auto *scene_ = new QtNodes::DataFlowGraphicsScene(*dataFlowGraphModel_, this);
    auto *view_ = new QtNodes::GraphicsView(scene_, this);

    auto *vlayout_ = new QVBoxLayout(this);
    vlayout_->setContentsMargins(0, 5, 0, 0);
    vlayout_->setSpacing(5);
    vlayout_->addWidget(menuBar);
    vlayout_->addWidget(view_);
    vlayout_->addStretch();
}

T_BasicAlgorithm::~T_BasicAlgorithm() {
    delete this->dataFlowGraphModel_;
}
