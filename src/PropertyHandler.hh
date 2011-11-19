#ifndef ALEMBICFS_PROPERTYHANDLER_HH
#define ALEMBICFS_PROPERTYHANDLER_HH

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

template<
    template < class > class SCALAROP,
    template < class > class ARRAYOP
    >
class PropertyHandler
{
public:

    int handle( PropertyData& propertyData, char* buf )
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
                        return SCALAROP< Alembic::Util::bool_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName(),
                                dataType.getExtent()
                                );
                    }
                    case Alembic::Util::kUint8POD:
                    {
                        return SCALAROP< Alembic::Util::uint8_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName(),
                                dataType.getExtent()
                                );
                    }
                    case Alembic::Util::kInt32POD:
                    {
                        return SCALAROP< Alembic::Util::int32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName(),
                                dataType.getExtent()
                                );
                    }
                    case Alembic::Util::kUint32POD:
                    {
                        return SCALAROP< Alembic::Util::uint32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName(),
                                dataType.getExtent()
                                );
                    }
                    case Alembic::Util::kFloat32POD:
                    {
                        return SCALAROP< Alembic::Util::float32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName(),
                                dataType.getExtent()
                                );
                    }
                    case Alembic::Util::kFloat64POD:
                    {
                        return SCALAROP< Alembic::Util::float64_t >()(
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
                        return ARRAYOP< Alembic::Util::bool_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kInt8POD:
                    {
                        return ARRAYOP< Alembic::Util::int8_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kUint8POD:
                    {
                        return ARRAYOP< Alembic::Util::uint8_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kInt16POD:
                    {
                        return ARRAYOP< Alembic::Util::int16_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kUint16POD:
                    {
                        return ARRAYOP< Alembic::Util::uint16_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kInt32POD:
                    {
                        return ARRAYOP< Alembic::Util::int32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kUint32POD:
                    {
                        return ARRAYOP< Alembic::Util::uint32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kInt64POD:
                    {
                        return ARRAYOP< Alembic::Util::int64_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kUint64POD:
                    {
                        return ARRAYOP< Alembic::Util::uint64_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kFloat16POD:
                    {
                        return ARRAYOP< Alembic::Util::float16_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kFloat32POD:
                    {
                        return ARRAYOP< Alembic::Util::float32_t >()(
                                buf,
                                propertyData.parent.getPtr(),
                                propertyData.header->getName()
                                );
                    }
                    case Alembic::Util::kFloat64POD:
                    {
                        return ARRAYOP< Alembic::Util::float64_t >()(
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

        return 0;
    }


private:

};

#endif // ALEMBICFS_PROPERTYHANDLER_HH

