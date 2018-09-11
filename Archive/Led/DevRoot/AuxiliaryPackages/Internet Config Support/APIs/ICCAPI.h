/* ���Start Header��� */

/* File:         ICCAPI.h
 * Generated by: 1.0d4
 * For:          IC 1.3
 * On:           Sunday, 14 July 1996, 20:19:55
 * 
 * This file is part of the Internet Configuration system and
 * is placed in the public domain for the benefit of all.
 */

/* ���End Header��� */

/*
	IMPORTANT NOTES ABOUT THE C CONVERSION
	--------------------------------------
	
	o Pascal "var" parameters are converted from "var x : y" to "y *x".  This
	  means that when you see the parameter "y *x" you should be aware that
		you *cannot pass in nil*.  In future this restriction may be eased,
		especially for the attr parameter to ICGetPref.  Parameters where nil
		is legal are declared using the explicit pointer type, ie "yPtr x".

	o Strings are *Pascal* strings.  This means that they must be word aligned.
		MPW and Think C do this automatically.  The last time I check, Metrowerks
		C does not.  If it still doesn't, then IMHO it's a bug in their compiler
		and you should report it to them.

	o The canonical Internet Config interface is defined in Pascal.  These
		headers have not been thoroughly tested.  If there is a conflict between
		these headers and the Pascal interfaces, the Pascal should take precedence.
*/

/* ///////////////////////////////////////////////////////////////////////////////// */

#ifndef __ICCAPI__
#define __ICCAPI__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __ICTYPES__
#include "ICTypes.h"
#endif

#ifndef __ICKEYS__
#include "ICKeys.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////// */

enum {
	internetConfigurationComponentType = 'PREF',							/* the component type */
	internetConfigurationComponentSubType = 'ICAp',					  /* the component subtype */
	internetConfigurationComponentInterfaceVersion0 = 0x00000000L,
	internetConfigurationComponentInterfaceVersion1 = 0x00010000L,
	internetConfigurationComponentInterfaceVersion2 = 0x00020000L,
	/* current version number is version 2 */
	internetConfigurationComponentInterfaceVersion = internetConfigurationComponentInterfaceVersion2
};

typedef ComponentInstance internetConfigurationComponent;

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

/* ���Start ICCAPI.h��� */

/* ***** Starting Up and Shutting Down ***** */

pascal ICError ICCStart(internetConfigurationComponent *inst, OSType creator);
  /* Call this at application initialisation. Set creator to your application
   * creator to allow for future expansion of the IC system. Returns
   * inst as a connection to the IC system.
   */

pascal ICError ICCStop(internetConfigurationComponent inst);
  /* It is illegal to call this routine inside a ICBegin/End pair.
   * Call this at application initialisation, after which inst
   * is no longer valid connection to IC.
   */

/* ***** Specifying a Configuration ***** */

pascal ICError ICCFindConfigFile(internetConfigurationComponent inst, short count, ICDirSpecArrayPtr folders)
FIVEWORDINLINE(0x2F3C, 6, 2, 0x7000, 0xA82A);
  /* It is illegal to call this routine inside a ICBegin/End pair.
   * Call to configure this connection to IC.
   * Set count as the number of valid elements in folders.
   * Set folders to a pointer to the folders to search.
   * Setting count to 0 and folders to nil is OK.
   * Searches the specified folders and then the Preferences folder
   * in a unspecified manner.
   */

