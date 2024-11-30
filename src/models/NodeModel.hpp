#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QtNodes/DataFlowGraphModel>
#include <memory>

#include "../nodes/ImageLoaderNode.hpp"
#include "../nodes/ImageShowNode.hpp"
#include "../nodes/VideoLoaderNode.hpp"
#include "../nodes/base/ThresholdNode.hpp"
#include "../nodes/yolo/YOLONode.hpp"
#include "../stdafx.h"

class NodeModel : public QObject {
    Q_OBJECT
    SINGLETON(NodeModel)
public:
    virtual ~NodeModel() noexcept = default;

    QtNodes::DataFlowGraphModel& graphModel() {
        return graphModel_;
    }

    QStandardItemModel* getNodeTreeModel() {
        auto* model = new QStandardItemModel(this);
        const auto reg = graphModel_.dataModelRegistry();
        auto categoryMap = reg->registeredModelsCategoryAssociation();

        // Create a map to store categories and their corresponding items
        std::unordered_map<QString, QStandardItem*> categoryItems;

        for (const auto& cat : reg->categories()) {
            auto* item = new QStandardItem(cat);
            item->setEditable(false);
            item->setSelectable(false);
            model->appendRow(item);
            categoryItems[cat] = item;
        }

        for (const auto& i : categoryMap) {
            if (categoryItems.find(i.second) != categoryItems.end()) {
                auto* node = new QStandardItem(i.first);
                node->setEditable(false);
                categoryItems[i.second]->appendRow(node);
            }
        }

        return model;
    }

private:
    NodeModel() : graphModel_(std::make_shared<QtNodes::NodeDelegateModelRegistry>()) {
        auto reg = graphModel_.dataModelRegistry();
        reg->registerModel<ImageLoaderNode>("输入输出");
        reg->registerModel<ImageShowNode>("输入输出");
        reg->registerModel<VideoLoaderNode>("输入输出");
        reg->registerModel<ThresholdNode>("基本处理");
        reg->registerModel<YOLONode>("目标检测");
    }

    QtNodes::DataFlowGraphModel graphModel_;
};
