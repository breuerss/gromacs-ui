#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include "../src/model/serializable.h"
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <memory>
#include <functional>
#include <QDebug>
#include <QMetaObject>
#include <QMetaMethod>
#include <optional>

template<typename ElementType, typename ConfigType, typename ValueType>
QList<QMetaObject::Connection> connectToSpinBox(
  QWidget* container,
  std::shared_ptr<ConfigType> model,
  const QString& elementName,
  void(ConfigType::*changed)(ValueType)
  )
{
  return connectToSpinBox<ElementType, ConfigType, ValueType>(container, model.get(), elementName, changed);
}

template<typename ElementType, typename ConfigType, typename ValueType>
QList<QMetaObject::Connection> connectToSpinBox(
  QWidget* container,
  ConfigType* model,
  const QString& elementName,
  void(ConfigType::*changed)(ValueType)
  )
{
  ElementType* widget = container->findChild<ElementType*>(elementName);
  if (!widget)
  {
    widget = dynamic_cast<ElementType*>(container);
  }
  QList<QMetaObject::Connection> conns;
  conns << QObject::connect(
    widget,
    QOverload<ValueType>::of(&ElementType::valueChanged),
    [model, elementName] (ValueType value) {
      if (value != model->property(elementName.toStdString().c_str()))
      {
        model->setProperty(elementName.toStdString().c_str(), value);
      }
    });

  conns << QObject::connect(
    model,
    changed,
    widget,
    &ElementType::setValue);

  ValueType value = model->property(elementName.toStdString().c_str()).template value<ValueType>();
  widget->setValue(value);
  return conns;
}

template<typename ValueType>
QMetaObject::Connection connectToComboBox(
  QWidget* container,
  std::shared_ptr<Model::Serializable> model,
  const QString& elementName,
  std::function<void(ValueType)>&& callback = nullptr
  )
{
  return connectToComboBox(container, model.get(), elementName, std::move(callback));
}

template<typename ValueType>
QMetaObject::Connection connectToComboBox(
  QWidget* container,
  std::shared_ptr<Model::Serializable> model,
  const QString& elementName,
  std::function<void(ValueType)>&& callback,
  void(*changed)(ValueType) = nullptr
  )
{
  return connectToComboBox(container, model.get(), elementName, std::move(callback), changed);
}

template<typename ValueType>
QMetaObject::Connection connectToComboBox(
  QWidget* container,
  Model::Serializable* model,
  const QString& elementName,
  std::function<void(ValueType)>&& callback = nullptr
  )
{
  QComboBox* widget = container->findChild<QComboBox*>(elementName);
  if (!widget)
  {
    widget = dynamic_cast<QComboBox*>(container);
  }

  auto conn = QObject::connect(
    widget,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [model, elementName, widget, callback] (int) {
      ValueType value = widget->currentData().value<ValueType>();
      model->setProperty(elementName.toStdString().c_str(), QVariant::fromValue(value));
      if (callback != nullptr)
      {
        callback(value);
      }
    });

  QVariant currentValue = model->property(elementName.toStdString().c_str());
  int index = widget->findData(currentValue);

  // if no entry is found take the first one
  if (index == -1)
  {
    index = 0;
  }

  //widget->setCurrentIndex(-1);
  widget->setCurrentIndex(index);

  return conn;
}

template<typename ValueType, typename ConfigType>
QList<QMetaObject::Connection> connectToComboBox(
  QWidget* container,
  ConfigType* model,
  const QString& elementName,
  std::function<void(ValueType)>&& callback,
  void(ConfigType::*changed)(ValueType)
  )
{
  QComboBox* widget = container->findChild<QComboBox*>(elementName);
  if (!widget)
  {
    widget = dynamic_cast<QComboBox*>(container);
  }

  QList<QMetaObject::Connection> conns;
  conns << QObject::connect(
    model, changed, [widget, model, elementName] (ValueType currentValue) {
      int index = widget->findData(QVariant::fromValue(currentValue));
      widget->setCurrentIndex(index);
    });

  conns << connectToComboBox(container, model, elementName, std::move(callback));
  return conns;
}

template<typename ValueType, typename ConfigType>
QList<QMetaObject::Connection> connectToComboBox(
  QWidget* container,
  ConfigType* model,
  const QString& elementName,
  void(ConfigType::*changed)(ValueType)
  )
{
  return connectToComboBox<ValueType, ConfigType>(container, model, elementName, [] (ValueType) {}, changed);
}

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  std::shared_ptr<Model::Serializable> model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback = nullptr
  );

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  Model::Serializable* model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback = nullptr
  );

template<typename ConfigType>
QList<QMetaObject::Connection> connectCheckBox(
  QCheckBox* widget,
  ConfigType* model,
  const QString& elementName,
  std::function<void(bool)>&& callback = nullptr,
  void(ConfigType::*changed)(bool) = nullptr
  )
{
  QList<QMetaObject::Connection> conns;
  conns << QObject::connect(
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

  bool value = model->property(elementName.toStdString().c_str()).toBool();
  widget->setChecked(value);
  conns << QObject::connect(
    model, changed, [widget, model, elementName] (bool currentValue) {
      widget->setChecked(currentValue);
    });

  return conns;
}

template<typename ConfigType>
QMetaObject::Connection connectCheckBox(
  QCheckBox* widget,
  std::shared_ptr<ConfigType> model,
  const QString& elementName,
  std::function<void(bool)>&& callback = nullptr,
  void(ConfigType::*changed)(bool) = nullptr
  )
{
  return connect(widget, model.get(), elementName, std::move(callback), changed);
}

template<typename TypeName = QVariant>
void setOptions(
  QComboBox* comboBox,
  const QList<QPair<QString, TypeName>>& options,
  int defaultIndex = 0
  )
{
  comboBox->clear();
  if (options.length())
  {
    setOptions<TypeName>(comboBox, options, options[defaultIndex].second);
  }
}

template<typename TypeName = QVariant>
void setOptions(
  QComboBox* comboBox,
  const QList<QPair<QString, TypeName>>& options,
  TypeName selectedOption
  )
{
  comboBox->clear();
  for (const auto& entry: options)
  {
    comboBox->addItem(entry.first, QVariant::fromValue(entry.second));
  }

  int defaultIndex = comboBox->findData(QVariant::fromValue(selectedOption));
  comboBox->setCurrentIndex(defaultIndex);
}


#endif // UICONNECTIONHELPER_H
