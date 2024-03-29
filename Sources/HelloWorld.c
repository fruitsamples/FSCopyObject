/*
	File:		HelloWorld.c
	
	Contains:	Source to a sample application which calls the FSCopyObject() engine to copy
				a file or folder to a selected destination.

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Copyright � 2002-2004 Apple Computer, Inc., All Rights Reserved
*/


#include <stdio.h>
#include <SIOUX.h>
#include <SIOUXGlobals.h>
#include "FSCopyObject.h"

static void				TestFSCopyObject	  (	DupeAction dupeAction );

static void				TestFSDeleteObjects	  ( );

static OSErr			ChooseSourceAndDest	  (	FSRef	*source, 
												FSRef	*dest );

static OSErr			ChooseObject		  (	FSRef	*objectRef,
												Boolean	getDir);

static OSErr			ExtractSingleItem	  (	const NavReplyRecord	*reply,
												FSRef					*item );

static pascal OSErr		MyFilterProc		  (	Boolean				containerChanged,
												ItemCount			currentLevel,
												OSErr				currentOSErr,
												const FSCatalogInfo	*catalogInfo,
												const FSRef			*ref,
												const FSSpec		*spec,
												const HFSUniStr255	*name,
												void				*yourDataPtr );

int main()
{
	EventRecord dummyEvent;
	UInt8		theChar;

	SIOUXSettings.autocloseonquit = true;
	SIOUXSettings.asktosaveonclose = false;
	SIOUXSettings.showstatusline = true;

	printf( "FSCopyObject 1.5\n\n" );
	printf( "This is a test application to demonstrate FSCopyObject and FSDeleteObjects.\n" );
	printf( "Type 'c' to run FSCopyObject with kDupeActionStandard\n" );
	printf( "Type 'R' to run FSCopyObject with kDupeActionReplace\n" );
	printf( "Type 'r' to run FSCopyObject with kDupeActionRename\n" );
	printf( "Type 'd' to run FSDeleteObjects\n\n" );

	printf( "Type 'q' to quit\n\n" );
	
	while( !SIOUXQuitting )
	{
		WaitNextEvent( everyEvent, &dummyEvent, 10, NULL );
		
		if( dummyEvent.what == keyDown )
		{
		 	theChar = (UInt8) (dummyEvent.message & charCodeMask);
		 	switch( theChar )
		 	{
		 		case	'c':
		 			TestFSCopyObject( kDupeActionStandard );	break;
		 		case	'R':
		 			TestFSCopyObject( kDupeActionReplace );		break;
		 		case	'r':
		 			TestFSCopyObject( kDupeActionRename );		break;
		 		case	'd':
		 			TestFSDeleteObjects();						break;
		 		case	'q':
		 			SIOUXQuitting = true;						break;
		 	}
		}
		else
		{
			SIOUXHandleOneEvent(&dummyEvent);
		}
	}
		
	return 0;
}

static void TestFSCopyObject( DupeAction dupeAction)
{
	FSRef			sourceRef,
					destRef,
					newRef;
	FSSpec			newSpec;
	OSErr			osErr;
	
	/* Get the source and destination objects via Nav Services... */
	osErr = ChooseSourceAndDest( &sourceRef, &destRef );
	if( osErr == noErr )
		osErr = FSCopyObject( &sourceRef, &destRef, 0, kFSCatInfoNone, 
							  dupeAction, NULL, true, false, MyFilterProc, NULL, &newRef, &newSpec);
	
	printf( "%s returned: %d", __FUNCTION__, osErr );
	if( osErr == noErr )
	{	
		newSpec.name[newSpec.name[0]+1] = '\0';
		printf(", '%s'\n",  newSpec.name+1 );
	}
}

static void TestFSDeleteObjects()
{
	FSRef		fileRef;
	OSErr		osErr;
	
	osErr = ChooseObject( &fileRef, false );
	if( osErr == noErr )
		osErr = FSDeleteObjects( &fileRef );
		
	printf( "%s returned: %d\n", __FUNCTION__, osErr );
}

	/* Use Navigation Services (because I'm a good Apple employee)	*/
	/* to choose a source file and a destination folder,			*/
	/* and return them as FSRefs.									*/
