/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_ModuleInit_inl
#define	_ModuleInit_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {

	template	<typename T>
		inline	ModuleInit::SingletonObjActualInitializer<T>::SingletonObjActualInitializer ():
			fThe ()
			{
			}
	template	<typename T>
		inline	ModuleInit::SingletonObjActualInitializer<T>::~SingletonObjActualInitializer ()
			{
			}
	template	<typename T>
		inline	const typename T&	ModuleInit::SingletonObjActualInitializer<T>::THE () const
			{
				return fThe;
			}
	template	<typename T>
		inline	typename T&	ModuleInit::SingletonObjActualInitializer<T>::THE ()
			{
				return fThe;
			}

		



		
	template	<typename	ACTUAL_MODULE_INITIALIZER>
		inline	ModuleInit::ModuleInitializer<ACTUAL_MODULE_INITIALIZER>::ModuleInitializer ()
			{
				if (sInitCnt++ == 0) {
					// no need to store pointer, cuz its the same as &sActualModuleInitializer_Storage
					(void)new (&sActualModuleInitializer_Storage) ACTUAL_MODULE_INITIALIZER ();
				}
			}
	template	<typename	ACTUAL_MODULE_INITIALIZER>
		inline	ModuleInit::ModuleInitializer<ACTUAL_MODULE_INITIALIZER>::~ModuleInitializer ()
			{
				if (--sInitCnt == 0) {
					reinterpret_cast<ACTUAL_MODULE_INITIALIZER*> (&sActualModuleInitializer_Storage)->~ACTUAL_MODULE_INITIALIZER ();
				}
			}
	template	<typename	ACTUAL_MODULE_INITIALIZER>
		inline	typename	ACTUAL_MODULE_INITIALIZER&	ModuleInit::ModuleInitializer<ACTUAL_MODULE_INITIALIZER>::Actual ()
			{
				Assert (sInitCnt > 0);	// we've been initialized, and not yet destroyed...
				return *reinterpret_cast<ACTUAL_MODULE_INITIALIZER*> (&sActualModuleInitializer_Storage);
			}
	template	<typename ACTUAL_MODULE_INITIALIZER>
		Byte									ModuleInit::ModuleInitializer<ACTUAL_MODULE_INITIALIZER>::sActualModuleInitializer_Storage[sizeof (ACTUAL_MODULE_INITIALIZER)];	// avoid actual memory allocation call - since only one of these
	template	<typename ACTUAL_MODULE_INITIALIZER>
		unsigned	short	ModuleInit::ModuleInitializer<ACTUAL_MODULE_INITIALIZER>::sInitCnt;

	}
}


#endif	/*_ModuleInit_inl*/
