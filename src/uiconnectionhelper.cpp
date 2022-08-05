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

