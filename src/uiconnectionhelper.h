#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include <QComboBox>
#include <memory>
#include <functional>

template<typename ElementType, typename ValueType>
void connectToUi(QWidget* container, std::shared_ptr<QVariantMap> model, const QString& elementName)
{
    ElementType* widget = container->findChild<ElementType*>(elementName);
    widget->setValue((*model)[elementName].value<ValueType>());
    QObject::connect(
        widget,
        QOverload<ValueType>::of(&ElementType::valueChanged),
            [model, elementName] (ValueType value) {
        (*model)[elementName] = value;
    });

}

template<typename ValueType>
void connectToUi(
        QWidget* container,
        std::shared_ptr<QVariantMap> model,
        const QString& elementName,
        std::function<void(ValueType)>&& callback = nullptr
        )
{
    QComboBox* widget = container->findChild<QComboBox*>(elementName);
    if (!widget)
    {
        widget = dynamic_cast<QComboBox*>(container);
    }
    int index = widget->findData((*model)[elementName]);
    widget->setCurrentIndex(index);
    QObject::connect(
        widget,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
            [model, elementName, widget, callback] (int) {
        ValueType value = widget->currentData().value<ValueType>();
        (*model)[elementName] = value;
        if (callback != nullptr)
        {
            callback(value);
        }
    });

    if (callback != nullptr)
    {
        ValueType value = widget->currentData().value<ValueType>();
        callback(value);
    }

}

#endif // UICONNECTIONHELPER_H
