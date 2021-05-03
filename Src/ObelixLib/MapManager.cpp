

#include <QFileInfo>

#ifdef USESHAPELIB
#include "shapefil.h"
#endif

#include "obelixtoolbox.h"

#include "MapManager.h"

MapManager::MapManager()
{
  mMapObjectList.clear();
}


bool MapManager::LoadShapeFile(QString pShapeFilename)
{
#ifdef USESHAPELIB
  int lNbEntities = 0;
  int lShapeType  = 0;
  double lMinBound[4];
  double lMaxBound[4];

  SHPInfo* lShapeHdl;

  // Open file
  lShapeHdl = SHPOpen(pShapeFilename.toStdString().c_str(), "rb+");

  // Get file info
  SHPGetInfo(lShapeHdl, &lNbEntities, &lShapeType, lMinBound, lMaxBound);

  // Loop on entities
  for(int lEntityIdx=0; lEntityIdx<lNbEntities; lEntityIdx++)
  {
    T_ShapeMapObject lMapShapeObject;
    //
    SHPObject* lShapeObject = SHPReadObject(lShapeHdl,lEntityIdx);

    // Loop on vertices
    for (int lVerticeIdx=0; lVerticeIdx<lShapeObject->nVertices; lVerticeIdx++)
    {
      T_ShapeMapPoint lMapShapePoint;
      //
      lMapShapePoint.Latitude  = lShapeObject->padfY[lVerticeIdx];
      lMapShapePoint.Longitude = lShapeObject->padfX[lVerticeIdx];
      //
      lMapShapeObject.Points.append(lMapShapePoint);
    }

    // Add to global list
    mMapObjectList.append(lMapShapeObject);

    // Destroy object
    SHPDestroyObject(lShapeObject);
  }

  // Close file
  SHPClose(lShapeHdl);
#endif
  //
  return  true;
}

QList<T_ShapeMapObject> MapManager::GetObjectInRange(double pLatitude, double pLongitude, double pRangeNm)
{
  return mMapObjectList;
}
