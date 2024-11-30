#pragma once

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "ImageData.hpp"


using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class ImageProcessBaseNode : public NodeDelegateModel {
    Q_OBJECT
public:
    ImageProcessBaseNode() = default;
    ~ImageProcessBaseNode() override = default;

    virtual void processImage() = 0;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const final;

    std::shared_ptr<NodeData> outData(PortIndex port) final;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const  /*port*/) override {
        if (nodeData) {
            if (auto d = std::dynamic_pointer_cast<ImageData>(nodeData);
                d && !d->image().isNull()) {
                originalImage_ = d->image();
                this->processImage();
            } else {
                originalImage_.fill(Qt::transparent);
            }
        } else {
            originalImage_.fill(Qt::transparent);
        }
        Q_EMIT dataUpdated(0);
    }

    bool resizable() const final {
        return false;
    }

protected:
    QWidget *centerWidget_{nullptr};
    QImage originalImage_;
    QImage processedImage_;
};

inline NodeDataType ImageProcessBaseNode::dataType(const PortType  /*portType*/,
                                                   const PortIndex  /*portIndex*/) const {
    return ImageData().type();
}

inline std::shared_ptr<NodeData> ImageProcessBaseNode::outData(const PortIndex  /*port*/) {
    return std::make_shared<ImageData>(processedImage_);
}