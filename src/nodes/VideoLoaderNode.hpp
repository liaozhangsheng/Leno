#pragma once

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QLabel>
#include <QObject>
#include <QTimer>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <opencv2/opencv.hpp>
#include <qtmetamacros.h>

#include "ImageData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class VideoLoaderNode : public NodeDelegateModel {
    Q_OBJECT

public:
    VideoLoaderNode();

    ~VideoLoaderNode() {
        delete label_;
        cap_.release();
        timer_->stop();
        delete timer_;
    }

    QString caption() const override {
        return {"Video Source"};
    }

    QString name() const override {
        return {"VideoLoaderNode"};
    }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> /*nodeData*/, PortIndex const portIndex) override {
    }

    bool resizable() const override {
        return true;
    }

    QWidget *embeddedWidget() override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    Q_SLOT void _playNextFrame();

    QLabel *label_{nullptr};
    QImage frame_;
    cv::VideoCapture cap_;
    QTimer *timer_{nullptr};
};

inline VideoLoaderNode::VideoLoaderNode() : timer_(new QTimer(this)) {
    connect(timer_, &QTimer::timeout, this, &VideoLoaderNode::_playNextFrame);
}

inline unsigned int VideoLoaderNode::nPorts(PortType portType) const {
    unsigned int result = 1;

    switch (portType) {
        case PortType::In:
            result = 0;
            break;

        case PortType::Out:
            result = 1;

        default:
            break;
    }

    return result;
}

inline NodeDataType VideoLoaderNode::dataType(PortType const /*portType*/,
                                              PortIndex const /*portIndex*/) const {
    return ImageData().type();
}

inline std::shared_ptr<NodeData> VideoLoaderNode::outData(PortIndex /*port*/) {
    return std::make_shared<ImageData>(frame_);
}

inline QWidget *VideoLoaderNode::embeddedWidget() {
    if (!label_) {
        label_ = new QLabel("Click to load Video");
        label_->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        QFont f = label_->font();
        f.setBold(true);
        f.setItalic(true);
        label_->setFont(f);
        label_->setMinimumSize(200, 200);
        label_->setMaximumSize(500, 300);
        label_->installEventFilter(this);
    }
    return label_;
}

inline bool VideoLoaderNode::eventFilter(QObject *object, QEvent *event) {
    if (object == label_) {
        int w_ = label_->width();
        int h_ = label_->height();

        if (event->type() == QEvent::MouseButtonPress) {
            QString fileName_ =
                QFileDialog::getOpenFileName(nullptr, tr("Open Video File"), QDir::homePath(),
                                             tr("Video Files (*.avi *.mp4 *.mkv)"));

            if (fileName_.isEmpty()) {
                return false;
            }

            cap_.open(fileName_.toStdString());
            if (!cap_.isOpened()) {
                return false;
            }

            timer_->start(50);

            return true;
        } else if (event->type() == QEvent::Resize) {
            if (!frame_.isNull()) {
                label_->setPixmap(QPixmap::fromImage(frame_.scaled(w_, h_, Qt::KeepAspectRatio)));
            }
        }
    }

    return false;
}

inline void VideoLoaderNode::_playNextFrame() {
    int w_ = label_->width();
    int h_ = label_->height();

    static cv::Mat tmpFrame_;
    cap_ >> tmpFrame_;
    if (tmpFrame_.empty()) [[unlikely]] {
        cap_.set(cv::CAP_PROP_POS_FRAMES, 0);  // Restart video
        cap_ >> tmpFrame_;
    }

    frame_ = QImage(tmpFrame_.data, tmpFrame_.cols, tmpFrame_.rows,
                    static_cast<qsizetype>(tmpFrame_.step), QImage::Format_RGB888)
                 .rgbSwapped();
    label_->setPixmap(QPixmap::fromImage(frame_.scaled(w_, h_, Qt::KeepAspectRatio)));
    Q_EMIT dataUpdated(0);
}
