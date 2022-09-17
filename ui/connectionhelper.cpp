#include "connectionhelper.h"

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback
  )
{
  return connectToLineEdit(widget, model.get(), elementName, std::move(callback));
}

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  QObject* model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback
  )
{
  auto conn = QObject::connect(
    widget,
    &QLineEdit::textChanged,
    [model, elementName, callback] (const QString& value) {
      model->setProperty(elementName.toStdString().c_str(), QVariant::fromValue(value));
      if (callback != nullptr)
      {
        callback(value);
      }
    });

  QString value = model->property(elementName.toStdString().c_str()).value<QString>();
  widget->setText(value);

  return conn;
}

QMetaObject::Connection connectToCheckbox(
  QCheckBox* widget,
  QObject* model,
  const QString& elementName,
  std::function<void(bool)>&& callback
  )
{
  auto conn = QObject::connect(
    widget,
    &QCheckBox::stateChanged,
    [model, elementName, callback] (int state) {
      bool value = state == Qt::Checked;
      model->setProperty(elementName.toStdString().c_str(), value);
      if (callback != nullptr)
      {
        callback(value);
      }
    });

  bool value = model->property(elementName.toStdString().c_str()).value<bool>();
  widget->setChecked(value);

  return conn;
}

QMetaObject::Connection connectToCheckbox(
  QCheckBox* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(bool)>&& callback
  )
{
  return connectToCheckbox(widget, model.get(), elementName, std::move(callback));
}