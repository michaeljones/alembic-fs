#ifndef ALEMBICFS_VALUESPROPERTYVIEW_HH
#define ALEMBICFS_VALUESPROPERTYVIEW_HH

#include "PropertyView.hh"
#include "OutputTrait.hh"

#include <sys/stat.h>

class ValuesPropertyView : public PropertyView
{
public:

    ValuesPropertyView() {}
    ~ValuesPropertyView() {}

    void getAttr( struct stat* statbuf )
    {
        statbuf->st_mode = S_IFREG | S_IRUSR;
        statbuf->st_size = 4096;
    }

    template< typename TYPE >
    int readScalarProperty(
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


    template< typename TYPE >
    int readArrayProperty(
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

    int handleProperty( PropertyData& propertyData, char* buf )
    {
        if ( propertyData.remainder[ 0 ] == "values" )
        {
            switch( propertyData.header->getPropertyType() )
            {
                case Alembic::AbcCoreAbstract::kCompoundProperty:
                {
                    return -EISDIR;
                }
                case Alembic::AbcCoreAbstract::kScalarProperty:
                {
                    const Alembic::AbcCoreAbstract::DataType& dataType = propertyData.header->getDataType();

                    switch ( dataType.getPod() )
                    {
                        case Alembic::Util::kBooleanPOD:
                        {
                            return readScalarProperty< Alembic::Util::bool_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        case Alembic::Util::kUint8POD:
                        {
                            return readScalarProperty< Alembic::Util::uint8_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        case Alembic::Util::kInt32POD:
                        {
                            return readScalarProperty< Alembic::Util::int32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        case Alembic::Util::kUint32POD:
                        {
                            return readScalarProperty< Alembic::Util::uint32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        case Alembic::Util::kFloat32POD:
                        {
                            return readScalarProperty< Alembic::Util::float32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        case Alembic::Util::kFloat64POD:
                        {
                            return readScalarProperty< Alembic::Util::float64_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName(),
                                    dataType.getExtent()
                                    );
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                case Alembic::AbcCoreAbstract::kArrayProperty:
                {
                    const Alembic::AbcCoreAbstract::DataType& dataType = propertyData.header->getDataType();

                    switch ( dataType.getPod() )
                    {
                        case Alembic::Util::kBooleanPOD:
                        {
                            return readArrayProperty< Alembic::Util::bool_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kInt8POD:
                        {
                            return readArrayProperty< Alembic::Util::int8_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kUint8POD:
                        {
                            return readArrayProperty< Alembic::Util::uint8_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kInt16POD:
                        {
                            return readArrayProperty< Alembic::Util::int16_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kUint16POD:
                        {
                            return readArrayProperty< Alembic::Util::uint16_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kInt32POD:
                        {
                            return readArrayProperty< Alembic::Util::int32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kUint32POD:
                        {
                            return readArrayProperty< Alembic::Util::uint32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kInt64POD:
                        {
                            return readArrayProperty< Alembic::Util::int64_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kUint64POD:
                        {
                            return readArrayProperty< Alembic::Util::uint64_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kFloat16POD:
                        {
                            return readArrayProperty< Alembic::Util::float16_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kFloat32POD:
                        {
                            return readArrayProperty< Alembic::Util::float32_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        case Alembic::Util::kFloat64POD:
                        {
                            return readArrayProperty< Alembic::Util::float64_t >(
                                    buf,
                                    propertyData.parent.getPtr(),
                                    propertyData.header->getName()
                                    );
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }
        }

        return 0;
    }

private:

};

#endif // ALEMBICFS_VALUESPROPERTYVIEW_HH


