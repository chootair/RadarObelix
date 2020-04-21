#include "GreenPresistImage.h"

void CalcPresistence(int pStartIxd, int pStopIdx, QImage* pImageSrc, QImage &pImageAdd, double pPersistenceRatio)
{
  for (int i=pStartIxd; i<pStopIdx; i++)
  {
    for (int j=0; j<pImageSrc->height(); j++)
    {
      pImageSrc->setPixel(i,j,qRgb(0,qMin(qGreen(pImageAdd.pixel(i,j))+pPersistenceRatio*qGreen(pImageSrc->pixel(i,j)), 255.0),0));
    }
  }
}

GreenPresistImage::GreenPresistImage(int pWidth, int pHeight):QImage(pWidth, pHeight, QImage::Format_ARGB32_Premultiplied)
{
  mPersistenceRatio = 0.8;
  this->fill(0xFF000000);
}

void GreenPresistImage::AppendImage(QImage &pImage)
{
  // Valid
  if ((pImage.width()  != this->width())  ||
      (pImage.height() != this->height()) ||
      (pImage.format() != this->format()))
  {
    return;
  }

  int s1 = this->width()/8;
  int s2 = 2*s1;
  int s3 = 3*s1;
  int s4 = 4*s1;
  int s5 = 5*s1;
  int s6 = 6*s1;
  int s7 = 7*s1;
  int s8 = 8*s1;

  QFuture<void> f1 = QtConcurrent::run(CalcPresistence, 0 , s1           , this, pImage, mPersistenceRatio);
  QFuture<void> f2 = QtConcurrent::run(CalcPresistence, s1, s2           , this, pImage, mPersistenceRatio);
  QFuture<void> f3 = QtConcurrent::run(CalcPresistence, s2, s3           , this, pImage, mPersistenceRatio);
  QFuture<void> f4 = QtConcurrent::run(CalcPresistence, s3, s4           , this, pImage, mPersistenceRatio);
  QFuture<void> f5 = QtConcurrent::run(CalcPresistence, s4, s5           , this, pImage, mPersistenceRatio);
  QFuture<void> f6 = QtConcurrent::run(CalcPresistence, s5, s6           , this, pImage, mPersistenceRatio);
  QFuture<void> f7 = QtConcurrent::run(CalcPresistence, s6, s7           , this, pImage, mPersistenceRatio);
  QFuture<void> f8 = QtConcurrent::run(CalcPresistence, s7, this->width(), this, pImage, mPersistenceRatio);

  f1.waitForFinished();
  f2.waitForFinished();
  f3.waitForFinished();
  f4.waitForFinished();
  f5.waitForFinished();
  f6.waitForFinished();
  f7.waitForFinished();
  f8.waitForFinished();

}

void GreenPresistImage::SetPersistence(double pPersistenceRatio)
{
  mPersistenceRatio = pPersistenceRatio;
}

void GreenPresistImage::Clear()
{
  this->fill(0);
}
