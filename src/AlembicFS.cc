#include "AlembicFS.hh"

#include "OutputTrait.hh"

#include <boost/algorithm/string.hpp>

#include <sstream>

AlembicFS* AlembicFS::s_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

AlembicFS* AlembicFS::instance()
{
    return s_instance;
}

void AlembicFS::setInstance( AlembicFS* instance )
{
    s_instance = instance;
}

AlembicFS::AlembicFS(
        const char* root,
        struct stat* statData,
        const char* path,
        Alembic::AbcGeom::IArchive* archive,
        PropertyViewLookup& propertyViewLookup
        )
 : m_root( root ),
   m_stat( statData ),
   m_path( path ),
   m_archive( archive ),
   m_propertyViewLookup( propertyViewLookup )
{
    
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

ClassifiedObject AlembicFS::getObjectFromPath( const char* path )
{
    std::string path_ = path;

    Alembic::AbcGeom::IObject iObj = m_archive->getTop();

    if ( path_ == "/" )
    {
        return ClassifiedObject( iObj, ClassifiedObject::kObject );
    }

    PathSegments strs;
    std::string delimiter = "/";
    std::string subpath = path_.substr( 1 );
    boost::split( strs, subpath, boost::is_any_of( delimiter ) );

    for ( uint32_t i=0; i < strs.size(); ++i )
    {
        if ( strs[ i ] == "properties" )
        {
            if ( i == strs.size() - 1 )
            {
                return ClassifiedObject( iObj, ClassifiedObject::kProperties );
            }

            // Create vector with remainder of the path after the "properties" level
            //
            PathSegments remainder( strs.begin() + i + 1, strs.end() );
            return ClassifiedObject( iObj, ClassifiedObject::kProperty, remainder );
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

    return ClassifiedObject( iObj, ClassifiedObject::kObject );
}


PropertyData AlembicFS::getPropertyDataFromPath(
        Alembic::AbcGeom::IObject iObj,
        const PathSegments& path
        )
{
    // Get the base compound property for the object which contains all the object's properties
    //
    Alembic::AbcGeom::ICompoundProperty parent = iObj.getProperties();

    // Iterate over the property part of the path (which is the only part we have at this point.) If
    // the full path was:
    //
    //    /object1/object2/properties/compound1/compound2/scalarproperty
    //
    // Then here we have and iterate over:
    //
    //    compound1/compound2/scalarproperty
    //
    PathSegments::const_iterator it = path.begin();
    PathSegments::const_iterator end = path.end();

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
            // Copy the remained of the path after this non-compound property we've found
            //
            PathSegments remainder( it + 1, path.end() );
            return PropertyData( property, parent, remainder );
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

    // Clear stat buffer
    memset( statbuf, 0, sizeof( struct stat ) );

    statbuf->st_size = m_stat->st_size;

    switch ( cObj.classification )
    {
        case ClassifiedObject::kObject:
        case ClassifiedObject::kProperties:
        {
            statbuf->st_mode = S_IFDIR | S_IRUSR;
            statbuf->st_size = 4096;
            break;
        }
        case ClassifiedObject::kProperty:
        {
            PropertyData propertyData = getPropertyDataFromPath(
                    cObj.iObj,
                    cObj.remainder
                    );

            if ( ! propertyData.header )
            {
                return -ENOENT;
            }

            // If there is no remainder then handle the property directly
            //
            if ( ! propertyData.remainder.size() )
            {
                switch( propertyData.header->getPropertyType() )
                {
                    // All property types are shown as directories
                    //
                    case Alembic::AbcCoreAbstract::kCompoundProperty:
                    case Alembic::AbcCoreAbstract::kScalarProperty:
                    case Alembic::AbcCoreAbstract::kArrayProperty:
                    {
                        statbuf->st_mode = S_IFDIR | S_IRUSR;
                        statbuf->st_size = 4096;
                        break;
                    }
                }
            }
            // Else handle the contents of the property directory
            else
            {
                PropertyViewLookup::iterator view = m_propertyViewLookup.find(
                        propertyData.remainder[ 0 ]
                        );
                if ( view != m_propertyViewLookup.end() )
                {
                    (*view).second->getAttr( statbuf );
                }
                else
                {
                    return -ENOENT;
                }
            }
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

    if( ( fileInfo->flags & 3 ) != O_RDONLY )
    {
        return -EACCES;
    }

    return 0;
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

    PropertyViewLookup::iterator view = m_propertyViewLookup.find(
            propertyData.remainder[ 0 ]
            );
    if ( view != m_propertyViewLookup.end() )
    {
        return (*view).second->handleProperty( propertyData, buf );
    }

    return -ENOENT;
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
    if ( cObj.classification == ClassifiedObject::kObject )
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
    else if ( cObj.classification == ClassifiedObject::kProperties )
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
    else if ( cObj.classification == ClassifiedObject::kProperty )
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
                break;
            }
            case Alembic::AbcCoreAbstract::kScalarProperty:
            case Alembic::AbcCoreAbstract::kArrayProperty:
            {
                PropertyViewLookup::iterator view = m_propertyViewLookup.begin();
                PropertyViewLookup::iterator end = m_propertyViewLookup.end();

                for ( ; view != end; ++view )
                {
                    filler( buf, view->first.c_str(), NULL, 0 );
                }
                break;
            }
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

