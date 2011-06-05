/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__ModuleInit_h__
#define	__ModuleInit_h__	1

#include	"../StroikaPreComp.h"

#include	"../Debug/Assertions.h"
#include	"../Support.h"



namespace	Stroika {	
	namespace	Foundation {


namespace	ModuleInit {


	/*
	 *	OVERVIEW:
	 *
	 *		This class does little but DOCUMENT a useful design pattern to avoid the nasty cross-module deadly embrace
	 *		caused by unreliable static object construction order.
	 */

	/*
	 *	USAGE EXAPMPLE:
	 *		namespace	ExampleModule {
	 *			namespace	Private {
	 *				struct	ActualModuleInit {
	 *					ActualModuleInit ();
	 *					~ActualModuleInit ();
	 *				};
	 *				extern	CriticalSection*	sCritSection_;		// example obj to init
	 *				CriticalSection&	GetCritSection_ ();
	 *			}
	 *		};
	 *		namespace	{
	 *			ModuleInit::ModuleInitializer<ExampleModule::Private::ActualModuleInit>	_MI_;	// this object constructed for the CTOR/DTOR per-module side-effects
	 *		}
	 */

	/*
	 *	In the ActualModuleInit::CTOR you initialize your module (in this case, sCritSection_). And in the 
	 *	ActualModuleInit::DTOR - you uninitialize (e.g. delete sCritSection_);
	 *
	 *	The reason this trick works reliably - is that the anonymouns namespace declaration of _MI_ - above - 
	 *	bumps/decrements reference counts in EVERY CONTAINING MODULE. The first module initialized - calls the
	 *	ActualModuleInit::CTOR - once - and not multiply. And the as the file-scope-lifetime variables are destroyed,
	 *	this count drops to zero, and the ActualModuleInit::DTOR gets called (once).
	 *
	 *	Because of this process, and so long as the details of ActualModuleInit don't go cross-module, we are assured
	 *	each module is initialized automatically, but just once, and in an order independent of the order generated by the
	 *	linker for static initializers for .obj files (not 100% sure of this - may need to rethink?).
	 *			-- LGP 2008-12-25
	 */


	// The type ACTUAL_MODULE_INITIALIZER - is the code which actually performs the real, module-specific
	// initialization.
	//
	// Initialiation of ACTUAL_MODULE_INITIALIZER is done in the constructor, and cleanup in the DTOR.
	// This class makes sure that - if you construct a ModuleInitializer<> object in every module which might
	// use this module, then the is constructed at the earliest time, and destroyed at the latest.
	template	<typename ACTUAL_MODULE_INITIALIZER>
		class	ModuleInitializer {
			public:
				ModuleInitializer ();
				~ModuleInitializer ();

			public:
				static	typename ACTUAL_MODULE_INITIALIZER&	Actual ();

			private:
				static	Byte				sActualModuleInitializer_Storage[sizeof (ACTUAL_MODULE_INITIALIZER)];	// avoid actual memory allocation call - since only one of these
				static	unsigned	short	sInitCnt;
		};



	// Only useful with ModuleInitializer - to implement singleton design pattern, but assuring constructed once per application,
	// at just the 'right' time - for access to this object.
	template	<typename T>
		class	SingletonObjActualInitializer {
			public:
				SingletonObjActualInitializer ();
				~SingletonObjActualInitializer ();
			public:
				nonvirtual const typename T& THE () const;
				nonvirtual typename T&	THE ();
			private:
				typename	T	fThe;
		};


	// Help to construct an object - and NEVER call its DTOR. Assure its
	// constructed before use. Can be helpful for objects that don't need to be destroyed, but need to
	// assure they are constructed before use.
	// Only thought out using this at file scope to wrap construciton of an object.
	template	<typename	T>
		struct	StaticSingletonObjectConstructionHelper {
			bool	fConstructed;
			Byte	fTBuf[sizeof (T)];

			operator T& ()
				{
					// for now - OK - I think - to avoid critical section - cuz these get used
					// before we've had a chance to make threads...
					if (not fConstructed) {
						new (&fTBuf) T ();
						fConstructed = true;
					}
					return * (reinterpret_cast<T*> (&fTBuf));
				};
		};



	/*
	 * See http://bugzilla/show_bug.cgi?id=439
	 *
	 * Allow use of regular constant declaration use when we have
	 * an underlying system where the constant is actually FETECHED from the argument function.
	 *
	 * Could use this in someplace like:
	 *		const ModuleInit::ConstantViaGetter<Enumeration,&Private::kTypeHiddenFilter_>	kTypeHiddenFilter;
	 *
	 *	See http://bugzilla/show_bug.cgi?id=439 for details.
	 */
	template	<typename BASETYPE, const typename BASETYPE& (*ValueGetter) ()> 
		struct ConstantViaGetter {
			inline	operator const typename BASETYPE () const
				{
					return (ValueGetter) ();
				}
			inline	const typename BASETYPE* operator-> () const
				{
					return &(ValueGetter) ();
				}
		};

}



	}
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__ModuleInit_h__*/
