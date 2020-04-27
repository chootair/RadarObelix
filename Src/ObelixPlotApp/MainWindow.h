#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

  void OnTimerTick();
  void on_pbxClearScope_clicked();
  void on_sbxPresistance_valueChanged(double arg1);
  void on_doubleSpinBox_valueChanged(double arg1);
  void on_sbxDisplayPxKtsRatio_valueChanged(double arg1);
  void on_pbxStart_clicked();
  void on_pbxStop_clicked();

private:
  Ui::MainWindow *ui;
  QTimer* mTimer;
};

#endif // MAINWINDOW_H
