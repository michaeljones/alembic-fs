#include "AlembicFS.hh"

#include <boost/algorithm/string.hpp>

#include <sstream>

AlembicFS* AlembicFS::_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

AlembicFS* AlembicFS::Instance() {
    if(_instance == NULL) {
        _instance = new AlembicFS();
    }
    return _instance;
}

AlembicFS::AlembicFS() {

}

AlembicFS::~AlembicFS() {

}

//
// Additional Methods
//
void AlembicFS::absPath(char dest[PATH_MAX], const char *path)
{
    strcpy(dest, m_root);
    strncat(dest, path, PATH_MAX);
    //printf("translated path: %s to %s\n", path, dest);
}

void AlembicFS::setStat(struct stat* _stat)
{
    m_stat = _stat;
}

void AlembicFS::setFile(const char* path)
{
    m_path = path;

    m_archive = new Alembic::AbcGeom::IArchive(
            Alembic::AbcCoreHDF5::ReadArchive(),
            path,
            Alembic::AbcGeom::ErrorHandler::kQuietNoopPolicy
            );

}

void AlembicFS::setRootDir(const char *path)
{
    printf("setting FS root to: %s\n", path);
    m_root = path;
}

AlembicFS::ClassifiedObject AlembicFS::getObjectFromPath( const char* path )
{
    std::string path_ = path;

    Alembic::AbcGeom::IObject iObj = m_archive->getTop();

    if ( path_ == "/" )
    {
        return ClassifiedObject( iObj, kObject );
    }

    std::vector< std::string > strs;
    std::string delimiter = "/";
    std::string subpath = path_.substr( 1 );
    boost::split( strs, subpath, boost::is_any_of( delimiter ) );

    for ( uint32_t i=0; i < strs.size(); ++i )
    {
        if ( strs[ i ] == "properties" )
        {
            if ( i == strs.size() - 1 )
            {
                return ClassifiedObject( iObj, kProperties );
            }

            // Create vector with remainder (skipping the "properties" entry
            //
            std::vector< std::string > remainder( strs.begin() + i + 1, strs.end() );
            return ClassifiedObject( iObj, kProperty, remainder );
        }
        else
        {
            iObj = iObj.getChild( strs[ i ] );
            if ( ! iObj.valid() )
            {
                break;
            }
        }
    }

    return ClassifiedObject( iObj, kObject );
}


AlembicFS::PropertyData AlembicFS::getPropertyDataFromPath(
        Alembic::AbcGeom::IObject iObj,
        const std::vector< std::string >& path
        )
{
    // Assume no hierarchy for the for the moment
    //
    Alembic::AbcGeom::ICompoundProperty parent = iObj.getProperties();

    std::vector< std::string >::const_iterator it = path.begin();
    std::vector< std::string >::const_iterator end = path.end();

    const Alembic::AbcCoreAbstract::PropertyHeader* property = NULL;

    for ( ; it != end; ++it )
    {
        if ( property && property->isCompound() )
        {
            parent = Alembic::AbcGeom::ICompoundProperty( parent, property->getName() );
        }

        property = parent.getPropertyHeader( *it );

        if ( ! property )
        {
            return PropertyData( NULL, parent );
        }

        if ( ! property->isCompound() )
        {
            return PropertyData( property, parent );
        }
    }

    return PropertyData( property, parent );
}


