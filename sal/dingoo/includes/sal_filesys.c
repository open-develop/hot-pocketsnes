

s32 sal_DirectoryGetCurrent(s8 *path, u32 size)
{
	strcpy(path,mCurrDir);
	return SAL_OK;
}

s32 sal_DirectoryGetItemCount(s8 *path, s32 *returnItemCount)
{
	u32 count=1; //always include parent dir entry
	DIR *d;
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
	d=NULL;
	
	*returnItemCount=count;
	return SAL_OK;
}

s32 sal_DirectoryGet(s8 *path, struct SAL_DIRECTORY_ENTRY *dir, s32 startIndex, s32 count)
{
	return SAL_ERROR;
#if 0
	s32 fileCount=0;
	DIR *d;
	struct dirent *de;
	u32 entriesRead=0;
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
	return SAL_OK;
#endif
}

s32 sal_DirectoryOpen(s8 *path, struct SAL_DIR *d)
{
	d->dir=opendir((const char*)path);
	d->needParent=1;
	if(d->dir) return SAL_OK;
	else return SAL_ERROR;
}

s32 sal_DirectoryClose(struct SAL_DIR *d)
{
	if(d)
	{
		if(d->dir)
		{
			closedir(d->dir);
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
		if(d->needParent)
		{
			strcpy(dir->filename,"..");
			strcpy(dir->displayName,"..");
			dir->type=SAL_FILE_TYPE_DIRECTORY;
			d->needParent=0;
			return SAL_OK;
		}
		else
		{
			if(dir)
			{
				de=readdir(d->dir);
				if(de)
				{
					strcpy(dir->filename,de->d_name);
					strcpy(dir->displayName,de->d_name);
					if (de->d_type == FSYS_ATTR_DIR)
					{
						dir->type=SAL_FILE_TYPE_DIRECTORY;
					}
					else
					{
						dir->type=SAL_FILE_TYPE_FILE;
					}
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
	}
	else
	{
		return SAL_ERROR;
	}
}


