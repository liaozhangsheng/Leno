#pragma once

#include <ElaComboBox.h>
#include <ElaSlider.h>

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../../YOLO.hpp"
#include "../ImageProcessBaseNode.hpp"


class YOLONode : public ImageProcessBaseNode {
    Q_OBJECT
public:
    YOLONode() = default;
    ~YOLONode() override = default;

    void processImage() override {
        if (!originalImage_.isNull()) [[likely]] {
            processedImage_ = originalImage_.copy();
            auto res = yolo_.detect(processedImage_);
            yolo_.draw_boxes(processedImage_, res);

            Q_EMIT dataUpdated(0);
        }
    }

    QString caption() const override {
        return {"YOLO"};
    }

    QString name() const override {
        return {"YOLONode"};
    }

    unsigned int nPorts(PortType portType) const override;

    QWidget *embeddedWidget() override;

private:
    YOLO yolo_;
};

inline unsigned int YOLONode::nPorts(PortType portType) const {
    unsigned int result_ = 1;

    switch (portType) {
        case PortType::In:
            result_ = 1;
            break;

        case PortType::Out:
            result_ = 1;

        default:
            break;
    }

    return result_;
}

inline QWidget *YOLONode::embeddedWidget() {
    if (!centerWidget_) {
        centerWidget_ = new QWidget();

        auto *vlayout_ = new QVBoxLayout(centerWidget_);
        vlayout_->setSpacing(10);

        auto *hlayout_modelVersion = new QHBoxLayout();
        auto *comboBox_modelVersion = new ElaComboBox();
        comboBox_modelVersion->addItem("YOLOnas");
        for (int i = 1; i <= 11; ++i) {
            comboBox_modelVersion->addItem(QString("YOLOv%1").arg(i));
        }
        comboBox_modelVersion->setCurrentIndex(11);
        connect(comboBox_modelVersion, &QComboBox::currentIndexChanged,
                [this](int index) { yolo_.setmodelVersion(static_cast<YOLO::ModelVersion>(index)); });
        hlayout_modelVersion->addStretch();
        hlayout_modelVersion->addWidget(comboBox_modelVersion);
        hlayout_modelVersion->addStretch();

        auto *hlayout_model = new QHBoxLayout();
        hlayout_model->setSpacing(5);

        auto *label_model = new QLabel("Model:");
        label_model->setFixedWidth(50);
        label_model->setAlignment(Qt::AlignCenter);

        auto *lineEdit_model = new QLineEdit();
        lineEdit_model->setAlignment(Qt::AlignCenter);

        auto *btn_model = new QPushButton("...");
        btn_model->setFixedWidth(30);
        connect(btn_model, &QPushButton::clicked, [this, lineEdit_model]() {
            QString fileName = QFileDialog::getOpenFileName(centerWidget_, "Open Model", "",
                                                            "Model Files (*.onnx)");
            if (!fileName.isEmpty()) {
                lineEdit_model->setText(fileName);
            }
        });

        hlayout_model->addWidget(label_model);
        hlayout_model->addWidget(lineEdit_model);
        hlayout_model->addWidget(btn_model);

        auto *hlayout_classes = new QHBoxLayout();
        hlayout_classes->setSpacing(5);

        auto *label_classes = new QLabel("Classes:");
        label_classes->setFixedWidth(50);
        label_classes->setAlignment(Qt::AlignCenter);

        auto *lineEdit_classes = new QLineEdit();
        lineEdit_classes->setAlignment(Qt::AlignCenter);

        auto *btn_classes = new QPushButton("...");
        btn_classes->setFixedWidth(30);
        connect(btn_classes, &QPushButton::clicked, [this, lineEdit_classes]() {
            QString fileName = QFileDialog::getOpenFileName(centerWidget_, "Open Classes", "",
                                                            "Text Files (*.txt)");
            if (!fileName.isEmpty()) {
                lineEdit_classes->setText(fileName);
            }
        });

        hlayout_classes->addWidget(label_classes);
        hlayout_classes->addWidget(lineEdit_classes);
        hlayout_classes->addWidget(btn_classes);

        auto *hlayout_load = new QHBoxLayout();
        auto *btn_load = new QPushButton("Load");

        connect(btn_load, &QPushButton::clicked, [this, lineEdit_model, lineEdit_classes, btn_load, comboBox_modelVersion]() {
            QString modelPath = lineEdit_model->text();
            QString classesPath = lineEdit_classes->text();
            if (modelPath.isEmpty() || classesPath.isEmpty()) {
                btn_load->setText("Load (failed)");
                return;
            }

            btn_load->setText("Load (succeed)");
            yolo_.load_model(static_cast<YOLO::ModelVersion>(comboBox_modelVersion->currentIndex()),
                             modelPath, classesPath);
        });

        hlayout_load->addStretch();
        hlayout_load->addWidget(btn_load);
        hlayout_load->addStretch();

        auto *hlayout_conf = new QHBoxLayout();
        hlayout_conf->setSpacing(5);

        auto *label_conf = new QLabel("Conf:");
        label_conf->setFixedWidth(40);
        label_conf->setAlignment(Qt::AlignCenter);

        auto *slider_conf = new ElaSlider(Qt::Horizontal);
        slider_conf->setMinimum(0);
        slider_conf->setMaximum(100);
        slider_conf->setOrientation(Qt::Horizontal);
        slider_conf->setValue(50);

        auto *lineEdit_conf = new QLineEdit();
        lineEdit_conf->setFixedWidth(40);
        lineEdit_conf->setAlignment(Qt::AlignCenter);
        lineEdit_conf->setClearButtonEnabled(false);
        lineEdit_conf->setText("0.50");
        lineEdit_conf->setValidator(new QDoubleValidator(0, 1, 2, lineEdit_conf));
        connect(lineEdit_conf, &QLineEdit::textChanged, [slider_conf](const QString &text) {
            bool ok;
            double value = text.toDouble(&ok);
            if (ok) {
                slider_conf->setValue(static_cast<int>(value * 100));
            }
        });

        connect(slider_conf, &QSlider::valueChanged, [this, lineEdit_conf](int value) {
            lineEdit_conf->setText(QString::number(value / 100.0, 'f', 2));
            yolo_.setconfThreshold(static_cast<float>(value / 100.0));
            this->processImage();
        });

        hlayout_conf->addWidget(label_conf);
        hlayout_conf->addWidget(slider_conf);
        hlayout_conf->addWidget(lineEdit_conf);

        auto *hlayout_iou = new QHBoxLayout();
        hlayout_iou->setSpacing(5);

        auto *label_iou = new QLabel("IOU:");
        label_iou->setFixedWidth(40);
        label_iou->setAlignment(Qt::AlignCenter);

        auto *slider_iou = new ElaSlider(Qt::Horizontal);
        slider_iou->setMinimum(0);
        slider_iou->setMaximum(100);
        slider_iou->setOrientation(Qt::Horizontal);
        slider_iou->setValue(40);

        auto *lineEdit_iou = new QLineEdit();
        lineEdit_iou->setFixedWidth(40);
        lineEdit_iou->setAlignment(Qt::AlignCenter);
        lineEdit_iou->setClearButtonEnabled(false);
        lineEdit_iou->setText("0.40");
        lineEdit_iou->setValidator(new QDoubleValidator(0, 1, 2, lineEdit_iou));
        connect(lineEdit_iou, &QLineEdit::textChanged, [=](const QString &text) {
            bool ok;
            double value = text.toDouble(&ok);
            if (ok) {
                slider_iou->setValue(static_cast<int>(value * 100));
            }
        });

        connect(slider_iou, &QSlider::valueChanged, [=](int value) {
            lineEdit_iou->setText(QString::number(value / 100.0, 'f', 2));
            yolo_.setnmsThreshold(static_cast<float>(value / 100.0));
            this->processImage();
        });

        hlayout_iou->addWidget(label_iou);
        hlayout_iou->addWidget(slider_iou);
        hlayout_iou->addWidget(lineEdit_iou);

        vlayout_->addLayout(hlayout_modelVersion);
        vlayout_->addLayout(hlayout_model);
        vlayout_->addLayout(hlayout_classes);
        vlayout_->addLayout(hlayout_load);
        vlayout_->addLayout(hlayout_conf);
        vlayout_->addLayout(hlayout_iou);
    }
    return centerWidget_;
}
