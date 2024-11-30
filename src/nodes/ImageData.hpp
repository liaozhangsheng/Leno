#pragma once

#include <QtGui/QImage>
#include <QtNodes/NodeData>


using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ImageData : public NodeData {
public:
    ImageData() = default;

    ImageData(QImage const &image) : image_(image) {
    }

    NodeDataType type() const override {
        //       id      name
        return {"image", "P"};
    }

    QImage image() const {
        return image_;
    }

    void setImage(QImage const &image) {
        image_ = image;
    }

private:
    QImage image_;
};
