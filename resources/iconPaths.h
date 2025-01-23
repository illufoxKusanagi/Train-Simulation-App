#ifndef ICONPATHS_H
#define ICONPATHS_H

#include <QMap>
#include <QString>

class IconPaths {
public:
  static QString getIconPath(const QString &iconName) {
    static QMap<QString, QString> iconPaths = {
        {"run", ":/icons/right-arrow-active.svg"},
        {"expand", ":/icons/expand.svg"},
        {"collapse", ":icons/collapse.svg"},
        {"constantActive", ":/icons/constant-active.svg"},
        {"trainActive", ":/icons/train-active.svg"},
        {"runningActive", ":/icons/running-active.svg"},
        {"trackActive", ":icons/track-active"},
        {"electricalActive", ":icons/electrical-active.svg"},
        {"outputActive", ":icons/play-active.svg"},
        {"constantDisabled", ":icons/constatn-disabled.svg"},
        {"trainDisabled", ":icons/train-disabled.svg"},
        {"runningDisabled", ":icons/running-disabled.svg"},
        {"trackDisabled", ":icons/track-disabled.svg"},
        {"electricalDisabled", ":icons/electrical-disabled.svg"},
        {"outputDisabled", ":icons/play-disabled.svg"},
    };
  };
};

#endif // ICONPATHS_H
