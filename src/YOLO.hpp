#pragma once

#include <QColor>
#include <QFile>
#include <QImage>
#include <QMap>
#include <QPainter>
#include <QRect>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "stdafx.h"

inline const QMap<int, QColor> &getColors() {
    static const QMap<int, QColor> colors = {
        {0, Qt::red},      {1, Qt::green},     {2, Qt::blue},         {3, Qt::cyan},
        {4, Qt::magenta},  {5, Qt::yellow},    {6, Qt::darkRed},      {7, Qt::darkGreen},
        {8, Qt::darkBlue}, {9, Qt::darkCyan},  {10, Qt::darkMagenta}, {11, Qt::darkYellow},
        {12, Qt::gray},    {13, Qt::darkGray}, {14, Qt::lightGray},   {15, Qt::black},
    };
    return colors;
}

class YOLO {
public:
    enum class ModelVersion : std::uint8_t {
        YOLOnas,
        YOLOv1,
        YOLOv2,
        YOLOv3,
        YOLOv4,
        YOLOv5,
        YOLOv6,
        YOLOv7,
        YOLOv8,
        YOLOv9,
        YOLOv10,
        YOLOv11,
    };

    struct DetectionResult {
        int classId{};
        float confidence{};
        QRect box;
    };

    Q_PRIVATE_CREATE(ModelVersion, modelVersion)
    Q_PRIVATE_CREATE(QString, modelPath)
    Q_PRIVATE_CREATE(QString, classesPath)
    Q_PRIVATE_CREATE(float, confThreshold)
    Q_PRIVATE_CREATE(float, nmsThreshold)

public:
    YOLO() : _pmodelVersion(ModelVersion::YOLOnas), _pconfThreshold(0.5F), _pnmsThreshold(0.4F) {
    }

    QVector<DetectionResult> detect(QImage &qimg);
    void draw_boxes(QImage &qimg, const QVector<DetectionResult> &results);
    void load_model(ModelVersion modelVersion, const QString &modelPath,
                    const QString &classesPath);

private:
    bool _load_classes(const QString &path);
    void _post_process(std::vector<cv::Mat> &outs, std::vector<int> &keep_classIds,
                       std::vector<float> &keep_confidences, std::vector<cv::Rect2d> &keep_boxes,
                       int nc = 80);

    cv::dnn::Net net_;
    QStringList classes_;
};

inline void YOLO::load_model(ModelVersion modelVersion, const QString &modelPath,
                             const QString &classesPath) {
    _pmodelVersion = modelVersion;
    _pmodelPath = modelPath;
    _pclassesPath = classesPath;
    net_ = cv::dnn::readNetFromONNX(modelPath.toStdString());
    _load_classes(classesPath);
}

inline QVector<YOLO::DetectionResult> YOLO::detect(QImage &qimg) {
    if (net_.empty() || classes_.isEmpty() || qimg.isNull()) {
        return QVector<YOLO::DetectionResult>{};
    }

    qimg.rgbSwap();
    cv::Mat img(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine());

    cv::dnn::Image2BlobParams imgParams{
        1.0 / 255,                     // scalefactor
        cv::Size(640, 480),            // size
        cv::Scalar(0, 0, 0),           // mean
        true,                          // swapRB
        CV_32F,                        // ddepth
        cv::dnn::DNN_LAYOUT_NCHW,      // layout
        cv::dnn::DNN_PMODE_LETTERBOX,  // padding mode
        114.0                          // padding value
    };

    std::vector<cv::Mat> outs;
    std::vector<int> keep_classIds;
    std::vector<float> keep_confidences;
    std::vector<cv::Rect2d> keep_boxes;
    std::vector<cv::Rect> boxes;

    cv::Mat inputBlob = cv::dnn::blobFromImageWithParams(img, imgParams);
    net_.setInput(inputBlob);
    net_.forward(outs, net_.getUnconnectedOutLayersNames());

    _post_process(outs, keep_classIds, keep_confidences, keep_boxes);

    boxes.reserve(keep_boxes.size());
    for (const auto &box : keep_boxes) {
        boxes.emplace_back(cvFloor(box.x - box.width / 2), cvFloor(box.y - box.height / 2),
                           cvFloor(box.width), cvFloor(box.height));
    }

    imgParams.blobRectsToImageRects(boxes, boxes, img.size());

    QVector<DetectionResult> results;
    for (int idx = 0; idx < boxes.size(); ++idx) {
        DetectionResult result;
        result.classId = keep_classIds[idx];
        result.confidence = keep_confidences[idx];
        result.box = QRect(boxes[idx].x, boxes[idx].y, boxes[idx].width, boxes[idx].height);
        results.push_back(result);
    }

    qimg.rgbSwap();

    return results;
}

