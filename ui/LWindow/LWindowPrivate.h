#ifndef LWINDOWPRIVATE_H
#define LWINDOWPRIVATE_H

#include <QLinearGradient>
#include <QMap>
#include <QObject>

#include "Def.h"
#include "stdafx.h"
class LWindow;
class ElaAppBar;
class ElaWidget;
class QHBoxLayout;
class QVBoxLayout;
class LWindowPrivate : public QObject
{
    Q_OBJECT
    Q_D_CREATE(LWindow)

public:
    explicit LWindowPrivate(QObject* parent = nullptr);
    ~LWindowPrivate();
    Q_SLOT void onThemeModeChanged(ElaThemeType::ThemeMode themeMode);

private:
    ElaThemeType::ThemeMode _themeMode;
    ElaAppBar* _appBar{nullptr};
    int _contentsMargins{5};

};

#endif // LWINDOWPRIVATE_H