pascal ICError ICCFindUserConfigFile(internetConfigurationComponent inst, ICDirSpec *where)
FIVEWORDINLINE(0x2F3C, 4, 14, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Similar to ICFindConfigFile except that it only searches the folder
   * specified in where.  If the input parameters are valid the routine
   * will always successful configure the instance, creating an
   * empty configuration if necessary
   * For use with double-clickable preference files.
   */

pascal ICError ICCGeneralFindConfigFile(internetConfigurationComponent inst, Boolean search_prefs, Boolean can_create, short count, ICDirSpecArrayPtr folders)
FIVEWORDINLINE(0x2F3C, 10, 30, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Call to configure this connection to IC.
   * This routine acts as a more general replacement for
   * ICFindConfigFile and ICFindUserConfigFile.
   * Set search_prefs to true if you want it to search the preferences folder.
   * Set can_create to true if you want it to be able to create a new config.
   * Set count as the number of valid elements in folders.
   * Set folders to a pointer to the folders to search.
   * Setting count to 0 and folders to nil is OK.
   * Searches the specified folders and then optionally the Preferences folder
   * in a unspecified manner.
   */

pascal ICError ICCChooseConfig(internetConfigurationComponent inst)
FIVEWORDINLINE(0x2F3C, 0, 33, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Requests the user to choose a configuration, typically using some
   * sort of modal dialog. If the user cancels the dialog the configuration
   * state will be unaffected.
   */

pascal ICError ICCChooseNewConfig(internetConfigurationComponent inst)
FIVEWORDINLINE(0x2F3C, 0, 34, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Requests the user to create a new configuration, typically using some
   * sort of modal dialog. If the user cancels the dialog the configuration
   * state will be unaffected.
   */

pascal ICError ICCGetConfigName(internetConfigurationComponent inst, Boolean longname, Str255 name)
FIVEWORDINLINE(0x2F3C, 6, 35, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * You must specify a configuration before calling this routine.
   * Returns a string that describes the current configuration at a user
   * level. Set longname to true if you want a long name, up to 255
   * characters, or false if you want a short name, typically about 32
   * characters.
   * The returned string is for user display only. If you rely on the
   * exact format of it, you will conflict with any future IC
   * implementation that doesn't use explicit preference files.
   */

pascal ICError ICCGetConfigReference(internetConfigurationComponent inst, ICConfigRefHandle ref)
FIVEWORDINLINE(0x2F3C, 4, 31, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * You must specify a configuration before calling this routine.
   * Returns a self-contained reference to the instance's current
   * configuration.
   * ref must be a valid non-nil handle and it will be resized to fit the
   * resulting data.
   */

pascal ICError ICCSetConfigReference(internetConfigurationComponent inst, ICConfigRefHandle ref, long flags)
FIVEWORDINLINE(0x2F3C, 8, 32, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Reconfigures the instance using a configuration reference that was
   * got using ICGetConfigReference reference. Set the
   * icNoUserInteraction_bit in flags if you require that this routine
   * not present a modal dialog. Other flag bits are reserved and should
   * be set to zero.
   * ref must not be nil.
   */

pascal ICError ICCSpecifyConfigFile(internetConfigurationComponent inst, FSSpec *config)
FIVEWORDINLINE(0x2F3C, 4, 3, 0x7000, 0xA82A);
  /* It is illegal to call this routine inside a ICBegin/End pair.
   * For use only by the IC application.
   * If you call this routine yourself, you will conflict with any
   * future IC implementation that doesn't use explicit preference files.
   */

/* ***** Getting Information ***** */

pascal ICError ICCGetSeed(internetConfigurationComponent inst, long *seed)
FIVEWORDINLINE(0x2F3C, 4, 4, 0x7000, 0xA82A);
  /* You do not have to specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Returns the current seed for the IC prefs database.
   * This seed changes each time a non-volatile preference is changed.
   * You can poll this to determine if any cached preferences change.
   */

pascal ICError ICCGetPerm(internetConfigurationComponent inst, ICPerm *perm)
FIVEWORDINLINE(0x2F3C, 4, 13, 0x7000, 0xA82A);
  /* You do not have to specify a configuration before calling this routine.
   * Returns the access permissions currently associated with this instance.
   * While applications normally know what permissions they have,
   * this routine is designed for use by override components.
   */

pascal ICError ICCDefaultFileName(internetConfigurationComponent inst, Str63 name)
FIVEWORDINLINE(0x2F3C, 4, 11, 0x7000, 0xA82A);
  /* You do not have to specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Returns the default file name for IC preference files.
   * Applications should never need to call this routine.
   * If you rely on information returned by this routine yourself,
   * you may conflict with any future IC implementation that doesn't use
   * explicit preference files.
   * The component calls this routine to set up the default IC file name.
   * This allows this operation to be intercepted by a component that has
   * captured us. It currently gets it from the component resource file.
   * The glue version is hardwired to "Internet Preferences".
   */

/* ***** Reading and Writing Preferences ***** */

pascal ICError ICCBegin(internetConfigurationComponent inst, ICPerm perm)
FIVEWORDINLINE(0x2F3C, 2, 5, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * It is illegal to call this routine inside a ICBegin/End pair.
   * Starting reading or writing multiple preferences.
   * A call to this must be balanced by a call to ICEnd.
   * Do not call WaitNextEvent between these calls.
   * The perm specifies whether you intend to read or read/write.
   * Only one writer is allowed per instance.
   * Note that this may open resource files that are not closed
   * until you call ICEnd.
   */

pascal ICError ICCGetPref(internetConfigurationComponent inst, ConstStr255Param key, ICAttr *attr, Ptr buf, long *size)
FIVEWORDINLINE(0x2F3C, 16, 6, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadOnlyPerm).
   * Reads the preference specified by key from the IC database to the
   * buffer pointed to by buf and size.
   * key must not be the empty string.
   * If buf is nil then no data is returned.
   * size must be non-negative.
   * attr and size are always set on return. On errors (except icTruncatedErr)
   * attr is set to ICattr_no_change and size is set to 0.
   * size is the actual size of the data.
   * attr is set to the attributes associated with the preference.
   * If this routine returns icTruncatedErr then the other returned
   * values are valid except that only the first size bytes have been
   * return. size is adjusted to reflect the true size of the preference.
   * Returns icPrefNotFound if there is no preference for the key.
   */

pascal ICError ICCSetPref(internetConfigurationComponent inst, ConstStr255Param key, ICAttr attr, Ptr buf, long size)
FIVEWORDINLINE(0x2F3C, 16, 7, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadWritePerm).
   * Sets the preference specified by key from the IC database to the
   * value pointed to by buf and size.
   * key must not be the empty string.
   * size must be non-negative. 
   * If buf is nil then the preference value is not set and size is ignored.
   * If buf is not nil then the preference value is set to the size
   * bytes pointed to by buf.
   * If attr is ICattr_no_change then the preference attributes are not set.
   * Otherwise the preference attributes are set to attr.
   * Returns icPermErr if the previous ICBegin was passed icReadOnlyPerm.
   * Returns icPermErr if current attr is locked, new attr is locked and buf <> nil.
   */

pascal ICError ICCFindPrefHandle(internetConfigurationComponent inst, ConstStr255Param key, ICAttr *attr, Handle prefh)
FIVEWORDINLINE(0x2F3C, 12, 36, 0x7000, 0xA82A);
  /* Requires IC 1.2.
   * You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadWritePerm).
   * This routine effectively replaces ICGetPrefHandle.
   * Reads the preference specified by key from the IC database into
   * a handle, prefh.
   * key must not be the empty string.
   * attr is set to the attributes associated with the preference.
   * You must set prefh to a non-nil handle before calling this routine.
   * If the preference does not exist, icPrefNotFoundErr is returned.
   */

pascal ICError ICCGetPrefHandle(internetConfigurationComponent inst, ConstStr255Param key, ICAttr *attr, Handle *prefh)
FIVEWORDINLINE(0x2F3C, 12, 26, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadOnlyPerm).
   * This routine is now obsolete. Use ICFindPrefHandle instead.
   * Reads the preference specified by key from the IC database into
   * a newly created handle, prefh.
   * key must not be the empty string.
   * attr is set to the attributes associated with the preference.
   * The incoming value of prefh is ignored.
   * A new handle is created in the current heap and returned in prefh.
   * If the routine returns an error, prefh is set to nil.
   * If the preference does not exist, no error is returned and prefh is set
   * to an empty handle.
   */

pascal ICError ICCSetPrefHandle(internetConfigurationComponent inst, ConstStr255Param key, ICAttr attr, Handle prefh)
FIVEWORDINLINE(0x2F3C, 12, 27, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadWritePerm).
   * Sets the preference specified by key from the IC database to the
   * value contained in prefh.
   * key must not be the empty string.
   * If prefh is nil then the preference value is not set.
   * If buf is not nil then the preference value is set to the data
   * contained in it.
   * If attr is ICattr_no_change then the preference attributes are not set.
   * Otherwise the preference attributes are set to attr.
   * Returns icPermErr if the previous ICBegin was passed icReadOnlyPerm.
   * Returns icPermErr if current attr is locked, new attr is locked and prefh <> nil.
   */

pascal ICError ICCCountPref(internetConfigurationComponent inst, long *count)
FIVEWORDINLINE(0x2F3C, 4, 8, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * You must be inside an ICBegin/End pair to call this routine.
   * Counts the total number of preferences.
   * If the routine returns an error, count is set to 0.
   */

pascal ICError ICCGetIndPref(internetConfigurationComponent inst, long n, Str255 key)
FIVEWORDINLINE(0x2F3C, 8, 9, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * You must be inside an ICBegin/End pair to call this routine.
   * Returns the key of the Nth preference.
   * n must be positive.
   * Returns icPrefNotFoundErr if n is greater than the total number of preferences.
   * If the routine returns an error, key is undefined.
   */

pascal ICError ICCDeletePref(internetConfigurationComponent inst, ConstStr255Param key)
FIVEWORDINLINE(0x2F3C, 4, 12, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * You must be inside an ICBegin/End pair to call this routine.
   * Deletes the preference specified by key.
   * key must not be the empty string.
   * Returns icPrefNotFound if the preference specified by key is not present.
   */

pascal ICError ICCEnd(internetConfigurationComponent inst)
FIVEWORDINLINE(0x2F3C, 0, 10, 0x7000, 0xA82A);
  /* You must specify a configuration before calling this routine.
   * You must be inside an ICBegin/End pair to call this routine.
   * Terminates a preference session, as started by ICBegin.
   * You must have called ICBegin before calling this routine.
   */

/* ***** User Interface Stuff ***** */

pascal ICError ICCEditPreferences(internetConfigurationComponent inst, ConstStr255Param key)
FIVEWORDINLINE(0x2F3C, 4, 15, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Instructs IC to display the user interface associated with editing
   * preferences and focusing on the preference specified by key.
   * If key is the empty string then no preference should be focused upon.
   * You must have specified a configuration before calling this routine.
   * You do not need to call ICBegin before calling this routine.
   * In the current implementation this launches the IC application
   * (or brings it to the front) and displays the window containing
   * the preference specified by key.
   * It may have a radically different implementation in future
   * IC systems.
   */

/* ***** URL Handling ***** */

pascal ICError ICCParseURL(internetConfigurationComponent inst, ConstStr255Param hint, Ptr data, long len, long *selStart, long *selEnd, Handle url)
FIVEWORDINLINE(0x2F3C, 24, 16, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Parses a URL out of the specified text and returns it in a canonical form
   * in a handle.
   * hint indicates the default scheme for URLs of the form "name@address".
   * If hint is the empty string then URLs of that form are not allowed.
   * data points to the start of the text. It must not be nil.
   * len indicates the length of the text. It must be non-negative.
   * selStart and selEnd should be passed in as the current selection of
   * the text. This selection is given in the same manner as TextEdit,
   * ie if selStart = selEnd then there is no selection only an insertion
   * point. Also selStart � selEnd and 0 � selStart � len and 0 � selEnd � len.
   * selStart and selEnd are returned as the bounds of the URL. If the
   * routine returns an error then these new boundaries may be
   * invalid but they will be close.
   * The incoming url handle must not be nil.  The resulting URL is normalised
   * and copied into the url handle, which is resized to fit.
   */

pascal ICError ICCLaunchURL(internetConfigurationComponent inst, ConstStr255Param hint, Ptr data, long len, long *selStart, long *selEnd)
FIVEWORDINLINE(0x2F3C, 20, 17, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Parses a URL out of the specified text and feeds it off to the
   * appropriate helper.
   * hint indicates the default scheme for URLs of the form "name@address".
   * If hint is the empty string then URLs of that form are not allowed.
   * data points to the start of the text. It must not be nil.
   * len indicates the length of the text. It must be non-negative.
   * selStart and selEnd should be passed in as the current selection of
   * the text. This selection is given in the same manner as TextEdit,
   * ie if selStart = selEnd then there is no selection only an insertion
   * point. Also selStart � selEnd and 0 � selStart � len and 0 � selEnd � len.
   * selStart and selEnd are returned as the bounds of the URL. If the
   * routine returns an error then these new boundaries may be
   * invalid but they will be close.
   * The URL is parsed out of the text and passed off to the appropriate
   * helper using the GURL AppleEvent.
   */

/* ***** Mappings Routines *****
 * 
 * Routines for interrogating mappings database.
 * 
 * ----- High Level Routines -----
 */

pascal ICError ICCMapFilename(internetConfigurationComponent inst, ConstStr255Param filename, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 8, 24, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * If you are getting or setting multiple preferences, you should place
   * these calls within an ICBegin/ICEnd pair.
   * If you call this routine outside of such a pair, it implicitly
   * calls ICBegin(inst, icReadWritePerm).
   * Takes the name of an incoming file and returns the most appropriate
   * mappings database entry, based on its extension.
   * filename must not be the empty string.
   * Returns icPrefNotFoundErr if no suitable entry is found.
   */

pascal ICError ICCMapTypeCreator(internetConfigurationComponent inst, OSType fType, OSType fCreator, ConstStr255Param filename, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 16, 25, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You must be inside an ICBegin/End pair to call this routine.
   * Takes the type and creator (and optionally the name) of an outgoing
   * file and returns the most appropriate mappings database entry.
   * The filename may be either the name of the outgoing file or
   * the empty string.
   * Returns icPrefNotFoundErr if no suitable entry found.
   */

/* ----- Mid Level Routines ----- */

pascal ICError ICCMapEntriesFilename(internetConfigurationComponent inst, Handle entries, ConstStr255Param filename, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 12, 28, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Takes the name of an incoming file and returns the most appropriate
   * mappings database entry, based on its extension.
   * entries must be a handle to a valid IC mappings database preference.
   * filename must not be the empty string.
   * Returns icPrefNotFoundErr if no suitable entry is found.
   */

pascal ICError ICCMapEntriesTypeCreator(internetConfigurationComponent inst, Handle entries, OSType fType, OSType fCreator, ConstStr255Param filename, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 20, 29, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Takes the type and creator (and optionally the name) of an outgoing
   * file and returns the most appropriate mappings database entry.
   * entries must be a handle to a valid IC mappings database preference.
   * The filename may be either the name of the outgoing file or
   * the empty string.
   * Returns icPrefNotFoundErr if no suitable entry found.
   */

/* ----- Low Level Routines ----- */

pascal ICError ICCCountMapEntries(internetConfigurationComponent inst, Handle entries, long *count)
FIVEWORDINLINE(0x2F3C, 8, 18, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Counts the number of entries in the mappings database.
   * entries must be a handle to a valid IC mappings database preference.
   * count is set to the number of entries.
   */

pascal ICError ICCGetIndMapEntry(internetConfigurationComponent inst, Handle entries, long ndx, long *pos, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 16, 19, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Gets the ndx'th entry in the mappings database.
   * entries must be a handle to a valid IC mappings database preference.
   * ndx must be in the range from 1 to the number of entries in the database.
   * The value of pos is ignored on input. pos is set to the position of
   * the ndx'th entry in the database and is suitable for passing back
   * into ICSetMapEntry.
   * Does not return any user data associated with the entry.
   */

pascal ICError ICCGetMapEntry(internetConfigurationComponent inst, Handle entries, long pos, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 12, 20, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Returns the entry located at position pos in the mappings database.
   * entries must be a handle to a valid IC mappings database preference.
   * pos should be 0 to get the first entry. To get the subsequent entries, add
   * entry.total_size to pos and iterate.
   * Does not return any user data associated with the entry.
   */

pascal ICError ICCSetMapEntry(internetConfigurationComponent inst, Handle entries, long pos, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 12, 21, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Sets the entry located at position pos in the mappings database.
   * entries must be a handle to a valid IC mappings database preference.
   * pos should be either a value returned from ICGetIndMapEntry or a value
   * calculated using ICGetMapEntry.
   * entry is a var parameter purely for stack space reasons. It is not
   * modified in any way.
   * Any user data associated with the entry is unmodified.
   */

pascal ICError ICCDeleteMapEntry(internetConfigurationComponent inst, Handle entries, long pos)
FIVEWORDINLINE(0x2F3C, 8, 22, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Deletes the mappings database entry at pos.
   * entries must be a handle to a valid IC mappings database preference.
   * pos should be either a value returned from ICGetIndMapEntry or a value
   * calculated using ICGetMapEntry.
   * Also deletes any user data associated with the entry.
   */

pascal ICError ICCAddMapEntry(internetConfigurationComponent inst, Handle entries, ICMapEntry *entry)
FIVEWORDINLINE(0x2F3C, 8, 23, 0x7000, 0xA82A);
  /* Requires IC 1.1.
   * You must specify a configuration before calling this routine.
   * You do not have to be inside an ICBegin/End pair to call this routine.
   * Adds an entry to the mappings database.
   * entries must be a handle to a valid IC mappings database preference.
   * The entry is added to the end of the entries database.
   * No user data is added.
   */


/* ���End ICCAPI.h��� */

#ifdef __cplusplus
}
#endif __cplusplus

#endif