#ifndef IO_GRACE_READER_H
#define IO_GRACE_READER_H

#include "graphdata.h"

namespace IO {

class GraceReader {

public:
  static GraphData readFile(const QString& graphFile);
  static void readControl(GraphData& graphData, QString& line);

};

}

#endif
