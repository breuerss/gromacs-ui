#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <memory>
#include <functional>
#include <QDebug>
#include <QMetaObject>

template<typename ElementType, typename ValueType>
QMetaObject::Connection connectToSpinBox(QWidget* container, std::shared_ptr<QObject> model, const QString& elementName)
{
  ElementType* widget = container->findChild<ElementType*>(elementName);
  if (!widget)
  {
    widget = dynamic_cast<ElementType*>(container);
  }
  auto conn = QObject::connect(
    widget,
    QOverload<ValueType>::of(&ElementType::valueChanged),
    [model, elementName] (ValueType value) {
      model->setProperty(elementName.toStdString().c_str(), value);
    });

  ValueType value = model->property(elementName.toStdString().c_str()).value<ValueType>();
  widget->setValue(value);
  return conn;
}

template<typename ValueType>
QMetaObject::Connection connectToComboBox(
  QWidget* container,
  std::shared_ptr<QObject> model,
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

  widget->setCurrentIndex(-1);
  widget->setCurrentIndex(index);

  return conn;
}

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback = nullptr
  );

QMetaObject::Connection connectToLineEdit(
  QLineEdit* widget,
  QObject* model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback = nullptr
  );

QMetaObject::Connection connectToCheckbox(
  QCheckBox* widget,
  QObject* model,
  const QString& elementName,
  std::function<void(bool)>&& callback = nullptr
  );

QMetaObject::Connection connectToCheckbox(
  QCheckBox* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(bool)>&& callback = nullptr
  );

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
