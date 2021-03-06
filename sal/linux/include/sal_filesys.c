/*
** Linux filesys for SAL.
**  posix file system
*/

#include <string.h>
#include <error.h>
#include <sys/stat.h>
#include <time.h>
#include "sal.h"

#ifndef SAL_NO_FILE_TIME_STAMP_SUPPORT
/* if there is no hardware clock - do not define SAL_FILE_TIME_STAMP_SUPPORT */
void sal_FileGetModTime(char * filename, char date_str[])
{
	struct stat aStat;
	struct tm *my_date = NULL;

	/*
	** Get file time stamp
	** stat() and then use st_mtime to convert to ISO format local date/time
	** if stat() fails, make string empty
	*/
	if (! stat(filename, &aStat))
	{
		my_date = localtime(&aStat.st_mtime);
		sprintf(date_str, "%04d-%02d-%02d %02d:%02d:%02d",
			1900 + my_date->tm_year,
			my_date->tm_mon + 1, my_date->tm_mday, 
			my_date->tm_hour, my_date->tm_min, my_date->tm_sec);
	}
	else
	{
		date_str[0]='\0';
	}
}
#endif /* SAL_NO_FILE_TIME_STAMP_SUPPORT */

s32 sal_DirectoryGetCurrent(s8 *path, u32 size)
{
	getcwd(path,size);
	return SAL_OK;
}

s32 sal_DirectoryCreate(s8 *path)
{
	s32 count=0;
	mkdir(path, 0x777);
	return SAL_OK;
}

s32 sal_DirectoryGetItemCount(s8 *path, s32 *returnItemCount)
{
	u32 count=0;
	struct DIR *d;
	struct dirent *de;

	d = opendir((const char*)path);

	if (d)
	{
		while ((de = readdir(d)))
		{
			count++;
		}
		closedir(d);
	}

	*returnItemCount=count;
	return SAL_OK;
}

s32 sal_DirectoryOpen(s8 *path, struct SAL_DIR *d)
{
	d->dir=opendir((const char*)path);

	if(d->dir) return SAL_OK;
	else return SAL_ERROR;
}

s32 sal_DirectoryClose(struct SAL_DIR *d)
{
	if(d)
	{
		if(d->dir)
		{
			(void) closedir(d->dir);
			d->dir=NULL;
			return SAL_OK;
		}
		else
		{
			return SAL_ERROR;
		}
	}
	else
	{
		return SAL_ERROR;
	}
}

s32 sal_DirectoryRead(struct SAL_DIR *d, struct SAL_DIRECTORY_ENTRY *dir)
{
	struct dirent *de=NULL;

	if(d)
	{
		if(dir)
		{
			de=readdir(d->dir);
			if(de)
			{
				/* Ignore "." */
				if (!strcmp(de->d_name, "."))
					de=readdir(d->dir);
			}
			if(de)
			{
				strcpy(dir->filename,de->d_name);
				strcpy(dir->displayName,de->d_name);
				if (de->d_type == 4)
					dir->type=SAL_FILE_TYPE_DIRECTORY;
				else
					dir->type=SAL_FILE_TYPE_FILE;
				return SAL_OK;
			}
			else
			{
				return SAL_ERROR;
			}
		}
		else
		{
			return SAL_ERROR;
		}
	}
	else
	{
		return SAL_ERROR;
	}
}

s32 sal_DirectoryGet(s8 *path, struct SAL_DIRECTORY_ENTRY *dir, s32 startIndex, s32 count)
{
	s32 fileCount=0;
	struct DIR *d;
	struct dirent *de;
	ulong entriesRead=0;
	char fullFilename[256];
	s32 endIndex=startIndex+count;
	long loc;

	d = opendir((const char*)path);

	if (d)
	{
		loc=telldir(d);
		seekdir(d,loc+startIndex);
		while ((de = readdir(d)))
		{
			if(startIndex >= endIndex)
			{
				//exit loop
				break;
			}

			//Is entry a file or directory
			if (de->d_type == 4) // Directory
			{
				strcpy(dir[fileCount].filename,de->d_name);
				strcpy(dir[fileCount].displayName,de->d_name);
				dir[fileCount].type=SAL_FILE_TYPE_DIRECTORY;
			}
			else
			{
				//File
				strcpy(dir[fileCount].filename,de->d_name);
				strcpy(dir[fileCount].displayName,de->d_name);
				dir[fileCount].type=SAL_FILE_TYPE_FILE;
			}
			fileCount++;
			startIndex++;
		}
		closedir(d);
	}
	return SAL_ERROR;
}



void sal_DirectoryGetParent(s8 *path)
{
	s32 i=0;
	s32 lastDir=-1, firstDir=-1;
	s8 dirSep[2] = {SAL_DIR_SEP};
	s8 dirSepBad[2] = {SAL_DIR_SEP_BAD};
	s32 len=(s32)strlen(path);

	if (len==1)
	{
		path[0]=dirSep[0];
		return;
	}
	
	for(i=0;i<len;i++)
	{
		if ((path[i] == dirSep[0]) || (path[i] == dirSepBad[0]))
		{
			//Directory seperator found
			if(lastDir==-1) firstDir = i;
			if(i+1 != len) lastDir = i;
		}
	}

	if (lastDir == firstDir) lastDir++;
	if (lastDir >= 0)
	{
		for(i=lastDir; i<len; i++)
		{
			path[i]=0;
		}
	}
	else
	{
		path[0]=0;
	}
}
