#include "ColorPickerButton.h"

#include <QVariant>

ColorPickerButton::ColorPickerButton(QWidget *parent):QPushButton(parent)
{
  connect(this, &QAbstractButton::clicked, this, &ColorPickerButton::OnClicked);
  setProperty("class", QVariant("ColorPickerButton"));
}

void ColorPickerButton::SetColor(QColor pColor)
{

  mColor = pColor;
  setStyleSheet(QString("QPushButton.ColorPickerButton{border: 0px; background-color: rgb(%1, %2, %3, %4);}").arg(mColor.red())
                                                                              .arg(mColor.green())
                                                                              .arg(mColor.blue())
                                                                              .arg(mColor.alpha()));
}

void ColorPickerButton::OnClicked()
{
  QColor lColor = QColorDialog::getColor(mColor,this,"Select Color", QColorDialog::ShowAlphaChannel);

  if (lColor.isValid() == true)
  {
    SetColor(lColor);
    emit ColorChanged(lColor);
  }
}
