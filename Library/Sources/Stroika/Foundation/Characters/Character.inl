/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Character_inl_
#define	_Stroika_Foundation_Characters_Character_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<cctype>

#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			inline	Character::Character ()
				 : fCharacterCode ('\0')
				{
				}
			inline	Character::Character (char c)
				: fCharacterCode (c)
				{
				}
			inline	Character::Character (wchar_t wc)
				: fCharacterCode (wc)
				{
				}
			inline	char	Character::GetAsciiCode () const
				{
					// ASSERT IS ASCII
					Require ('0' <= fCharacterCode and fCharacterCode <= 0x8f);	// not sure this is right range...
					return static_cast<char> (fCharacterCode);
				}
			inline	wchar_t	Character::GetCharacterCode () const
				{
					return (fCharacterCode);
				}
			template<>
				inline	wchar_t	Character::As () const
					{
						return GetCharacterCode ();
					}
			inline	bool	Character::IsAscii () const
				{
					return fCharacterCode <= 0x7f;
				}
			inline	bool	Character::IsWhitespace () const
				{
					return !!iswspace (fCharacterCode);
				}
			inline	bool	Character::IsDigit () const
				{
					return !!iswdigit (fCharacterCode);
				}
			inline	bool	Character::IsHexDigit () const
				{
					return !!iswxdigit (fCharacterCode);
				}
			inline	bool	Character::IsAlphabetic () const
				{
					return !!iswalpha (fCharacterCode);
				}
			inline	bool	Character::IsAlphaNumeric () const
				{
					return !!iswalnum (fCharacterCode);
				}
			inline	bool	Character::IsPunctuation () const
				{
					return !!iswpunct (fCharacterCode);
				}

			
			
			inline	bool	operator== (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () == rhs.GetCharacterCode ());
				}
			inline	bool	operator!= (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () != rhs.GetCharacterCode ());
				}
			inline	bool	operator>= (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () >= rhs.GetCharacterCode ());
				}
			inline	bool	operator> (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () > rhs.GetCharacterCode ());
				}
			inline	bool	operator<= (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () <= rhs.GetCharacterCode ());
				}
			inline	bool	operator< (Character lhs, Character rhs)
				{
					// not sure what do do if characer sets differ?
					return (lhs.GetCharacterCode () < rhs.GetCharacterCode ());
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Characters_Character_inl_*/
