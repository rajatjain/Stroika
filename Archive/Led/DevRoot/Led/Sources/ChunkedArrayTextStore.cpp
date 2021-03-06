/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/ChunkedArrayTextStore.cpp,v 2.78 2003/11/26 16:19:38 lewis Exp $
 *
 * Changes:
 *	$Log: ChunkedArrayTextStore.cpp,v $
 *	Revision 2.78  2003/11/26 16:19:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.77  2003/03/21 14:46:42  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.76  2003/03/19 14:23:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.75  2003/03/19 01:52:46  lewis
 *	SPR#1352 - minor (possible) speed tweek to OVERLAP code
 *	
 *	Revision 2.74  2003/03/16 16:15:30  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of
 *	direct calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.73  2003/03/04 21:30:27  lewis
 *	SPR#1315 - In ChunkedArrayTextStore - use new LRUCache<ELEMENT> to keep track of
 *	CollectAllMarkersEtc... cache instead of private LRU cache implementation.
 *	
 *	Revision 2.72  2002/11/21 13:45:01  lewis
 *	added assert
 *	
 *	Revision 2.71  2002/11/17 15:55:45  lewis
 *	SPR#1173- Replace/ReplaceWithoutUpdate change
 *	
 *	Revision 2.70  2002/10/24 01:57:13  lewis
 *	GCC bug workaround - qVirtualBaseMixinCallDuringCTORBug
 *	
 *	Revision 2.69  2002/09/13 15:29:23  lewis
 *	Add TextStore::ConstructNewTextStore () support
 *	
 *	Revision 2.68  2002/09/04 01:52:35  lewis
 *	Lose qCannotExpandTemplateOfTypeWithNoExternalLinkage as it appears to NOT be a bug
 *	
 *	Revision 2.67  2002/05/06 21:33:41  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.66  2001/11/27 00:29:48  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.65  2001/09/26 21:19:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.64  2001/08/30 18:42:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.63  2001/08/29 23:36:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.62  2001/08/28 18:54:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.61  2001/08/28 18:43:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.60  2001/07/19 02:21:45  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.59  2001/04/26 16:39:29  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.58  2001/03/05 21:17:05  lewis
 *	SPR#0855- be sure TextStore itself is part of MarkerOwner list (so it gets adjusted
 *	by new code in ChunkedArrayTextStore which keeps separate marker trees for each owner)
 *	
 *	Revision 2.57  2001/01/03 14:39:59  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
 *	
 *	Revision 2.56  2000/09/08 15:59:46  lewis
 *	add a little more debugging help if you accidentally leave a marker in a Led textstore -
 *	added debug code that collects all the offending markers into a vector after
 *	the assert erorr - so easier to track down
 *	
 *	Revision 2.55  2000/08/13 05:21:46  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 2.54  2000/08/13 03:30:14  lewis
 *	SPR#0822- added TextStore::PreRemoveMarker
 *	
 *	Revision 2.53  2000/07/27 04:29:12  lewis
 *	SPR#0300 finally fixed (I hope). Fixed code I for keeping track of hackmarker statistics so
 *	worked propelry with new multi-rooted tree. That helped me to stumble accross this bug again -
 *	and with the multirooted trees, it was a little easier to see where the bug had to be.
 *	HOPE this is now fixed!
 *	
 *	Revision 2.52  2000/07/26 06:05:53  lewis
 *	fix bug with last checkin - must create teh fTextStoreHook for the ChunkedArrayTextStore
 *	itself (for hackmarkers it creates
 *	
 *	Revision 2.51  2000/07/26 05:51:13  lewis
 *	re-disable hack workaround for why still some hack-markers left around. Maybe easier
 *	to debug now? Try and see if/when it fails again
 *	
 *	Revision 2.50  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg
 *	to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a
 *	table of speedups.
 *	
 *	Revision 2.49  2000/05/30 13:34:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2000/04/27 02:00:53  lewis
 *	SPR#0745- revised TextStore::Overlap algorithm (AGAIN), and made corresponding change
 *	to ChunkedArrayTexxtStore's QUICK_Overlap() function. Updated docs as well. This change
 *	was motiviated by a bug where CollectAllMarkersInRange(N.N+1) didnt seem to be capturing
 *	a marker which was at N+1 - and that seemed strange
 *	
 *	Revision 2.47  2000/04/24 21:10:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.46  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.45  2000/03/31 16:59:13  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro
 *	
 *	Revision 2.44  2000/01/11 22:15:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.43  1999/12/11 18:33:38  lewis
 *	qUseLRUCacheForRecentlyLookedUpMarkers support as part of SPR#0652
 *	
 *	Revision 2.42  1999/11/28 13:50:54  lewis
 *	Fix Typos
 *	
 *	Revision 2.41  1999/11/28 13:38:22  lewis
 *	add extra Led_Require() check in DTOR to make end-user debugging easier
 *	
 *	Revision 2.40  1999/11/13 22:16:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.39  1999/11/13 22:14:35  lewis
 *	cleanup DTOR code so we get better assert error and give pointer to where in docs it
 *	talks about hwo to debug cases where we forget to remove markers
 *	
 *	Revision 2.38  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.37  1999/07/18 01:30:01  lewis
 *	add qIncludePrefixFile support
 *	
 *	Revision 2.36  1999/07/13 22:39:17  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 2.35  1999/06/28 14:57:24  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.34  1999/05/03 22:05:06  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.33  1999/05/03 21:41:26  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.32  1999/03/10 15:41:22  lewis
 *	some minor (GROSS) speed tweeks to QUICK_Overlap() and the CollectAllMarkers code. This
 *	is about as tweeked as I can imagine this code getting without major redesign
 *	
 *	Revision 2.31  1998/10/30 14:28:29  lewis
 *	Use vector<> instead of Led_Array<>.
 *	And I THINK ChunkedArrayTextStore::TextChunk moved here from header file?
 *	
 *	Revision 2.30  1998/07/24  01:06:31  lewis
 *	re-enable the gawd-aweful hack/workaround for my linger buggy cleaning up hack
 *	markers (in ChunkedArrayTextStore::DTOR).
 *
 *	Revision 2.29  1998/04/25  01:30:12  lewis
 *	Require added markers have owner which has already been added to the textstore.
 *
 *	Revision 2.28  1998/04/08  01:46:04  lewis
 *	Disable workaround for fTotalHackMarkersPresent bug for now. Comes up very rarely.
 *	While in the early develepment stages
 *	see if I can find a decent reprodicible case. Maybe re-enabel again
 *	for final 2.3 release?
 *
 *	Revision 2.27  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.26  1997/09/29  21:27:30  lewis
 *	Change definition of Overlap (spr#0489).
 *
 *	Revision 2.25  1997/09/29  15:01:38  lewis
 *	Lose qLedFirstIndex support and qSupportLed21CompatAPI.
 *	qKeepTrackOfChildCountAndAvoidSomePossiblyAdds and other hacks to speed AddMarker1.
 *
 *	Revision 2.24  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.23  1997/07/15  05:26:14  lewis
 *	Fixed serious crasher bugs (DOA) with qLedFirstIndex=0
 *
 *	Revision 2.22  1997/07/12  20:11:57  lewis
 *	Renamed TextStore_ to TextStore.
 *	qLedFirstIndex = 0 support
 *	AutoDoc support.
 *
 *	Revision 2.21  1997/06/24  03:30:47  lewis
 *	Lose old test code
 *
 *	Revision 2.20  1997/06/23  16:15:03  lewis
 *	More speed tweeks - SetMarkerLength_/SetMarkerStart_ wrappers that avoid virtual function calls.
 *	(from profiling rtf read).
 *	Similalrly lose GetRange calls.
 *
 *	Revision 2.19  1997/06/18  20:08:15  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/06/18  02:55:49  lewis
 *	CollectAllMarkersInRangeIntoAndBelow_21Compat.
 *	MarkerSink& support.
 *	QUICK_Contains hack, and QUICK_Overlap.
 *
 *	Revision 2.17  1997/01/10  03:10:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.15  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1996/10/31  00:10:53  lewis
 *	Add assertion.
 *
 *	Revision 2.13  1996/09/03  15:04:48  lewis
 *	Add assertion.
 *
 *	Revision 2.12  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/08/05  06:03:12  lewis
 *	Change param names to fit new programmer docs.
 *
 *	Revision 2.10  1996/06/01  02:07:33  lewis
 *	Lose old no-RTTI support.
 *
 *	Revision 2.9  1996/05/23  19:32:47  lewis
 *	Mosely just added a couple throw specifiers.
 *	Lose obsolete GetMarkerRange ().
 *	Support new ChunkedArrayMarkerHook::GetStartEnd ()
 *	Use GetRange() in a couple places instead of old 2-virtual lookup way of initializing.
 *
 *	Revision 2.8  1996/03/16  18:59:13  lewis
 *	Invariant/WalkSubTreeAndCheckInvariants() now const.
 *	A number of exception-safety cleanups - Adding hack markers can no longer
 *	ever raise an exception (just silently don't add them).
 *	And setting marker range can no longer raise excpetion.
 *	I think the only remaining exception bug in this code is that
 *	PossiblyAddHackMarkers() isn't 100% bulletproof in case of excpetions.
 *	So - if debugging - I do a catch and Assert(false) to warn trouble may
 *	come up.
 *
 *	Revision 2.7  1996/02/26  18:53:51  lewis
 *	Use Led_Max etc instead of Max to avoid name conflicts.
 *
 *	Revision 2.6  1996/01/22  05:16:56  lewis
 *	Lose qTmpHackNoTryCatch workaround.
 *
 *	Revision 2.5  1995/12/09  05:37:00  lewis
 *	When adding marker, assert owner->PeekAtTextStore() == this TextStore.
 *
 *	Revision 2.4  1995/12/06  01:32:06  lewis
 *	Work around new qCannotExpandTemplateOfTypeWithNoExternalLinkage.
 *
 *	Revision 2.3  1995/10/19  22:19:57  lewis
 *	Got rid of bug workarounds for old compiler bugs.
 *	New xtra arg to UpdateInfo.
 *	Use DoAboutToUpdateCalls/Did instead of manual loops
 *
 *	Revision 2.2  1995/10/09  22:28:22  lewis
 *	UpdateInfo moved form Marker to MarkerOwner class and added
 *	DidUpdateText/AboutToUpdateText methods to markerOwners
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.19  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.18  1995/05/29  23:55:38  lewis
 *	Huge changes - again!
 *	First ripped out most of the ifdefs backward comatability stuff.
 *	Then fixed a number of memleak bugs (in the presence of excpetions - SPR 0290.
 *	Handle exceptions in calls to DidUpdate()- SPR 0294.
 *	Reworked ChunkedArrayTextStore::PossiblyAddHackMarkers () to handle excpetions - SPR 0293.
 *	Reworked ChunkedArrayTextStore::InsertAfter_ () to handle exceotions - SPR 0292.
 *	Tweekd ChunkedArrayTextStore::AdjustMarkersForInsertAfter1 () for case at
 *	end of buffer, and did explcit-stack optimization on CollectAllMarkersInRangeIntoAndBelow () - SPR 0296.
 *	Re-enabled code at start of DTOR to delete hackmarkers - else we get
 *	assert about there not all being deleted. I no longer see why this is
 *	necessar,y but have defered the bug and left i the work around - See SPR 0300.
 *
 *	Revision 1.17  1995/05/26  06:42:40  lewis
 *	HUGE changes to the marker code implemtation. MUCH faster now.
 *	Mostly left new and old implemntations around in this version for
 *	debugging. Cleanup in a later pass. Also, tried adding more
 *	agressive asserts about getting rid of useless hack markers.
 *	They still fail sometimes, so disabled for now.
 *	Get those working in my next release.
 *
 *	Revision 1.16  1995/05/25  09:51:35  lewis
 *	A large number of experimental changes to speedup marker code. Use
 *	explict stack instead of recusion, and sterls depth hack to reduce the
 *	depth of the tree. Speedups largely untesed - SPR 0280.
 *
 *	Revision 1.15  1995/05/24  07:23:56  lewis
 *	Many attempts at optimizing marker code. Mostly things along
 *	the lines of tail recursion optimizations. Work in-progress. Much
 *	not really usable.
 *
 *	Revision 1.14  1995/05/21  17:09:50  lewis
 *	Fix SPR 0271 - wasn't always deleting all hack markers under weird conditions
 *	on destruction of a text window.
 *
 *	Revision 1.13  1995/05/20  04:56:05  lewis
 *	Fix name Invariants.
 *	make fIsHackMarker:1; and fIsDeletedMarker:1;
 *	to save space (probably should have tested for speed hit?).
 *
 *	Revision 1.12  1995/05/18  08:11:26  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.11  1995/05/06  19:47:14  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.10  1995/05/05  19:46:55  lewis
 *	Use Led_tChar instead of char - to later facilitate UNICODE support.
 *
 *	Revision 1.9  1995/04/20  06:45:59  lewis
 *	Use block allocation for hack marker.
 *	And started revising marker update logic - then backed off (SPR#0212).
 *
 *	Revision 1.8  1995/04/18  00:14:09  lewis
 *	Got rid of old ifdefs array code plus a couple of fixes to
 *	the linklist code - particularly SetMarkerRange().
 *
 *	Revision 1.7  1995/04/16  19:28:46  lewis
 *	Added RemoveMarkers override for SPR 207.
 *	Added qUseNewLinkListForMarkerList define and lots of code changes
 *	for SPR 202. This was a substantial change. All the ifdefs are ugly
 *	however, and I will do a checkin following this with all that
 *	removed. However - this version works with BOTH ways compiling - array
 *	based or linked list. I just think linked list is better, so I'm losting
 *	the array one. If bugs come up - I'm doing this checkin with both
 *	to make it easy to revert.
 *
 *	Revision 1.6  1995/03/29  21:04:49  lewis
 *	Got rid of obsolete unused Split() (SPR#0187).
 *
 *	Revision 1.5  1995/03/17  03:01:45  lewis
 *	A few performance tweeks.
 *	Instead of using RTTI/IsA_HackMarker() approach - have a bool we test to
 *	see if markers are hack markers in OurStuff(). This made AllHackMarkers()
 *	test DRAMTICLY faster. See SPR#170.
 *	Tweek Replace() and Insert_/Delete_ for case of 0 insertion/deletion.
 *
 *	Revision 1.4  1995/03/13  03:20:03  lewis
 *	Renamed Array<T> Led_Array<T>.
 *	Similary for BlockAllocated.
 *
 *	Revision 1.3  1995/03/06  21:31:06  lewis
 *	Deleted some commented out code.
 *
 *	Revision 1.2  1995/03/02  05:46:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"ChunkedArrayTextStore.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif



#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
        #pragma warn -8008
#endif





	// Debug later why this doesn't work. Actually - I think I'm still going to do a lot more
	// on the invarients with hackmarkers, so I can more efficeintly add and remove them!
	// This should be fine as-is for the 1.0 release however - LGP950527
#if		qDebug
	//#define	qHeavyMarkerDebugging		1
#endif


class	ChunkedArrayTextStore::TextChunk {
	public:
		TextChunk ();
		TextChunk (const Led_tChar* copyFrom, size_t bytesToCopy);

	public:
		enum	{ kTextChunkSize	=	(4096-sizeof (size_t)-8)/sizeof(Led_tChar)	};	// good guess as to how big we can allocate and still have stdlib allocator
																						// not waste any space for rounding up

	/*
	 *	All indexes are relative to this TextChunk - and it is a programming error to
	 *	insert past end of buffer - or to delete too many characters.
	 */
	public:
		nonvirtual	size_t				GetLength () const throw ();
		nonvirtual	size_t				GetBytesCanAccommodate () const throw ();
		nonvirtual	void				CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ();
		nonvirtual	const Led_tChar*	PeekAfter (size_t charPos) const throw ();
		nonvirtual	void				InsertAfter (const Led_tChar* what, size_t howMany, size_t after) throw ();
		nonvirtual	void				DeleteAfter (size_t howMany, size_t after) throw ();

	private:
		size_t		fTotalTcharsUsed;
		Led_tChar	fData[kTextChunkSize];
};

//	class	ChunkedArrayTextStore::TextChunk
	inline	ChunkedArrayTextStore::TextChunk::TextChunk ():
		fTotalTcharsUsed (0)
		{
		}
	inline	ChunkedArrayTextStore::TextChunk::TextChunk (const Led_tChar* copyFrom, size_t bytesToCopy):
		fTotalTcharsUsed (bytesToCopy)
		{
			Led_Assert (bytesToCopy <= kTextChunkSize);
			Led_AssertNotNil (copyFrom);
			(void)::memcpy (fData, copyFrom, bytesToCopy * sizeof (Led_tChar));
		}
	inline	size_t	ChunkedArrayTextStore::TextChunk::GetLength () const throw ()
		{
			return (fTotalTcharsUsed);
		}
	inline	size_t	ChunkedArrayTextStore::TextChunk::GetBytesCanAccommodate () const throw ()
		{
			return (kTextChunkSize-fTotalTcharsUsed);
		}
	inline	void	ChunkedArrayTextStore::TextChunk::CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ()
		{
			Led_AssertNotNil (buffer);
			Led_Assert (from + count <= fTotalTcharsUsed);
			(void)::memcpy (buffer, &fData[from], count * sizeof (Led_tChar));
		}
	inline	const Led_tChar*	ChunkedArrayTextStore::TextChunk::PeekAfter (size_t charPos) const throw ()
		{
			Led_Assert (charPos >= 0);
			Led_Assert (charPos < fTotalTcharsUsed);
			return (&fData[charPos]);
		}
	inline	void	ChunkedArrayTextStore::TextChunk::InsertAfter (const Led_tChar* what, size_t howMany, size_t after) throw ()
		{
			Led_Assert (what != 0 or howMany == 0);
			Led_Assert (after >= 0);
			Led_Assert (after <= fTotalTcharsUsed);	// cannot insert past end (other than appending)
			Led_Assert (after + howMany <= kTextChunkSize);
			/*
			 *	If we are overwriting existing bytes- shovel them off to the right first.
			 *	Then copyin the new data.
			 */
			Led_Assert (fTotalTcharsUsed >= after);
			size_t	bytesToMove	=	fTotalTcharsUsed - after;
			if (bytesToMove != 0) {
				(void)memmove (&fData[after+howMany], &fData[after], bytesToMove * sizeof (Led_tChar));
			}
			(void)::memcpy (&fData[after], what, howMany * sizeof (Led_tChar));
			fTotalTcharsUsed += howMany;
		}
	inline	void	ChunkedArrayTextStore::TextChunk::DeleteAfter (size_t howMany, size_t after) throw ()
		{
			Led_Require (after + howMany <= fTotalTcharsUsed);
			size_t	bytesToMove	=	fTotalTcharsUsed - ( after + howMany );
			if (bytesToMove != 0) {
				(void)::memmove (&fData [after], &fData [after+howMany], bytesToMove * sizeof (Led_tChar));
			}
			fTotalTcharsUsed -= howMany;
		}













	/*
	 *	Tried a bunch of different sizes. Smaller sizes have produced no perceptable speed difference,
	 *	and the DO require more memory (more hack markers), so I pick this size. And larger and I CAN
	 *	see some slow down. -- LGP 950528
	 *
	 *	Played around with this again briefly, and 50 still appears to be about the best
	 *	value - LGP 960515.
	 */
	const	size_t	kEnufChildrenToApplyHackMarkers	=	50;




	// This is what goes in a fTextStoreHook of a Marker* when we add it...
	class	ChunkedArrayMarkerHook : public Marker::HookData {
		public:
			ChunkedArrayMarkerHook ():
				Marker::HookData (),
				fParent (NULL),
				fIsHackMarker (false),
				fIsDeletedMarker (false),
				fIsPreRemoved (false),
				fFirstSubMarker (NULL),
				fNextSubMarker (NULL),
				fStart (0),
				fLength (0),
				fOwner (NULL)
				{
				}

		public:
			LED_DECLARE_USE_BLOCK_ALLOCATION (ChunkedArrayMarkerHook);

		public:
			override	MarkerOwner*	GetOwner () const;
			override	size_t			GetStart () const;
			override	size_t			GetEnd () const;
			override	size_t			GetLength () const;
			override	void			GetStartEnd (size_t* start, size_t* end)  const;


			nonvirtual	void	AddToChildList (Marker* marker);
			nonvirtual	size_t	CountChildren () const;
			nonvirtual	bool	CountChildrenMoreThan (size_t n) const;		// return true iff at least n children


			// NB: As far as I know - ordering in this linked list doesn't matter.
			// Consider replacing it with a lhs/rhs child pointers and stricly having binary tree?
			// Maybe not cuz then we lose our contains property for subtrees
			Marker*		fFirstSubMarker;		// singly linked list of submarkers
			Marker*		fNextSubMarker;			// (next link with same parent)

			Marker*			fParent;
			bool			fIsHackMarker:1;
			bool			fIsDeletedMarker:1;	// true only for REAL markers who are deleted
			bool			fIsPreRemoved:1;
			size_t			fStart;
    		size_t			fLength;
			MarkerOwner*	fOwner;
	};









	// Subclass only to do block allocation
	class	HackMarker : public Marker {
		public:
			HackMarker ():	Marker () {}

		public:
			LED_DECLARE_USE_BLOCK_ALLOCATION (HackMarker);
	};



MarkerOwner*	ChunkedArrayMarkerHook::GetOwner () const
{
//OLD COMMENT OBSOLETE - LGP 2000-07-25 - // NOT CLEAR IF WE SHOULD ALLOW NULL RETURN????
	Led_EnsureNotNil (fOwner);	// LGP 2000-07-25 - this can never be NULL - cuz we always require a non-NULL marker owner when adding!
	return fOwner;
}

size_t	ChunkedArrayMarkerHook::GetStart () const
{
	Led_Assert (fStart < 0x8000000);		// a really big number - we don't have enough memory to trigger
											// this - only point is to test of accidental cast of negnum to
											// size_t.
	return fStart;
}

size_t	ChunkedArrayMarkerHook::GetEnd () const
{
	Led_Assert (fStart < 0x8000000);	// See GetStart
	Led_Assert (fLength < 0x8000000);	//     ''
	return fStart + fLength;
}

size_t			ChunkedArrayMarkerHook::GetLength () const
{
	Led_Assert (fLength < 0x8000000);	// See GetStart
	return fLength;
}

void			ChunkedArrayMarkerHook::GetStartEnd (size_t* start, size_t* end)  const
{
	Led_Assert (fStart < 0x8000000);	// See GetStart
	Led_Assert (fLength < 0x8000000);	//     ''
	Led_RequireNotNil (start);
	Led_RequireNotNil (end);
	*start = fStart;
	*end = fStart + fLength;
}











/*
 ********************************************************************************
 ********************** Some private inline function definitions ****************
 ********************************************************************************
 */
	static	inline	ChunkedArrayMarkerHook*	OurStuff (const Marker* marker)
		{
			Led_RequireNotNil (marker);
			Led_RequireNotNil ((ChunkedArrayMarkerHook*)marker->fTextStoreHook);
			Led_AssertMember (marker->fTextStoreHook, ChunkedArrayMarkerHook);
			Led_EnsureNotNil (marker->fTextStoreHook);
			return (ChunkedArrayMarkerHook*)marker->fTextStoreHook;
		}

	inline	void	ChunkedArrayMarkerHook::AddToChildList (Marker* marker)
		{
			Led_AssertNotNil (marker);
			OurStuff (marker)->fNextSubMarker = fFirstSubMarker;
			fFirstSubMarker = marker;
		}
	inline	size_t	ChunkedArrayMarkerHook::CountChildren () const
		{
			size_t	nChildren	=	0;
			for (Marker* curChild = fFirstSubMarker; curChild != NULL; curChild = OurStuff (curChild)->fNextSubMarker) {
				nChildren++;
			}
			return (nChildren);
		}
	inline	bool	ChunkedArrayMarkerHook::CountChildrenMoreThan (size_t n) const		// return true iff at least n children
		{
			size_t	nChildren	=	0;
			for (Marker* curChild = fFirstSubMarker; curChild != NULL; curChild = OurStuff (curChild)->fNextSubMarker) {
				nChildren++;
				if (nChildren >= n) {
					return true;
				}
			}
			return (nChildren >= n);
		}
	inline	bool	ChunkedArrayTextStore::AllHackMarkers (const Marker* m)
		{
			Led_AssertNotNil (m);
			return (OurStuff (m)->fIsHackMarker and (OurStuff (m)->fFirstSubMarker == NULL or AllSubMarkersAreHackMarkerTrees (m)));
		}
	inline	ChunkedArrayTextStore::TextChunk*	ChunkedArrayTextStore::AtomicAddChunk (size_t atArrayPos)
		{
			TextChunk*	t	=	new TextChunk ();
			Led_AssertNotNil (t);
			try {
			//	fTextChunks.InsertAt (t, atArrayPos);
				fTextChunks.insert (fTextChunks.begin () + atArrayPos, t);
			}
			catch (...) {
				delete t;
				throw;
			}
			Led_AssertNotNil (t);
			return (t);
		}





	static	inline	size_t	GetMarkerStart_ (Marker* m)
		{
			Led_RequireNotNil (m);
			Led_Ensure (OurStuff (m)->fStart == m->GetStart ());
			return OurStuff (m)->fStart;
		}
	static	inline	void	SetMarkerStart_ (Marker* m, size_t s)
		{
			OurStuff (m)->fStart = s;
		}
	static	inline	size_t	GetMarkerLength_ (Marker* m)
		{
			Led_RequireNotNil (m);
			Led_Ensure (OurStuff (m)->fLength == m->GetLength ());
			return OurStuff (m)->fLength;
		}
	static	inline	void	SetMarkerLength_ (Marker* m, size_t s)
		{
			OurStuff (m)->fLength = s;
		}
	static	inline	void	SetMarkerOwner_ (Marker* m, MarkerOwner* o)
		{
			OurStuff (m)->fOwner = o;
		}




	static	inline	bool	QUICK_Overlap (ChunkedArrayMarkerHook* h, size_t from, size_t to)
		{
			Led_RequireNotNil (h);
			/*
			 *	Avoid virtual function call on m to get its range, for speed.
			 */
			size_t	start	=	h->fStart;
			if (start > to) {
				return false;
			}
			size_t	end		=	start + h->fLength;

			if ((from <= end) and (start <= to)) {
				// Maybe overlap - handle nuanced cases of zero-sized overlaps
				size_t	overlapSize;
				if (to >= end) {
					size_t	a	=	end - from;
					size_t	b	=	end - start;
					overlapSize = min (a, b);
				}
				else {
					size_t	a	=	to - from;
					size_t	b	=	to - start;
					overlapSize = min (a, b);
				}
				if (overlapSize == 0) {
					return end == start;
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}
	static	inline	bool	QUICK_Overlap (const Marker& m, size_t from, size_t to)
		{
			bool	result	= QUICK_Overlap (OurStuff (&m), from, to);
			Led_Assert (result == TextStore::Overlap (m, from, to));
			return result;
		}
	static	inline	bool	QUICK_Contains (size_t containedStart, size_t containedEnd, size_t containerStart, size_t containerEnd)
		{
			return (containedStart >= containerStart) and (containerEnd >= containedEnd);
		}
	static	inline	bool	QUICK_Contains (const Marker& containedMarker, const Marker& containerMarker)
		{
			ChunkedArrayMarkerHook*	h	=	OurStuff (&containerMarker);
			size_t	containerStart		=	h->fStart;
			size_t	containerEnd		=	containerStart + h->fLength;

			ChunkedArrayMarkerHook*	h1	=	OurStuff (&containedMarker);
			size_t	containedStart		=	h1->fStart;
			size_t	containedEnd		=	containedStart + h1->fLength;;
			bool	result				=	QUICK_Contains (containedStart, containedEnd, containerStart, containerEnd);
			Led_Assert (result == Contains (containedMarker, containerMarker));
			return result;
		}







	struct	ChunkedArrayTextStore::CollectLookupCacheElt  {
		vector<Marker*>	fMarkers;
		size_t			fFrom;
		size_t			fTo;

		struct	COMPARE_ITEM {
			COMPARE_ITEM (size_t from, size_t to): fFrom (from), fTo (to) {}

			size_t	fFrom;
			size_t	fTo;
		};

		nonvirtual	void	Clear ()
			{
				fFrom = static_cast<size_t> (-1);
			}
		static	bool	Equal (const CollectLookupCacheElt& lhs, const COMPARE_ITEM& rhs)
			{
				return lhs.fFrom == rhs.fFrom and lhs.fTo == rhs.fTo;
			}
	};







	class	ChunkedArrayMarkerOwnerHook : public MarkerOwner::HookData {
		private:
			typedef	MarkerOwner::HookData	inherited;
		public:
			ChunkedArrayMarkerOwnerHook (MarkerOwner* mo, size_t len):
				inherited (),
				#if		qUseLRUCacheForRecentlyLookedUpMarkers
					fCache (2),
				#endif
				fRootMarker ()
				#if		qKeepChunkedArrayStatistics
					,fTotalMarkersPresent (0),
					fTotalHackMarkersPresent (0),
					fPeakTotalMarkersPresent (0),
					fPeakHackMarkersPresent (0),
					fTotalHackMarkersAlloced (0)
				#endif
				{
					Led_Assert (fRootMarker.fTextStoreHook == NULL);
					fRootMarker.fTextStoreHook = new ChunkedArrayMarkerHook ();
					SetMarkerOwner_ (&fRootMarker, mo);
					SetMarkerStart_ (&fRootMarker, 0);
					SetMarkerLength_ (&fRootMarker, len);
				}
			~ChunkedArrayMarkerOwnerHook ()
				{
					#if		qKeepChunkedArrayStatistics
						Led_Require (fTotalMarkersPresent == 0);	// ALL MARKERS MUST BE REMOVED BEFORE EDITOR DESTROYED!!!!
																	// If this assertion ever triggers, then it may well be a Led client
																	// bug and not a Led-bug per-se. Check that all your markers have been
																	// removed!
																	//
																	// See Led's FAQ#27 - http://www.sophists.com/Led/LedClassLib/ClassLibDocs/Recipes.html#27
					#endif




					//----Disabled this BUG WORKAROUND cleanup code again - now that its localized per-MarkerOwner -
					// maybe easier to debug if it ever comes up again - LGP 2000-07-26. NOTE - this code should NOT be enabled! except to
					// work around a Led ChunkedArrayTextStore bug which doesn't appear to exist anymore... LGP 2001-08-28 (3.0c1x).
					//
					//----Enabled this cleanup code again to avoid needless asserts in LEC's 5.0 product. Debug this later... Again...LGP980629
					//---- THIS CLEANUP CODE STILL BROKEN - SEEN ERROR SPORADICLY, BUT STILL NO REPRODUCIBLE CASE - Harmless anyhow - leave this as is - LGP 980406
					//---- tmp re-enable - and see if this is fixed, or perhaps debug it... - LGP 980316
					// Should not be needed anymore!!!
					// And yet it is!!! - try file natfta.txt!!! - LGP 950527 - See SPR 0300
					#if 0
						// Walk children of root marker and see if any hack markers left... Any children left at this point MUST be
						// hack markers.
						{
							// Note that FreeHackTree will remove the argument marker from the parents list, so we can just keep using
							// the first child of fRootMarker til there are none...
							for (Marker* mi = OurStuff (&fRootMarker)->fFirstSubMarker; mi != NULL; mi = OurStuff (&fRootMarker)->fFirstSubMarker) {
								Led_Assert (AllHackMarkers (mi));
								FreeHackTree (mi);
							}
						}
					#endif


					#if		qKeepChunkedArrayStatistics
						Led_Assert (fTotalHackMarkersPresent == 0);	// If the fTotalMarkersPresent==0 and fTotalHackMarkersPresent != 0, then
																	// this is a Led bug (see walk children/FreeHackTree code above for workaround)
					#endif

					Led_Assert (OurStuff (&fRootMarker)->fFirstSubMarker == NULL);	// Everything should be deleted by now


					if (fRootMarker.fTextStoreHook != NULL) {
						SetMarkerOwner_ (&fRootMarker, NULL);
					}
					delete ((ChunkedArrayMarkerHook*)fRootMarker.fTextStoreHook);
				}

	#if		qUseLRUCacheForRecentlyLookedUpMarkers
		public:
			typedef	ChunkedArrayTextStore::CollectLookupCacheElt	CollectLookupCacheElt;
			typedef	LRUCache<CollectLookupCacheElt>					CACHE_TYPE;
//			typedef	CACHE_TYPE::CacheElement						CacheElement;
		public:
			nonvirtual	void	ClearCache ()
				{
					fCache.ClearCache ();
				}
			nonvirtual	CollectLookupCacheElt*	LookupElement (const CollectLookupCacheElt::COMPARE_ITEM& item)
				{
					return fCache.LookupElement (item);
				}
			nonvirtual	CollectLookupCacheElt*	AddNew ()
				{
					return fCache.AddNew ();
				}
		private:
			CACHE_TYPE	fCache;
	#endif

		public:
			inline	bool	AllHackMarkers (const Marker* m)
				{
					Led_AssertNotNil (m);
					return (OurStuff (m)->fIsHackMarker and (OurStuff (m)->fFirstSubMarker == NULL or AllSubMarkersAreHackMarkerTrees (m)));
				}
		public:
			nonvirtual	bool	AllSubMarkersAreHackMarkerTrees (const Marker* m)
				{
					Led_AssertNotNil (m);
					Led_Assert (OurStuff (m)->fIsHackMarker);

					// Try unwinding tail recusion using explicit stack
					size_t							stackDepth	=	0;
					Led_SmallStackBuffer<Marker*>	stack (0);

				RoutineTop:
					// Do in TWO loops rather than one simply because it is much faster to check the tree
					// breadth-wise than depth-wise (no recursion/function call) so probably faster to check
					// this way (assuming we get negative answers) even though we visit each node twice (assuming
					// we end up with a yes
					#if		qNoSupportForNewForLoopScopingRules
						{
					#endif
					for (Marker* mi = OurStuff (m)->fFirstSubMarker; mi != NULL;) {
						ChunkedArrayMarkerHook*	misStuff	=	OurStuff (mi);
						Led_Assert (misStuff->fParent == m);
						if (not misStuff->fIsHackMarker) {
							return false;
						}
						mi = misStuff->fNextSubMarker;
					}
					#if		qNoSupportForNewForLoopScopingRules
						}
					#endif

					Marker* mi	= OurStuff (m)->fFirstSubMarker;
					for (; mi != NULL; mi = OurStuff (mi)->fNextSubMarker) {
						Led_Assert (OurStuff (mi)->fIsHackMarker);
						// Save args, then push new args
						stack.GrowToSize (stackDepth+1);
						stack[stackDepth] = mi;
						stackDepth++;
						m = mi;			// bind formal arg
						goto RoutineTop;
				AfterCallPoint:
						Led_Assert (stackDepth > 0);
						stackDepth--;
						mi = stack[stackDepth];
					}

					if (stackDepth != 0) {
						// pop the stack and continue in the loop
						goto AfterCallPoint;
					}
					return (true);
				}

		public:
			nonvirtual	void	FreeHackTree (Marker* m)
				{
					Led_AssertNotNil (m);
					Led_Assert (OurStuff (m)->fIsHackMarker);

					// remove self from parent first, then blindly do tree delete...
					Marker*	parent	=	OurStuff (m)->fParent;
					Led_AssertNotNil (parent);
					Marker*	prevMarker	=	NULL;
					for (Marker* mi = OurStuff (parent)->fFirstSubMarker; mi != NULL; (prevMarker = mi), (mi = OurStuff (mi)->fNextSubMarker)) {
						Led_Assert (OurStuff (mi)->fParent == parent);
						if (mi == m) {
							if (prevMarker == NULL) {
								Led_Assert (OurStuff (parent)->fFirstSubMarker == mi);
								OurStuff (parent)->fFirstSubMarker = OurStuff (mi)->fNextSubMarker;
							}
							else {
								Led_Assert (OurStuff (prevMarker)->fNextSubMarker == mi);
								OurStuff (prevMarker)->fNextSubMarker = OurStuff (mi)->fNextSubMarker;
							}
							OurStuff (mi)->fNextSubMarker = NULL;
							FreeHackTree1 (m);
							return;
						}
					}
					Led_Assert (false);	// if we get here then we weren't in our parents linked list of children !!! BAD!!!
				}

			nonvirtual	void	FreeHackTree1 (Marker* m)
				{
					#if		qDebug
						Led_Assert (OurStuff (m)->fIsHackMarker);
					#endif
					for (Marker* mi = OurStuff (m)->fFirstSubMarker; mi != NULL; ) {
						Marker*	nextMI	 = OurStuff (mi)->fNextSubMarker;
						FreeHackTree1 (mi);
						mi = nextMI;
					}
					Led_AssertNotNil (m->fTextStoreHook);
					bool	isDeletedMarker	=	OurStuff (m)->fIsDeletedMarker;
					delete ((ChunkedArrayMarkerHook*)m->fTextStoreHook);

					if (isDeletedMarker) {
						m->fTextStoreHook = NULL;								// so not deleted twice by RemoveMarkers()
					}
					else {
						#if		qDebug
							m->fTextStoreHook = (ChunkedArrayMarkerHook*)666;	// magic # so we know these are bad... Should never be referenced after this
						#endif
						delete m;
						#if		qKeepChunkedArrayStatistics
							Led_Assert (fTotalHackMarkersPresent > 0);
							fTotalHackMarkersPresent--;
						#endif
					}
				}
			
		public:
			Marker		fRootMarker;

#if		qKeepChunkedArrayStatistics
		public:
			unsigned long		fTotalMarkersPresent;		// real markers - not including hack and root
			unsigned long		fTotalHackMarkersPresent;
			unsigned long		fPeakTotalMarkersPresent;
			unsigned long		fPeakHackMarkersPresent;
			unsigned long		fTotalHackMarkersAlloced;	// total freed = fTotalHackMarkersAlloced - fTotalHackMarkersPresent
#endif
	};


	inline	ChunkedArrayMarkerOwnerHook*	GetAMOH (const MarkerOwner* mo)
		{
			Led_RequireNotNil (mo);
			Led_RequireNotNil (mo->fTextStoreHook);
			Led_RequireNotNil (dynamic_cast<ChunkedArrayMarkerOwnerHook*> (mo->fTextStoreHook));
			return dynamic_cast<ChunkedArrayMarkerOwnerHook*> (mo->fTextStoreHook);
		}
	inline	ChunkedArrayMarkerOwnerHook*	GetAMOH (ChunkedArrayMarkerHook* moh)
		{
			Led_RequireNotNil (moh);
			return GetAMOH (moh->fOwner);
		}
	inline	ChunkedArrayMarkerOwnerHook*	GetAMOH (Marker* m)
		{
			return GetAMOH (OurStuff (m));
		}









/*
 ********************************************************************************
 ******************************** ChunkedArrayTextStore *************************
 ********************************************************************************
 */


/*
 *	Preliminary Design ideas:
 *
 *		Try to keep all chunks at least 1/2 full - except the last. Do invariant to assure
 *	this. Reason is to avoid memory waste - otherwise we might end up with a 4096byte block
 *	for each character!!!!
 *
 *		On second thought - this isn't such a great idea. Might be better to have a separate
 *	coalesceall routine. Wind thru the list of chunks and if it even finds two adjacent that
 *	can be joined - then do so.
 */
ChunkedArrayTextStore::ChunkedArrayTextStore ():
	inherited (),
	fLength (0),
	fTextChunks (),
	fCachedChunkIndex (0),
	fCachedChunkIndexesOffset (0)
{
	fTextStoreHook = new ChunkedArrayMarkerOwnerHook (this, 2);
}

ChunkedArrayTextStore::~ChunkedArrayTextStore ()
{
	Led_Require (GetMarkerOwners ().size () == 1);	// Really this should properly be checked in the TextStore::DTOR - and it is.
													// But if this test fails, other tests within THIS DTOR will likely also fail. And
													// those can be confusing. This diagnostic should clearly indicate to users that they've
													// forgotten to remove some MarkerOwners - like Views or MarkerCovers, or ParagraphDatabases,
													// etc.
													//
													// Note also - that since all markers must be removed for a given MarkerOwner before IT can be removed
													// this constraint also implies we contain no markers!


	delete fTextStoreHook;
	fTextStoreHook = NULL;

	// NB: We don't consider it an error to not remove all the text from the text editor.
	for (size_t i = 0; i < fTextChunks.size (); i++) {
		delete fTextChunks[i];
	}
}

/*
@METHOD:		ChunkedArrayTextStore::ConstructNewTextStore
@DESCRIPTION:	<p>See @'TextStore::ConstructNewTextStore' ().</p>
*/
TextStore*	ChunkedArrayTextStore::ConstructNewTextStore () const
{
	return new ChunkedArrayTextStore ();
}
		
void	ChunkedArrayTextStore::AddMarkerOwner (MarkerOwner* owner)
{
	Led_RequireNotNil (owner);
	Led_Require (owner->fTextStoreHook == NULL);
	inherited::AddMarkerOwner (owner);
	try {
		owner->fTextStoreHook = new ChunkedArrayMarkerOwnerHook (owner, GetLength ()+2);		// include ALL text, and ALL other markers - one more than
																								// the text imager does - not sure if any of that needed
																								// anymore??? LGP 941006 -yes probably is cuz we do >1 
																								// length there - not so we get notified - that is
																								// already handled -but so we get autogrowing...
	}
	catch (...) {
		inherited::RemoveMarkerOwner (owner);
		throw;
	}
}

void	ChunkedArrayTextStore::RemoveMarkerOwner (MarkerOwner* owner)
{
	Led_RequireNotNil (owner);
	Led_RequireNotNil (owner->fTextStoreHook);
	Led_RequireNotNil (dynamic_cast<ChunkedArrayMarkerOwnerHook*> (owner->fTextStoreHook));
	#if		qKeepChunkedArrayStatistics
		{
			ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> (owner->fTextStoreHook);
			Led_Require (camoh->fTotalMarkersPresent == 0);	// ALL MARKERS MUST BE REMOVED BEFORE EDITOR DESTROYED!!!!
															// If this assertion ever triggers, then it may well be a Led client
															// bug and not a Led-bug per-se. Check that all your markers have been
															// removed!
															//
															// Look just below at 'markersWhichShouldHaveBeenDeleted' to see just what markers are left.
															//
															// See Led's FAQ#27 - http://www.sophists.com/Led/LedClassLib/ClassLibDocs/Recipes.html#27
			#if		qDebug
				if (camoh->fTotalMarkersPresent != 0) {
					vector<Marker*>		markersWhichShouldHaveBeenDeleted;
					VectorMarkerSink	tmp (&markersWhichShouldHaveBeenDeleted);
					CollectAllMarkersInRangeInto (0, GetEnd () + 2, owner, tmp);
					int breakHere = 1;	// Look here at the 'markersWhichShouldHaveBeenDeleted' - and see which ones they are!!!
				}
			#endif
		}
	#endif
	delete owner->fTextStoreHook;
	owner->fTextStoreHook = NULL;
	inherited::RemoveMarkerOwner (owner);
}

void	ChunkedArrayTextStore::CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ()
{
	// Note that it IS NOT an error to call CopyOut for multibyte characters and split them. This is one of the few
	// API routines where that is so...
	Led_RequireNotNil (buffer);
	Led_Require (from >= 0);
	Led_Require (from + count <= GetEnd ());	// Be sure all Led_tChars requested fall in range

	ChunkAndOffset	chunkIdx	=	FindChunkIndex (from);
	for (size_t bytesToGo = count; bytesToGo != 0; ) {
		Led_Assert (chunkIdx.fChunk >= 0);
		Led_Assert (chunkIdx.fChunk < fTextChunks.size ());	// can call copyOut where this isn't true - but
															// better not be copying any bytes then!!!
		TextChunk*	t	=	fTextChunks[chunkIdx.fChunk];
		Led_AssertNotNil (t);
		size_t	copyFromThisGuy	=	Led_Min (bytesToGo, t->GetLength ()-(chunkIdx.fOffset));
		t->CopyOut (chunkIdx.fOffset, copyFromThisGuy, &buffer[count-bytesToGo]);

		// For next time through the loop
		chunkIdx.fOffset = 0;
		chunkIdx.fChunk++;
		bytesToGo -= copyFromThisGuy;
	}
}

void	ChunkedArrayTextStore::ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount)
{
#if		qUseLRUCacheForRecentlyLookedUpMarkers
	for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
		GetAMOH (*i)->ClearCache ();
	}
#endif
	Led_Assert (from <= to);
#if		qMultiByteCharacters
	Led_Assert (Led_IsValidMultiByteString (withWhat, withWhatCount));

	Assert_CharPosDoesNotSplitCharacter (from);
	Assert_CharPosDoesNotSplitCharacter (to);
#endif

	if (from != to or withWhatCount != 0) {
// THIS ISN't QUITE RIGHT - A GOOD APPROX HOWEVER...
// cuz we don't update markers properly yet... Close - but not quite, a replace
// is treated as a delete/insert - which isn't quite what we want...
		/*
		 *	Though the implication for updating markers is slightly different, for updating just
		 *	the text, we can treat this as a delete, followed by an insert.
		 */

		/*
		 *	We could either do insert first or delete first.
		 *
		 *		To assure the proper semantics for replace inside of markers however,
		 *	we must at least update the markers for INSERTION before updating them
		 *	for deletion. So this argues to do insert then delete.
		 *
		 *		But - only paying attention to the speed of inserting the text,
		 *	it is probably (maybe) faster to delete first and then insert.
		 *	(actually if the delete causes a coalesce this may not be true).
		 *
		 *		We could have both, by splitting the marker update code out from
		 *	the text insert/delete routines. But right now I think the simplest
		 *	(and most correct) thing todo is just do the insert first.
		 *
		 *	<<
		 *		WHOOPS! This rationale is WRONG - even this way, we would not get
		 *		stuff inserted. Perhaps we really shouldn't. Unclear. See SPR#0212.
		 *		For now, leave things as they were!!!
		 *	>>
		 */
		if (from != to) {
			DeleteAfter_ (to-from, from);
		}
		if (withWhatCount != 0) {
			InsertAfter_ (withWhat, withWhatCount, from);
		}
	}
}

void	ChunkedArrayTextStore::InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after)
{
	Led_Assert (howMany > 0);
	Led_Assert (what != NULL);

	Led_Assert (howMany > 0);
	Led_Assert (what != NULL);

	ChunkAndOffset	chunkIdx	=	FindChunkIndex (after);

	/*
	 *	Make sure we have a good chunk to insert into - at least to begin with. This simplfies
	 *	the code below which just tries to insert into the current chunk pointed to, and then
	 *	inserts extra chunks as needed. This just removes a special case down there.
	 */
	if (chunkIdx.fChunk >= fTextChunks.size ()) {
		if (fTextChunks.size () == 0) {
			(void)AtomicAddChunk (0);
		}
		else {
			// If we are pointing past the end of a chunk - maybe we can just
			// go back and insert inside the previous chunk. If not - THEN
			// append a new one...
			Led_Assert (chunkIdx.fChunk >= 1);
			TextChunk*	preChunk	=	fTextChunks[chunkIdx.fChunk-1];
			if (preChunk->GetBytesCanAccommodate () == 0) {
				(void)AtomicAddChunk (fTextChunks.size ());
			}
			else {
				chunkIdx.fChunk --;
				chunkIdx.fOffset = preChunk->GetLength ();
			}
		}
	}
	TextChunk*	t	=	fTextChunks[chunkIdx.fChunk];
	Led_AssertNotNil (t);


	/*
	 *	Now add text and update bytesXFered as we go. On exceptions, we update fLength with ONLY
	 *	AS MUCH AS WE ADDED. Similarly, we update the extents by only as much as we added.
	 */
	size_t		bytesXfered	=	0;
	try {
		if (t->GetBytesCanAccommodate () >= howMany) {
			/*
			 *	We're happy - we can just insert into this chunk.
			 */
			t->InsertAfter (what, howMany, chunkIdx.fOffset);
			bytesXfered += howMany;
		}
		else {
			/*
			 *	Now things are MUCH more compicated. The stuff at the end of this
			 *	chunk must be pushed forward into the following chunk - if
			 *	it fits - and otherwise - we must build a new - following -
			 *	chunk to accomodate it.
			 *
			 *	Then - once we are truely appending to this chunk - we are
			 *	all set.
			 */
			{
				if (chunkIdx.fChunk == fTextChunks.size ()-1) {
					(void)AtomicAddChunk (chunkIdx.fChunk+1);
				}
				TextChunk*	followingChunk	=	fTextChunks[chunkIdx.fChunk+1];
				Led_AssertNotNil (followingChunk);
				size_t				splitPoint		=	chunkIdx.fOffset;
				size_t				bytesToShift	=	t->GetLength ()-(splitPoint);
				const Led_tChar*	peekAfter		=	(bytesToShift==0)? NULL: t->PeekAfter (splitPoint);
				if (followingChunk->GetBytesCanAccommodate () >= bytesToShift) {
					followingChunk->InsertAfter (peekAfter, bytesToShift, 0);	// MUST PREPEND these chars
				}
				else {
					// complexities within the complexities! If there isn't room in the following chunk
					// then we do something to make room. A simple approach is to just cons up a new
					// chunk and stick it between. This may be less than ideal - but it will do for now...
					// LGP 941022
					followingChunk = AtomicAddChunk (chunkIdx.fChunk+1);		// insert AFTER current item (before old following)...
					followingChunk->InsertAfter (peekAfter, bytesToShift, 0);
				}
				t->DeleteAfter (bytesToShift, splitPoint);						// shoundn't shuffle anything - just update length count
			}
	
			/*
			 *	Ah - finally. Now that we've dispensed with the preliminaries - we can get on with the
			 *	crux of our work. This is to append text to this chunk. And then keep looping. And
			 *	as we loop - prepend to the following chunk ONLY IF IT COMPLETES THE TRANSFER,
			 *	and otherwise - keep building new chunks, and inserting them.
			 */
			size_t	bytesToGo		=	howMany;
			size_t	copyFromThisGuy	=	Led_Min (bytesToGo, t->GetBytesCanAccommodate ());
			t->InsertAfter (&what[bytesXfered], copyFromThisGuy, chunkIdx.fOffset);
	
			bytesToGo -= copyFromThisGuy;
			bytesXfered += copyFromThisGuy;
	
			for (; bytesToGo != 0; ) {
				chunkIdx.fChunk ++;
				if (chunkIdx.fChunk <= fTextChunks.size ()-1 and
					fTextChunks[chunkIdx.fChunk]->GetBytesCanAccommodate () >= bytesToGo
					) {
					// Yippie - we're done. Just prepend here, and lets go...
					t =	fTextChunks[chunkIdx.fChunk];
					t->InsertAfter (&what[bytesXfered], bytesToGo, 0);
					bytesXfered += bytesToGo;
					bytesToGo -= bytesToGo;
					Led_Assert (bytesToGo == 0);
					Led_Assert (bytesXfered == howMany);
				}
				else {
					// Since we've filled the previous - and cannot fit in the following,
					// we'll have to create another...
					t = AtomicAddChunk (chunkIdx.fChunk);
					Led_AssertNotNil (t);
					copyFromThisGuy	=	Led_Min (bytesToGo, t->GetBytesCanAccommodate ());
					t->InsertAfter (&what[bytesXfered], copyFromThisGuy, 0);
					bytesToGo -= copyFromThisGuy;
					bytesXfered += copyFromThisGuy;
				}
			}
		}
	}
	catch (...) {
		fLength += bytesXfered;
		AdjustMarkersForInsertAfter (after, bytesXfered);
		throw;
	}

	Led_Assert (howMany == bytesXfered);
	fLength += bytesXfered;
	AdjustMarkersForInsertAfter (after, bytesXfered);
}

void	ChunkedArrayTextStore::DeleteAfter_ (size_t howMany, size_t after)
{
	Led_Assert (after >= 0);
	Led_Assert ((after) + howMany <= GetLength ());
	Led_Assert (howMany > 0);

	ChunkAndOffset	chunkIdx	=	FindChunkIndex (after);
	for (size_t bytesToGo = howMany; bytesToGo != 0; ) {

		TextChunk*	t	=	fTextChunks[chunkIdx.fChunk];
		Led_AssertNotNil (t);

		// Now try to insert what we can into this section...
		size_t	deleteFromThisGuy	=	Led_Min (bytesToGo, t->GetLength ()-(chunkIdx.fOffset));
		Led_Assert (deleteFromThisGuy != 0);
		t->DeleteAfter (deleteFromThisGuy, chunkIdx.fOffset);
		bytesToGo -= deleteFromThisGuy;
		fLength -= deleteFromThisGuy;

		// For next time through the loop
		chunkIdx.fOffset = 0;
		chunkIdx.fChunk += 1;

		// MUST DO SOMETHING TO COALECE BLOCKS
		// Not a solution - but at LEAST we can eliminate ZERO LENGTH BLOCKS!!!
		// SPR#0012

		if (t->GetLength () == 0) {
			chunkIdx.fChunk -= 1;	// so we refer to same block after removeat
			fTextChunks.erase (fTextChunks.begin () + chunkIdx.fChunk);
			delete t;
		}
	}
	AdjustMarkersForDeleteAfter (after, howMany);
}

void	ChunkedArrayTextStore::AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner)
{
	Led_RequireNotNil (marker);
	Led_RequireNotNil (owner);
	#if		!qVirtualBaseMixinCallDuringCTORBug
	Led_Require (owner->PeekAtTextStore () == this);
	#endif
	Led_Require (owner == this or IndexOf (GetMarkerOwners (), owner) != kBadIndex);	// new Led 2.3 requirement - not strictly required internally yet - but it will be - LGP 980416
	Led_Require (marker->fTextStoreHook == NULL);
	Led_Require (lhs < 0x80000000);				// not real test, just sanity check
	Led_Require (length < 0x80000000);			// not real test, just sanity check
	Led_Require (lhs + length <= fLength + 1);	// perhaps shouldn't require, but current implementation DOES require this
	Invariant ();

	ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> (owner->fTextStoreHook);
	Led_AssertNotNil (camoh);

#if		qUseLRUCacheForRecentlyLookedUpMarkers
	camoh->ClearCache ();
#endif

	marker->fTextStoreHook = new ChunkedArrayMarkerHook ();

	//DONT call - cuz asserts owner NON-NULL	Led_Assert (marker->GetOwner () == NULL);
	SetMarkerOwner_ (marker, owner);

	SetMarkerStart_ (marker, lhs);
	SetMarkerLength_ (marker, length);

	AddMarker1 (marker, &camoh->fRootMarker, true);

#if		qKeepChunkedArrayStatistics
	camoh->fTotalMarkersPresent++;
	camoh->fPeakTotalMarkersPresent = Led_Max (camoh->fPeakTotalMarkersPresent, camoh->fTotalMarkersPresent);
#endif
	Invariant ();
}

#ifndef	qKeepTrackOfChildCountAndAvoidSomePossiblyAdds
	#define	qKeepTrackOfChildCountAndAvoidSomePossiblyAdds	1
#endif

void	ChunkedArrayTextStore::AddMarker1 (Marker* marker, Marker* insideMarker, bool canAddHackMarkers)
{
	Led_AssertNotNil (marker);
	Led_AssertNotNil (insideMarker);
	Led_Assert (marker != insideMarker);
	Led_AssertNotNil (marker->GetOwner ());
	Led_AssertNotNil (insideMarker->GetOwner ());
	Led_Assert (QUICK_Contains (*marker, *insideMarker));
	Led_Assert (OurStuff (marker)->fParent == NULL);
	Led_Assert (OurStuff (marker)->fNextSubMarker == NULL);	// should be properly removed from sibling list - OK if we keep SUBMARKER list!

	// NB: These are used throughout the loop, but are invariant throughout this function call
	size_t	markerStart		=	GetMarkerStart_ (marker);
	size_t	markerEnd		=	markerStart + GetMarkerLength_ (marker);

	/*
	 *	Somewhat tricky.
	 *
	 *		The basic idea here is to build a tree of submarkers corresponding to
	 *	the already existent Contains() relationship. The KEY FEATURE of the
	 *	Contains() relationship - is that it remains INVARIANT under text insertions/
	 *	deletions (and the marker updates they cause). This means the tree structure
	 *	we build - remains valid with respect to that relationship. So - we can use
	 *	the tree structure to find all items in a particular range without scanning
	 *	all markers - just apx Log n of them (assuming we can keep the tree balanced,
	 *	and all notes at most k-ary where k is constant).
	 */
	Marker*	specificInsideMarker	=	insideMarker;
Again:
	#if		qKeepTrackOfChildCountAndAvoidSomePossiblyAdds
		size_t	specificInsideMarkerChildCount	=	0;	// use in PossiblyAddHackMarkers optimization below
	#endif
	Marker*	prevMarker	=	NULL;
	for (Marker* curChild = OurStuff (specificInsideMarker)->fFirstSubMarker; curChild != NULL;
			(prevMarker = curChild), (curChild = OurStuff (curChild)->fNextSubMarker)) {
		Led_Assert (marker != curChild);

		#if		qKeepTrackOfChildCountAndAvoidSomePossiblyAdds
			specificInsideMarkerChildCount++;
		#endif

		/*
		 *	Important optimization. Check if the two markers are equal, and if so, then insert right here,
		 *	replacing the marker (by position) and pushing him down to be one of our new children.
		 *
		 *	The reason this is important is if we have a very tall tree with lots of markers all at the same
		 *	position (e.g. 2500 zero length sentence extents in LVEJ) then inserting a new one could
		 *	involve walking a very long linked list all the way down to the leaf of the tree. Not wrong,
		 *	but not worth the long walk!
		 *
		 *	Note - no good reason to not do this for OurStuff (marker)->fFirstSubMarker == NULL - except more
		 *	complex case - ignore for now - LGP 950525
		 */
		size_t	curChildStart	=	GetMarkerStart_ (curChild);
		size_t	curChildEnd		=	curChildStart + GetMarkerLength_ (curChild);
		if (markerStart == curChildStart and markerEnd == curChildEnd
			and OurStuff (marker)->fFirstSubMarker == NULL
			) {
			Led_Assert (QUICK_Contains (*marker, *curChild));	// we could have gone deeper in contains, but why bother?

			/*
			 *	Set 'marker' up to point all the places curChild used to
			 */
			Led_Assert (OurStuff (marker)->fParent == NULL);
			OurStuff (marker)->fParent = OurStuff (curChild)->fParent;
		
			Led_Assert (OurStuff (marker)->fFirstSubMarker == NULL);
			OurStuff (marker)->fFirstSubMarker = OurStuff (curChild)->fFirstSubMarker;
		
			Led_Assert (OurStuff (marker)->fNextSubMarker == NULL);
			OurStuff (marker)->fNextSubMarker = OurStuff (curChild)->fNextSubMarker;
		
			// Make those who used to point to 'curChild' now point to our new marker
			if (prevMarker == NULL) {
				Led_Assert (OurStuff (marker)->fParent == specificInsideMarker);
				Led_Assert (OurStuff (specificInsideMarker)->fFirstSubMarker == curChild);
				OurStuff (specificInsideMarker)->fFirstSubMarker = marker;
			}
			else {
				Led_Assert (OurStuff (prevMarker)->fNextSubMarker == curChild);
				OurStuff (prevMarker)->fNextSubMarker = marker;
			}
		
			// Now patch 'curChild' to now REALLY become OUR one and only child
			OurStuff (curChild)->fNextSubMarker = OurStuff (marker)->fFirstSubMarker;
			OurStuff (marker)->fFirstSubMarker = curChild;
			OurStuff (curChild)->fFirstSubMarker = NULL;
			OurStuff (curChild)->fParent = marker;
		
			// Now walk all our children and adjust thier parent pointer - since they used to point
			// to 'curChild'
			for (Marker* mm = OurStuff (marker)->fFirstSubMarker; mm != NULL; mm = OurStuff (mm)->fNextSubMarker) {
				Led_Assert (OurStuff (mm)->fParent == curChild or (mm == curChild));
				OurStuff (mm)->fParent = marker;
			}

			goto	Done;
		}

		Led_Assert (QUICK_Contains (*marker, *curChild) == QUICK_Contains (markerStart, markerEnd, curChildStart, curChildEnd));
		if (QUICK_Contains (markerStart, markerEnd, curChildStart, curChildEnd)) {
			specificInsideMarker = curChild;
			goto Again;
		}
	}

	#if		qKeepTrackOfChildCountAndAvoidSomePossiblyAdds
		if (specificInsideMarkerChildCount < kEnufChildrenToApplyHackMarkers) {
			canAddHackMarkers = false;
		}
	#endif

	/*
	 *	If we got here - then the marker we are adding was not strictly contained
	 *	inside any of our existing submarkers. We must just add it to our list - for
	 *	now - maybe later - adding hack markers to help create depth in the tree.
	 */
	OurStuff (specificInsideMarker)->AddToChildList (marker);
	Led_Assert (OurStuff (marker)->fParent == NULL);
	OurStuff (marker)->fParent = specificInsideMarker;

Done:
	if (canAddHackMarkers) {
		PossiblyAddHackMarkers (specificInsideMarker);
	}
}

void	ChunkedArrayTextStore::PossiblyAddHackMarkers (Marker* insideMarker)
{
	/*
	 *	No point in introducing hack markers unless we have any chance of reducing the breadth, and no chance of
	 *	that if we don't have an inside marker at least two wide!!!
	 */
	if (GetMarkerLength_ (insideMarker) >= 2 and OurStuff (insideMarker)->CountChildrenMoreThan (kEnufChildrenToApplyHackMarkers)) {
		size_t	insideMarkerStart	=	GetMarkerStart_ (insideMarker);
		size_t	insideMarkerEnd		=	insideMarkerStart + GetMarkerLength_ (insideMarker);
		size_t	insideMarkerLength	=	insideMarkerEnd-insideMarkerStart;

		/*
		 *	 Add two hack markers splitting the current cell.
		 */
		Marker*	lhs	=	NULL;
		Marker*	rhs	=	NULL;

		try {
			lhs	=	AddHackMarkerHelper_ (insideMarker, insideMarkerStart, insideMarkerLength/2);
			size_t	rhsStart	=	insideMarkerStart + insideMarkerLength/2;
			rhs	=	AddHackMarkerHelper_ (insideMarker, rhsStart, insideMarkerEnd - rhsStart);
	
			// Now, walk through the list of markers, and see if any of the existing items can be move
			// into the hack-markers...
			Led_Assert (OurStuff (insideMarker)->CountChildren () > 2);
			Marker*	prevMarker	=	NULL;
			for (Marker* curMarker = OurStuff (insideMarker)->fFirstSubMarker; curMarker != NULL;) {
				Marker*	nextMarker	=	OurStuff (curMarker)->fNextSubMarker;
				if (curMarker != lhs and curMarker != rhs) {	// skip hack markers
					if (QUICK_Contains (*curMarker, *lhs)) {
						Led_AssertNotNil (prevMarker);		// cuz two hack markers come first
						// Now remove old, and re-call add marker
						OurStuff (prevMarker)->fNextSubMarker = OurStuff (curMarker)->fNextSubMarker;
						OurStuff (curMarker)->fNextSubMarker = NULL;
						Led_Assert (OurStuff (curMarker)->fParent == insideMarker);
						OurStuff (curMarker)->fParent = NULL;
						AddMarker1 (curMarker, lhs, false);
						// don't reset prev, but make cur now be next
						curMarker = nextMarker;
						continue;
		
					}
					else if (QUICK_Contains (*curMarker, *rhs)) {
						Led_AssertNotNil (prevMarker);		// cuz two hack markers come first
						// Now remove old, and re-call add marker
						OurStuff (prevMarker)->fNextSubMarker = OurStuff (curMarker)->fNextSubMarker;
						OurStuff (curMarker)->fNextSubMarker = NULL;
						Led_Assert (OurStuff (curMarker)->fParent == insideMarker);
						OurStuff (curMarker)->fParent = NULL;
						AddMarker1 (curMarker, rhs, false);
						// don't reset prev, but make cur now be next
						curMarker = nextMarker;
						continue;
					}
				}
				prevMarker = curMarker;
				curMarker = nextMarker;
			}
			LoseIfUselessHackMarker (lhs);
			LoseIfUselessHackMarker (rhs);
		}
		catch (...) {
// MUST BE MORE CAREFUL HOW WE DELETE HACK MARKERS!!!
// THIS CODE IS CURRENTLY UNSAFE
Led_Assert (false);	// so when debugging at least we get a hint as to what went wrong... LGP 960313
	//		delete lhs;
	//		delete rhs;
	
			// NOTE: WE DON'T RE-THROW here!!!
			// Simply eat the exception. No big deal if no memory to add a hack-marker.
		}
	}
}

void	ChunkedArrayTextStore::RemoveMarkers (Marker*const markerArray[], size_t markerCount)
{
#if		qUseLRUCacheForRecentlyLookedUpMarkers
	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	// could only do this for affected markers, but we want to avoid redundant clearcache calls - and this is simpler...
	// Probably no real performance harm...
	for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
		GetAMOH (*i)->ClearCache ();
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif
#endif
	Led_Assert (markerCount == 0 or markerArray != NULL);

	/*
	 *	This is a fairly primitive implemenation, and I can easily imagine doing much better.
	 *	But this is so simple, and runs at least 20x faster than the old code (for 500K delete
	 *	all text test case). It seems fast enough for now - LGP 950416.
	 */
	for (size_t i = 0; i < markerCount; i++) {
		Led_Assert (not OurStuff (markerArray [i])->fIsHackMarker);
		OurStuff (markerArray [i])->fIsHackMarker = true;
		OurStuff (markerArray [i])->fIsDeletedMarker = true;
		#if		qKeepChunkedArrayStatistics
			Led_Assert (GetAMOH (markerArray [i])->fTotalMarkersPresent >= 1);
			GetAMOH (markerArray [i])->fTotalMarkersPresent -= 1;
		#endif
	}

	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	for (size_t i = 0; i < markerCount; i++) {
		Marker*	marker	=	markerArray [i];
		if (marker->fTextStoreHook != NULL) {	// if its NULL - that means its already been destroyed as a
												// hack marker in a previous lap through the loop
			Led_Assert (OurStuff (markerArray [i])->fIsHackMarker);
			RemoveMarker1 (marker);
			SetMarkerOwner_ (marker, NULL);
			Led_AssertNotNil (marker->fTextStoreHook);
			delete ((ChunkedArrayMarkerHook*)marker->fTextStoreHook);
			marker->fTextStoreHook = NULL;	// set to NULL so if we encounter it again, we won't try to delete it
		}
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif
}

void	ChunkedArrayTextStore::PreRemoveMarker (Marker* marker)
{
	Led_RequireNotNil (marker);
	Led_Require (not OurStuff (marker)->fIsPreRemoved);
	OurStuff (marker)->fIsPreRemoved = true;
}

void	ChunkedArrayTextStore::RemoveMarker1 (Marker* marker)
{
	Led_AssertNotNil (marker);
	Led_AssertNotNil (OurStuff (marker)->fParent);

	/*
	 *	In order to remove an element from a tree, we must simply find out parent, and our
	 *	leftmost sibling, in order to remove ourselves.
	 *
	 *	However, the other big thing todo is that each of our children must get redistributed
	 *	through the tree.
	 */
	Marker*	parent			=	OurStuff (marker)->fParent;
	Marker* ourLeftSibling	=	OurStuff (parent)->fFirstSubMarker;
	if (ourLeftSibling == marker) {
		ourLeftSibling = NULL;
	}
	else {
		Led_AssertNotNil (ourLeftSibling);
		for (; OurStuff (ourLeftSibling)->fNextSubMarker != marker ; ourLeftSibling = OurStuff (ourLeftSibling)->fNextSubMarker) {
			Led_AssertNotNil (ourLeftSibling);	// cuz we must be in our parents list someplace before the end!
		}
	}
	Led_Assert (ourLeftSibling == NULL or OurStuff (ourLeftSibling)->fNextSubMarker == marker);

	/*
	 *	Good. At this point we have our parent and our previous marker, so we can remove ourselves
	 *	easily. But first some hair.
	 */
	if (ourLeftSibling == NULL) {
		Led_Assert (OurStuff (parent)->fFirstSubMarker == marker);
		OurStuff (parent)->fFirstSubMarker = OurStuff (marker)->fNextSubMarker;
	}
	else {
		Led_Assert (OurStuff (ourLeftSibling)->fNextSubMarker == marker);
		OurStuff (ourLeftSibling)->fNextSubMarker = OurStuff (marker)->fNextSubMarker;
	}
	OurStuff (marker)->fNextSubMarker = NULL;
	Led_Assert (OurStuff (marker)->fParent == parent);
	OurStuff (marker)->fParent = NULL;

	/*
	 *	Now - if that marker had any submarkers - those must all be moved up to the
	 *	parent. This should probably be done in such a way as to redistribute them - 
	 *	but well do it the quick way for now...
	 */
	size_t	moveCount	=	0;
	for (Marker* markerToMove = OurStuff (marker)->fFirstSubMarker; markerToMove != NULL; ) {
		// first find who will be next marker in linked list since the below add will
		// blow our next ptr away
		Marker*	nextMarkerToMove	=	OurStuff (markerToMove)->fNextSubMarker;
		OurStuff (markerToMove)->fNextSubMarker = NULL;
		OurStuff (markerToMove)->fParent = NULL;
		AddMarker1 (markerToMove, parent, false);
		markerToMove = nextMarkerToMove;
		moveCount++;
	}
	if (moveCount > 1) {
		PossiblyAddHackMarkers (parent);
	}

	/*
	 *	Now check and see if the removeall of from parent has created any empty hack trees.
	 *
	 *	<NOTE - THIS CAN PROBABLY BE DONE BETTER - WE CAN CHECK BASED ON IF WE RMEOVED/MOVED HACK MARKJERS AND AVOID THIS TEST>
	 *	LGP 950525
	 */
	for (Marker* hmi = OurStuff (parent)->fFirstSubMarker; hmi != NULL; ) {
		ChunkedArrayMarkerHook*	hmiStuff	=	OurStuff (hmi);
		Marker*					nexthmi		=	hmiStuff->fNextSubMarker;	// save this away before FreeHackTree call so we don't lose anything!
		if (OurStuff (hmi)->fIsHackMarker) {
			if (AllHackMarkers (hmi)) {
				GetAMOH (hmi)->FreeHackTree (hmi);
			}
			else {
				LoseIfUselessHackMarker (hmi);
			}
		}
		hmi = nexthmi;
	}


	// FIX FOR LONGSTANDING BUG CLEANUPING HACKMARKERS???
	{
		// really must do while loop walking UP parent tree
		for (Marker* hmi = parent; OurStuff (hmi)->fIsHackMarker; ) {
			Marker*	nextUp	=	OurStuff (hmi)->fParent;
			if (AllHackMarkers (hmi)) {
				Led_AssertNotNil (nextUp);		// cuz top marker is fRootMarker - and thats not a hack marker!
				GetAMOH (hmi)->FreeHackTree (hmi);
				hmi = nextUp;
			}
			else {
				break;
			}
		}
	}
}

Marker*	ChunkedArrayTextStore::AddHackMarkerHelper_ (Marker* insideMarker, size_t start, size_t length)
{
	HackMarker*	marker	=	new HackMarker ();

	Led_AssertNotNil (insideMarker);

	try {
		Led_Assert (marker->fTextStoreHook == NULL);
		marker->fTextStoreHook = new ChunkedArrayMarkerHook ();
		Led_Assert (OurStuff (marker)->fParent == NULL);
		OurStuff (marker)->fParent = insideMarker;
		SetMarkerOwner_ (marker, OurStuff (insideMarker)->fOwner);	// owner must be same as that of 'insideMarker' - so that all the caching/tree/stats etc kept coherent...
		SetMarkerStart_ (marker, start);
		SetMarkerLength_ (marker, length);
		OurStuff (insideMarker)->AddToChildList (marker);
	
		Led_Assert (not OurStuff (marker)->fIsHackMarker);
		OurStuff (marker)->fIsHackMarker = true;
		#if		qKeepChunkedArrayStatistics
			GetAMOH (insideMarker)->fTotalHackMarkersAlloced++;
			GetAMOH (insideMarker)->fTotalHackMarkersPresent++;
			GetAMOH (insideMarker)->fPeakHackMarkersPresent = Led_Max (GetAMOH (insideMarker)->fPeakHackMarkersPresent, GetAMOH (insideMarker)->fTotalHackMarkersPresent);
		#endif
	}
	catch (...) {
		// this is safe cuz only point we can throw is 'new ChunkedArrayMarkerHook ()', and at that point
		// nobody refers to our marker.
		delete marker;
		throw;
	}
	return (marker);
}

void	ChunkedArrayTextStore::LoseIfUselessHackMarker (Marker* potentiallyUselessHackMarker)
{
	Led_AssertNotNil (potentiallyUselessHackMarker);
	Led_Assert (OurStuff (potentiallyUselessHackMarker)->fIsHackMarker);

	Marker*	firstChild		=	OurStuff (potentiallyUselessHackMarker)->fFirstSubMarker;

	bool	truelyUseless	=	(firstChild == NULL) or (OurStuff (firstChild)->fNextSubMarker == NULL);

	/*
	 *	Any hack marker with zero or one children is serving no purpose. So remove them.
	 *
	 *	Here we first check for the harder case of one child. In that case we must
	 *	move the markers children up into our parent right where we are.
	 */
	if (firstChild != NULL and  OurStuff (firstChild)->fNextSubMarker == NULL) {
		Led_Assert (truelyUseless);
		Marker*	goodParent	=	OurStuff (potentiallyUselessHackMarker)->fParent;
		Led_Assert (OurStuff (firstChild)->fParent == potentiallyUselessHackMarker);
		OurStuff (firstChild)->fParent = goodParent;
		OurStuff (firstChild)->fNextSubMarker = OurStuff (goodParent)->fFirstSubMarker;
		OurStuff (goodParent)->fFirstSubMarker = firstChild;
		OurStuff (potentiallyUselessHackMarker)->fFirstSubMarker = NULL;		// avoid deleting subtree in FreeHackTree () below
	}

	// Now we are left with a truelyUseless marker, with no children, so just remove it from parent, and dispose of it.
	if (truelyUseless) {
		GetAMOH (potentiallyUselessHackMarker)->FreeHackTree (potentiallyUselessHackMarker);
	}
}

void	ChunkedArrayTextStore::SetMarkerRange (Marker* marker, size_t start, size_t end) throw ()
{
	Led_RequireNotNil (marker);
#if		qUseLRUCacheForRecentlyLookedUpMarkers
	GetAMOH (marker)->ClearCache ();
#endif
	/*
	 *	NB:		There is still alot of room to improve this routine's efficency, by
	 *	being more clever about how we move the marker and its children when we no
	 *	longer fit. If we are growing marker - then the children still fit, and can be
	 *	moved with the marker. If they DON'T fit, we can move then directly up to our parent,
	 *	and then move the marker. And we can inline make decisions here about whether to
	 *	add/remove hack markers, and where (maybe replace ourselves with a hack marker instead
	 *	of moving children up to parent? - yes - probably a VERY good idea!)
	 *
	 *	These optimizations can - and probably will - wait til after the 1.0 release - LGP 950527.
	 *
	 */
	Led_Assert (start >= 0);
	Led_Assert (end >= 0);
	Led_Assert (start <= end);
	Led_AssertNotNil (marker);

	size_t	len	=	end-start;

	// changing the start, or end may force a re-ordering...
	if (GetMarkerStart_ (marker) != start or GetMarkerLength_ (marker) != len) {
		/*
		 *	If this marker is still contained in its parent - no need to remove it.
		 *	Furthermore - if all its children remain in it - no need to move them up.
		 *	But the worst case is - remove marker - and re-add it.
		 */
		Marker*		parent	=	OurStuff (marker)->fParent;
		Led_AssertNotNil (parent);
		Led_Assert (QUICK_Contains (*marker, *parent));
		if (Contains (start, end, *parent)) {
			#if		qDebug
				// before we re-adjust anything. - make sure all is well...
				{
					for (Marker* mi = OurStuff (marker)->fFirstSubMarker; mi != NULL; mi = OurStuff (mi)->fNextSubMarker) {
						Led_Assert (QUICK_Contains (*mi, *marker));
					}
				}
			#endif

			SetMarkerStart_ (marker, start);
			SetMarkerLength_ (marker, len);

			/*
			 *		Now check each child - and if it is still contained in the new range - leave it - 
			 *	otherwise - remove it, and move it to the parent.
			 */
			Marker*	prevMarker	=	NULL;
			for (Marker* mi = OurStuff (marker)->fFirstSubMarker; mi != NULL;) {
				Marker*	nextMarker	=	OurStuff (mi)->fNextSubMarker;
				if (not QUICK_Contains (*mi, *marker)) {	// cuz we just patched our start/size
					if (prevMarker == NULL) {
						Led_Assert (OurStuff (marker)->fFirstSubMarker == mi);
						OurStuff (marker)->fFirstSubMarker = OurStuff (mi)->fNextSubMarker;
					}
					else {
						Led_Assert (OurStuff (prevMarker)->fNextSubMarker == mi);
						OurStuff (prevMarker)->fNextSubMarker = nextMarker;
					}
					OurStuff (mi)->fNextSubMarker = NULL;
					OurStuff (mi)->fParent = NULL;
					AddMarker1 (mi, parent, false);	// don't bother with adding hack markers - possibly might help but unlikely since we are going
													// into same list we started in (though possibly with differnt size)
				}
				else {
					prevMarker = mi;
				}
				mi = nextMarker;
			}
		}
		else {
			MarkerOwner*	owner	=	marker->GetOwner ();
			RemoveMarker (marker);
			AddMarker (marker, start, len, owner);
		}
	}
}

	struct	StackContext {
		const Marker*	saved_belowHere;
		Marker*			saved_mi;
	};
void	ChunkedArrayTextStore::CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const
{
	Led_RequireNotNil (owner);	// though it can be TextStore::kAnyMarkerOwner.
	Led_Require (from <= to);

	/*
	 *	Please pardon the complexity of this routine. It was once written simply and clearly as a recursive function.
	 *	But it was a central bottleneck to Led's speed. So the recursion was unwound, and changed to use an
	 *	explicit, inline stack -- purely for speed.
	 *
	 *	Similarly for this LRUCache code (SPR#0652).
	 */
	if (owner == kAnyMarkerOwner) {
		for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
			CollectAllMarkersInRangeInto_Helper_MO (from, to, *i, output);
		}
	}
	else {
		#if		qUseLRUCacheForRecentlyLookedUpMarkers
			ChunkedArrayMarkerOwnerHook*	camoh		=	GetAMOH (owner);
			CollectLookupCacheElt*			cacheItem	=	camoh->LookupElement (CollectLookupCacheElt::COMPARE_ITEM (from, to));
			if (cacheItem != NULL) {
				for (vector<Marker*>::const_iterator i = cacheItem->fMarkers.begin (); i != cacheItem->fMarkers.end (); ++i) {
					if (not OurStuff (*i)->fIsPreRemoved) {
						output.Append (*i);
					}
				}
				return;
			}
			if (cacheItem == NULL and (to-from) <= 3) {		// only try to cache small guys (but at least size 3 cuz of _Surrounding() calls...
				cacheItem = camoh->AddNew ();
				cacheItem->fFrom = from;
				cacheItem->fTo = to;
				cacheItem->fMarkers.clear ();
				CollectAllMarkersInRangeInto_Helper_MO (from, to, owner, output, cacheItem);
				return;
			}
		#endif
		CollectAllMarkersInRangeInto_Helper_MO (from, to, owner, output);
	}
}

void	ChunkedArrayTextStore::CollectAllMarkersInRangeInto_Helper_MO (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output
				#if		qUseLRUCacheForRecentlyLookedUpMarkers
											, CollectLookupCacheElt* fillingCache
				#endif
															) const
{
	ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> (owner->fTextStoreHook);
	Led_AssertNotNil (camoh);
	const Marker* 						belowHere	=	&camoh->fRootMarker;
	size_t								stackDepth	=	0;
	Led_SmallStackBuffer<StackContext>	stack (0);

RoutineTop:
	Led_AssertNotNil (belowHere);
	Marker*					mi	=	OurStuff (belowHere)->fFirstSubMarker;
	ChunkedArrayMarkerHook*	mio	=	mi==NULL? NULL: OurStuff (mi);	// be SURE to always keep 'mio' in sync with 'mi'
	for (; mi != NULL; (mi = mio->fNextSubMarker), (mio = mi==NULL? NULL: OurStuff (mi))) {
		Led_AssertNotNil (mi);
		Led_AssertNotNil (mio);
		Led_Assert (mio == OurStuff (mi));
		Led_Assert (QUICK_Overlap (mio, from, to) == QUICK_Overlap (*mi, from, to));
		if (QUICK_Overlap (mio, from, to)) {
			if (not mio->fIsHackMarker and not mio->fIsPreRemoved) {
				#if		qUseLRUCacheForRecentlyLookedUpMarkers
					if (fillingCache != NULL) {
						fillingCache->fMarkers.push_back (mi);
					}
				#endif
				output.Append (mi);
			}
			// Save args, then push new args
			stack.GrowToSize (stackDepth+1);
			stack[stackDepth].saved_belowHere = belowHere;
			stack[stackDepth].saved_mi = mi;
			stackDepth++;
			belowHere = mi;			// bind formal arg
			goto RoutineTop;
AfterCallPoint:
			Led_Assert (stackDepth > 0);
			stackDepth--;
			belowHere = stack[stackDepth].saved_belowHere;
			mi = stack[stackDepth].saved_mi;
			Led_AssertNotNil (mi);
			mio	=	OurStuff (mi);
		}
	}
	if (stackDepth != 0) {
		// pop the stack and continue in the loop
		goto AfterCallPoint;
	}
}

ChunkedArrayTextStore::ChunkAndOffset	ChunkedArrayTextStore::FindChunkIndex_ (size_t charPos) const
{
	Led_Assert (charPos >= 0);
	Led_Assert (charPos <= GetEnd ());
	size_t	totalChunks		=	fTextChunks.size ();
	size_t	bytePosSoFar	=	0;


	/*
	 *	A note of caution about the cahing of the indexes here. We currently never bother
	 *	invalidating the fCachedChunkIndexesOffset/fCachedChunkIndex because the only
	 *	way we can get in trouble with them is if somebody inserts stuff or deletes stuff
	 *	BEFORE our cached indexes. But all the code that does the insert/delete calls this
	 *	routine first, and only modifies stuff AFTER the ChunkAndOffset () we return - this
	 *	not affecting our cache (since we never return an answer < our cached value).
	 *
	 *	This is a somewhat fragile strategy - and care should be taken in modifying code
	 *	which might change this assumption. Of course - if there is a change that breaks this-
	 *	all that needs to be done is to call the invalidate routine.
	 */
	size_t	startAt	=	0;
	if (charPos >= fCachedChunkIndexesOffset) {
		startAt = fCachedChunkIndex;
		Led_Assert (fCachedChunkIndexesOffset >= 0);
		bytePosSoFar = fCachedChunkIndexesOffset;
	}
	for (size_t i = startAt; i < totalChunks; i++) {
		Led_AssertNotNil (fTextChunks[i]);
		size_t	curChunkSize	=	fTextChunks[i]->GetLength ();
		if (bytePosSoFar + curChunkSize > charPos) {
			Led_Assert (charPos >= bytePosSoFar + 0);
			// set new cache value
			fCachedChunkIndex = i;
			fCachedChunkIndexesOffset = bytePosSoFar + 0;
			return (ChunkAndOffset (i, charPos-bytePosSoFar));
		}
		bytePosSoFar += curChunkSize;
	}
	return (ChunkAndOffset (totalChunks, 0));		// we might be appending - so this is OK.. Refer to non-existent chunk
}

void	ChunkedArrayTextStore::AdjustMarkersForInsertAfter (size_t after, size_t howMany)
{
	for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
		ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> ((*i)->fTextStoreHook);
		Led_AssertNotNil (camoh);
		AdjustMarkersForInsertAfter1 (after, howMany, &camoh->fRootMarker);
	}
}

void	ChunkedArrayTextStore::AdjustMarkersForInsertAfter1 (size_t after, size_t howMany, Marker* startAt)
{
	Led_AssertNotNil (startAt);

	// Try unwinding tail recusion using explicit stack
	size_t							stackDepth	=	0;
	Led_SmallStackBuffer<Marker*>	stack (0);

	Marker*	mi	=	NULL;		// declared up here instead of down below to avoid MSVC 2.1 compiler bug.
								// LGP 950527

RoutineTop:
	/*
	 *	Adjust THIS marker.
	 */
	{
		size_t	start	=	GetMarkerStart_ (startAt);
		size_t	len		=	GetMarkerLength_ (startAt);
		size_t	end		=	start + len;
		if (after < start) {
			SetMarkerStart_ (startAt, start+howMany);
		}
		else if (after >= start and after < end) {
			SetMarkerLength_ (startAt, len+howMany);
		}
		else {
			Led_Assert (after >= end);	// then we don't need to adjust any of our children either!
			if (stackDepth == 0) {
				return;
			}
			else {
				goto AfterCallPoint;
			}
		}
	}

	/*
	 *	Adjust its children...
	 */
	for (mi = OurStuff (startAt)->fFirstSubMarker; mi != NULL; mi = OurStuff (mi)->fNextSubMarker) {
		// Save args, then push new args
		stack.GrowToSize (stackDepth+1);
		stack[stackDepth] = mi;
		stackDepth++;
		startAt = mi;			// bind formal arg
		goto RoutineTop;
AfterCallPoint:
		Led_Assert (stackDepth > 0);
		stackDepth--;
		mi = stack[stackDepth];
	}
	if (stackDepth != 0) {
		// pop the stack and continue in the loop
		goto AfterCallPoint;
	}
}

void	ChunkedArrayTextStore::AdjustMarkersForDeleteAfter (size_t after, size_t howMany)
{
	for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
		ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> ((*i)->fTextStoreHook);
		Led_AssertNotNil (camoh);
		AdjustMarkersForDeleteAfter1 (after, howMany, &camoh->fRootMarker);
	}
}

void	ChunkedArrayTextStore::AdjustMarkersForDeleteAfter1 (size_t after, size_t howMany, Marker* startAt)
{
	Led_AssertNotNil (startAt);


	// Try unwinding tail recusion using explicit stack
	size_t							stackDepth	=	0;
	Led_SmallStackBuffer<Marker*>	stack (0);

	Marker*	mi	=	NULL;		// declared up here instead of down below to avoid MSVC 2.1 compiler bug.
								// LGP 950527
RoutineTop:
	/*
	 *	Adjust THIS marker.
	 */
	{
		size_t	start	=	GetMarkerStart_ (startAt);
		size_t	len		=	GetMarkerLength_ (startAt);
		size_t	end		=	start + len;
		if (after < start) {
       //			size_t	newStart	=	start;
			if (howMany + after <= start) {
				Led_Assert (start >= howMany + 0);
				SetMarkerStart_ (startAt, start-howMany);
			}
			else {
				Led_Assert (howMany > (start-after));
				size_t	deleteNCharsOffFront = howMany - (start-after);
				size_t	moveFront = howMany - deleteNCharsOffFront;
				SetMarkerStart_ (startAt, start-moveFront);
				/*
				 * Note when the whole extent is deleted - we simply pin the size to zero.
				 */
				SetMarkerLength_ (startAt, (len>deleteNCharsOffFront)? (len-deleteNCharsOffFront): 0);
			}
		}
		else if (after >= start and after < end) {
			size_t	newEnd	=	end;
			if (end-after < howMany) {
				newEnd = after;
			}
			else {
				newEnd -= howMany;
			}
			Led_Assert (newEnd >= start);
			size_t	newLen	=	newEnd - start;
			SetMarkerLength_ (startAt, newLen);
		}
		else {
			Led_Assert (after >= end);	// then we don't need to adjust any of our children either!
			if (stackDepth == 0) {
				return;
			}
			else {
				goto AfterCallPoint;
			}
		}
	}

	/*
	 *	Adjust its children...
	 */
	for (mi = OurStuff (startAt)->fFirstSubMarker; mi != NULL; mi = OurStuff (mi)->fNextSubMarker) {
		// Save args, then push new args
		stack.GrowToSize (stackDepth+1);
		stack[stackDepth] = mi;
		stackDepth++;
		startAt = mi;			// bind formal arg
		goto RoutineTop;
AfterCallPoint:
		Led_Assert (stackDepth > 0);
		stackDepth--;
		mi = stack[stackDepth];
	}

	if (stackDepth != 0) {
		// pop the stack and continue in the loop
		goto AfterCallPoint;
	}
}

bool	ChunkedArrayTextStore::AllSubMarkersAreHackMarkerTrees (const Marker* m)
{
	Led_AssertNotNil (m);
	Led_Assert (OurStuff (m)->fIsHackMarker);

	// Try unwinding tail recusion using explicit stack
	size_t							stackDepth	=	0;
	Led_SmallStackBuffer<Marker*>	stack (0);

RoutineTop:
	// Do in TWO loops rather than one simply because it is much faster to check the tree
	// breadth-wise than depth-wise (no recursion/function call) so probably faster to check
	// this way (assuming we get negative answers) even though we visit each node twice (assuming
	// we end up with a yes
	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	for (Marker* mi = OurStuff (m)->fFirstSubMarker; mi != NULL;) {
		ChunkedArrayMarkerHook*	misStuff	=	OurStuff (mi);
		Led_Assert (misStuff->fParent == m);
		if (not misStuff->fIsHackMarker) {
			return false;
		}
		mi = misStuff->fNextSubMarker;
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif

	Marker* mi	= OurStuff (m)->fFirstSubMarker;
	for (; mi != NULL; mi = OurStuff (mi)->fNextSubMarker) {
		Led_Assert (OurStuff (mi)->fIsHackMarker);
		// Save args, then push new args
		stack.GrowToSize (stackDepth+1);
		stack[stackDepth] = mi;
		stackDepth++;
		m = mi;			// bind formal arg
		goto RoutineTop;
AfterCallPoint:
		Led_Assert (stackDepth > 0);
		stackDepth--;
		mi = stack[stackDepth];
	}

	if (stackDepth != 0) {
		// pop the stack and continue in the loop
		goto AfterCallPoint;
	}
	return (true);
}

#if		qDebug
void	ChunkedArrayTextStore::Invariant_ () const
{
	TextStore::Invariant_ ();
	for (vector<MarkerOwner*>::const_iterator i = GetMarkerOwners ().begin (); i != GetMarkerOwners ().end (); ++i) {
		ChunkedArrayMarkerOwnerHook*	camoh	=	dynamic_cast<ChunkedArrayMarkerOwnerHook*> ((*i)->fTextStoreHook);
		Led_AssertNotNil (camoh);
		WalkSubTreeAndCheckInvariants (&camoh->fRootMarker);
	}
}
#endif

#if		qDebug
void	ChunkedArrayTextStore::WalkSubTreeAndCheckInvariants (const Marker* m) const
{
	Led_AssertNotNil (m);
	Led_AssertNotNil (OurStuff (m));
	for (const Marker* subMarker = OurStuff (m)->fFirstSubMarker; subMarker != NULL; subMarker = OurStuff (subMarker)->fNextSubMarker) {
		Led_AssertNotNil (subMarker);
		Led_AssertNotNil (OurStuff (subMarker));
		Led_Assert (OurStuff (subMarker)->fParent == m);
		Led_Assert (QUICK_Contains (*subMarker, *m));
		#if		qHeavyMarkerDebugging
			Led_Assert (not OurStuff (subMarker)->fIsHackMarker or OurStuff (subMarker)->fFirstSubMarker != NULL);	// leaf hacks should be deleted!
		#endif
		WalkSubTreeAndCheckInvariants (subMarker);
	}
}
#endif



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
