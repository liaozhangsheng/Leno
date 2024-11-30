#pragma once

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QLabel>
#include <QObject>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "ImageData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageShowNode : public NodeDelegateModel {
    Q_OBJECT

public:
    ImageShowNode() = default;
    ~ImageShowNode() override = default;
    QString caption() const override {
        return {"Image Display"};
    }

    QString name() const override {
        return {"ImageShowNode"};
    }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const /*port*/) override {
        return nullptr;
    };

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget *embeddedWidget() override {
        if (!label_) {
            label_ = new QLabel("Image will appear here");
            label_->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            QFont f_ = label_->font();
            f_.setBold(true);
            f_.setItalic(true);
            label_->setFont(f_);
            label_->setMinimumSize(200, 200);
            label_->installEventFilter(this);
        }
        return label_;
    }

    bool resizable() const override {
        return true;
    }

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *label_{nullptr};
    QImage image_;
};

inline unsigned int ImageShowNode::nPorts(PortType portType) const {
    unsigned int result_ = 1;

    switch (portType) {
        case PortType::In:
            result_ = 1;
            break;

        case PortType::Out:
            result_ = 0;

        default:
            break;
    }

    return result_;
}

inline bool ImageShowNode::eventFilter(QObject *object, QEvent *event) {
    if (object == label_) {
        int w_ = label_->width();
        int h_ = label_->height();

        if (event->type() == QEvent::Resize) {
            if (!image_.isNull()) {
                label_->setPixmap(QPixmap::fromImage(image_).scaled(w_, h_, Qt::KeepAspectRatio));
            }
        }
    }

    return false;
}

inline NodeDataType ImageShowNode::dataType(PortType const /*portType*/,
                                            PortIndex const /*portIndex*/) const {
    return ImageData().type();
}

inline void ImageShowNode::setInData(std::shared_ptr<NodeData> nodeData,
                                     PortIndex const /*portIndex*/) {
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<ImageData>(nodeData);

        if (!d || d->image().isNull()) {
            return;
        }

        int w_ = label_->width();
        int h_ = label_->height();

        image_ = d->image();
        label_->setPixmap(QPixmap::fromImage(image_).scaled(w_, h_, Qt::KeepAspectRatio));
    }

    Q_EMIT dataUpdated(0);
}
