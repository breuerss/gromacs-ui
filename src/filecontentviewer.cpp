#include "filecontentviewer.h"
#include "qfontmetrics.h"
#include "qtextedit.h"

#include <QFile>
#include <QHBoxLayout>
#include <QFontMetrics>

FileContentViewer::FileContentViewer(const QString& fileName)
  : fileName(fileName)
{
  QHBoxLayout* layout = new QHBoxLayout(this);
  QTextEdit* viewer = new QTextEdit(this);
  layout->addWidget(viewer);
  setLayout(layout);
  viewer->setReadOnly(true);

  QFont font;
  font.setFamily("Courier");
  font.setStyleHint(QFont::Monospace);
  font.setFixedPitch(true);
  font.setPointSize(12);
  viewer->setFont(font);

  QFontMetrics metrics(font);
  viewer->setTabStopDistance(4 * metrics.horizontalAdvance(' '));

  QFile file(fileName);
  file.open(QFile::ReadOnly);
  viewer->setText(file.readAll());
  file.close();

  connect(this, &QDialog::finished, [this] (int) {
    this->deleteLater();
  });

  resize(800, 600);
  setWindowTitle(fileName);
}
