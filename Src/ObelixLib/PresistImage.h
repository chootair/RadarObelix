#ifndef PRESISTIMAGE_H
#define PRESISTIMAGE_H

#include <QImage>
#include <QtConcurrent/QtConcurrentRun>


class PresistImage : public QImage
{
public:
  PresistImage(int pWidth, int pHeight);
  void AppendImage(QImage &pImage);
  void SetPersistence(double pPersistenceRatio);
  void Clear();

private:
  double mPersistenceRatio;
  bool   mOnlyOnGreen;
};

#endif // PRESISTIMAGE_H
