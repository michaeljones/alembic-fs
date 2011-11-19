#ifndef ALEMBICFS_JSONVALUESPROPERTYVIEW_HH
#define ALEMBICFS_JSONVALUESPROPERTYVIEW_HH

#include "PropertyHandler.hh"
#include "PropertyView.hh"
#include "OutputTrait.hh"

#include <sys/stat.h>

class Separator
{
public:

    Separator( const std::string& sep, size_t count )
     : m_count( count ), m_sep( sep ) {}

    friend std::ostream& operator<<( std::ostream& os, Separator& separator );

private:

    size_t m_count;
    const std::string m_sep;
};

std::ostream& operator<<( std::ostream& os, Separator& separator )
{
    if ( separator.m_count <= 1 )
    {
        return os;
    }

    separator.m_count -= 1;

    os << separator.m_sep;
    return os;
}

class JsonValuesPropertyView : public PropertyView
{
public:

    void getAttr( struct stat* statbuf )
    {
        statbuf->st_mode = S_IFREG | S_IRUSR;
        statbuf->st_size = 4096;
    }

    template< typename TYPE >
    struct JsonValuesScalarOp
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

            typename OutputTrait< TYPE >::output_type min
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::max();

            typename OutputTrait< TYPE >::output_type max
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::min();

            Separator separator( ", ", extent );

            stream << "{" << std::endl;
            stream << "    \"values\" : [ ";

            for ( uint32_t i = 0; i < extent; ++i )
            {
                stream << typename OutputTrait< TYPE >::output_type( data[ i ] ) << separator;

                min = std::min(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        min 
                        );

                max = std::max(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        max 
                        );
            }

            stream << " ]," << std::endl;

            stream << "    \"max\" : " << max << "," << std::endl;
            stream << "    \"min\" : " << min << std::endl;

            stream << "}" << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };


    template< typename TYPE >
    struct JsonValuesArrayOp
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

            Separator separator( ", ", numVals );

            typename OutputTrait< TYPE >::output_type min
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::max();

            typename OutputTrait< TYPE >::output_type max
                = std::numeric_limits< typename OutputTrait< TYPE >::output_type >::min();

            stream << "{" << std::endl;
            stream << "    \"values\" : [ ";

            for ( size_t i = 0; i < numVals; ++i )
            {
                stream << typename OutputTrait< TYPE >::output_type( data[ i ] ) << separator;

                min = std::min(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        min 
                        );

                max = std::max(
                        typename OutputTrait< TYPE >::output_type( data[ i ] ),
                        max 
                        );
            }

            stream << " ]," << std::endl;

            stream << " \"max\" : " << max << "," << std::endl;
            stream << " \"min\" : " << min << std::endl;

            stream << "}" << std::endl;

            sprintf( buf, "%s", stream.str().c_str() );

            return stream.str().size();
        }
    };

    int handleProperty( PropertyData& propertyData, char* buf )
    {
        PropertyHandler< JsonValuesScalarOp, JsonValuesArrayOp > propertyHandler;

        return propertyHandler.handle( propertyData, buf );
    }

private:

};

#endif // ALEMBICFS_VALUESPROPERTYVIEW_HH


