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
class ImageLoaderNode : public NodeDelegateModel {
    Q_OBJECT

public:
    ImageLoaderNode() = default;

    ~ImageLoaderNode() override = default;

    QString caption() const override {
        return {"Image Source"};
    }

    QString name() const override {
        return {"ImageLoaderNode"};
    }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> /*nodeData*/, PortIndex const portIndex) override {
    }

    QWidget *embeddedWidget() override;

    bool resizable() const override {
        return true;
    }

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *label_{nullptr};
    QImage image_;
};

inline unsigned int ImageLoaderNode::nPorts(PortType portType) const {
    unsigned int result_ = 1;

    switch (portType) {
        case PortType::In:
            result_ = 0;
            break;

        case PortType::Out:
            result_ = 1;

        default:
            break;
    }

    return result_;
}

inline bool ImageLoaderNode::eventFilter(QObject *object, QEvent *event) {
    if (object == label_) {
        int w_ = label_->width();
        int h_ = label_->height();

        if (event->type() == QEvent::MouseButtonPress) {
            QString fileName_ =
                QFileDialog::getOpenFileName(nullptr, tr("Open Image"), QDir::currentPath(),
                                             tr("Image Files (*.png *.jpg *.bmp)"));

            if (fileName_.isEmpty()) {
                return false;
            }

            image_ = QImage(fileName_);
            image_ = image_.convertToFormat(QImage::Format_RGB888);
            label_->setPixmap(QPixmap::fromImage(image_.scaled(w_, h_, Qt::KeepAspectRatio)));
            Q_EMIT dataUpdated(0);

            return true;
        } else if (event->type() == QEvent::Resize) {
            if (!image_.isNull()) {
                label_->setPixmap(QPixmap::fromImage(image_.scaled(w_, h_, Qt::KeepAspectRatio)));
            }
        }
    }

    return false;
}

inline NodeDataType ImageLoaderNode::dataType(PortType const /*portType*/,
                                              PortIndex const /*portIndex*/) const {
    return ImageData().type();
}

inline std::shared_ptr<NodeData> ImageLoaderNode::outData(PortIndex /*port*/) {
    return std::make_shared<ImageData>(image_);
}

inline QWidget *ImageLoaderNode::embeddedWidget() {
    if (!label_) {
        label_ = new QLabel("Click to load image");
        label_->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        QFont f_ = label_->font();
        f_.setBold(true);
        f_.setItalic(true);
        label_->setFont(f_);
        label_->setMinimumSize(200, 200);
        label_->setMaximumSize(500, 300);
        label_->installEventFilter(this);
    }
    return label_;
}
