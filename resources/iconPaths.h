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
        {"constantActive", ":/icons/icons/constant-active.svg"},
        {"trainActive", ":/icons/icons/train-active.svg"},
        {"runningActive", ":/icons/icons/running-active.svg"},
        {"trackActive", ":/icons/icons/train-track-active"},
        {"electricalActive", ":/icons/icons/electrical-active.svg"},
        {"outputActive", ":/icons/icons/play-active.svg"},
        {"constantDisabled", ":/icons/icons/constant-disabled.svg"},
        {"trainDisabled", ":/icons/icons/train-disabled.svg"},
        {"runningDisabled", ":/icons/icons/running-disabled.svg"},
        {"trackDisabled", ":/icons/icons/train-track-disabled.svg"},
        {"electricalDisabled", ":/icons/icons/electrical-disabled.svg"},
        {"outputDisabled", ":/icons/icons/play-disabled.svg"},
    };
    // Mengembalikan path ikon jika ditemukan
    if (iconPaths.contains(iconName)) {
      return iconPaths.value(iconName);
    } else {
      // Mengembalikan string kosong atau path default jika tidak ditemukan
      return QString();
    }
  };
};

#endif // ICONPATHS_H
