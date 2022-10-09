#include "connectionhelper.h"

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  std::shared_ptr<Model::Serializable> model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback
  )
{
  return connectToLineEdit(widget, model.get(), elementName, std::move(callback));
}

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  Model::Serializable* model,
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


