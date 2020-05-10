#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QPushButton>
#include <QColorDialog>
#include <QColor>

class ColorPickerButton : public QPushButton
{
  Q_OBJECT

public:
  ColorPickerButton(QWidget *parent = nullptr);
  inline QColor Color() const {return mColor;}
  void SetColor(QColor pColor);


signals:
  void ColorChanged(QColor pColor);

private slots:
  void OnClicked();
private:
  QColor mColor;
};

#endif // COLORPICKERBUTTON_H
