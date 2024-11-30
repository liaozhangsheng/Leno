#include <ElaAcrylicUrlCard.h>
#include <ElaFlowLayout.h>
#include <ElaImageCard.h>
#include <ElaPopularCard.h>
#include <ElaScrollArea.h>
#include <ElaText.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "T_Home.h"

T_Home::T_Home(QWidget *parent) : ElaScrollPage{parent} {
    auto *widget_central = new QWidget(this);
    widget_central->setWindowTitle("主页");

    auto *card_background = new ElaImageCard(this);
    card_background->setCardImage(QImage(":/res/Home_Background.png"));

    // 卡片水平滚动区域
    auto *scrollArea_cards = new ElaScrollArea(card_background);
    scrollArea_cards->setWidgetResizable(true);
    scrollArea_cards->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_cards->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_cards->setIsGrabGesture(Qt::Horizontal, true);

    auto *scrollAreaWidget_cards = new QWidget(this);
    scrollAreaWidget_cards->setStyleSheet("background-color:transparent;");
    scrollArea_cards->setWidget(scrollAreaWidget_cards);

    auto *hlayout_background = new QHBoxLayout(card_background);
    hlayout_background->setContentsMargins(0, 0, 0, 0);
    hlayout_background->addWidget(scrollArea_cards);

    // 卡片
    auto *card_github = new ElaAcrylicUrlCard(this);
    card_github->setUrl("https://github.com/Liniyous/ElaWidgetTools");
    card_github->setCardPixmap(QPixmap(":/res/github.jpg"));
    card_github->setTitle("ElaTool Github");
    card_github->setSubTitle("Use ElaWidgetTools To Create A Cool Project");

    auto *card_author = new ElaAcrylicUrlCard(this);
    card_author->setUrl("https://space.bilibili.com/21256707");
    card_author->setCardPixmap(QPixmap(":/res/author.jpg"));
    card_author->setTitle("ElaWidgetTool");
    card_author->setSubTitle("8009963@qq.com");

    auto *card_qtnodes = new ElaAcrylicUrlCard(this);
    card_qtnodes->setUrl("https://github.com/paceholder/nodeeditor");
    card_qtnodes->setCardPixmap(QPixmap(":/res/qtnodes.jpg"));
    card_qtnodes->setTitle("QtNodes");
    card_qtnodes->setSubTitle("A Flow-Based Programming Library");

    auto *hlayout_cardScrollAreaWidget = new QHBoxLayout(scrollAreaWidget_cards);
    hlayout_cardScrollAreaWidget->setSpacing(15);
    hlayout_cardScrollAreaWidget->setContentsMargins(20, 40, 0, 0);
    hlayout_cardScrollAreaWidget->addWidget(card_github);
    hlayout_cardScrollAreaWidget->addWidget(card_author);
    hlayout_cardScrollAreaWidget->addWidget(card_qtnodes);
    hlayout_cardScrollAreaWidget->addStretch();

    auto *text_ = new ElaText("实用功能", this);

    // 卡片流式布局
    auto *card_basicAlgorithm = new ElaPopularCard(this);
    connect(card_basicAlgorithm, &ElaPopularCard::popularCardClicked, this, [=]() {});
    card_basicAlgorithm->setTitle("基础操作");
    card_basicAlgorithm->setCardButtontext("使用");
    card_basicAlgorithm->setSubTitle("5.0⭐ 基本图像处理算法");
    card_basicAlgorithm->setCardPixmap(QPixmap(":/res/shutiao.svg"));
    card_basicAlgorithm->setDetailedText(
        "使用OpenCV实现的基本图像处理算法，包括边缘检测、图像分割、图像融合等。");
    // card_basicAlgorithm->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    auto *card_YOLO = new ElaPopularCard(this);
    connect(card_YOLO, &ElaPopularCard::popularCardClicked, this, [=]() {});
    card_YOLO->setTitle("YOLO");
    card_YOLO->setCardButtontext("使用");
    card_YOLO->setSubTitle("5.0⭐ YOLO系列目标检测");
    card_YOLO->setCardPixmap(QPixmap(":/res/hanbao.svg"));
    card_YOLO->setDetailedText("使用OpenCV-dnn模块加载YOLO系列模型，实现目标检测功能。");
    // card_YOLO->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    auto *card_custom = new ElaPopularCard(this);
    connect(card_custom, &ElaPopularCard::popularCardClicked, this, [=]() {});
    card_custom->setTitle("自定义");
    card_custom->setCardButtontext("使用");
    card_custom->setSubTitle("5.0⭐ 自定义检测模型");
    card_custom->setCardPixmap(QPixmap(":/res/tangguoguan.svg"));
    card_custom->setDetailedText("通过插件形式加载自定义模型，实现自定义检测功能。");
    // card_custom->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    auto *layout_flow = new ElaFlowLayout(0, 5, 5);
    layout_flow->setIsAnimation(true);
    layout_flow->addWidget(card_basicAlgorithm);
    layout_flow->addWidget(card_YOLO);
    layout_flow->addWidget(card_custom);

    auto *vlayout_central = new QVBoxLayout(widget_central);
    vlayout_central->setContentsMargins(0, 0, 0, 0);
    vlayout_central->setSpacing(20);
    vlayout_central->addWidget(card_background);
    vlayout_central->addWidget(text_);
    vlayout_central->addLayout(layout_flow);
    vlayout_central->addStretch();

    this->addCentralWidget(widget_central);
}
