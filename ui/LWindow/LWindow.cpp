#include "LWindow.h"

#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScreen>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "Def.h"
#include "ElaApplication.h"
#include "ElaMenu.h"
#include "ElaTheme.h"
#include "ElaWindowStyle.h"
#include "LWindowPrivate.h"
Q_TAKEOVER_NATIVEEVENT_CPP(LWindow, d_func()->_appBar);
LWindow::LWindow(QWidget* parent)
    : QMainWindow{parent}, d_ptr(new LWindowPrivate())
{
    Q_D(LWindow);
    d->q_ptr = this;

    setProperty("ElaBaseClassName", "LWindow");
    resize(1020, 680); // 默认宽高
    
    // 自定义AppBar
    d->_appBar = new ElaAppBar(this);
    d->_appBar->setWindowButtonFlag(ElaAppBarType::RouteBackButtonHint, false);
    connect(d->_appBar, &ElaAppBar::closeButtonClicked, this, &LWindow::closeButtonClicked);

    // 主题变更
    d->_themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, d, &LWindowPrivate::onThemeModeChanged);
    connect(d->_appBar, &ElaAppBar::themeChangeButtonClicked, d, [=]() {
        eTheme->setThemeMode(eTheme->getThemeMode() == ElaThemeType::Light ? ElaThemeType::Dark : ElaThemeType::Light);
    });

    setObjectName("LWindow");
    setStyleSheet("#LWindow{background-color:transparent;}");
    // setStyle(new ElaWindowStyle(style()));

    //延时渲染
    QTimer::singleShot(1, this, [=, this] {
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Window, ElaThemeColor(d->_themeMode, WindowBase));
        this->setPalette(palette);
    });
    eApp->syncMica(this);
    connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
        d->onThemeModeChanged(d->_themeMode);
    });
}

LWindow::~LWindow()
{
}

void LWindow::setIsStayTop(bool isStayTop)
{
    Q_D(LWindow);
    d->_appBar->setIsStayTop(isStayTop);
    Q_EMIT pIsStayTopChanged();
}

bool LWindow::getIsStayTop() const
{
    return d_ptr->_appBar->getIsStayTop();
}

void LWindow::setIsFixedSize(bool isFixedSize)
{
    Q_D(LWindow);
    d->_appBar->setIsFixedSize(isFixedSize);
    Q_EMIT pIsFixedSizeChanged();
}

bool LWindow::getIsFixedSize() const
{
    return d_ptr->_appBar->getIsFixedSize();
}

void LWindow::setIsDefaultClosed(bool isDefaultClosed)
{
    Q_D(LWindow);
    d->_appBar->setIsDefaultClosed(isDefaultClosed);
    Q_EMIT pIsDefaultClosedChanged();
}

bool LWindow::getIsDefaultClosed() const
{
    return d_ptr->_appBar->getIsDefaultClosed();
}

void LWindow::setAppBarHeight(int appBarHeight)
{
    Q_D(LWindow);
    d->_appBar->setAppBarHeight(appBarHeight);
    Q_EMIT pAppBarHeightChanged();
}

QWidget* LWindow::getCustomWidget() const
{
    Q_D(const LWindow);
    return d->_appBar->getCustomWidget();
}

void LWindow::setCustomWidgetMaximumWidth(int width)
{
    Q_D(LWindow);
    d->_appBar->setCustomWidgetMaximumWidth(width);
    Q_EMIT pCustomWidgetMaximumWidthChanged();
}

int LWindow::getCustomWidgetMaximumWidth() const
{
    Q_D(const LWindow);
    return d->_appBar->getCustomWidgetMaximumWidth();
}

void LWindow::moveToCenter()
{
    if (isMaximized() || isFullScreen())
    {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    auto geometry = screen()->availableGeometry();
#else
    auto geometry = qApp->screenAt(this->geometry().center())->geometry();
#endif
    setGeometry((geometry.left() + geometry.right() - width()) / 2, (geometry.top() + geometry.bottom() - height()) / 2, width(), height());
}

void LWindow::setCustomWidget(ElaAppBarType::CustomArea customArea, QWidget* widget)
{
    Q_D(LWindow);
    d->_appBar->setCustomWidget(customArea, widget);
    Q_EMIT customWidgetChanged();
}

int LWindow::getAppBarHeight() const
{
    Q_D(const LWindow);
    return d->_appBar->getAppBarHeight();
}

void LWindow::setWindowButtonFlag(ElaAppBarType::ButtonType buttonFlag, bool isEnable)
{
    Q_D(LWindow);
    d->_appBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void LWindow::setWindowButtonFlags(ElaAppBarType::ButtonFlags buttonFlags)
{
    Q_D(LWindow);
    d->_appBar->setWindowButtonFlags(buttonFlags);
}

ElaAppBarType::ButtonFlags LWindow::getWindowButtonFlags() const
{
    return d_ptr->_appBar->getWindowButtonFlags();
}

void LWindow::closeWindow()
{
    Q_D(LWindow);
    d->_appBar->closeWindow();
}

QMenu* LWindow::createPopupMenu()
{
    ElaMenu* menu = nullptr;
    QList<QDockWidget*> dockwidgets = findChildren<QDockWidget*>();
    if (dockwidgets.size())
    {
        menu = new ElaMenu(this);
        for (int i = 0; i < dockwidgets.size(); ++i)
        {
            QDockWidget* dockWidget = dockwidgets.at(i);
            if (dockWidget->parentWidget() == this)
            {
                menu->addAction(dockwidgets.at(i)->toggleViewAction());
            }
        }
        menu->addSeparator();
    }

    QList<QToolBar*> toolbars = findChildren<QToolBar*>();
    if (toolbars.size())
    {
        if (!menu)
        {
            menu = new ElaMenu(this);
        }
        for (int i = 0; i < toolbars.size(); ++i)
        {
            QToolBar* toolBar = toolbars.at(i);
            if (toolBar->parentWidget() == this)
            {
                menu->addAction(toolbars.at(i)->toggleViewAction());
            }
        }
    }
    if (menu)
    {
        menu->setMenuItemHeight(28);
    }
    return menu;
}
