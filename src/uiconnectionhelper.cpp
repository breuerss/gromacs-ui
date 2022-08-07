#include "uiconnectionhelper.h"

void setOptions(
        QComboBox* comboBox,
        const QList<QPair<QString, QVariant>>& options,
        int defaultIndex
        )
{
    for (const auto& entry: options)
    {
        comboBox->addItem(entry.first, entry.second);
    }

    comboBox->setCurrentIndex(defaultIndex);
}

void connectToLineEdit(
        QLineEdit* widget,
        std::shared_ptr<QObject> model,
        const QString& elementName,
        std::function<void(const QString&)>&& callback
        )
{
    QObject::connect(
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
}

void connectToCheckbox(
        QCheckBox* widget,
        std::shared_ptr<QObject> model,
        const QString& elementName,
        std::function<void(bool)>&& callback
        )
{
    QObject::connect(
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
}
