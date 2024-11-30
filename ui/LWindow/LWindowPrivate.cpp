#include "LWindowPrivate.h"

#include <QApplication>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

#include "ElaApplication.h"
#include "ElaTheme.h"
#include "LWindow.h"
LWindowPrivate::LWindowPrivate(QObject* parent)
    : QObject{parent}, _themeMode(ElaThemeType::Light), q_ptr(nullptr)
{
}

LWindowPrivate::~LWindowPrivate()
{
}

void LWindowPrivate::onThemeModeChanged(ElaThemeType::ThemeMode themeMode)
{
    Q_Q(LWindow);
    _themeMode = themeMode;
    if (!eApp->getIsEnableMica())
    {
        QPalette palette = q->palette();
        palette.setBrush(QPalette::Window, ElaThemeColor(_themeMode, WindowBase));
        q->setPalette(palette);
    }
}
