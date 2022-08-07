#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include <QComboBox>
#include <memory>
#include <functional>

template<typename ElementType, typename ValueType>
void connectToUi(QWidget* container, std::shared_ptr<QObject> model, const QString& elementName)
{
    ElementType* widget = container->findChild<ElementType*>(elementName);
    widget->setValue(model->property(elementName.toStdString().c_str()).value<ValueType>());
    QObject::connect(
        widget,
        QOverload<ValueType>::of(&ElementType::valueChanged),
            [model, elementName] (ValueType value) {
        model->setProperty(elementName.toStdString().c_str(), value);
    });

}

template<typename ValueType>
void connectToUi(
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
    int index = widget->findData(model->property(elementName.toStdString().c_str()));
    widget->setCurrentIndex(index);
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

    if (callback != nullptr)
    {
        ValueType value = widget->currentData().value<ValueType>();
        callback(value);
    }

}

void setOptions(
        QComboBox* comboBox,
        const QList<QPair<QString, QVariant>>& options,
        int defaultIndex = 0
        );


#endif // UICONNECTIONHELPER_H
