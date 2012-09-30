/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __LedLineItServerItem_h__
#define __LedLineItServerItem_h__   1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <afxole.h>

#include    "Stroika/Frameworks/Led/Support.h"

#include    "LedLineItConfig.h"



class   LedLineItDocument;

class   LedLineItServerItem : public COleServerItem {
public:
    LedLineItServerItem (LedLineItDocument* pContainerDoc);

public:
    nonvirtual  LedLineItDocument*  GetDocument () const;

public:
    override    BOOL OnDraw (CDC* pDC, CSize& rSize);
    override    BOOL OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize);

protected:
    virtual void Serialize (CArchive& ar);   // overridden for document i/o

#ifdef _DEBUG
public:
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    DECLARE_DYNAMIC(LedLineItServerItem)
};

#endif  /*__LedLineItServerItem_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

