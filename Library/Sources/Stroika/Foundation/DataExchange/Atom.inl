/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_inl_
#define _Stroika_Foundation_DataExchange_Atom_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  ()
                : fValue_ (ATOM_MANAGER::kEmpty)
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const String& src)
                : fValue_ (ATOM_MANAGER::Intern (src))
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const Atom& src)
                : fValue_ (ATOM_src.fValue_)
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  String   Atom<ATOM_MANAGER>::GetName () const
            {
                return fValue_ == ATOM_MANAGER::kEmpty ?
                       String () :
                       ATOM_MANAGER::Extract (fValue_)
                       ;
            }
            template    <typename   ATOM_MANAGER>
            inline  int Atom<ATOM_MANAGER>::Compare (Atom rhs) const
            {
                // @todo be more careful about overflow
                return static_cast<int> (fValue_ - rhs.fValue_);
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator< (Atom rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator<= (Atom rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator> (Atom rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator>= (Atom rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator== (Atom rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator!= (Atom rhs) const
            {
                return Compare (rhs) != 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool    Atom<ATOM_MANAGER>::empty () const
            {
                return fValue_ == ATOM_MANAGER::kEmpty;
            }
            template    <typename   ATOM_MANAGER>
            inline  void    Atom<ATOM_MANAGER>::clear ()
            {
                fValue_ = ATOM_MANAGER::kEmpty;
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_Atom_inl_*/
