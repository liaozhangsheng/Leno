#ifndef LWINDOW_H
#define LWINDOW_H

#include <QMainWindow>

#include "Def.h"
#include "ElaAppBar.h"
#include "stdafx.h"
class LWindowPrivate;
class LWindow : public QMainWindow
{
    Q_OBJECT
    Q_Q_CREATE(LWindow)
    Q_PROPERTY_CREATE_Q_H(bool, IsStayTop)
    Q_PROPERTY_CREATE_Q_H(bool, IsFixedSize)
    Q_PROPERTY_CREATE_Q_H(bool, IsDefaultClosed)
    Q_PROPERTY_CREATE_Q_H(int, AppBarHeight)
    Q_PROPERTY_CREATE_Q_H(int, CustomWidgetMaximumWidth)
    Q_TAKEOVER_NATIVEEVENT_H
public:
    explicit LWindow(QWidget* parent = nullptr);
    ~LWindow();

    void moveToCenter();

    void setCustomWidget(ElaAppBarType::CustomArea customArea, QWidget* customWidget);
    QWidget* getCustomWidget() const;

    void setWindowButtonFlag(ElaAppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(ElaAppBarType::ButtonFlags buttonFlags);
    ElaAppBarType::ButtonFlags getWindowButtonFlags() const;

    void closeWindow();
Q_SIGNALS:
    Q_SIGNAL void closeButtonClicked();

    Q_SIGNAL void customWidgetChanged();

protected:
    virtual QMenu* createPopupMenu() override;
};

#endif // LWINDOW_H
