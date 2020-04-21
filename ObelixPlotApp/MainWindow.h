#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ObelixUdpReader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_pbxRun_clicked(bool checked);
  void OnTimerTick();

  void on_pbxClearScope_clicked();

  void on_sbxPresistance_valueChanged(double arg1);

  void on_doubleSpinBox_valueChanged(double arg1);

private:
  Ui::MainWindow *ui;
  //
    QTimer* mTimer;
  //
  ObelixUdpReader* mObelixVideoReader;
  ObelixUdpReader* mObelixTrackReader;
};

#endif // MAINWINDOW_H
