#ifndef ALEMBICFS_PROPERTYDATA_HH
#define ALEMBICFS_PROPERTYDATA_HH

#include "Types.hh"

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

struct PropertyData
{
    PropertyData(
            const Alembic::AbcCoreAbstract::PropertyHeader* header_,
            Alembic::AbcGeom::ICompoundProperty parent_
            )
     : header( header_ ),
       parent( parent_ ) {}

    PropertyData(
            const Alembic::AbcCoreAbstract::PropertyHeader* header_,
            Alembic::AbcGeom::ICompoundProperty parent_,
            PathSegments& _remainder
            )
     : header( header_ ),
       parent( parent_ ),
       remainder( _remainder ) {}

    const Alembic::AbcCoreAbstract::PropertyHeader* header;
    Alembic::AbcGeom::ICompoundProperty parent;
    PathSegments remainder;
};


#endif // ALEMBICFS_PROPERTYDATA_HH


