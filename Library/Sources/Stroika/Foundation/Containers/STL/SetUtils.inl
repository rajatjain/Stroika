
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_SetUtils_inl_
#define _Stroika_Foundation_Containers_STL_SetUtils_inl_    1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   STL {


#if 0
                template    <typename T>
                inline  set<T>  operator- (const set<T>& lhs, const set<T>& rhs)
                {
                    return Difference (lhs, rhs);
                }
                template    <typename T>
                set<T>  operator+ (const set<T>& lhs, const set<T>& rhs)
                {
                    set<T>  result  =   lhs;
                    for (typename set<T>::const_iterator i = rhs.begin (); i != rhs.end (); ++i) {
                        if (result.find (*i) == result.end ()) {
                            result.insert (*i);
                        }
                    }
                    return result;
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>& operator+= (set<T>& lhs, const FROMCONTAINER& rhs)
                {
                    for (typename FROMCONTAINER::const_iterator i = rhs.begin (); i != rhs.end (); ++i) {
                        if (lhs.find (*i) == lhs.end ()) {
                            lhs.insert (*i);
                        }
                    }
                    return lhs;
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>& operator-= (set<T>& lhs, const FROMCONTAINER& rhs)
                {
                    for (typename FROMCONTAINER::const_iterator i = rhs.begin (); i != rhs.end (); ++i) {
                        typename set<T>::iterator fi    =   lhs.find (*i);
                        if (fi != lhs.end ()) {
                            lhs.erase (fi);
                        }
                    }
                    return lhs;
                }
#endif


                template    <typename T, typename FROMCONTAINER>
                inline  set<T>  mkSfromC (const FROMCONTAINER& rhs)
                {
                    return set<T> (rhs.begin (), rhs.end ());
                }



                template    <typename T>
                void    Intersect (set<T>* s1, const set<T>& s2)
                {
                    RequireNotNull (s1);
                    // Sloppy - but hopefully adequate implementation
                    if (not s1->empty () and not s2.empty ()) {
                        *s1 = Intersection (*s1, s2);
                    }
                }



                template    <typename T>
                set<T>  Intersection (const set<T>& s1, const set<T>& s2)
                {
                    set<T>  result;
                    for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (s2.find (*i) != s2.end ()) {
                            result.insert (*i);
                        }
                    }
                    return result;
                }
                template    <typename T>
                bool    Intersect (const set<T>& s1, const set<T>& s2)
                {
                    for (typename set<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
                        if (s2.find (*i) != s2.end ()) {
                            return true;
                        }
                    }
                    return false;
                }

                template    <typename T, typename FROMCONTAINER>
                void    Union (set<T>* s1, const FROMCONTAINER& s2)
                {
                    for (auto i : s2) {
                        if (s1->find (i) == s1->end ()) {
                            s1->insert (i);
                        }
                    }
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>  Union (const set<T>& s1, const FROMCONTAINER& s2)
                {
                    set<T>  result  =   s1;
                    Union (&result, s2);
                    return result;
                }


                template    <typename T, typename FROMCONTAINER>
                void    Difference (set<T>* s1, const FROMCONTAINER& s2)
                {
                    RequireNotNull (s1);
                    for (auto i : s2) {
                        if (s1->find (i) != s1->end ()) {
                            result.erase (i);
                        }
                    }
                    return result;
                }
                template    <typename T, typename FROMCONTAINER>
                set<T>  Difference (const set<T>& s1, const FROMCONTAINER& s2)
                {
                    set<T>  result = s1;
                    Difference (&result, s2);
                    return result;
                }

#if 0
                template    <typename T>
                set<T>  Difference (const set<T>& lhs, const set<T>& rhs)
                {
                    set<T>  result;
                    for (typename set<T>::const_iterator i = lhs.begin (); i != lhs.end (); ++i) {
                        if (rhs.find (*i) == rhs.end ()) {
                            result.insert (*i);
                        }
                    }
                    return result;
                }
#endif


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_STL_SetUtils_inl_*/
