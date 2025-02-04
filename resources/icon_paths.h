#ifndef ICONPATHS_H
#define ICONPATHS_H

#include <QMap>
#include <QString>

class IconPaths {
public:
  static QString getIconPath(const QString &iconName) {
    static QMap<QString, QString> iconPaths = {
        {"run", ":/icons/icons/right-arrow-active.svg"},
        {"expand", ":/icons/icons/expand.svg"},
        {"collapse", ":/icons/icons/collapse.svg"},
        {"constant", ":/icons/icons/constant-active.svg"},
        {"train", ":/icons/icons/train-active.svg"},
        {"running", ":/icons/icons/running-active.svg"},
        {"track", ":/icons/icons/train-track-active"},
        {"electrical", ":/icons/icons/electrical-active.svg"},
        {"output", ":/icons/icons/play-active.svg"},
        {"constantDisabled", ":/icons/icons/constant-disabled.svg"},
        {"trainDisabled", ":/icons/icons/train-disabled.svg"},
        {"runningDisabled", ":/icons/icons/running-disabled.svg"},
        {"trackDisabled", ":/icons/icons/train-track-disabled.svg"},
        {"electricalDisabled", ":/icons/icons/electrical-disabled.svg"},
        {"outputDisabled", ":/icons/icons/play-disabled.svg"},
    };
    if (iconPaths.contains(iconName)) {
      return iconPaths.value(iconName);
    } else {
      return QString();
    }
  };
};

#endif // ICONPATHS_H
