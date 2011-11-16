#ifndef ALEMBICFS_OUTPUTTRAIT_HH
#define ALEMBICFS_OUTPUTTRAIT_HH

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

template< typename T >
struct OutputTrait
{
    typedef T output_type;
};

template<>
struct OutputTrait< Alembic::Util::int8_t >
{
    typedef int32_t output_type;
};

template<>
struct OutputTrait< Alembic::Util::uint8_t >
{
    typedef uint32_t output_type;
};


#endif // ALEMBICFS_OUTPUTTRAIT_HH


