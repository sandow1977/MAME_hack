/*      FILE.C
 *
 * High-level file I/O for MIDAS Digital Audio System
 *
 * $Id: file.c,v 1.3 1997/07/31 10:56:40 pekangas Exp $
 *
 * Copyright 1996,1997 Housemarque Inc.
 *
 * This file is part of MIDAS Digital Audio System, and may only be
 * used, modified and distributed under the terms of the MIDAS
 * Digital Audio System license, "license.txt". By continuing to use,
 * modify or distribute this file you indicate that you have
 * read the license and understand and accept it fully.
*/

/****************************************************************************\
* Note! This module basicly calls directly the raw file I/O functions.
* This kind of separation, however, is useful as it allows other kinds
* of file systems, such as a compressed file library, to be added by
* simply relinking MIDAS with another high-level file I/O module.
\****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lang.h"
#include "mtypes.h"
#include "errors.h"
#include "mmem.h"
#include "rawfile.h"
#include "file.h"

RCSID(const char *file_rcsid = "$Id: file.c,v 1.3 1997/07/31 10:56:40 pekangas Exp $";)


/****************************************************************************\
*
* Function:     int fileOpen(char *fileName, int openMode, fileHandle *file);
*
* Description:  Opens a file for reading or writing
*
* Input:        char *fileName          name of file
*               int openMode            file opening mode, see enum rfOpenMode
*               fileHandle *file        pointer to file handle
*
* Returns:      MIDAS error code.
*               File handle is stored in *file.
*
\****************************************************************************/

int CALLING fileOpen(char *fileName, int openMode, fileHandle *file)
{
    int         error;
    int         rfMode;
    fileHandle  hdl;

    /* allocate file structure: */
    if ( (error = memAlloc(sizeof(fileFile), (void**) &hdl)) != OK )
        PASSERROR(ID_fileOpen)

    switch ( openMode )
    {
        case fileOpenRead:      /* open file for reading */
            rfMode = rfOpenRead;
            break;

        case fileOpenWrite:     /* open file for writing */
            rfMode = rfOpenWrite;
            break;

        case fileOpenReadWrite: /* open file for reading and writing */
            rfMode = rfOpenReadWrite;
            break;

        default:
            ERROR(ID_fileOpen, errInvalidArguments);
            return errInvalidArguments;
    }

    /* open file: */
    if ( (error = rfOpen(fileName, rfMode, (rfHandle*) &hdl->rf)) != OK )
        PASSERROR(ID_fileOpen)

    *file = hdl;

    return OK;
}




/****************************************************************************\
*
* Function:     int fileClose(fileHandle file);
*
* Description:  Closes a file opened with fileOpen().
*
* Input:        fileHandle file         handle of an open file
*
* Returns:      MIDAS error code
*
\****************************************************************************/

int CALLING fileClose(fileHandle file)
{
    int         error;

    /* close file: */
    if ( (error = rfClose(file->rf)) != OK )
        PASSERROR(ID_fileClose)

    /* deallocate file structure: */
    if ( (error = memFree(file)) != OK )
        PASSERROR(ID_fileClose)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileGetSize(fileHandle file, long *fileSize);
*
* Description:  Get the size of a file
*
* Input:        fileHandle file         handle of an open file
*               ulong *fileSize         pointer to file size
*
* Returns:      MIDAS error code.
*               File size is stored in *fileSize.
*
\****************************************************************************/

int CALLING fileGetSize(fileHandle file, long *fileSize)
{
    int         error;

    /* get file size to *fileSize: */
    if ( (error = rfGetSize(file->rf, fileSize)) != OK )
        PASSERROR(ID_fileGetSize)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileRead(fileHandle file, void *buffer, ulong numBytes);
*
* Description:  Reads binary data from a file
*
* Input:        fileHandle file         file handle
*               void *buffer            reading buffer
*               ulong numBytes          number of bytes to read
*
* Returns:      MIDAS error code.
*               Read data is stored in *buffer, which must be large enough
*               for it.
*
\****************************************************************************/

int CALLING fileRead(fileHandle file, void *buffer, ulong numBytes)
{
    int         error;

    /* read data from file: */
    if ( (error = rfRead(file->rf, buffer, numBytes)) != OK )
        PASSERROR(ID_fileRead)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileWrite(fileHandle file, void *buffer, ulong numBytes);
*
* Description:  Writes binary data to a file
*
* Input:        fileHandle file         file handle
*               void *buffer            pointer to data to be written
*               ulong numBytes          number of bytes to write
*
* Returns:      MIDAS error code
*
\****************************************************************************/

int CALLING fileWrite(fileHandle file, void *buffer, ulong numBytes)
{
    int         error;

    /* write data to file: */
    if ( (error = rfWrite(file->rf, buffer, numBytes)) != OK )
        PASSERROR(ID_fileWrite)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileSeek(fileHandle file, long newPosition, int seekMode);
*
* Description:  Seeks to a new position in file. Subsequent reads and writes
*               go to the new position.
*
* Input:        fileHandle file         file handle
*               long newPosition        new file position
*               int seekMode            file seek mode, see enum rfSeekMode
*
* Returns:      MIDAS error code
*
\****************************************************************************/

int CALLING fileSeek(fileHandle file, long newPosition, int seekMode)
{
    int         error;
    int         rfMode;

    switch ( seekMode )
    {
        case fileSeekAbsolute:      /* seek to an absolute position */
            rfMode = rfSeekAbsolute;
            break;

        case fileSeekRelative:      /* seek relative to current position */
            rfMode = rfSeekRelative;
            break;

        case fileSeekEnd:           /* seek from end of file */
            rfMode = rfSeekEnd;
            break;

        default:
            ERROR(ID_fileSeek, errInvalidArguments);
            return errInvalidArguments;
    }

    /* seek to new file position: */
    if ( (error = rfSeek(file->rf, newPosition, rfMode)) != OK )
        PASSERROR(ID_fileSeek)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileGetPosition(fileHandle file, long *position);
*
* Description:  Reads the current position in a file
*
* Input:        fileHandle file         file handle
*               long *position          pointer to file position
*
* Returns:      MIDAS error code.
*               Current file position is stored in *position.
*
\****************************************************************************/

int CALLING fileGetPosition(fileHandle file, long *position)
{
    int         error;

    /* get current file position to *position: */
    if ( (error = rfGetPosition(file->rf, position)) != OK )
        PASSERROR(ID_fileGetPosition)

    return OK;
}




/****************************************************************************\
*
* Function:     int fileExists(char *fileName, int *exists);
*
* Description:  Checks if a file exists or not
*
* Input:        char *fileName          file name, ASCIIZ
*               int *exists             pointer to file exists status
*
* Returns:      MIDAS error code.
*               *exists contains 1 if file exists, 0 if not.
*
\****************************************************************************/

int CALLING fileExists(char *fileName, int *exists)
{
    int         error;

    if ( (error = rfFileExists(fileName, exists)) != OK )
        PASSERROR(ID_fileExists);

    return OK;
}
