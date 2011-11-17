#ifndef ALEMBICFS_PROPERTYVIEW_HH
#define ALEMBICFS_PROPERTYVIEW_HH

#include "PropertyData.hh"

#include <string>
#include <map>

class PropertyView
{
public:

    PropertyView() {}
    ~PropertyView() {}

    virtual void getAttr( struct stat* statbuf ) = 0;

    virtual int handleProperty( PropertyData& propertyData, char* buf ) = 0;

private:

};

typedef std::map< std::string, PropertyView* > PropertyViewLookup;

#endif // ALEMBICFS_PROPERTYVIEW_HH