static OSErr ChooseSourceAndDest(FSRef *source, FSRef *dest)
{
	OSErr		osErr;
	
							/* get the source				*/
	osErr = ChooseObject( source, false );
	if( osErr == noErr )	/* get the destination folder	*/
		osErr = ChooseObject( dest, true );

	return osErr;
}

static OSErr ChooseObject( FSRef *objectRef, Boolean getDir)
{
	NavDialogOptions	options;
	NavReplyRecord 		reply;	
	OSErr				osErr;
	
	if( objectRef == NULL )
		return paramErr;
	
	osErr = (! NavServicesAvailable()) ? -1 : noErr;

	if( osErr == noErr )
		osErr = NavGetDefaultDialogOptions(&options);
	else
		printf( "Navigation Services is required\n" );

	if (osErr == noErr) {
			/* only select one object at a time */
		options.dialogOptionFlags &= ~kNavAllowMultipleFiles;
			/* if an alias or symfile is selected, return the symfile/alias itself	*/
			/* also support packages												*/
		options.dialogOptionFlags |= kNavDontResolveAliases | kNavSupportPackages;
		
		if( getDir )	/* caller wants a directory	*/
			osErr = NavChooseFolder(NULL, &reply, &options, NULL, NULL, NULL);			
		else			/* caller wants an object */
			osErr = NavChooseObject(NULL, &reply, &options, NULL, NULL, NULL);
	}
	if (osErr == noErr) {
		osErr = ExtractSingleItem(&reply, objectRef);
		
		(void) NavDisposeReply(&reply);
	}	

	return osErr;
}

	/* This function extracts a single FSRef from a NavReplyRecord. */
static OSErr ExtractSingleItem(const NavReplyRecord *reply, FSRef *item)
{
	FSSpec fss;
	SInt32 itemCount;
	DescType junkType;
	AEKeyword junkKeyword;
	Size junkSize;
	OSErr osErr;
	
	osErr = AECountItems(&reply->selection, &itemCount);
	if( itemCount != 1 )	/* we only work with one object at a time */
		osErr = paramErr;
	if( osErr == noErr )
		osErr = AEGetNthPtr(&reply->selection, 1, typeFSS, &junkKeyword, &junkType, &fss, sizeof(fss), &junkSize);
	if( osErr == noErr )
	{
		mycheck(junkType == typeFSS);
		mycheck(junkSize == sizeof(FSSpec));
		
		/* We call FSMakeFSSpec because sometimes Nav is braindead		*/
		/* and gives us an invalid FSSpec (where the name is empty).	*/
		/* While FSpMakeFSRef seems to handle that (and the file system	*/
		/* engineers assure me that that will keep working (at least	*/
		/* on traditional Mac OS) because of the potential for breaking	*/
		/* existing applications), I'm still wary of doing this so		*/
		/* I regularise the FSSpec by feeding it through FSMakeFSSpec.	*/
		
		if( fss.name[0] == 0 )
			osErr = FSMakeFSSpec(fss.vRefNum, fss.parID, fss.name, &fss);
		if( osErr == noErr )
			osErr = FSpMakeFSRef(&fss, item);
	}

	return osErr;
}

static pascal OSErr MyFilterProc(	Boolean				/*containerChanged*/,
									ItemCount			currentLevel,
									OSErr				currentOSErr,
									const FSCatalogInfo	*/*catalogInfo*/,
									const FSRef			*/*ref*/,
									const FSSpec		*spec,
									const HFSUniStr255	*/*name*/,
									void				*/*yourDataPtr*/ )
{	
	FSSpec	tmpSpec = *spec;
	
	/* If an error occured during the copy, you can process that error here			*/
	/* or, for example, you can also process key combo's (i.e. command-'.') to		*/
	/* cancel copies in	progress (i.e. return userCanceledErr)						*/
	/* Note: If currentOSErr != noErr, the FSRef and other info might not be valid	*/

	if( currentOSErr == noErr )
	{
		tmpSpec.name[ tmpSpec.name[0] + 1 ] = '\0';
		printf( "%d  %s: %d\n", currentLevel, tmpSpec.name + 1,currentOSErr );
	} 
	
	/* if noErr: continue with the copy						  */
	/* else bail											  */
	return currentOSErr;
}

