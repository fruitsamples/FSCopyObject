About FSCopyObject:

Contains source to demonstrate how to copy/delete files and folders with HFS+ API's.
It also demonstrates a technique to rename an object if an object of the same name
exists in the destination.

For proper viewing/alignment of the source: disable wrap around text, enable Tabs, and
set tab width to 4 spaces (Project Builder has wrap-around-text enabled, and tab width
set to 8 by default) 

FSCopyObject.mcp is a CodeWarrior 8.3 Standard C Console project that builds Carbon,
Classic, Mach-O SIOUX applications as well as a command line tool (FSCopyObject_CW_D).

FSCopyObject.xcode is a XCode 1.1 CoreServices Tool that builds a command line tool
(FSCopyObject_X_D)

FSCopyObject.pbproj is a Project Builder 2.0.1 CoreServices Tool that builds a command
line tool (FSCopyObject_PB_D).

All binaries demonstrate the use and different features of FSCopyObject and FSDeleteObjects.
Both FSCopyObject and FSDeleteObjects are MP safe routines.

FSCopyObject Version History:

1.5
	Added:	XCode project (FSCopyObject.xcode)
	Added:	New arg list format for Command Line tools
	Added:	Changed kDupeActionIgnore to kDupeActionStandard, no functionality change
	Added:	Optional parameter added to FSCopyObject to provide a name for the new object
	Added:	CopyObjectFilterProcs now must return an error code, and FSCopyObject will
			bail if an error is received from a CopyObjectFilterProc
	Fixed:	Only call GetVolParms once for source and destination vols during Preflight
	Fixed:	[3390210, 3482028] Fixed issue where a zero length object would be created
			when the current user did not have proper permissions to the source object or
			when there was not enough disk space to create the new object object

1.4
	Added:	FSCopyObject now returns an FSSpec to the new object
	Added:	Complete rewrite of copy folder engine to better utilize FSGetCatalogInfoBulk
	Added:	Split CopyForks into DropBox and Regular destination variants
	Added:	dwarning support, for debugging spew
	Fixed:	Only try to use PBHCopyFileSync if the remote volumes actually support it
	Fixed:	No more recursion in FSCopyObject
	Fixed:	[3188680] In the Command Line tool, worked around bug in the X File Manager,
			2489632, where FSPathMakeRef doesn't handle symlinks properly.  It automatically
			resolves symlinks and returns an FSRef to the symlinks target, not the symlink
			itself as expected.
	Fixed:	[3188701] Fixed possible data loss issue when, in replace mode only, the
			source object was inside the destination directory.
	Fixed:	[3188704] Fixed problem where the error code returned from a parameter check
			was being overwritten in FSCopyObject and UnicodeNameGetHFSName
		   
1.3:
	Added: 	Project Builder project (FSCopyObject.pbproj)
	Added:	command line tool target to FSCopyObject.mcp (FSCopyObject_CW_D)
	Moved: 	binaries to build directory

1.2:
	Added:	Entire project now builds in C
	Added:	Mach-O target
	Added:	Options to rename or replace the object being copied
	Fixed:	Fixed issue where some of the info passed to filter proc would,
			under curtain situations, describe the origenal object
			not the new one
	Fixed:	Code cleanup

1.1:
	Fixed:	Worked around issue with FSSetCatalogInfo returning -36 on symbolic links
	Fixed:	Code cleanup

1.0:
	First Checkin

