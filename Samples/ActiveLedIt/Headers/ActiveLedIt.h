/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	__ActiveLedIt_h__
#define	__ActiveLedIt_h__	1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include	<afxctl.h>

#include	"ActiveLedItConfig.h"


class	ActiveLedItApp : public COleControlModule {
	private:
		typedef	COleControlModule	inherited;
	public:
		override	BOOL	InitInstance ();
		override	int		ExitInstance ();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__ActiveLedIt_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

