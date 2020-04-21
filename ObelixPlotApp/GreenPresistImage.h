#ifndef GREENPRESISTIMAGE_H
#define GREENPRESISTIMAGE_H

#include <QImage>
#include <QtConcurrent/QtConcurrentRun>


class GreenPresistImage : public QImage
{
public:
  GreenPresistImage(int pWidth, int pHeight);
  void AppendImage(QImage &pImage);
  void SetPersistence(double pPersistenceRatio);
  void Clear();

private:
  double mPersistenceRatio;
};

#endif // GREENPRESISTIMAGE_H