//
//  Core Filesystem Methods
//
int AlembicFS::getattr( const char *path, struct stat *statbuf )
{
    printf("getattr(%s)\n", path);

    ClassifiedObject cObj = getObjectFromPath( path );

    if ( ! cObj.iObj.valid() )
    {
        return -ENOENT;
    }

    statbuf->st_size = m_stat->st_size;

    switch ( cObj.classification )
    {
        case kObject:
        case kProperties:
        {
            statbuf->st_mode = S_IFDIR | S_IRUSR;
            statbuf->st_size = 4096;
            break;
        }
        case kProperty:
        {
            PropertyData propertyData = getPropertyDataFromPath(
                    cObj.iObj,
                    cObj.remainder
                    );

            if ( ! propertyData.header )
            {
                return -ENOENT;
            }

            switch( propertyData.header->getPropertyType() )
            {
                case Alembic::AbcCoreAbstract::kCompoundProperty:
                {
                    statbuf->st_mode = S_IFDIR | S_IRUSR;
                    statbuf->st_size = 4096;
                    break;
                }
                case Alembic::AbcCoreAbstract::kScalarProperty:
                case Alembic::AbcCoreAbstract::kArrayProperty:
                {
                    statbuf->st_mode = S_IFREG | S_IRUSR;
                    statbuf->st_size = 4096;
                    break;
                }
            }
            break;
        }
    }

    // Fill in the rest of the stat data from the initial stat we did on the abc file
    //
    statbuf->st_dev = m_stat->st_dev;
    statbuf->st_ino = m_stat->st_ino;
    statbuf->st_nlink = m_stat->st_nlink;
    statbuf->st_uid = m_stat->st_uid;
    statbuf->st_gid = m_stat->st_gid;
    statbuf->st_rdev = m_stat->st_rdev;
    statbuf->st_blksize = m_stat->st_blksize;
    statbuf->st_blocks = m_stat->st_blocks;
    statbuf->st_atime = m_stat->st_atime;
    statbuf->st_mtime = m_stat->st_mtime;
    statbuf->st_ctime = m_stat->st_ctime;

    return 0;
}

