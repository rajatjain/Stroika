/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_h_
#define _Stroika_Foundation_Characters_String_h_    1

#include    "../StroikaPreComp.h"

#include    <locale>
#include    <string>

#include    "../Containers/Sequence.h"
#include    "../Containers/Set.h"
#include    "../Execution/ModuleInit.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"
#include    "SDKString.h"

#include    "Character.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * Description:
 *
 *      This family of classes using something akin to the Letter/Envelope paradigm
 *      described in Copliens "Advanced C++ Programming Styles and Idioms").
 *
 *      The class String () is the main thing that users of this class will see. In
 *      fact, it would be EXTREMELY unusual to interact with the class String in any
 *      other way. However, as part of the internal implementation, and in some cases
 *      for optimizations in places where an alternate text representation is preferred,
 *      there may be some interest in the "letters" inside of Strings.
 *
 *      The class that String's own is a reference counted pointer to String::Rep.
 *      This class is abstract, and defines a protocol for getting at the string data
 *      (Character*) and for getting/setting the string length. All the rest of the
 *      String class is built in this interface, and subclasses of String::Rep only
 *      needed follow that prootcol to be used interchangably with the rest of Strings.
 *
 *      This is which NEED be used. However, Stroika provides several String SUBTYPES
 *      which act EXACTLY like a String, but offer different performance behaviors.
 *
 *              o   String_BufferedArray
 *              o   String_ExternalMemoryOwnership_ApplicationLifetime     (aka String_Constant)
 *              o   String_ExternalMemoryOwnership_StackLifetime
 *              o   String_wstring
 *
 *      Possible future additions
 *              o   String_Common
 *              o   String_AsciiOnlyOptimized
 *
 * TODO:
 *
 *      @todo   EXPLAIN why InsertAt () returns string and Append() doesnt! Or - change it!
 *              Basic idea is that append is SO convnentit (+=) - that we just must support that
 *              and it can be done safely.
 *
 *              But InsertAt () is less common, and so we can just encourage the more safe strategy of
 *              returning a new string.
 *
 *              Weak arguemnt! BUt best I can come up with...
 *
 *      @todo   WRITEUP THREAD SAFETY:
 *              Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
 *              but externally, envelope cannot be read/write or write/write at the same time). � document examples.
 *
 *      @todo   Handle few remaining cases of '// @todo - NOT ENVELOPE THREADSAFE' in implementaiton - mostly on
 *              Appends and Removes.
 *
 *              NO WORSE THAN THAT - MUCH IS NOT ENVELOPE THREADSAFE. Issue is that even with no sharing (sharecount=1)
 *              if I have one thread reading and one writing - that's intrinsically not safe!
 *
 *              I THINK the only ways out are:
 *                  o   lock every operation (makes reads too costly)
 *                  o   Do all multistep operations in reps (so they can manage locking (I think that
 *                      still means read/write locks like we have for Containers).
 *                  o   OR every changing operation makes a new string rep. THAT - as it turns out- maybe
 *                      the way to go (lisp/functional programming).
 *
 *              That may mean the CopyOnWrite stuff is useless here?
 *
 *      @todo   Cleanup SubString (), and String::SubString_ use of SharedByValue<TRAITS>::ReadOnlyReference for
 *              performance. At some level - in String::SubString_ - we have a (hidden) sharedPtr and it would
 *              be safe and performant in that case to re-use that shared_ptr to make a new String envelope.
 *
 *              However, I'm not sure its safe in general to have SharedByValue<TRAITS>::ReadOnlyReference expose
 *              its shared_ptr, which appears needed to make this happen.
 *
 *              Not a biggie opportunity, so we can delay this -- LGP 2014-04-10
 *
 *      @todo   Change APIs that return vector to return Iterable<> using CreateGenerator (). Tried once and worked
 *              very nicely.
 *
 *      @todo   Annotate basic string aliases as (basic_string alias - as below). At least try and think
 *              through if this seems ugly/pointless.
 *
 *      @todo   Add AsSDKString(TCHAR BUF) overload - as performance tweek. Not sure can be easily done safely so
 *              not sure of this, but could be a significant performance advantage. Test more...
 *
 *              Maybe better - AsTChar* - which takes BUFFER arg. And OPTIONALLY uses that buffer arg (depending
 *              on sizeof TCHAR???
 *
 *              Or maybe better - just change code that uses TSTring version of APIs to use W version of APIS!!!
 *              I THINK THAT IS BEST!
 *
 *      @todo   Add PadLeft/PadRight or FillLeft/FilLRight() - not sure which name is better. But idea is to
 *              produce a string which is indentical to the orig except that IF start len < n, then expand it with
 *              the given arg char repeated on teh left or right.
 *
 *      @todo   Add FromISOLatin1()
 *
 *      @todo   Performance optimize FromUTF8, and AsUtf8() functions to not go through intermediate string
 *              objects!
 *
 *      @todo   Add #include of Comparer and template specialize Comparer for String??? Maybe
 *              Maybe not needed. Or maybe can avoid the #include and just do template specailizaiton?
 *
 *      @todo   WHEN we have String_stdwstring implemented  - we can do String (wstring&& ctor that
 *              does std::move of wstring - and builds String_stdwstring rep! That could be notably
 *              faster for constructing Strings from wstring - in some cases...
 *
 *      @todo   Add a String_AsciiOnlyOptimized or String_isolatin1Optimized class (instead of String_UTF8).
 *              Idea is THIS can be very efficient and we can detect automatically when to create in
 *              normal string CTOR (i think - if strlen > 3? - check if all ascii and do).
 *
 *              This will automatically morph to other class (like buffered as we do for readonly). Maybe that
 *              means we need a backend, but no front-end?
 *
 *              OBSOLETE TODO
 *                  Consider adding a new subtype of string - OPTIMIZAITON - which takes an ASCII argument (so can do less checking
 *                  and be more compact??? Perhaps similarly for REP storing stuff as UTF8?
 *
 *                  CLOSELY RELATED - MAYBE DO part of above (no check CTOR except assert).
 *
 *      @todo   Think out and document the whole choice about 'readonly' strings and all modifying member
 *              functions returning a new string. Has performance implications, but also usability.
 *              Not sure what way to go (probably do it), but clearly document!!! And docment reasons.
 *              Maybe can be done transparently with envelope (maybe already done?)
 *
 *      @todo   RFind() API should be embellished to include startAt etc, like regular Find () - but not 100%
 *              sure - think through...
 *
 *      @todo   Document better what APIs CHANGE the string, and what APIs have no effect. Verbs like
 *              "ToLowerCase" are AMBIGUOUS.
 *
 *      @todo   CLEANUP Tokenize API, AND VERY IMPROTANMTLY - either DOCUMENT or indirect to Tokenize() API.
 *              MANY people will expect a String.Split() function to exist. This is what the existing TOKENIZE() API does but thats
 *              not obvious!!!! MUST BE SUPER CLEARLY DOCUMENTED.
 *              Be sure docs for TOKENIZE are clear this is not a FLEX replacement - but just a very simple 'split' like functionaliuty.
 *              not totally clear what name is best (split or tokenize()).
 *
 *              Closely related to my existing FIND () API. Maybe this is just a comment?
 *
 *              But also review:
 *                  http://qt-project.org/doc/qt-5.0/qtcore/qstring.html#split
 *
 *                  especially:
 *                      QString line = "forename\tmiddlename  surname \t \t phone";
 *                      QRegularExpression sep("\\s+");
 *                      str = line.section(sep, 2, 2); // str == "surname"
 *                      str = line.section(sep, -3, -2); // str == "middlename  surname"
 *              Make sure our FIND is at least this simple, and maybe diff between find and split is FIND the regular expression names the things
 *              looked for and SPLIT() uses regexp to name the separators?
 *
 *              Add something like the above to the String String demo app (when it exists)
 *
 *      @todo   MAYBE also add ReplaceOne() function (we have ReplaceAll() now).
 *
 *      @todo   Add overload for ReplaceAll() where first arg is a lambda on a character, so easier to generalzie to replace all whitespace, etc.
 *
 *      @todo   Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
 *              it is all correct for each (especially SetStorage () sutff looks quesitonable)
 *
 *      @todo   String::Remove()
 *              CRAPPY API - probably delete or redo. See if ever used!!! RemoveAll or RemoveFirst() would be
 *              better names (thogh all woudl be a change in functionality) - and maybe return a bool so
 *              you can tell if it did anyhting? Also - make case match an optional param.
 *
 *      @todo   Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation
 *
 *      @todo   Try and get rid of the Peek () API
 *
 *              OR better document why its OK (unexpected throw stuff may make it OK).
 *
 *              Maybe also add GetRepSupportedFeatures() API? Maybe not worth it. If no - dumcument why someplace!
 *
 *      @todo   Add Ranged insert public envelope API, and add APPEND (not just operaotr+) API. See/maybe use new
 *              Stroika Range type?
 *
 *      @todo   Migrate most of the StringUtils stuff here like:
 *              > Contains- with CI optin
 *overload so can be string arg OR lambda!
 *              > Compare () - returns < less > more =0 for equal- with CI optin
 *              > Equals() - with CI optin
 *
 *      @todo   Add Left()/Right()/Mid() funtions - like basic (simple, vaguely useful - especially 'Right'()).
 *
 *      @todo   Add NormalizeSpace (Character useSpaceCharacter = ' ');
 *              see Qt 'QString::simplify()'. Idea is Trim() (right and left) - plus replace contiguous substrings with
 *              Character::IsSpace() with a single (given) space character.
 *
 *      @todo   Compare
 *          template    <typename TCHAR>
 *              basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text)
 *                  {
 *                      std::locale loc1;   // default locale
 *                      const ctype<TCHAR>& ct = use_facet<ctype<TCHAR>>(loc1);
 *                      typename basic_string<TCHAR>::const_iterator i = text.end ();
 *                      for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i-1)); --i)
 *                          ;
 *                      return basic_string<TCHAR> (text.begin (), i);
 *                  }
 *          with the TRIM() implementation I wrote here - in String. Not sure we want to use the locale stuff? Maybe?
 *
 *      @todo   Implement String_Common
 *              NOT YET IMPLEMETNED - EVEN IN FAKE FORM - BECAUSE I"M NOT SURE OF SEMANTICS YET!
 *
 *              String_Common is a subtype of string you can use to construct a String object freely.
 *              It has no semantics requirements. However, it SHOULD only be used for strings which are
 *              commonly used, and where you wish to save space. The implementation will keep the memory for String_Common strings
 *              allocated permanently - for the lifetime of the application, and will take potentially
 *              extra time looking for the given string.
 *
 *              We MAY handle this like the DataExchange::Atom<> class - where we store the string in
 *              a hashtable (or map), and do quick lookup of associated index, and also store in a table
 *              (intead of vector of strings, use a big buffer we APPEND to, and whose index is the value
 *              of the stored rep. Then doing a PEEK() is trivial and efficient.
 *              class   String_Common : public String {
 *                  public:
 *                      explicit String_Common (const String& from);
 *              };
 *
 *      @todo   Add a String_UTF8 backend - (maybe a number of variants). Key is that these can be more compact
 *              and for many operaitons, just fine, but for insert, and a[i] = quite sucky.
 *              [RETHINK - See String_AsciiOnlyOptimized - and probably dont do this]
 *
 *      @todo   Handle Turkish toupper('i') problem. Maybe use ICU. Maybe add optional LOCALE parameter to routines where this matters.
 *              Maybe use per-thread global LOCALE settings. Discuss with KDJ.
 *              KDJ's BASIC SUGGESTION is - USE ICU and 'stand on their shoulders'.
 *
 *      @todo   CircularSubString() is not a good name for what it does. But overloading SubString() seemed worse
 *              it was likely to mask bugs...
 *
 */



// forward declare so we can provide operator<<, but doing #include iostream/iosfwd creates much more dependency
namespace   std {
    template    <class charT> struct char_traits;
    template<>  struct char_traits<wchar_t>;
    template <class charT, class traits> class basic_ostream;
    using   wostream    =   basic_ostream<wchar_t, char_traits<wchar_t>>;
}


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            class   RegularExpression;


            /**
             *  The Stroika String class is an alternatve for the wstring class, which should be largely
             *  interoperable with code using wstring (there is wstring constructor and As<wstring>()
             *  methods).
             *
             *  The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is
             *  no obvious way to map the Stroika String to a string. However, if you specify a codepage
             *  for conversion, or are converting to/from SDKString/SDKChar, there is builtin support for that.
             *
             *
             *  EOS Handling:
             *      The Stroika String class does support having embedded NUL-characters. It also supports
             *      returning wchar_t* strings which are NUL-terminated. But - Stroika generally does NOT
             *      maintain strings internally as NUL-terminated (generally). It may add a performance
             *      overhead when you call the c_str() method to force a NUL-character termination.
             *      See String::c_str ().
             *
             *      Also note that some subclasses of String (e.g. future String_stdwstring) may not support
             *      internal NUL-characters, if their underling implementation doesn't allow for that.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *                              with caveats!
             *
             *                              const wchar_t* c_str ()
             *                              const wchar_t* As<const wchar_t*> ()
             *                              const Character* As<const Character*> ()
             *
             *                              all return 'internal' pointers whose lifetime extends until the next modification of the String object.
             *                              But if another thread modifies this String object, that might not be usefully long.
             *
             *                              You can always copy a 'shared' String to a copy no other thread is modifying, and call c_str() on that.
             *
             *
             *      @see   Concrete::String_BufferedArray
             *      @see   Concrete::String_ExternalMemoryOwnership_ApplicationLifetime     (aka String_Constant)
             *      @see   Concrete::String_ExternalMemoryOwnership_StackLifetime
             *      @see   Concrete::String_Common
             *
             *  \note   Design Choice - Iterable<T> / Iterator<T> behavior
             *      We have two basic choices of how to define the behavior of iterators:
             *      o   Live Update (like we do for Containers) - where changes to the
             *          String appear in iteration.
             *
             *      o   Snapshot at the start of iteration
             *
             *      The advantages of 'live update' are that its probably better / clearer semantics. We may
             *      want to switch to that. But to implement, we need to keep the update and iteration code
             *      in sync.
             *
             *      'Snapshot at the start of iteration' can be more efficient, and easier to implement.
             *      So - we do that for now.
             *
             *      Among 'snapshot' impl choices - we COULD do lazy copy snapshot (COW). That would perform best.
             *      But to do so - the way the code is currently structured, we would need to use enable_shared_from_this
             *      so we can recover a shared_ptr in ReadOnlyRep::_Rep::MakeIterator.
             *
             *      enable_shared_from_this() would add costs (at least size) even when we dont use String iteration
             *      (which we didnt even impelement for a couple years, so may not be that critical).
             *
             *      For now - stick to simple impl - of just copy on start of iteration.
             *          -- LGP 2013-12-17
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *          Uses immutable string rep pattern.
             *
             *  \note   Static Initialization, file scope variables, application lifetime variables.
             *          It \em IS safe to use the String class at file scope. The constructors are carefully crafted to
             *          operate properly, even if used at file scope, and to initialize other strings or objects.
             *          @see "Test of STATIC FILE SCOPE INITIALIZATION"
             */
            class   String : public Traversal::Iterable<Character> {
            private:
                using   inherited   =   Iterable<Character>;

            public:
#if     !qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
                static  constexpr size_t    kBadIndex   = wstring::npos;
#else
                DEFINE_CONSTEXPR_CONSTANT (size_t, kBadIndex, -1);
#endif

            public:
                /**
                 * All the constructors are obvious, except to note that NUL-character ARE allowed in strings,
                 * except for the case of single char* argument constructors - which find the length based on
                 * the terminating NUL-character.
                 */
                String ();
                String (const char16_t* cString);
                String (const char32_t* cString);
                String (const wchar_t* cString);
                String (const wchar_t* from, const wchar_t* to);
                String (const Character* from, const Character* to);
                String (const wstring& r);
                String (const Iterable<Character>& src);
                String (const String& from) noexcept;
                String (String&& from) noexcept;
                explicit String (const Character& c);

            public:
                ~String () = default;

            protected:
                class   _IRep;

            protected:
                using   _IterableSharedPtrIRep  =   Iterable<Character>::_SharedPtrIRep;

            protected:
                using   _SharedPtrIRep          =   SharedPtrImplementationTemplate<_IRep>;

            protected:
                /**
                 */
                using   _SafeReadRepAccessor = Iterable<Character>::_SafeReadRepAccessor<_IRep>;

            protected:
                /**
                 * \req rep MUST be not-null
                 *  However, with move constructor, it maybe null on exit.
                 */
                String (const _SharedPtrIRep& rep) noexcept;
                String (_SharedPtrIRep&& rep) noexcept;

            public:
                nonvirtual  String& operator= (const String& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  String& operator= (String && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  String& operator= (String && newString) = default;
#endif

            public:
                /**
                 *  Create a String object from a 'char-based' utf-8 encoded string.
                 */
                static  String  FromUTF8 (const char* from);
                static  String  FromUTF8 (const char* from, const char* to);
                static  String  FromUTF8 (const string& from);

            public:
                /**
                 *  Create a String object from a 'SDKChar' (os-setting - current code page) encoded string.
                 *  See @SDKChar
                 *  See @SDKString
                 */
                static  String  FromSDKString (const SDKChar* from);
                static  String  FromSDKString (const SDKChar* from, const SDKChar* to);
                static  String  FromSDKString (const SDKString& from);

            public:
                /**
                 *  Create a String object from a 'char-based' (os-setting - current code page) encoded string.
                 */
                static  String  FromNarrowSDKString (const char* from);
                static  String  FromNarrowSDKString (const char* from, const char* to);
                static  String  FromNarrowSDKString (const string& from);

            public:
                /**
                 *  Create a String object from a 'char-based' on the encoding from the argument locale.
                 *  This throws an exception if there is an error performing the conversion.
                 */
                static  String  FromNarrowString (const char* from, const locale& l);
                static  String  FromNarrowString (const char* from, const char* to, const locale& l);
                static  String  FromNarrowString (const string& from, const locale& l);

            public:
                /**
                 *  Create a String object from ascii text. This function requires that its arguments all ascii (no high-bit set)
                 */
                static  String  FromAscii (const char* from);
                static  String  FromAscii (const string& from);

            private:
                static  _SharedPtrIRep  mkEmpty_ ();
                static  _SharedPtrIRep  mk_ (const wchar_t* start, const wchar_t* end);
                static  _SharedPtrIRep  mk_ (const wchar_t* start, const wchar_t* end, size_t reserveLen);
                static  _SharedPtrIRep  mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2);

            public:
                nonvirtual  String& operator+= (Character appendage);
                nonvirtual  String& operator+= (const String& appendage);
                nonvirtual  String& operator+= (const wchar_t* appendageCStr);

            public:
                nonvirtual  String  operator+ (const String& rhs) const;
                nonvirtual  String  operator+ (const wchar_t* appendageCStr) const;

            public:
                /**
                 *  Returns the number of characters in the String. Note that this may not be the same as bytes,
                 *  does not include NUL termination, and doesn't in any way respect NUL termination (meaning
                 *  a nul-character is allowed in a Stroika string.
                 */
                nonvirtual  size_t  GetLength () const;

            public:
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  Alias for *this = String ();
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                nonvirtual  const Character   GetCharAt (size_t i) const;

            public:
                /**
                 */
                nonvirtual  void        SetCharAt (Character c, size_t i);

            public:
                /**
                 *  \brief return (read-only) Character object
                 *
                 *  Alias for GetCharAt (size_t i) const;
                 */
                nonvirtual  const Character   operator[] (size_t i) const;

            public:
                /**
                 *  InsertAt() constructs a new string by taking this string, and inserting the argument
                 *  characters.
                 *
                 *  \em Note that for repeated insertions, this is much less efficient than just
                 *      using StringBuilder.
                 */
                nonvirtual  String      InsertAt (Character c, size_t at) const;
                nonvirtual  String      InsertAt (const String& s, size_t at) const;
                nonvirtual  String      InsertAt (const wchar_t* from, const wchar_t* to, size_t at) const;
                nonvirtual  String      InsertAt (const Character* from, const Character* to, size_t at) const;

            public:
                /**
                 *  Append() adds the given argument characters to the end of this string object.
                 *
                 *  Unlike InsertAt() - this modifies 'this' string, rather than returning a new one.
                 *
                 *  \em Note that for repeated insertions, this is much less efficient than just
                 *      using StringBuilder.
                 *
                 *  \em Note that it is legal, but pointless to pass in an empty string to insert
                 */
                nonvirtual  void        Append (Character c);
                nonvirtual  void        Append (const String& s);
                nonvirtual  void        Append (const wchar_t* s);
                nonvirtual  void        Append (const wchar_t* from, const wchar_t* to);
                nonvirtual  void        Append (const Character* from, const Character* to);

            public:
                /**
                 * Remove the characters at 'charAt' (RemoveAt/1) or between 'from' and 'to'.
                 *
                 * It is an error if this implies removing characters off the end of the string.
                 *
                 *  \req (charAt < GetLength ())
                 *  \req (from <= to)
                 *  \req (to <= GetLength ())
                 *
                 *  \em Note that this is quite inefficent: consider using StringBuffer (@todo is that the right name)???
                 */
                nonvirtual  String      RemoveAt (size_t charAt) const;
                nonvirtual  String      RemoveAt (size_t from, size_t to) const;

            public:
                /**
                 *  Remove the first occurence of Character 'c' from the string. Not an error if none
                 *  found.
                 *
                 *  \em Note that this is quite inefficent: consider using StringBuffer (@todo is that the right name)???
                 */
                nonvirtual  String      Remove (Character c) const;

            public:
                /**
                 *  Produce a substring of this string, starting at 'from', and up to 'to' (or end of string
                 *  for one-arg overload).
                 *
                 *  *NB* This function treats the second argument differntly than String::substr () -
                 *  which respects the STL basic_string API. This function treats the second argument
                 *  as a 'to', STL substr() treats it as a count. This amounts to the same thing for the
                 *  very common cases of substr(N) - because second argument is defaulted, and,
                 *  substr (0, N) - because then the count and end are the same.
                 *
                 *  \req  (from <= to);
                 *  \req  (to <= GetLength ());     // for 2-arg variant
                 *
                 *  @see substr
                 *  @see CircustlarSubString
                 */
                nonvirtual  String      SubString (size_t from) const;
                nonvirtual  String      SubString (size_t from, size_t to) const;

            public:
                /**
                 *  This is like SubString() except that if from/to are negative, they are treated as relative to the end
                 *  of the String.
                 *
                 *  So for example, CircularSubString (0, -1) is equivalent to SubString (0, GetLength () - 1) - and so is an
                 *  error if the string is empty.
                 *
                 *  Similarly, CircularSubString (-5) is equivalent to SubString (GetLength ()-5, GetLength ()) - so can be used
                 *  to grab the end of a string.
                 *
                 *  \note \em Design Note
                 *      We chose not to overload SubString() with this functionality because it would have been to easy
                 *      to mask bugs.
                 */
                nonvirtual  String      CircularSubString (ptrdiff_t from) const;
                nonvirtual  String      CircularSubString (ptrdiff_t from, ptrdiff_t to) const;

            private:
                static  String  SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to);

            public:
                /**
                 *  Return 'count' copies of this String (concatenated after one another).
                 */
                nonvirtual  String  Repeat (unsigned int count) const;

            public:
                /**
                 *  Returns true if the argument character or string is found anywhere inside this string.
                 *  This is equivalent to
                 *      return Match (".*" + X + L".*");    // If X had no characters which look like they are part of
                 *                                          // a regular expression
                 *
                 *  @see Match
                 */
                nonvirtual  bool    Contains (Character c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  bool    Contains (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Returns true iff the given substring is contained in this string.
                 *
                 *  Similar to:
                 *      return Match (X + L".*");
                 *  except for the fact that with StartsWith() doesn't interpet 'X' as a regular expression
                 *
                 *  @see Match
                 *  @see EndsWith
                 */
                nonvirtual  bool    StartsWith (const Character& c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  bool    StartsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Returns true iff the given substring is contained in this string.
                 *
                 *  Similar to:
                 *      return Match (X + L".*");
                 *  except for the fact that with StartsWith() doesn't interpet 'X' as a regular expression
                 *
                 *  @see Match
                 *  @see StartsWith
                 */
                nonvirtual  bool    EndsWith (const Character& c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  bool    EndsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Apply the given regular expression return true if it matches this string. This only
                 *  returns true if the expression matches the ENTIRE string - all the way to the end.
                 *  @see FindEach() or @see Find - to find a set of things which match.
                 *
                 *  For example:
                 *      Assert (String (L"abc").Match (L"abc"));
                 *      Assert (not (String (L"abc").Match (L"bc")));
                 *      Assert (String (L"abc").Match (L".*bc"));
                 *      Assert (not String (L"abc").Match (L"b.*c"));
                 *
                 *  Details on the regular expression language/format can be found at:
                 *      http://en.wikipedia.org/wiki/C%2B%2B11#Regular_expressions
                 *
                 *  @see Contains
                 *  @see StartsWith
                 *  @see EndsWith
                 *  @see Find
                 *  @see FindEach
                 */
                nonvirtual  bool    Match (const RegularExpression& regEx) const;

            public:
                /**
                 *  Find returns the index of the first occurance of the given Character/substring argument in
                 *  this string. Find () always returns a valid string index, which is followed by the
                 *  given substring, or kBadIndex otherwise.
                 *
                 *  Find () can optionally be provided a 'startAt' offset to begin the search at.
                 *
                 *  And the overload taking a RegularExpression - returns BOTH the location where the match
                 *  is found, but the length of the match.
                 *
                 *  Note - for the special case of Find(empty-string) - the return value is 0 if this string
                 *  is non-empty, and kBadIndex if this string was empty.
                 *
                 *  \req (startAt <= GetLength ());
                 *
                 *  @see FindEach ()
                 *  @see FindEachString ()
                 */
                nonvirtual  size_t                  Find (Character c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (Character c, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (const String& subString, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  pair<size_t, size_t>    Find (const RegularExpression& regEx, size_t startAt = 0) const;

            public:
                /**
                 *  This is just like Find, but captures all the matching results in an iterable result.
                 *  The reason the overload for RegularExpression's returns a list of pair<size_t,size_t> is because
                 *  the endpoint of the match is ambiguous. For fixed string Find, the end of match is computable
                 *  from the arguments.
                 *
                 *  FindEach () can be more handy to use than directly using Find () in scenarios where you want
                 *  to iterate over each match:
                 *      e.g.:
                 *          for (auto i : s.FindEach ("xxx")) {....}
                 *
                 *  Also, to count matches, you can use:
                 *      size_t nMatches = FindEach (matchexp).size ();
                 *
                 *  Note: FindEach handles the special case of an empty match as ignored, so FindEach(empty-str-or-regexp)
                 *  always returns an empty list. Also - for the String case, it returns distinct matches, so if you
                 *  search String ("AAAA").FindEach ("AA"), you will get 2 answers ({0, 2}).
                 *
                 *  @see Find ()
                 *  @see FindEachString ()
                 */
                nonvirtual  vector<pair<size_t, size_t>>    FindEach (const RegularExpression& regEx) const;
                nonvirtual  vector<size_t>                  FindEach (const String& string2SearchFor, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Run Find (), starting at the beginning of the string and iterating through, capturing all the
                 *  occurrances of the given argument substring. Accumulate those results and return the actual
                 *  substrings.
                 *
                 *  *Design Note*:
                 *
                 *  There is no overload for FindEachString() taking a Character or SubString, because the results
                 *  wouldn't be useful. Their count might be, but the results would each be identical to the argument.
                 */
                nonvirtual  vector<String>  FindEachString (const RegularExpression& regEx) const;

            public:
                /**
                 * RFind (substring) returns the index of the last occurance of the given substring in
                 * this string. This function always returns a valid string index, which is followed by the
                 * given substring, or kBadIndex otherwise.
                 */
                nonvirtual  size_t  RFind (Character c) const;
                nonvirtual  size_t  RFind (const String& subString) const;

            public:
                /**
                 * Apply the given regular expression, with 'with' and replace each match. This doesn't
                 * modify this string, but returns the replacement string.
                 *
                 * CHECK - BUT HI HTINK WE DEFINE TO REPLACE ALL? OR MAKE PARAM?
                 * See regex_replace () for definition of the regEx language
                 *
                 *   Require (not string2SearchFor.empty ());
                 *       TODO: GIVE EXAMPLES
                 *
                 *  Note - it IS legal to have with contain the original search for string, or even
                 *  to have it 'created' as part of where it gets
                 *  inserted. The implementation will only replace those that pre-existed.
                 */
                nonvirtual  String  ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  String  ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Break this String into constituent parts. This is a simplistic API (@todo perhaps enahnce
                 *  with regular expressions?) - but at least handy as is.
                 *
                 *      @todo   Perhaps make trim be a trim-lambda? Maybe just as overload
                 *
                 *  This is often called 'Split' in other APIs.
                 *
                 *  EXAMPLE USE:
                 *      String  t { L"ABC DEF G" };
                 *      Assert (t.Tokenize ().length () == 3);
                 *      Assert (t.Tokenize ()[1] = L"DEF");
                 *
                 *  EXAMPLE USE:
                 *      String  t { L"foo=   7" };
                 *      auto    tt = t.Tokenize ({ '=' });
                 *      Assert (t.length () == 2);
                 *      Assert (t[1] == L"7");
                 */
                nonvirtual  Containers::Sequence<String>  Tokenize (const function<bool(Character)>& isTokenSeperator, bool trim = true) const;
                nonvirtual  Containers::Sequence<String>  Tokenize (const Containers::Set<Character>& delimiters, bool trim = true) const;

            public:
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                inline  static  bool    DefaultTrimArg_ (Character c)               {       return c.IsWhitespace ();       }
#endif

#if     !qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                /**
                 * String LTrim () scans the characters form the left to right, and applies the given
                 * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
                 * and the resulting string is returned. This does not modify the current string its
                 * applied to - just returns the trimmed string.
                 */
                nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
                /**
                 * String RTrim () scans the characters form the right to left, and applies the given
                 * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
                 * and the resulting string is returned. This does not modify the current string its
                 * applied to - just returns the trimmed string.
                 */
                nonvirtual  String  RTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
                /**
                 * String Trim () is locally equivalent to RTrim (shouldBeTrimmed).LTrim (shouldBeTrimmed).
                 */
                nonvirtual  String  Trim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
#else
                nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
                nonvirtual  String  RTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
                nonvirtual  String  Trim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
#endif

            public:
                /**
                 * Walk the entire string, and produce a new string consisting of all characters for which
                 * the predicate 'removeCharIf' returned false.
                 */
                nonvirtual  String  StripAll (bool (*removeCharIf) (Character)) const;

            public:
                /**
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivalent. Note that non-lower-case characters (such as punctuation) un unchanged.
                 */
                nonvirtual  String  ToLowerCase () const;

            public:
                /**
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivalent. Note that non-upper-case characters (such as punctuation) un unchanged.
                 */
                nonvirtual  String  ToUpperCase () const;

            public:
                /**
                 * Return true if the string contains zero non-whitespace characters.
                 */
                nonvirtual  bool    IsWhitespace () const;

            public:
                /**
                 *  This function is for GUI/display purposes. It returns the given string, trimmed down
                 *  to at most maxLen characters, and removes surrounding whitespace.
                 *
                 *  Note in the 3-arg overload, the elipsis string MAY be the empty string.
                 */
                nonvirtual  String  LimitLength (size_t maxLen, bool keepLeft = true) const;
                nonvirtual  String  LimitLength (size_t maxLen, bool keepLeft, const String& ellipsis) const;

            public:
                /**
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom)
                 *  is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;

            public:
                /**
                 * Convert String losslessly into a standard C++ type (right now just <wstring>,
                 * <const wchar_t*>,<const Character*> supported)
                 *
                 * For the special cases of <T=const wchar_t*>, and <T=const Character*>, the returned
                 * result is NOT NUL-terminated.
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                template    <typename   T>
                nonvirtual  void    As (T* into) const;

            public:
                /*
                 *  explicit operator T () provides an alterntive syntax to As<> - depending on user
                 *  preference or context. Note - its important that this is explicit - to avoid
                 *  creating overload problems
                 *
                 *      EXPERIMENTAL AS OF 2014-02-11 (v2.0a21)
                 */
                template    <typename   T>
                nonvirtual  explicit operator T () const;

            public:
                /**
                 *  Create a narrow string object from this, based on the encoding from the argument locale.
                 *  This throws an exception if there is an error performing the conversion, and the 'into' overload
                 *  leaves 'into' in an undefined (but safe) state.
                 */
                nonvirtual  string  AsNarrowString (const locale& l) const;
                nonvirtual  void    AsNarrowString (const locale& l, string* into) const;

            public:
                /**
                 * Convert String losslessly into a standard C++ type (right now just <string> supported).
                 * Note - template param is optional.
                 */
                template    <typename   T>
                nonvirtual  T   AsUTF8 () const;
                template    <typename   T>
                nonvirtual  void    AsUTF8 (T* into) const;
                nonvirtual  string  AsUTF8 () const;
                nonvirtual  void    AsUTF8 (string* into) const;

            public:
                /**
                 */
                nonvirtual  SDKString   AsSDKString () const;
                nonvirtual  void        AsSDKString (SDKString* into) const;

            public:
                /**
                 */
                nonvirtual  string  AsNarrowSDKString () const;
                nonvirtual  void    AsNarrowSDKString (string* into) const;

            public:
                /**
                 * Convert String losslessly into a standard C++ type (right now just <string> supported).
                 * The source string MUST be valid ascii characters (asserted)
                 */
                template    <typename   T>
                nonvirtual  T   AsASCII () const;
                template    <typename   T>
                nonvirtual  void    AsASCII (T* into) const;
                nonvirtual  string  AsASCII () const;
                nonvirtual  void    AsASCII (string* into) const;

            public:
                /**
                 *  Only defined for CHAR_TYPE=Character or wchar_t (for now).
                 *
                 *  Lifetime is ONLY up until next method access to String, so this API is intrinsically not threadsafe.
                 *  That is - if you call this on a String, you better not be updating the string at the same time!
                 *
                 *  \note THREAD-SAFETY!
                 *
                 *  @see c_str()
                 */
                template    <typename CHAR_TYPE>
                nonvirtual pair<const CHAR_TYPE*, const CHAR_TYPE*> GetData () const;

            public:
                /**
                  *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                  */
                nonvirtual  int Compare (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const Character* rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const wchar_t* rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 * @brief   Return true of the two argument strings are equal. This is equivalent to
                 *              lhs.Compare (rhs, co);
                 */
                nonvirtual  bool    Equals (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  bool    Equals (const wchar_t* rhs, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const String& rhs)
                 */
                nonvirtual  bool operator< (const String& rhs) const;
                nonvirtual  bool operator<= (const String& rhs) const;
                nonvirtual  bool operator> (const String& rhs) const;
                nonvirtual  bool operator>= (const String& rhs) const;
                nonvirtual  bool operator== (const String& rhs) const;
                nonvirtual  bool operator!= (const String& rhs) const;
                nonvirtual  bool operator< (const Character* rhs) const;
                nonvirtual  bool operator<= (const Character* rhs) const;
                nonvirtual  bool operator> (const Character* rhs) const;
                nonvirtual  bool operator>= (const Character* rhs) const;
                nonvirtual  bool operator== (const Character* rhs) const;
                nonvirtual  bool operator!= (const Character* rhs) const;
                nonvirtual  bool operator< (const wchar_t* rhs) const;
                nonvirtual  bool operator<= (const wchar_t* rhs) const;
                nonvirtual  bool operator> (const wchar_t* rhs) const;
                nonvirtual  bool operator>= (const wchar_t* rhs) const;
                nonvirtual  bool operator== (const wchar_t* rhs) const;
                nonvirtual  bool operator!= (const wchar_t* rhs) const;

            public:
                /**
                 *  basic_string alias for npos = kBadIndex
                 */
#if     !qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
                static  constexpr size_t    npos   = kBadIndex;
#else
                DEFINE_CONSTEXPR_CONSTANT (size_t, npos, kBadIndex);
#endif

            public:
                /**
                 *  basic_string alias: size = GetLength
                 */
                nonvirtual  size_t          size () const;

            public:
                /**
                 *  basic_string alias: length = GetLength
                 */
                nonvirtual  size_t          length () const;

            public:
                /**
                 *  As with STL, the return value of the data () function should NOT be assumed to be
                 *  NUL-terminated
                 *
                 *  The lifetime of the pointer returned is gauranteed until the next call to this String
                 *  envelope class (that is if other reps change, or are acceessed this data will not
                 *  be modified)
                 *
                 *  \note THREAD-SAFETY! - NOT THREADSAFE
                 */
                nonvirtual  const wchar_t*  data () const;

            public:
                /**
                 *  This will always return a value which is NUL-terminated. Note that Stroika generally
                 *  does NOT keep strings in NUL-terminated form, so this could be a costly function,
                 *  requiring a copy of the data.
                 *
                 *  The lifetime of the pointer returned is gauranteed until the next call to this String
                 *  envelope class (that is if other reps change, or are acceessed this data will not be modified)
                 *  Note also that Stroika strings ALLOW internal nul bytes, so though the Stroika string
                 *  class NUL-terminates, it does nothing to prevent already existng NUL bytes from causing
                 *  confusion elsewhere.
                 *
                 *  Lifetime is ONLY up until next method access to String, so this API is intrinsically not threadsafe.
                 *  That is - if you call this on a String, you better not be updating the string at the same time!
                 *
                 *  \note THREAD-SAFETY! - NOT threadsafe
                 *
                 *  @see GetData ()
                 */
                nonvirtual  const wchar_t*  c_str () const;

            private:
                nonvirtual  const wchar_t*  c_str_ () const;

            public:
                /**
                 * need more overloads
                 */
                nonvirtual  size_t find (wchar_t c, size_t startAt = 0) const;

            public:
                /**
                 * need more overloads
                 */
                nonvirtual  size_t rfind (wchar_t c) const;

            public:
                /**
                 *  mimic (much of - need more overloads) STL variant
                 */
                nonvirtual  void erase (size_t from = 0, size_t count = kBadIndex);

            public:
                /**
                 */
                nonvirtual  void    push_back (wchar_t c);
                nonvirtual  void    push_back (Character c);

            public:
                /**
                 *  Compatable with STL::basic_string::subtr() - which interprets second argument as count. Not the same
                 *  as Stroika::String::SubString (where the second argument is a 'to')
                 *
                 *  @see SubString
                 *
                 *  From http://en.cppreference.com/w/cpp/string/basic_string/substr
                 *      Returns a substring [pos, pos+count). If the requested substring extends
                 *      past the end of the string, or if count == npos, the returned substring is [pos, size()).
                 *      std::out_of_range if pos > size()
                 */
                nonvirtual  String      substr (size_t from, size_t count = kBadIndex) const;

            private:
                // Use _SafeReadRepAccessor instead
                nonvirtual  const _IRep&    ConstGetRep_ () const;

            protected:
                nonvirtual  void    _AssertRepValidType () const;

            public:
                friend  wostream&   operator<< (wostream& out, const String& s);
                friend  String      operator+ (const wchar_t* lhs, const String& rhs);
            };


            template    <>
            void    String::As (wstring* into) const;
            template    <>
            wstring String::As () const;
            template    <>
            const wchar_t*  String::As () const;
            template    <>
            const Character*    String::As () const;

            template    <>
            void    String::AsUTF8 (string* into) const;
            template    <>
            string  String::AsUTF8 () const;

            template    <>
            void    String::AsASCII (string* into) const;
            template    <>
            string  String::AsASCII () const;

            template    <>
            pair<const Character*, const Character*> String::GetData () const;
            template    <>
            pair<const wchar_t*, const wchar_t*> String::GetData () const;


            /**
             * Protected helper Rep class.
             */
            class   String::_IRep
                : public Iterable<Character>::_IRep
#if     !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                , public Traversal::IterableBase::enable_shared_from_this_SharedPtrImplementationTemplate<String::_IRep>
#endif
            {
            protected:
                using   _IterableSharedPtrIRep  =   String::_IterableSharedPtrIRep;

            protected:
                using   _SharedPtrIRep      =   String::_SharedPtrIRep;

            protected:
                _IRep () = default;

            protected:
                _IRep (const wchar_t* start, const wchar_t* end);

            public:
                virtual ~_IRep () = default;

            protected:
                // PROTECTED INLINE UTILITY
                nonvirtual  void    _SetData (const wchar_t* start, const wchar_t* end);
            protected:
                // PROTECTED INLINE UTILITY
                nonvirtual     size_t  _GetLength () const;
            protected:
                // PROTECTED INLINE UTILITY
                nonvirtual     Character   _GetAt (size_t index) const;
            protected:
                // PROTECTED INLINE UTILITY
                nonvirtual     const Character*    _Peek () const;

                // Overrides for Iterable<Character>
            public:
                virtual Traversal::Iterator<Character>              MakeIterator (Traversal::IteratorOwnerID suggestedOwner) const override;
                virtual size_t                                      GetLength () const override;
                virtual bool                                        IsEmpty () const override;
                virtual void                                        Apply (_APPLY_ARGTYPE doToElement) const override;
                virtual Traversal::Iterator<Character>              FindFirstThat (_APPLYUNTIL_ARGTYPE, Traversal::IteratorOwnerID suggestedOwner) const override;

            public:
                nonvirtual  Character   GetAt (size_t index) const;

            public:
                nonvirtual  const Character*    Peek () const;

            public:
                nonvirtual  pair<const Character*, const Character*> GetData () const;

            public:
                // return nullptr if its not already NUL-terminated
                virtual const wchar_t*      c_str_peek () const noexcept;

            public:
                /*
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;

            protected:
                const wchar_t*  _fStart;
                const wchar_t*  _fEnd;

            private:
                friend  class   String;
            };


            /**
             *  Return the concatenation of these two strings.
             *
             *  EXPERIMENTAL AS OF 2014-02-11 - unsure how this works with namespace imports (requires using Foundation::Characters) and not totally
             *  clear how overloading interacts with String::operator+ .. so fiddle a bit and see..
             *
             *  But advantage of this CAN be that L"x" + String () works, and doesnt work with member operator+.
             *      -- LGP 2014-02-11
             */
            String  operator+ (const wchar_t* lhs, const String& rhs);


            /**
             *  Use Stroika String more easily with std::ostream.
             *
             *  \note   EXPERIMENTAL API (added /as of 2014-02-15 - Stroika 2.0a21)
             *
             *  \note   Note sure how well this works being in a namespace!
             *
             *  \note   Intentionally dont do operator>> because not so well defined for strings (could do as wtith STL I guess?)
             *
             *  \note   tried to use templates to avoid the need to create a dependency of this module on iostream,
             *          but that failed (maybe doable but overloading was trickier).
             */
            wostream&    operator<< (wostream& out, const String& s);


            /**
             *  This can be referenced in your ModuleInitializer<> to force correct inter-module construction order.
             */
            Execution::ModuleDependency MakeModuleDependency_String ();


        }


        namespace Execution {


            // early alpha placeholder test
            template    <>
            class Synchronized<Characters::String> {
            public:
                using   ContainerType =     Characters::String;
                using   ElementType   =     ContainerType::ElementType;
            public:
                Synchronized () : fDelegate_ () {}
                Synchronized (const Synchronized& src) : fDelegate_ (src) {}
                Synchronized (Synchronized&& src) : fDelegate_ (move (src)) {}
                Synchronized (const ContainerType& src) : fDelegate_ (src) {}
                Synchronized (ContainerType&& src) : fDelegate_ (move (src)) {}
                Synchronized (const char16_t* cString) : fDelegate_ (cString) {}
                Synchronized (const char32_t* cString) : fDelegate_ (cString) {}
                Synchronized (const wchar_t* cString) : fDelegate_ (cString) {}
                Synchronized (const wchar_t* from, const wchar_t* to) : fDelegate_ (from, to) {}
                Synchronized (const Characters::Character* from, const Characters::Character* to) : fDelegate_ (from, to) {}
                Synchronized (const wstring& r) : fDelegate_ (r) {}
                Synchronized (const Traversal::Iterable<Characters::Character>& src)  : fDelegate_ (src) {}
                explicit Synchronized (const Characters::Character& c)  : fDelegate_ (c) {}
                const Synchronized& operator= (const Synchronized& rhs)         { fDelegate_ = rhs.fDelegate_; return *this; }
                Traversal::Iterator<ElementType> begin () const                 { return fDelegate_.begin (); }
                Traversal::Iterator<ElementType> end () const                   { return fDelegate_.end (); }
                operator ContainerType () const                                 { return fDelegate_;  }
                ContainerType  operator+ (const ContainerType& rhs) const       { return fDelegate_ + rhs; }
                ContainerType  operator+ (const wchar_t* appendageCStr) const   { return fDelegate_ + appendageCStr; }
                bool operator== (const ContainerType& rhs) const                { return fDelegate_ == rhs; }
                bool operator!= (const ContainerType& rhs) const                { return fDelegate_ != rhs; }
                bool    empty () const                                          { return fDelegate_.empty (); }
                void    clear ()                                                { fDelegate_.clear (); }

            private:
                ContainerType   fDelegate_;
                mutex           fLock_;
            };


        }

    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String.inl"

#endif  /*_Stroika_Foundation_Characters_String_h_*/
