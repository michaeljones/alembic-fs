#ifndef ALEMBICFS_MINVALUEPROPERTYVIEW_HH
#define ALEMBICFS_MINVALUEPROPERTYVIEW_HH

#include "PropertyHandler.hh"
#include "PropertyView.hh"
#include "OutputTrait.hh"

#include <sys/stat.h>

class MinValuePropertyView : public PropertyView
{
public:

    void getAttr( struct stat* statbuf )
    {
        statbuf->st_mode = S_IFREG | S_IRUSR;
        statbuf->st_size = 4096;
    }

    template< typename TYPE >
    struct MinScalarOp
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

            typename OutputTrait< TYPE >::output_type min
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::max();

            for ( uint32_t i = 0; i < extent; ++i )
            {
                min = std::min(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        min 
                        );
            }

            std::ostringstream stream;
            stream << min << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };


    template< typename TYPE >
    struct MinArrayOp
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

            typename OutputTrait< TYPE >::output_type min
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::max();

            for ( size_t i = 0; i < numVals; ++i )
            {
                min = std::min(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        min 
                        );
            }

            std::ostringstream stream;
            stream << min << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };

    int handleProperty( PropertyData& propertyData, char* buf )
    {
        PropertyHandler< MinScalarOp, MinArrayOp> propertyHandler;

        return propertyHandler.handle( propertyData, buf );
    }

private:

};

#endif // ALEMBICFS_MINVALUEPROPERTYVIEW_HH