int AlembicFS::readlink(const char *path, char *link, size_t size) {
    printf("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(readlink(fullPath, link, size));
}

int AlembicFS::mknod(const char *path, mode_t mode, dev_t dev) {
    printf("mknod(path=%s, mode=%d)\n", path, mode);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    
    //handles creating FIFOs, regular files, etc...
    return RETURN_ERRNO(mknod(fullPath, mode, dev));
}

int AlembicFS::mkdir(const char *path, mode_t mode) {
    printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(mkdir(fullPath, mode));
}

int AlembicFS::unlink(const char *path) {
    printf("unlink(path=%s\n)", path);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(unlink(fullPath));
}

int AlembicFS::rmdir(const char *path) {
    printf("rmkdir(path=%s\n)", path);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(rmdir(fullPath));
}

int AlembicFS::symlink(const char *path, const char *link) {
    printf("symlink(path=%s, link=%s)\n", path, link);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(symlink(fullPath, link));
}

int AlembicFS::rename(const char *path, const char *newpath) {
    printf("rename(path=%s, newPath=%s)\n", path, newpath);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(rename(fullPath, newpath));
}

int AlembicFS::link(const char *path, const char *newpath) {
    printf("link(path=%s, newPath=%s)\n", path, newpath);
    char fullPath[PATH_MAX];
    char fullNewPath[PATH_MAX];
    absPath(fullPath, path);
    absPath(fullNewPath, newpath);
    return RETURN_ERRNO(link(fullPath, fullNewPath));
}

int AlembicFS::chmod(const char *path, mode_t mode) {
    printf("chmod(path=%s, mode=%d)\n", path, mode);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(chmod(fullPath, mode));
}

int AlembicFS::chown(const char *path, uid_t uid, gid_t gid) {
    printf("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(chown(fullPath, uid, gid));
}

int AlembicFS::truncate(const char *path, off_t newSize) {
    printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(truncate(fullPath, newSize));
}

int AlembicFS::utime(const char *path, struct utimbuf *ubuf) {
    printf("utime(path=%s)\n", path);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(utime(fullPath, ubuf));
}

int AlembicFS::open(const char *path, struct fuse_file_info *fileInfo)
{
    printf("open(path=%s)\n", path);
    return 0;
}

template< typename TYPE, typename AS >
int readType(
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
        stream << AS(data[ i ]) << " ";
    }

    stream << std::endl;

    sprintf( buf, "%s", stream.str().c_str() );

    return stream.str().size();
}


int AlembicFS::read(
        const char *path,
        char *buf,
        size_t size,
        off_t offset,
        struct fuse_file_info *fileInfo
        )
{
    printf("read(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);

    ClassifiedObject cObj = getObjectFromPath( path );
    if ( ! cObj.iObj.valid() )
    {
        return -ENOENT;
    }

    PropertyData propertyData = getPropertyDataFromPath(
            cObj.iObj,
            cObj.remainder
            );

    if ( ! propertyData.header )
    {
        return -ENOENT;
    }

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
                    Alembic::Abc::IBoolProperty boolProperty(
                            propertyData.parent,
                            propertyData.header->getName()
                            );

                    bool value = boolProperty.getValue();

                    std::ostringstream stream;
                    stream << std::boolalpha << value << std::endl;;

                    sprintf( buf, "%s", stream.str().c_str() );

                    return stream.str().size();
                }
                case Alembic::Util::kUint8POD:
                {
                    return readType< Alembic::Util::uint8_t, uint32_t >(
                            buf,
                            propertyData.parent.getPtr(),
                            propertyData.header->getName(),
                            dataType.getExtent()
                            );
                }
                case Alembic::Util::kInt32POD:
                {
                    return readType< Alembic::Util::int32_t, Alembic::Util::int32_t >(
                            buf,
                            propertyData.parent.getPtr(),
                            propertyData.header->getName(),
                            dataType.getExtent()
                            );
                }
                case Alembic::Util::kUint32POD:
                {
                    return readType< Alembic::Util::uint32_t, Alembic::Util::uint32_t >(
                            buf,
                            propertyData.parent.getPtr(),
                            propertyData.header->getName(),
                            dataType.getExtent()
                            );
                }
                case Alembic::Util::kFloat32POD:
                {
                    return readType< Alembic::Util::float32_t, Alembic::Util::float32_t >(
                            buf,
                            propertyData.parent.getPtr(),
                            propertyData.header->getName(),
                            dataType.getExtent()
                            );
                }
                case Alembic::Util::kFloat64POD:
                {
                    return readType< Alembic::Util::float64_t, Alembic::Util::float64_t >(
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
                case Alembic::Util::kInt32POD:
                {
                    Alembic::Abc::CompoundPropertyReaderPtr ptr = propertyData.parent.getPtr();
                    Alembic::Abc::BasePropertyReaderPtr vals = ptr->getProperty(
                            propertyData.header->getName()
                            );

                    Alembic::Abc::ArraySamplePtr arraySamplePtr;
                    vals->asArrayPtr()->getSample( 0, arraySamplePtr );

                    size_t numVals = arraySamplePtr->getDimensions().numPoints();
                    int32_t* data = (int32_t *)(arraySamplePtr->getData());

                    std::ostringstream stream;

                    for ( size_t i = 0; i < numVals; ++i )
                    {
                        stream << data[ i ] << " ";
                    }

                    stream << std::endl;

                    sprintf( buf, "%s", stream.str().c_str() );

                    return stream.str().size();
                }
                case Alembic::Util::kUint32POD:
                {
                    Alembic::Abc::CompoundPropertyReaderPtr ptr = propertyData.parent.getPtr();
                    Alembic::Abc::BasePropertyReaderPtr vals = ptr->getProperty(
                            propertyData.header->getName()
                            );

                    Alembic::Abc::ArraySamplePtr arraySamplePtr;
                    vals->asArrayPtr()->getSample( 0, arraySamplePtr );

                    size_t numVals = arraySamplePtr->getDimensions().numPoints();
                    uint32_t* data = (uint32_t *)(arraySamplePtr->getData());

                    std::ostringstream stream;

                    for ( size_t i = 0; i < numVals; ++i )
                    {
                        stream << data[ i ] << " ";
                    }

                    stream << std::endl;

                    sprintf( buf, "%s", stream.str().c_str() );

                    return stream.str().size();
                }
                case Alembic::Util::kFloat32POD:
                {
                    Alembic::Abc::CompoundPropertyReaderPtr ptr = propertyData.parent.getPtr();
                    Alembic::Abc::BasePropertyReaderPtr vals = ptr->getProperty(
                            propertyData.header->getName()
                            );

                    Alembic::Abc::ArraySamplePtr arraySamplePtr;
                    vals->asArrayPtr()->getSample( 0, arraySamplePtr );

                    size_t numVals = arraySamplePtr->getDimensions().numPoints();
                    Alembic::Util::float32_t* data = (Alembic::Util::float32_t*)(arraySamplePtr->getData());

                    std::ostringstream stream;

                    for ( size_t i = 0; i < numVals; ++i )
                    {
                        stream << data[ i ] << " ";
                    }

                    stream << std::endl;

                    sprintf( buf, "%s", stream.str().c_str() );

                    return stream.str().size();
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

int AlembicFS::write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    printf("write(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
    return RETURN_ERRNO(pwrite(fileInfo->fh, buf, size, offset));
}

int AlembicFS::statfs(const char *path, struct statvfs *statInfo) {
    printf("statfs(path=%s)\n", path);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(statvfs(fullPath, statInfo));
}

int AlembicFS::flush(const char *path, struct fuse_file_info *fileInfo) {
    printf("flush(path=%s)\n", path);
    //noop because we don't maintain our own buffers
    return 0;
}

int AlembicFS::release(const char *path, struct fuse_file_info *fileInfo) {
    printf("release(path=%s)\n", path);
    return 0;
}

int AlembicFS::fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    printf("fsync(path=%s, datasync=%d\n", path, datasync);
    if(datasync) {
        //sync data only
        return RETURN_ERRNO(fdatasync(fi->fh));
    } else {
        //sync data + file metadata
        return RETURN_ERRNO(::fsync(fi->fh));
    }
}

int AlembicFS::setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
        path, name, value, (int)size, flags);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
}

int AlembicFS::getxattr(const char *path, const char *name, char *value, size_t size)
{
    // Redirect to built-in getxattr called with path to the original alembic file
    //
    return RETURN_ERRNO( ::getxattr( m_path.c_str(), name, value, size ) );
}

int AlembicFS::listxattr(const char *path, char *list, size_t size) {
    printf("listxattr(path=%s, size=%d)\n", path, (int)size);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(llistxattr(fullPath, list, size));
}

int AlembicFS::removexattr(const char *path, const char *name) {
    printf("removexattry(path=%s, name=%s)\n", path, name);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(lremovexattr(fullPath, name));
}

int AlembicFS::readdir(
        const char *path,
        void *buf,
        fuse_fill_dir_t filler,
        off_t offset,
        struct fuse_file_info *fileInfo
        )
{
    printf("readdir(path=%s, offset=%d)\n", path, (int)offset);

    ClassifiedObject cObj = getObjectFromPath( path );

    if ( ! cObj.iObj.valid() )
    {
        return -ENOENT;
    }

    // Should be a switch statement
    //
    if ( cObj.classification == kObject )
    {
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);

        // Add an entry for each child
        //
        for ( size_t i = 0 ; i < cObj.iObj.getNumChildren() ; i++ )
        {
            const char* name = cObj.iObj.getChildHeader( i ).getName().c_str();
            filler(buf, name, NULL, 0);
        }

        // Add an entry for properties
        //
        Alembic::AbcGeom::ICompoundProperty properties = cObj.iObj.getProperties();
        if ( properties.getNumProperties() )
        {
            filler(buf, "properties", NULL, 0);
        }
    }
    else if ( cObj.classification == kProperties )
    {
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);

        Alembic::AbcGeom::ICompoundProperty properties = cObj.iObj.getProperties();

        for ( uint32_t p = 0; p < properties.getNumProperties(); ++p )
        {
            const Alembic::AbcCoreAbstract::PropertyHeader& header =
                properties.getPropertyHeader( p);
            const std::string& name = header.getName();

            filler(buf, name.c_str(), NULL, 0);
        }
    }
    else if ( cObj.classification == kProperty )
    {
        PropertyData propertyData = getPropertyDataFromPath(
                cObj.iObj,
                cObj.remainder
                );

        if ( ! propertyData.header || ! propertyData.header->isCompound() )
        {
            return -ENOENT;
        }

        Alembic::AbcGeom::ICompoundProperty property(
                propertyData.parent,
                propertyData.header->getName()
                );

        for ( uint32_t p = 0; p < property.getNumProperties(); ++p )
        {
            const Alembic::AbcCoreAbstract::PropertyHeader& header =
                property.getPropertyHeader( p);
            const std::string& name = header.getName();

            filler(buf, name.c_str(), NULL, 0);
        }
    }

    return 0;
}

int AlembicFS::releasedir(const char *path, struct fuse_file_info *fileInfo) {
    printf("releasedir(path=%s)\n", path);
    closedir((DIR*)fileInfo->fh);
    return 0;
}

int AlembicFS::fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    return 0;
}

int AlembicFS::truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    printf("truncate(path=%s, offset=%d)\n", path, (int)offset);
    char fullPath[PATH_MAX];
    absPath(fullPath, path);
    return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
}

void* AlembicFS::init(struct fuse_conn_info *conn) {
    return 0;
}

