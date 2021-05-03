#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QString>


typedef struct _T_ShapeMapPoint
{
  double Latitude;  ///< Latitude  [°]
  double Longitude; ///< Longitude [°]
}T_ShapeMapPoint;


typedef struct _T_ShapeMapObject
{
  uint8_t                Type;   ///< Obelix type of map object
  QList<T_ShapeMapPoint> Points; ///< List of Obelix point in the object
}T_ShapeMapObject;


class MapManager
{
public:
  MapManager();
  bool LoadShapeFile(QString pShapeFilename);

  QList<T_ShapeMapObject> GetObjectInRange(double pLatitude, double pLongitude, double pRangeNm);

private:


  QList<T_ShapeMapObject> mMapObjectList;


};

#endif // MAPMANAGER_H
