#ifndef IO_GRACE_READER_H
#define IO_GRACE_READER_H

#include <QString>
namespace IO {

class GraphData;

class GraceReader {

public:
  static GraphData readFile(const QString& graphFile);

private:
  static void readControl(GraphData& graphData, QString& line);
  static QString createCleanStringFrom(const QStringList& elements);

};

}

#endif
