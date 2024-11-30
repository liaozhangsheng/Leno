#include <ElaDockWidget.h>
#include <ElaMenu.h>
#include <ElaMenuBar.h>
#include <ElaTabWidget.h>
#include <ElaToolBar.h>
#include <ElaToolButton.h>
#include <ElaTreeView.h>
#include <qkeysequence.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtNodes/GraphicsView>
#include <QtNodes/internal/DataFlowGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>

#include "../src/models/NodeModel.hpp"
#include "Def.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : LWindow(parent) {
    this->_init_window();
    this->_init_content();

    this->moveToCenter();
}

void MainWindow::_init_window() {
    this->setWindowTitle("Leno");
    this->setWindowIcon(QIcon(":/favicon.svg"));
    this->resize(1080, 720);
}

void MainWindow::_init_content() {
    // 菜单栏
    auto* menu_ = new ElaMenuBar(this);
    menu_->setFixedHeight(20);
    auto* widget_menu = new QWidget(this);
    auto* vlayout_menu = new QVBoxLayout(widget_menu);
    vlayout_menu->setContentsMargins(20, 5, 0, 0);
    vlayout_menu->addWidget(menu_);
    vlayout_menu->addStretch();
    this->setCustomWidget(ElaAppBarType::LeftArea, widget_menu);
    this->setCustomWidgetMaximumWidth(400);

    ElaMenu* menu_file = menu_->addMenu(ElaIconType::Files, "文件");
    menu_file->setMenuItemHeight(20);
    auto* new_action = menu_file->addElaIconAction(ElaIconType::FilePlus, " 新建 ");
    auto* load_action = menu_file->addElaIconAction(ElaIconType::FolderPlus, " 打开 ");

    menu_file->addElaIconAction(ElaIconType::MemoCircleCheck, " 保存 ");
    menu_file->addElaIconAction(ElaIconType::BookCopy, "另存为");

    menu_->addElaIconAction(ElaIconType::Gear, "设置");
    menu_->addElaIconAction(ElaIconType::CircleQuestion, "帮助");

    // 节点dock
    auto* dock_nodes = new ElaDockWidget("Nodes", this);
    dock_nodes->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock_nodes->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock_nodes->setWindowIcon(QIcon(":/res/node.svg"));

    auto* tree_nodes = new ElaTreeView();
    tree_nodes->setHeaderHidden(true);
    tree_nodes->setModel(NodeModel::instance().getNodeTreeModel());
    dock_nodes->setWidget(tree_nodes);

    // 节点详情dock
    auto* dock_details = new ElaDockWidget("Details", this);
    dock_details->setAllowedAreas(Qt::RightDockWidgetArea);
    dock_details->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock_details->setWindowIcon(QIcon(":/res/detail.svg"));

    // 画布
    auto* tab_graph = new ElaTabWidget(this);
    auto* scene = new QtNodes::DataFlowGraphicsScene(NodeModel::instance().graphModel(), tab_graph);
    auto* graph_view = new QtNodes::GraphicsView(scene, tab_graph);
    tab_graph->addTab(graph_view, "Graph");

    // BUG: 为什么新建画布会出错？
    // connect(new_action, &QAction::triggered, this, [this, tab_graph]() {
    //     auto *scene = new QtNodes::DataFlowGraphicsScene(NodeModel::instance().graphModel(),
    //     tab_graph); auto *graph_view = new QtNodes::GraphicsView(scene, tab_graph);
    //     tab_graph->addTab(graph_view, "Graph");
    // });

    this->addDockWidget(Qt::LeftDockWidgetArea, dock_nodes);
    this->setCentralWidget(tab_graph);
    this->addDockWidget(Qt::RightDockWidgetArea, dock_details);
    resizeDocks({dock_nodes, dock_details}, {200, 200}, Qt::Horizontal);
}