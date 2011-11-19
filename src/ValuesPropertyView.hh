#ifndef ALEMBICFS_VALUESPROPERTYVIEW_HH
#define ALEMBICFS_VALUESPROPERTYVIEW_HH

#include "PropertyHandler.hh"
#include "PropertyView.hh"
#include "OutputTrait.hh"

#include <sys/stat.h>

class ValuesPropertyView : public PropertyView
{
public:

    void getAttr( struct stat* statbuf )
    {
        statbuf->st_mode = S_IFREG | S_IRUSR;
        statbuf->st_size = 4096;
    }

    template< typename TYPE >
    struct ValuesScalarOp
    {
        int operator()(
                char *buf,
                const Alembic::Abc::CompoundPropertyReaderPtr ptr,
                const std::string& propertyName,
                const Alembic::Abc::uint8_t extent
                )
        {
            Alembic::Abc::BasePropertyReaderPtr vals = ptr->getScalarProperty( propertyName );

            std::vector< TYPE > data( extent );

            vals->asScalarPtr()->getSample( 0, &(data.front()) );

            std::ostringstream stream;

            for ( uint32_t i = 0; i < extent; ++i )
            {
                stream << typename OutputTrait< TYPE >::output_type( data[ i ] ) << " ";
            }

            stream << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };


    template< typename TYPE >
    struct ValuesArrayOp
    {
        int operator()(
                char *buf,
                const Alembic::Abc::CompoundPropertyReaderPtr ptr,
                const std::string& propertyName
                )
        {
            Alembic::Abc::BasePropertyReaderPtr vals = ptr->getProperty(
                    propertyName
                    );

            Alembic::Abc::ArraySamplePtr arraySamplePtr;
            vals->asArrayPtr()->getSample( 0, arraySamplePtr );

            size_t numVals = arraySamplePtr->getDimensions().numPoints();
            TYPE* data = (TYPE *)(arraySamplePtr->getData());

            std::ostringstream stream;

            for ( size_t i = 0; i < numVals; ++i )
            {
                stream << typename OutputTrait< TYPE >::output_type( data[ i ] ) << " ";
            }

            stream << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };

    int handleProperty( PropertyData& propertyData, char* buf )
    {
        PropertyHandler< ValuesScalarOp, ValuesArrayOp > propertyHandler;

        return propertyHandler.handle( propertyData, buf );
    }

private:

};

#endif // ALEMBICFS_VALUESPROPERTYVIEW_HH


