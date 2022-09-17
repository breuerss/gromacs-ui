#ifndef PIPELINE_VIEW_STYLING_H
#define PIPELINE_VIEW_STYLING_H

#include <QString>

namespace Pipeline { namespace View {

static const QString stylesheet(
  "* {"
  "   border-radius: 10px;"
  "   color: white;"
  "   background: transparent;"
  "}"
  "QLabel { "
  "   padding-top: 10px;"
  "}"
  "QComboBox { "
  "   combobox-popup: 0;"
  "   background: transparent;"
  "   selection-background-color: purple;"
  "}"
  "QComboBox::drop-down { "
  "   background: transparent;"
  "   border-radius: 10px;"
  "}"
  "QComboBox::down-arrow { "
  "   color: white;"
  "}"
  "QAbstractSpinBox, QLineEdit, QListView, QComboBox {"
  "   border: 1px solid rgba(255, 255, 255, 0.5);"
  "   border-radius: 10px;"
  "   padding: 5px;"
  "}"
  "QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {"
  "   background: transparent;"
  "}"
  "QListView {"
  "   padding: 25px;"
  "   background: rgba(0, 0, 0, 0.5);"
  "}"
  "QGroupBox {"
  "   padding-top: 15px;"
  "   margin-bottom: 5px;"
  "   margin-top: 15px;"
  "}"
  "QGroupBox::title {"
  "   padding-top: 10px;"
  "   margin-bottom: 5px;"
  "   subcontrol-origin: margin;"
  "   subcontrol-position: top left;"
  "}"
  "QRadioButton::indicator {"
  "}"
  );
} }

#endif
