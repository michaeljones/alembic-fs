#ifndef ALEMBICFS_CLASSIFIEDOBJECT_HH
#define ALEMBICFS_CLASSIFIEDOBJECT_HH

#include "Types.hh"

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

class ClassifiedObject
{
public:

    enum Classification
    {
        kObject = 0,
        kProperties,
        kProperty
    };

public:

    ClassifiedObject( Alembic::AbcGeom::IObject _iObj, Classification _classification )
     : iObj( _iObj ), classification( _classification ) {}

    ClassifiedObject(
            Alembic::AbcGeom::IObject _iObj,
            Classification _classification,
            PathSegments& _remainder
            )
     : iObj( _iObj ),
       classification( _classification ),
       remainder( _remainder ) {}

    Alembic::AbcGeom::IObject iObj;
    Classification classification;
    PathSegments remainder;
};


#endif // ALEMBICFS_CLASSIFIEDOBJECT_HH