inline void YOLO::draw_boxes(QImage &qimg, const QVector<DetectionResult> &results) {
    QPainter painter(&qimg);
    QFont font = painter.font();
    font.setPointSize(16);
    painter.setFont(font);

    for (const auto &r : results) {
        painter.setPen(QPen(::getColors()[r.classId], 4));
        painter.drawRect(r.box);
        QString label = QString("%1: %2").arg(classes_[r.classId]).arg(r.confidence, 0, 'f', 2);
        painter.drawText(r.box.topLeft(), label);
    }
}

inline bool YOLO::_load_classes(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        classes_.append(in.readLine());
    }

    return true;
}

inline void YOLO::_post_process(std::vector<cv::Mat> &outs, std::vector<int> &keep_classIds,
                                std::vector<float> &keep_confidences,
                                std::vector<cv::Rect2d> &keep_boxes, int nc) {
    std::vector<int> classIds_;
    std::vector<float> confidences_;
    std::vector<cv::Rect2d> boxes_;

    if (_pmodelVersion == ModelVersion::YOLOv8 || _pmodelVersion == ModelVersion::YOLOv9 ||
        _pmodelVersion == ModelVersion::YOLOv10 || _pmodelVersion == ModelVersion::YOLOv11) {
        cv::transposeND(outs[0], {0, 2, 1}, outs[0]);
    }
    if (_pmodelVersion == ModelVersion::YOLOnas) {
        // outs contains 2 elemets of shape [1, 8400, 80] and [1, 8400, 4]. Concat them to get [1,
        // 8400, 84]
        cv::Mat concat_out;
        // squeeze the first dimension
        outs[0] = outs[0].reshape(1, outs[0].size[1]);
        outs[1] = outs[1].reshape(1, outs[1].size[1]);
        cv::hconcat(outs[1], outs[0], concat_out);
        outs[0] = concat_out;
        // remove the second element
        outs.pop_back();
        // unsqueeze the first dimension
        outs[0] = outs[0].reshape(0, std::vector<int>{1, 8400, nc + 4});
    }

    // assert if last dim is 85 or 84
    CV_CheckEQ(outs[0].dims, 3,
               "Invalid output shape. The shape should be [1, #anchors, 85 or 84]");
    CV_CheckEQ((outs[0].size[2] == nc + 5 || outs[0].size[2] == 80 + 4), true,
               "Invalid output shape: ");

    for (auto preds_ : outs) {
        preds_ = preds_.reshape(1, preds_.size[1]);
        for (int i = 0; i < preds_.rows; ++i) {
            float obj_conf =
                (_pmodelVersion == ModelVersion::YOLOnas ||
                 _pmodelVersion == ModelVersion::YOLOv8 || _pmodelVersion == ModelVersion::YOLOv9 ||
                 _pmodelVersion == ModelVersion::YOLOv10 || _pmodelVersion == ModelVersion::YOLOv11)
                    ? 1.0F
                    : preds_.at<float>(i, 4);
            if (obj_conf < _pconfThreshold) {
                continue;
            }

            cv::Mat scores_ = preds_.row(i).colRange(
                (_pmodelVersion == ModelVersion::YOLOnas ||
                 _pmodelVersion == ModelVersion::YOLOv8 || _pmodelVersion == ModelVersion::YOLOv9 ||
                 _pmodelVersion == ModelVersion::YOLOv10 || _pmodelVersion == ModelVersion::YOLOv11)
                    ? 4
                    : 5,
                preds_.cols);
            double conf_;
            cv::Point maxLoc_;
            cv::minMaxLoc(scores_, nullptr, &conf_, nullptr, &maxLoc_);

            conf_ =
                (_pmodelVersion == ModelVersion::YOLOnas ||
                 _pmodelVersion == ModelVersion::YOLOv8 || _pmodelVersion == ModelVersion::YOLOv9 ||
                 _pmodelVersion == ModelVersion::YOLOv10 || _pmodelVersion == ModelVersion::YOLOv11)
                    ? conf_
                    : conf_ * obj_conf;
            if (conf_ < _pconfThreshold) {
                continue;
            }

            // get bbox coords
            auto *det = preds_.ptr<float>(i);
            double cx = det[0];
            double cy = det[1];
            double w = det[2];
            double h = det[3];

            // [x1, y1, x2, y2]
            if (_pmodelVersion == ModelVersion::YOLOnas ||
                _pmodelVersion == ModelVersion::YOLOv10 ||
                _pmodelVersion == ModelVersion::YOLOv11) {
                boxes_.emplace_back(cx, cy, w, h);
            } else {
                boxes_.emplace_back(cx - 0.5 * w, cy - 0.5 * h, cx + 0.5 * w, cy + 0.5 * h);
            }
            classIds_.push_back(maxLoc_.x);
            confidences_.push_back(static_cast<float>(conf_));
        }
    }

    std::vector<int> keep_idx;
    cv::dnn::NMSBoxes(boxes_, confidences_, _pconfThreshold, _pnmsThreshold, keep_idx);

    for (int idx : keep_idx) {
        keep_classIds.push_back(classIds_[idx]);
        keep_confidences.push_back(confidences_[idx]);
        keep_boxes.push_back(boxes_[idx]);
    }
}
