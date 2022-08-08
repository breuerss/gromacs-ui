#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <memory>
#include <functional>
#include <QDebug>

template<typename ElementType, typename ValueType>
void connectToSpinBox(QWidget* container, std::shared_ptr<QObject> model, const QString& elementName)
{
  ElementType* widget = container->findChild<ElementType*>(elementName);
  if (!widget)
  {
    widget = dynamic_cast<ElementType*>(container);
  }
  QObject::connect(
    widget,
    QOverload<ValueType>::of(&ElementType::valueChanged),
    [model, elementName] (ValueType value) {
      model->setProperty(elementName.toStdString().c_str(), value);
    });

  ValueType value = model->property(elementName.toStdString().c_str()).value<ValueType>();
  widget->setValue(value);
}

template<typename ValueType>
void connectToComboBox(
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

  QVariant currentValue = model->property(elementName.toStdString().c_str());
  int index = widget->findData(currentValue);
  QObject::connect(
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

  widget->setCurrentIndex(index);
}

void connectToLineEdit(
  QLineEdit* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(const QString&)>&& callback = nullptr
  );

void connectToCheckbox(
  QCheckBox* widget,
  std::shared_ptr<QObject> model,
  const QString& elementName,
  std::function<void(bool)>&& callback = nullptr
  );

void setOptions(
  QComboBox* comboBox,
  const QList<QPair<QString, QVariant>>& options,
  int defaultIndex = 0
  );


#endif // UICONNECTIONHELPER_H
