#ifndef APPPROVIDER_H
#define APPPROVIDER_H

#include <QString>

class AppProvider {
public:
  static QString get(const QString& binary);
};

#endif
