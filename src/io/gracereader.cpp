#include "gracereader.h"
#include "src/io/graphdata.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

namespace IO {

GraphData GraceReader::readFile(const QString& graphFile)
{
  QFile file(graphFile);
  file.open(QFile::ReadOnly);

  GraphData graphData;

  QTextStream in(&file);
  while(!in.atEnd()) {
    QString line = in.readLine();    
    if (line.startsWith("#"))
    {
      continue;
    }

    if (line.startsWith("@"))
    {
      readControl(graphData, line);
      continue;
    }

    auto elements = line.split(" ", Qt::SkipEmptyParts);
    for (int index = 0; index < elements.size(); index++)
    {
      graphData.data[index].entries.push_back(elements[index].toFloat());
    }
  }

  file.close();

  return graphData;
}

void GraceReader::readControl(GraphData& graphData, QString& line)
{
  line.remove(0, 1);
  auto elements = line.split(" ", Qt::SkipEmptyParts);

  QString control = elements.takeFirst();
  if (control == "title")
  {
    graphData.title = createCleanStringFrom(elements);
  }
  else if (control == "xaxis" && elements[0] == "label")
  {
    elements.removeFirst();
    graphData.xLabel = createCleanStringFrom(elements);
  }
  else if (control == "yaxis" && elements[0] == "label")
  {
    elements.removeFirst();
    graphData.yLabel = createCleanStringFrom(elements);
  }
  else if (
      control.contains(QRegularExpression("^s\\d+$")) && elements[0] == "legend")
  {
    elements.removeFirst();
    int index = control.remove(0, 1).toInt() + 1;
    while (graphData.data.size() <= index)
    {
      graphData.data.push_back(GraphData::Series());
    }
    graphData.data[index].legend = createCleanStringFrom(elements);
  }
}

QString GraceReader::createCleanStringFrom(const QStringList& elements)
{
  return elements.join(" ")
    .remove("\"")
    .replace("\\s", "_{")
    .replace("\\N", "}");
}

}
