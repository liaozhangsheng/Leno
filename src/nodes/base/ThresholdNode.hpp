#pragma once

#include <ElaSlider.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

#include "../ImageProcessBaseNode.hpp"

class ThresholdNode : public ImageProcessBaseNode {
    Q_OBJECT
public:
    ThresholdNode() = default;
    ~ThresholdNode() override = default;

    void processImage() override {
        if (!originalImage_.isNull()) [[likely]] {
            processedImage_ = originalImage_.convertToFormat(QImage::Format_Grayscale8);
            cv::Mat mat_gray{processedImage_.height(), processedImage_.width(), CV_8UC1,
                             processedImage_.bits()};
            cv::threshold(mat_gray, mat_gray, slider_threshold_->value(), 255, cv::THRESH_BINARY);

            Q_EMIT dataUpdated(0);
        }
    }

    QString caption() const override {
        return {"Threshold"};
    }

    QString name() const override {
        return {"ThresholdNode"};
    }

    unsigned int nPorts(PortType portType) const override;

    QWidget *embeddedWidget() override;

private:
    ElaSlider *slider_threshold_{nullptr};
};

inline unsigned int ThresholdNode::nPorts(const PortType portType) const {
    unsigned int result = 1;

    switch (portType) {
        case PortType::In:
            result = 1;
            break;

        case PortType::Out:
            result = 1;

        default:
            break;
    }

    return result;
}

inline QWidget *ThresholdNode::embeddedWidget() {
    if (!centerWidget_) {
        centerWidget_ = new QWidget();

        auto *layout = new QVBoxLayout(centerWidget_);
        layout->setSpacing(10);

        auto *hLayout_threshold = new QHBoxLayout();
        hLayout_threshold->setSpacing(5);

        auto *label_threshold = new QLabel("Threshold");
        label_threshold->setAlignment(Qt::AlignCenter);

        slider_threshold_ = new ElaSlider();
        slider_threshold_->setMinimum(0);
        slider_threshold_->setMaximum(255);
        slider_threshold_->setOrientation(Qt::Horizontal);

        auto *lineEdit_threshold = new QLineEdit();
        lineEdit_threshold->setMaximumWidth(40);
        lineEdit_threshold->setAlignment(Qt::AlignCenter);
        lineEdit_threshold->setClearButtonEnabled(false);
        lineEdit_threshold->setText("0");
        lineEdit_threshold->setValidator(new QIntValidator(0, 255, lineEdit_threshold));
        connect(lineEdit_threshold, &QLineEdit::textChanged, [this](const QString &text) {
            bool ok_;
            int value_ = text.toInt(&ok_);
            if (ok_) [[likely]] {
                slider_threshold_->setValue(value_);
            }
        });

        connect(slider_threshold_, &QSlider::valueChanged, [this, lineEdit_threshold](int value_) {
            lineEdit_threshold->setText(QString::number(value_));
            this->processImage();
        });

        hLayout_threshold->addWidget(label_threshold);
        hLayout_threshold->addWidget(slider_threshold_);
        hLayout_threshold->addWidget(lineEdit_threshold);

        layout->addLayout(hLayout_threshold);
    }

    return centerWidget_;
}
