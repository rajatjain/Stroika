﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Reader/Writers(JSON/XML)
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchange/BadFormatException.h"
#if     qHasFeature_LZMA
#include    "Stroika/Foundation/DataExchange/7z/Reader.h"
#endif
#include    "Stroika/Foundation/DataExchange/INI/Reader.h"
#include    "Stroika/Foundation/DataExchange/INI/Writer.h"
#include    "Stroika/Foundation/DataExchange/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/DataExchange/XML/Reader.h"
#include    "Stroika/Foundation/DataExchange/XML/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Streams/MemoryStream.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Streams/TextReader.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Characters::String;
using   DataExchange::VariantValue;
using   Memory::Byte;



///// @todo ADD SEPEARET MODULE TO TEST VARIANTVALUE!!!



/*
 * Validating JSON parse results:
 *      http://json.parser.online.fr/
 */











namespace {
    namespace Test1_7zArchive_ {
        namespace Private_ {
            void    ReadHardwired7zFile_ ()
            {
                // od sample_zip.7z -t x1 -w32 | sed 's/ /,0x/g'
                static
#if     qCompilerAndStdLib_largeconstexprarray_Buggy
                const
#else
                constexpr
#endif
                Byte    ksample_zip_7z_[] = {
                    0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c, 0x00, 0x03, 0xad, 0xd3, 0x3a, 0xb4, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xd9, 0x4d, 0x4c
                    , 0x00, 0x2d, 0x8c, 0x02, 0x26, 0x10, 0xd9, 0x9e, 0x69, 0xe9, 0xf2, 0xc8, 0xac, 0x29, 0x7c, 0xec, 0x89, 0x1d, 0x9e, 0x5d, 0xaf, 0x2f, 0x82, 0x5d, 0x47, 0x3b, 0x79, 0x78, 0x47, 0x44, 0xf8, 0x55
                    , 0xee, 0xb7, 0xc7, 0x3d, 0x8b, 0x6a, 0x2f, 0xc5, 0x53, 0x4d, 0x57, 0x4a, 0x1a, 0xb0, 0x16, 0x86, 0x3b, 0xb5, 0x33, 0xa9, 0x9c, 0xcc, 0x0a, 0xf0, 0xc6, 0xb8, 0xe7, 0x69, 0xf8, 0x6a, 0xc7, 0x90
                    , 0x20, 0xf1, 0xe0, 0x7c, 0x99, 0x2c, 0x89, 0xc6, 0x8c, 0x8b, 0x97, 0x02, 0x6e, 0x46, 0xc4, 0x4c, 0x53, 0x48, 0x09, 0x0e, 0x66, 0x88, 0x0e, 0xfc, 0x76, 0x15, 0xe6, 0xbe, 0x73, 0x62, 0x74, 0x8a
                    , 0x82, 0x4f, 0x0e, 0xdf, 0x7c, 0xde, 0x7c, 0x4d, 0xaf, 0x15, 0x71, 0xbc, 0x98, 0x74, 0x9c, 0x85, 0x38, 0xa6, 0x69, 0x0d, 0xe3, 0xa8, 0x98, 0xfb, 0x70, 0xf4, 0x57, 0x96, 0x6c, 0x99, 0x25, 0x44
                    , 0x2e, 0x58, 0x66, 0x89, 0x0c, 0x94, 0x66, 0x96, 0x74, 0x64, 0x12, 0x82, 0xc7, 0x32, 0xec, 0x30, 0xd2, 0xec, 0x4b, 0xfa, 0xc6, 0xbc, 0x86, 0x4e, 0xf3, 0x58, 0xa7, 0x62, 0xb6, 0xbe, 0xc6, 0x80
                    , 0x61, 0x1e, 0x38, 0xc5, 0xb2, 0xd0, 0xcb, 0x3f, 0x46, 0xff, 0xf2, 0x5a, 0xd8, 0xd2, 0xb2, 0xa8, 0xf5, 0xdc, 0xb6, 0xea, 0xfe, 0x89, 0xfc, 0xa8, 0x98, 0x14, 0x58, 0x72, 0xf1, 0x41, 0x77, 0x8b
                    , 0xec, 0xb8, 0x71, 0x4c, 0xf0, 0xd3, 0xed, 0x99, 0x70, 0x04, 0xbe, 0xcf, 0x3b, 0x46, 0x22, 0x60, 0x41, 0x65, 0xf9, 0x24, 0x60, 0x82, 0x6d, 0x2f, 0xc6, 0x6e, 0x18, 0xf1, 0x23, 0xd2, 0x3f, 0x02
                    , 0x89, 0x9e, 0x91, 0x98, 0x7a, 0x11, 0x5f, 0x9d, 0xf9, 0x08, 0xd6, 0xfd, 0xee, 0xaa, 0x57, 0xb9, 0x80, 0x7a, 0xde, 0x48, 0x56, 0xd8, 0x6a, 0x41, 0x04, 0x39, 0xc7, 0x27, 0xb0, 0xcc, 0xe4, 0xcc
                    , 0x0a, 0x75, 0x64, 0x09, 0xdf, 0x04, 0x86, 0x75, 0x36, 0x2e, 0xbe, 0x26, 0xac, 0x20, 0x20, 0x94, 0x37, 0xe1, 0xd8, 0x01, 0x7c, 0x70, 0x34, 0x6b, 0x6d, 0xdd, 0x3f, 0xeb, 0x29, 0x41, 0x85, 0xb3
                    , 0x5d, 0xfa, 0x17, 0xad, 0xba, 0xf1, 0x83, 0x8a, 0x59, 0x41, 0x94, 0xb4, 0xed, 0x45, 0xdd, 0x1d, 0xd8, 0x54, 0xc4, 0xd0, 0x2a, 0xf3, 0x06, 0x34, 0xec, 0xa8, 0x82, 0x00, 0xf4, 0x6b, 0xa0, 0x67
                    , 0xf6, 0x07, 0x39, 0xb7, 0x0a, 0xf6, 0x4a, 0xa6, 0x13, 0x71, 0x28, 0x5c, 0x50, 0x2a, 0xe9, 0x3f, 0xf3, 0x95, 0xa2, 0xdf, 0x86, 0x76, 0x5d, 0xf2, 0xdd, 0xea, 0x20, 0x5d, 0xed, 0x21, 0xcd, 0xb6
                    , 0xa5, 0x7d, 0x22, 0x92, 0x76, 0x5e, 0x06, 0x50, 0x94, 0x2c, 0xf5, 0xce, 0xff, 0x09, 0x68, 0xa0, 0xe5, 0xaf, 0x11, 0x19, 0x1b, 0x47, 0x6d, 0x35, 0x5a, 0xc6, 0x99, 0x5d, 0xbf, 0x98, 0xe9, 0x12
                    , 0xbe, 0x0a, 0xda, 0x6d, 0x5e, 0x32, 0x7d, 0xf2, 0x61, 0x83, 0x03, 0xaf, 0xa9, 0xe1, 0x71, 0x7b, 0x95, 0xa1, 0x34, 0xf8, 0xd6, 0xa7, 0x81, 0x5b, 0x7b, 0xe5, 0x9b, 0xda, 0x72, 0x01, 0xea, 0x93
                    , 0x47, 0x95, 0xe6, 0xf0, 0xd7, 0xf5, 0x03, 0xa5, 0x12, 0x81, 0xec, 0xc4, 0x73, 0x53, 0xdf, 0xaa, 0xc0, 0x44, 0x31, 0x83, 0x22, 0x91, 0x48, 0x23, 0x67, 0x74, 0x04, 0x93, 0xb3, 0x20, 0x6d, 0x66
                    , 0x7a, 0xd5, 0x33, 0xac, 0x99, 0x5e, 0xa4, 0x8c, 0xf4, 0x32, 0x38, 0x6f, 0xf2, 0x3a, 0x87, 0x4e, 0x93, 0x89, 0x31, 0xc0, 0xe0, 0xdd, 0xf4, 0x27, 0xec, 0x54, 0xf4, 0x4a, 0x91, 0x49, 0x28, 0x22
                    , 0x71, 0x7b, 0x99, 0xd7, 0xd8, 0x55, 0xcc, 0xd3, 0xbb, 0x1f, 0x03, 0x61, 0xcd, 0xf3, 0x25, 0xdf, 0xbe, 0x65, 0x1b, 0x01, 0xe4, 0x3d, 0x6d, 0xdf, 0x43, 0xac, 0xcf, 0xfc, 0x76, 0xb0, 0x73, 0x78
                    , 0x61, 0x7c, 0x4c, 0x3d, 0x91, 0xbb, 0x3e, 0x68, 0x93, 0xc5, 0x01, 0x61, 0x4b, 0xb8, 0x71, 0xd1, 0x74, 0x96, 0xf3, 0x86, 0x37, 0x5f, 0x82, 0x5d, 0x79, 0xd0, 0xb3, 0xd4, 0xf7, 0x12, 0x78, 0x6a
                    , 0x65, 0x7e, 0xe6, 0x2f, 0xd5, 0xb7, 0xa3, 0x2d, 0xd7, 0xb6, 0x81, 0xcc, 0xb5, 0xd4, 0x5a, 0xf2, 0x4e, 0x84, 0xcd, 0xd4, 0x78, 0x8e, 0x4e, 0xed, 0x0e, 0x99, 0x1b, 0x14, 0xd3, 0x03, 0x6c, 0x1c
                    , 0x88, 0xd0, 0x5c, 0xe8, 0xbc, 0x6c, 0x8c, 0x51, 0xfb, 0x5e, 0xcb, 0xa3, 0x87, 0xad, 0x30, 0x54, 0x3d, 0xed, 0x25, 0xa4, 0xa4, 0x9f, 0xfa, 0x4f, 0x4c, 0x83, 0x84, 0xb6, 0x73, 0xe6, 0x84, 0xa0
                    , 0x33, 0xd0, 0x72, 0x4b, 0xaf, 0xbb, 0x25, 0xed, 0x0d, 0xa3, 0xef, 0xdd, 0xc0, 0x0b, 0x2b, 0x64, 0x58, 0xb3, 0x80, 0x48, 0xaa, 0x95, 0x38, 0x3e, 0x25, 0x40, 0x8b, 0xd5, 0x61, 0xd0, 0xf0, 0x36
                    , 0xca, 0xde, 0x32, 0x55, 0xeb, 0x16, 0xc0, 0x3f, 0xb0, 0xb7, 0x9c, 0x34, 0x36, 0x75, 0x52, 0x47, 0x53, 0xb7, 0x63, 0x2d, 0x6b, 0x5b, 0x17, 0xd5, 0xe5, 0x3b, 0xa4, 0xd0, 0xf0, 0x92, 0x3c, 0xb6
                    , 0x48, 0x9a, 0xd1, 0x24, 0x0b, 0x5f, 0x96, 0x85, 0x9c, 0x60, 0x1b, 0x8f, 0x1e, 0x4d, 0xa0, 0x15, 0x8f, 0x81, 0x2d, 0xcc, 0x86, 0x5e, 0xba, 0xba, 0xba, 0x8f, 0xd7, 0xf7, 0x41, 0x38, 0x38, 0x20
                    , 0xe5, 0xa6, 0x4e, 0x73, 0xea, 0xc5, 0x72, 0xcf, 0x5e, 0x6d, 0xae, 0x5c, 0x29, 0x6e, 0x3a, 0x4c, 0xcb, 0x90, 0x2c, 0xc4, 0xa7, 0x93, 0xd4, 0x86, 0xe8, 0xc3, 0xb6, 0x0d, 0xc7, 0xaa, 0x40, 0x3f
                    , 0x00, 0xc3, 0xa5, 0x47, 0xd5, 0xc7, 0xb1, 0x87, 0xf4, 0x05, 0x3b, 0x6d, 0x89, 0x8f, 0x8b, 0x30, 0x05, 0x11, 0x56, 0x50, 0xab, 0x9a, 0x0e, 0x7e, 0xa8, 0xf9, 0x85, 0x2c, 0xb6, 0x27, 0x26, 0xe5
                    , 0xb4, 0xe9, 0xab, 0x96, 0x87, 0x72, 0xbd, 0x09, 0x93, 0x3d, 0x86, 0x65, 0x6c, 0x72, 0xc3, 0x87, 0xf2, 0x1b, 0xe8, 0x17, 0x59, 0x17, 0xe5, 0xad, 0x22, 0x9d, 0x6e, 0x02, 0x7c, 0x70, 0xe9, 0xf2
                    , 0x00, 0xc5, 0xac, 0xd7, 0x5d, 0x92, 0x8e, 0x15, 0x90, 0xe0, 0x29, 0xb5, 0xfd, 0x4a, 0x92, 0x2f, 0xfc, 0xa7, 0xdc, 0x55, 0xff, 0x60, 0xab, 0xf3, 0x8a, 0x61, 0x98, 0x80, 0x68, 0x7b, 0xfc, 0xb0
                    , 0x08, 0x07, 0x12, 0xbb, 0x32, 0x3f, 0xf7, 0x8d, 0x7a, 0xeb, 0x2b, 0x3b, 0x79, 0xc8, 0xe9, 0x14, 0xe8, 0xc5, 0x7e, 0x83, 0xf6, 0xb9, 0x95, 0xfa, 0xad, 0x3c, 0x9d, 0xf5, 0xec, 0xe8, 0xd9, 0x13
                    , 0x8f, 0x32, 0xc2, 0xbe, 0x4c, 0x2e, 0x28, 0x5d, 0x4f, 0x1d, 0xe2, 0xe7, 0xa0, 0xda, 0xf8, 0x96, 0xb0, 0x2f, 0x15, 0xc1, 0xc8, 0xc0, 0x51, 0xa1, 0xe1, 0xfb, 0x42, 0x05, 0xee, 0xa5, 0x66, 0x9d
                    , 0x38, 0x87, 0x61, 0x72, 0xbc, 0x69, 0x36, 0xa3, 0x89, 0xbc, 0x43, 0x1b, 0x14, 0x64, 0xe9, 0xad, 0x79, 0xc8, 0x62, 0xdf, 0x3f, 0xc4, 0xf8, 0x52, 0xfc, 0x5c, 0xae, 0xa1, 0x1a, 0x95, 0xd4, 0x9f
                    , 0xe8, 0x28, 0x6d, 0xc2, 0xbe, 0x9a, 0xc9, 0xa1, 0x20, 0x0e, 0x67, 0x6a, 0x90, 0x53, 0x49, 0x2f, 0xfd, 0x12, 0x69, 0xf8, 0xac, 0xf5, 0x40, 0x19, 0x53, 0x1f, 0x7f, 0x74, 0x89, 0xf5, 0x44, 0xa3
                    , 0x36, 0x4f, 0x19, 0x01, 0x9d, 0x10, 0xaa, 0x81, 0xc2, 0x67, 0x6e, 0x50, 0xab, 0xe2, 0xfb, 0x7a, 0x82, 0xe8, 0x0d, 0x4f, 0xd9, 0x65, 0x37, 0x88, 0x83, 0xa9, 0x83, 0xe3, 0x5c, 0x89, 0x38, 0xfe
                    , 0xf5, 0xac, 0x5a, 0x6f, 0x22, 0x3a, 0xcf, 0x6c, 0x6d, 0x48, 0xd2, 0x1b, 0xf8, 0x18, 0x2c, 0xf0, 0xbb, 0xc3, 0xcb, 0x8b, 0x71, 0xca, 0xbb, 0xa5, 0xdc, 0x6e, 0x14, 0x45, 0x75, 0x1b, 0x19, 0x60
                    , 0xdc, 0x5b, 0xc7, 0x1c, 0x96, 0xba, 0x80, 0x63, 0x6a, 0x55, 0x6d, 0x07, 0xe2, 0xf0, 0x7a, 0x2c, 0x6b, 0xa8, 0xc2, 0x7f, 0xcc, 0xdd, 0x26, 0xb4, 0x20, 0x79, 0x83, 0x74, 0x55, 0x90, 0xbe, 0x8f
                    , 0x9a, 0x4f, 0xd4, 0x9f, 0x65, 0xd4, 0x20, 0x34, 0x1f, 0x5d, 0x2b, 0xc5, 0x18, 0x69, 0xa8, 0xbc, 0x69, 0xbe, 0xa5, 0xb0, 0xde, 0x27, 0xcf, 0x1f, 0x99, 0x75, 0x6c, 0x66, 0xd3, 0x84, 0x89, 0x27
                    , 0x79, 0xe7, 0x22, 0x56, 0xe8, 0x4f, 0x04, 0x2f, 0x4c, 0xf7, 0xde, 0x43, 0x1b, 0x75, 0xe0, 0xbb, 0x86, 0x1f, 0xa0, 0x01, 0xc1, 0x2c, 0x9b, 0x4d, 0xf6, 0x92, 0xdb, 0x96, 0x1e, 0x9c, 0x04, 0x32
                    , 0x62, 0xd6, 0xb2, 0x47, 0xdd, 0xbb, 0x19, 0xde, 0x78, 0x72, 0x00, 0x62, 0x3f, 0xa4, 0x27, 0xf8, 0x78, 0x2b, 0x58, 0x63, 0xd2, 0x93, 0x1c, 0x1d, 0xd1, 0x98, 0x89, 0x4a, 0x69, 0x43, 0x5a, 0x5e
                    , 0x05, 0x93, 0x75, 0x6f, 0x2c, 0x70, 0xb7, 0x05, 0x04, 0xf5, 0xb9, 0x3b, 0xf9, 0xab, 0x8c, 0x3d, 0xc6, 0xbe, 0x06, 0x54, 0xe9, 0x06, 0xb1, 0x5f, 0xdd, 0x31, 0xec, 0xd8, 0x67, 0x6f, 0x38, 0xf5
                    , 0x84, 0xf5, 0x75, 0xf3, 0xa1, 0x58, 0xfa, 0x22, 0x6b, 0x4c, 0xf4, 0xc3, 0xe5, 0xea, 0x25, 0xf6, 0x9f, 0x4b, 0x82, 0x62, 0x50, 0x27, 0x02, 0x8a, 0xf4, 0x00, 0xc4, 0xbc, 0x23, 0x5c, 0x0f, 0x03
                    , 0xed, 0xa1, 0x37, 0x94, 0xcd, 0xcd, 0x33, 0x7a, 0xf8, 0x93, 0x81, 0x21, 0xac, 0x53, 0x35, 0xba, 0x27, 0x0e, 0x11, 0xb6, 0x9d, 0xba, 0x0e, 0x8b, 0x14, 0x6a, 0x71, 0xfc, 0x22, 0x20, 0x5a, 0xe4
                    , 0x3e, 0xbc, 0xf3, 0xd5, 0x60, 0xa2, 0x52, 0xbc, 0x4d, 0xab, 0x06, 0x32, 0x32, 0xb4, 0xd7, 0x41, 0xfa, 0x4f, 0x7d, 0x05, 0xb5, 0x78, 0xa8, 0x3d, 0xaf, 0x9b, 0xef, 0x5e, 0x02, 0xb5, 0x56, 0x20
                    , 0xe0, 0x08, 0xc0, 0x33, 0x49, 0xa8, 0x0d, 0xc0, 0x41, 0x2f, 0x1c, 0x7a, 0xa8, 0x21, 0xbb, 0x3b, 0x76, 0x8b, 0x85, 0x8f, 0x3d, 0x2c, 0x9c, 0x18, 0x84, 0xcd, 0x63, 0x4f, 0x7f, 0xde, 0x31, 0xb1
                    , 0xe6, 0x50, 0xe0, 0xdb, 0xa9, 0xed, 0x89, 0xd4, 0xee, 0xa6, 0x3f, 0xd6, 0x08, 0x29, 0x45, 0xf7, 0x10, 0xbd, 0x4c, 0x04, 0x22, 0xcc, 0x1d, 0x68, 0x54, 0x98, 0xf2, 0x9d, 0x7d, 0x03, 0xdc, 0xff
                    , 0x80, 0xd3, 0x93, 0x48, 0xbe, 0x72, 0x97, 0x4f, 0x60, 0xa8, 0xe4, 0x11, 0x33, 0xfb, 0xf8, 0x15, 0x21, 0x1e, 0xb0, 0x22, 0x7c, 0x20, 0xd2, 0x6e, 0x01, 0xaf, 0x90, 0xd9, 0x47, 0xa9, 0xb5, 0xce
                    , 0xb1, 0xdf, 0xd8, 0xd2, 0xaf, 0x62, 0x80, 0x22, 0x58, 0x9a, 0x39, 0x6c, 0xbb, 0x21, 0x56, 0xf1, 0x1b, 0x1c, 0xba, 0x13, 0x8e, 0x8d, 0xea, 0xc2, 0x5b, 0x69, 0x66, 0xe7, 0x95, 0x75, 0x57, 0x1e
                    , 0xf4, 0x31, 0x97, 0xd5, 0x29, 0x19, 0x5c, 0xf5, 0x98, 0x09, 0x86, 0x40, 0xbf, 0xdd, 0xb6, 0x4d, 0xde, 0x98, 0xc1, 0xa3, 0x92, 0x92, 0xc0, 0x5d, 0xd5, 0x30, 0xc9, 0xab, 0x80, 0x09, 0x7f, 0x14
                    , 0x05, 0x9d, 0x33, 0x11, 0x53, 0xfc, 0xba, 0x9d, 0xbe, 0x1d, 0x33, 0xda, 0xa3, 0x98, 0x6b, 0x74, 0x95, 0x88, 0xba, 0x80, 0x57, 0xc0, 0x07, 0xd9, 0x08, 0x3f, 0x3a, 0x39, 0x5f, 0xb8, 0xe9, 0x61
                    , 0x9a, 0xdd, 0x59, 0xb2, 0x8e, 0xa1, 0xeb, 0x4b, 0x7c, 0xd5, 0x0e, 0x07, 0xed, 0x24, 0x41, 0x34, 0x03, 0x23, 0x75, 0x8e, 0x7e, 0xb7, 0xdb, 0x95, 0xd2, 0xd7, 0x54, 0x56, 0x34, 0xc9, 0x5d, 0xf8
                    , 0xdf, 0xc1, 0x94, 0xaa, 0x59, 0xcc, 0x60, 0xfa, 0xe8, 0x08, 0x2a, 0x1a, 0xdc, 0x03, 0xab, 0xf7, 0xf3, 0x1b, 0xfa, 0x3a, 0xf8, 0x8e, 0x02, 0x8a, 0x2e, 0x2b, 0x50, 0x4b, 0x7d, 0x92, 0x98, 0x81
                    , 0x35, 0x75, 0xba, 0xa3, 0xd5, 0xe9, 0x70, 0x27, 0xce, 0xc2, 0xe9, 0xe8, 0x96, 0xff, 0x94, 0x7c, 0x53, 0xe0, 0x38, 0x0b, 0xcc, 0x8d, 0xfe, 0xce, 0x3a, 0x94, 0x82, 0xc2, 0xca, 0x78, 0xa7, 0x3e
                    , 0x62, 0x32, 0x9b, 0x61, 0xb0, 0x90, 0x1e, 0x09, 0xa0, 0x75, 0x04, 0x43, 0x3a, 0x70, 0x65, 0x04, 0x0c, 0x9f, 0x22, 0xac, 0x0a, 0x7c, 0xe1, 0x62, 0x39, 0x13, 0xab, 0xe4, 0xfc, 0x1d, 0x1b, 0xcf
                    , 0x8f, 0x10, 0x31, 0x00, 0x75, 0xfd, 0x6a, 0x60, 0x8d, 0x67, 0x9d, 0x58, 0x31, 0x1a, 0x74, 0x37, 0x98, 0x19, 0x46, 0x47, 0xb9, 0xa8, 0x8f, 0xbd, 0x39, 0x0e, 0xbd, 0xda, 0x86, 0xbc, 0x97, 0xc4
                    , 0x68, 0x9c, 0xca, 0x82, 0x2c, 0x74, 0xd7, 0x78, 0xa4, 0x69, 0xd1, 0x34, 0x8f, 0x57, 0x43, 0xb9, 0x47, 0x02, 0xe5, 0x29, 0x58, 0xda, 0x51, 0x0c, 0x13, 0x16, 0x24, 0x24, 0x79, 0xbe, 0xb4, 0x6e
                    , 0xf5, 0x5d, 0xc7, 0xd7, 0xe9, 0xab, 0x1a, 0x13, 0x19, 0x97, 0x06, 0x97, 0x8b, 0xf9, 0xd0, 0x37, 0xcb, 0x17, 0x44, 0x85, 0xa9, 0x18, 0x4d, 0x85, 0x76, 0xee, 0x9b, 0xd6, 0x5c, 0x8e, 0x24, 0x9f
                    , 0x5c, 0xf3, 0xc2, 0xed, 0x6e, 0xc5, 0x18, 0xfa, 0xc1, 0x14, 0x5a, 0xf0, 0xe1, 0xc0, 0xdb, 0x26, 0x1f, 0xe4, 0xd9, 0x7d, 0x79, 0xcc, 0xb3, 0x02, 0x72, 0x99, 0x60, 0xa4, 0xec, 0x2f, 0x78, 0xb8
                    , 0xa0, 0xd1, 0xf6, 0x90, 0x03, 0x08, 0x09, 0xde, 0xc4, 0xf9, 0xa6, 0x6f, 0x7f, 0x40, 0x62, 0xa7, 0x40, 0x10, 0xce, 0xd6, 0x90, 0x51, 0x2e, 0x8c, 0x81, 0x57, 0xc6, 0x62, 0x04, 0x87, 0x83, 0xb7
                    , 0x01, 0x11, 0xff, 0xa8, 0xc1, 0xc7, 0x8a, 0xcd, 0x2a, 0xed, 0x96, 0x57, 0xd5, 0x08, 0xd9, 0xbf, 0xd3, 0x66, 0x37, 0xb0, 0x3f, 0x2e, 0x66, 0x1f, 0x4d, 0xc9, 0xdd, 0x95, 0x2d, 0x37, 0x8e, 0xc0
                    , 0xf2, 0xdc, 0xba, 0x66, 0xea, 0x70, 0x3a, 0x71, 0x21, 0x49, 0x00, 0x9c, 0x0c, 0xcd, 0xb3, 0x34, 0x29, 0x11, 0xb6, 0x90, 0xd2, 0x37, 0x0b, 0xfe, 0x5f, 0x5d, 0x5e, 0x5f, 0x7a, 0x7b, 0x21, 0xb9
                    , 0x28, 0x87, 0x0b, 0xd8, 0xc2, 0x41, 0xed, 0x93, 0x64, 0x32, 0xde, 0xbd, 0x6a, 0xaa, 0x1e, 0x35, 0x6a, 0x95, 0xde, 0x53, 0x4d, 0xb9, 0x05, 0xec, 0xba, 0xeb, 0xb1, 0x41, 0x4c, 0xb4, 0xa5, 0xa5
                    , 0x87, 0x9b, 0x54, 0x53, 0xc1, 0x2c, 0x01, 0x86, 0x11, 0x94, 0x5f, 0xfe, 0xc1, 0x45, 0x8d, 0x9d, 0x3d, 0x9d, 0x13, 0x00, 0x00, 0x81, 0x33, 0x07, 0xae, 0x0f, 0xd5, 0x7b, 0xab, 0xa9, 0xd7, 0x24
                    , 0xd3, 0xfe, 0xb3, 0x7e, 0x2f, 0x89, 0x92, 0xbe, 0xbe, 0x3e, 0x27, 0x92, 0xcd, 0x04, 0x52, 0x61, 0x82, 0x2a, 0x95, 0x6e, 0xe4, 0x7b, 0xdc, 0x16, 0x19, 0x3a, 0x88, 0xd2, 0xe7, 0xdf, 0xf7, 0xae
                    , 0x3a, 0xd5, 0x66, 0x91, 0x94, 0x5b, 0x54, 0xa7, 0x9e, 0xf2, 0xd3, 0x51, 0x25, 0x12, 0xf1, 0xd2, 0x1d, 0x8b, 0xd9, 0xf3, 0xf3, 0x75, 0xc4, 0xce, 0xde, 0x09, 0xe4, 0xd5, 0x23, 0x4e, 0x1e, 0xf4
                    , 0x99, 0x60, 0x3d, 0x5d, 0x82, 0xb0, 0x31, 0x2a, 0xbf, 0xf6, 0x10, 0xea, 0xb6, 0xae, 0x18, 0xb8, 0xfb, 0x50, 0xc7, 0x96, 0x28, 0x7e, 0x5f, 0x6f, 0xd5, 0xf5, 0x6d, 0x6b, 0xfe, 0x24, 0x82, 0xe2
                    , 0xbd, 0xee, 0x55, 0xd0, 0x27, 0xc8, 0x28, 0x02, 0x98, 0x41, 0x8d, 0x04, 0x2b, 0x55, 0xc0, 0x57, 0x4c, 0x87, 0x6f, 0x36, 0x84, 0xfb, 0x71, 0x10, 0xae, 0xed, 0x71, 0x06, 0x81, 0x03, 0xcc, 0x90
                    , 0x26, 0xa4, 0xd3, 0x3a, 0x30, 0xe8, 0x60, 0xbf, 0x3c, 0xa7, 0xe1, 0x0d, 0x42, 0x0a, 0xf4, 0x21, 0xbb, 0xf4, 0x82, 0xe5, 0x83, 0x5d, 0x85, 0xa6, 0xe3, 0x2f, 0x9c, 0xfe, 0x36, 0xc6, 0xcb, 0x67
                    , 0x27, 0xcf, 0x86, 0x2d, 0x37, 0xe0, 0x27, 0x20, 0x16, 0x34, 0x6a, 0x5e, 0xd5, 0x56, 0x27, 0x4a, 0x18, 0x95, 0xb8, 0xd5, 0x0c, 0xdc, 0xf1, 0x2e, 0x53, 0xf1, 0x48, 0x6e, 0xdb, 0xe9, 0x62, 0x2e
                    , 0x67, 0x53, 0x1f, 0xca, 0x65, 0x93, 0xab, 0x99, 0xca, 0x00, 0x17, 0x06, 0x87, 0x53, 0x01, 0x09, 0x80, 0xd7, 0x00, 0x07, 0x0b, 0x01, 0x00, 0x01, 0x23, 0x03, 0x01, 0x01, 0x05, 0x5d, 0x00, 0x10
                    , 0x00, 0x00, 0x0c, 0x81, 0x9b, 0x0a, 0x01, 0xa0, 0xee, 0xa0, 0x06, 0x00, 0x00
                };
                Assert (sizeof (ksample_zip_7z_) == 2157);
#if     qHasFeature_LZMA
                _7z::ArchiveReader  reader (Streams::ExternallyOwnedMemoryInputStream<Byte> (begin (ksample_zip_7z_), end (ksample_zip_7z_)));
                VerifyTestResult ((reader.GetContainedFiles () == Set<String> {L"sample_zip/BlockAllocation-Valgrind.supp", L"sample_zip/Common-Valgrind.supp", L"sample_zip/TODO.txt", L"sample_zip/Tests-Description.txt"}));

                {
                    using   Streams::InputStream;
                    using   Streams::TextReader;
                    using   Memory::Byte;
                    VerifyTestResult (reader.GetData (L"sample_zip/TODO.txt").size () == 243);
                    VerifyTestResult (reader.GetData (L"sample_zip/BlockAllocation-Valgrind.supp").size () == 4296);
                    VerifyTestResult (reader.GetData (L"sample_zip/Common-Valgrind.supp").size () == 1661);
                    VerifyTestResult (reader.GetData (L"sample_zip/Tests-Description.txt").size () == 1934);
                    VerifyTestResult (TextReader (reader.GetData (L"sample_zip/TODO.txt").As<InputStream<Byte>> ()).ReadAll ().Contains (L"Once any of the ThreadSafetyBuiltinObject tests work - with the locking stuff - add more concrete tyeps"));
                    VerifyTestResult (TextReader (reader.GetData (L"sample_zip/Tests-Description.txt").As<InputStream<Byte>> ()).ReadAll ().Contains (L"[30]	Foundation::DataExchange::Other"));
                    try {
                        auto i = reader.GetData (L"file-not-found");
                        VerifyTestResult (false);
                    }
                    catch (...) {
                        // good
                    }
                }
#endif
            }
        }
        void    DoAll_ ()
        {
            Private_::ReadHardwired7zFile_ ();
        }
    }
}







namespace {
    namespace INI_ONLY_ {

        void    DoBasicReader1_ ()
        {
            stringstream    tmp;
            tmp << "NAME=\"Ubuntu\"" << endl;
            tmp << "VERSION=\"13.10, Saucy Salamander\"" << endl;
            tmp << "ID=ubuntu" << endl;
            tmp << "ID_LIKE=debian" << endl;
            tmp << "PRETTY_NAME=\"Ubuntu 13.10\"" << endl;
            tmp << "VERSION_ID=\"13.10\"" << endl;
            tmp << "HOME_URL=\"http://www.ubuntu.com/\"" << endl;
            tmp << "SUPPORT_URL=\"http://help.ubuntu.com/\"" << endl;
            tmp << "BUG_REPORT_URL=\"http://bugs.launchpad.net/ubuntu/\"" << endl;
            INI::Profile p = INI::Reader ().ReadProfile (tmp);
            VerifyTestResult (p.fNamedSections.empty ());
            VerifyTestResult (p.fUnnamedSection.fProperties.LookupValue (L"NAME") == L"Ubuntu");
            VerifyTestResult (p.fUnnamedSection.fProperties.LookupValue (L"SUPPORT_URL") == L"http://help.ubuntu.com/");
        }

        void    DoAll_ ()
        {
            DoBasicReader1_ ();
        }
    }
}



namespace   {
    namespace JSON_ONLY_ {
        /*
         *  This section is for using the direct - JSON-only APIs, and verifying the results look like good JSON
         */


        namespace Test_01_BasicWriterTests_ {
            void    CheckMatchesExpected_WRITER_ (const VariantValue& v, const string& expected)
            {
                Streams::MemoryStream<Byte>    out;
                DataExchange::JSON::Writer ().Write (v, out);
                string x = out.As<string> ();
                for (string::size_type i = 0; i < min (x.length (), expected.length ()); ++i) {
                    if (x[i] != expected[i]) {
                        VerifyTestResult (false);
                    }
                }
                VerifyTestResult (out.As<string> () == expected);
            }
            void    DoIt ()
            {
                {
                    VariantValue    v1 = L"hello world";
                    CheckMatchesExpected_WRITER_ (v1, "\"hello world\"\n");
                }
                {
                    VariantValue    v1 =    3;
                    CheckMatchesExpected_WRITER_ (v1, "3\n");
                }
                {
                    VariantValue    v1 =    4.7;
                    CheckMatchesExpected_WRITER_ (v1, "4.7\n");
                }
                {
                    VariantValue    v1 =    L"\"";
                    CheckMatchesExpected_WRITER_ (v1, "\"\\\"\"\n");
                }
                {
                    // array
                    vector<VariantValue>    v;
                    v.push_back (3);
                    v.push_back (7);
                    v.push_back (L"cookie");
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_WRITER_ (v1, "[\n    3,\n    7,\n    \"cookie\"\n]\n");
                }
                {
                    // object
                    map<wstring, VariantValue>   v;
                    v[L"Arg1"] = 32;
                    v[L"Arg2"] = L"Cookies";
                    v[L"Arg3"] = Containers::Sequence<VariantValue> ({ 19 });
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_WRITER_ (v1, "{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}\n");
                }
            }
        }


        namespace Test_02_BasicReaderTests_ {
            void    CheckMatchesExpected_READER_ (const string& v, const VariantValue& expected)
            {
                stringstream    tmp;
                tmp << v;
                VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                VerifyTestResult (v1 == expected);
            }

            void    DoIt ()
            {
                {
                    VariantValue    v1 = L"hello world";
                    CheckMatchesExpected_READER_ ("\"hello world\"", v1);
                }
                {
                    VariantValue    v1 =    3;
                    CheckMatchesExpected_READER_ ("3", v1);
                }
                {
                    VariantValue    v1 =    L"\uFDD0";
                    CheckMatchesExpected_READER_ ("\"\\uFDD0\"", v1);
                }
                {
                    VariantValue    v1 =    4.7;
                    CheckMatchesExpected_READER_ ("4.7", v1);
                }
                {
                    // array
                    vector<VariantValue>    v;
                    v.push_back (3);
                    v.push_back (7);
                    v.push_back (L"cookie");
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_READER_ ("[\n    3,\n    7,\n    \"cookie\"\n]", v1);
                }
                {
                    // object
                    map<wstring, VariantValue>   v;
                    v[L"Arg1"] = 32;
                    v[L"Arg2"] = L"Cookies";
                    v[L"Arg3"] = Containers::Sequence<VariantValue> ({ 19 });
                    VariantValue    v1 =    v;
                    CheckMatchesExpected_READER_ ("{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}", v1);
                }
                {
                    // Bug found in another JSON reader (sent me by Ryan - 2011-07-27)
                    const   string  kExample    =   "{\"nav_items\":[{\"main_link\":{\"href\":\"/about/index.html\",\"text\":\"Who We Are\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/company-management.html\",\"text\":\"Management\"}},{\"header\":{\"href\":\"/about/mission-statement.html\",\"text\":\"Mission\"}},{\"header\":{\"href\":\"/about/company-history.html\",\"text\":\" History\"}},{\"header\":{\"href\":\"/about/headquarters.html\",\"text\":\"Corporate Headquarters\"}},{\"header\":{\"href\":\"/about/diversity.html\",\"text\":\"Diversity\"}},{\"header\":{\"href\":\"/about/supplier-diversity.html\",\"text\":\"Supplier Diversity\"}}]}]},{\"main_link\":{\"href\":\"http://investor.compuware.com\",\"text\":\"Investor Relations\"}},{\"main_link\":{\"href\":\"/about/newsroom.html\",\"text\":\"News Room\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/analyst-reports\",\"text\":\"Analyst Reports\"}},{\"header\":{\"href\":\"/about/awards-recognition.html\",\"text\":\"Awards and Recognition\"}},{\"header\":{\"href\":\"/about/blogs.html\",\"text\":\"Blog Home\"}},{\"header\":{\"href\":\"/about/press-analyst-contacts.html\",\"text\":\"Contact Us\"}},{\"header\":{\"href\":\"/about/customers.html\",\"text\":\"Customers\"}},{\"header\":{\"href\":\"/about/press-mentions\",\"text\":\"Press Mentions\"}},{\"header\":{\"href\":\"/about/press-releases\",\"text\":\"Press Releases\"}},{\"header\":{\"href\":\"/about/press-resources.html\",\"text\":\"Press Resources\"}}]}]},{\"main_link\":{\"href\":\"#top\",\"text\":\"Sponsorships\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"/about/lemans-sponsorship.html\",\"text\":\"Le Mans\"}},{\"header\":{\"href\":\"/about/nhl-sponsorship.html\",\"text\":\"NHL\"}},{}]}]},{\"main_link\":{\"href\":\"/about/community-involvement.html\",\"text\":\"Community Involvement\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"http://communityclicks.compuware.com\",\"text\":\"Community Clicks Blog\"}},{\"header\":{\"href\":\"javascript:securenav('/forms/grant-eligibility-form.html')\",\"text\":\"Grant Eligibility Form\"}},{}]}]},{\"main_link\":{\"href\":\"/government/\",\"text\":\"Government\"}}]}";
                    stringstream    tmp;
                    tmp << kExample;
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (v1.GetType () == VariantValue::Type::eMap);
                }

            }
        }


        namespace Test_03_CheckCanReadFromSmallBadSrc_ {
            void    VerifyThisStringFailsToParse_ (const string& s)
            {
                stringstream    tmp;
                tmp << s;
                try {
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (false);   // should get exception
                }
                catch (const DataExchange::BadFormatException&) {
                    // GOOD
                }
                catch (...) {
                    VerifyTestResult (false);   // should get BadFormatException
                }
            }
            void    DoIt ()
            {
                VerifyThisStringFailsToParse_ ("n");
                VerifyThisStringFailsToParse_ ("'");
                VerifyThisStringFailsToParse_ ("\"");
                VerifyThisStringFailsToParse_ ("[");
                VerifyThisStringFailsToParse_ ("}");
                VerifyThisStringFailsToParse_ ("]");
            }
        }


        namespace Test_04_CheckStringQuoting_ {

            void    CheckRoundtrip_encode_decode_unchanged (const VariantValue& v)
            {
                string  encodedRep;
                {
                    Streams::MemoryStream<Byte>    out;
                    DataExchange::JSON::Writer ().Write (v, out);
                    encodedRep = out.As<string> ();
                }
                {
                    stringstream    tmp;
                    tmp << encodedRep;
                    VariantValue    vOut = DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (vOut == v);
                }
            }

            void    DoIt ()
            {
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"cookie"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"c:\\"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"'"));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\""));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\\u20a9")); //  ₩
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\u20a9"));  //  ₩
                CheckRoundtrip_encode_decode_unchanged (VariantValue (L"\"apple\""));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<long long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long int>::min ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long int>::max ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long long int>::min ()));
#if 1
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<unsigned long long int>::max ()));
#endif
            }
        }


        namespace Test_05_ParseRegressionTest_1_ {
            void    DoIt ()
            {
                {
                    const char  kJSONExample_[] =
                        "{"
                        "    \"Automated Backups\" : {"
                        "        \"From\" : {"
                        "            \"CurrentHRWildcard\" : true,"
                        "            \"PrintName\" : \"{Current HR}\""
                        "        },"
                        "        \"LastRanAt\" : {"
                        "            \"ID-ca22f72c-9ff5-4082-82d0-d9763c64ddd6\" : \"2013-03-03T13:53:05-05:00\""
                        "        },"
                        "        \"Operation\" : 0,"
                        "        \"Output\" : {"
                        "            \"AttachmentPolicy\" : 2,"
                        "            \"Format\" : \"application/x-healthframe-snapshotphr-3\","
                        "            \"MaxFiles\" : 0,"
                        "            \"NamePolicy\" : 1,"
                        "            \"Password\" : \"\""
                        "        },"
                        "        \"PolicyName\" : \"Automated Backups\","
                        "        \"Schedule\" : 2,"
                        "        \"To\" : {"
                        "            \"DefaultBackupDirectory\" : true,"
                        "            \"PrintName\" : \"{Default Backup Directory}\""
                        "        }"
                        "    }"
                        "}"
                        ;
                    VariantValue v = DataExchange::JSON::Reader ().Read (Streams::ExternallyOwnedMemoryInputStream<Byte> (reinterpret_cast<const Byte*> (std::begin (kJSONExample_)), reinterpret_cast<const Byte*> (std::begin (kJSONExample_)) + strlen (kJSONExample_)));
                    map<wstring, VariantValue>  mv  =   v.As<map<wstring, VariantValue>> ();
                    VerifyTestResult (mv[L"Automated Backups"].GetType () == VariantValue::Type::eMap);
                    map<wstring, VariantValue>  outputMap   =   v.As<map<wstring, VariantValue>> ()[L"Output"].As<map<wstring, VariantValue>> ();
                    outputMap[L"MaxFiles"] = 123456789;
                    mv[L"Output"] = outputMap;
                    v = mv;

                    string  jsonExampleWithUpdatedMaxFilesReference;
                    {
                        Streams::MemoryStream<Byte>    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        jsonExampleWithUpdatedMaxFilesReference = tmpStrm.As<string> ();
                    }
                    {
                        // Verify change of locale has no effect on results
                        locale  prevLocale  =   locale::global (locale ("C"));
                        Streams::MemoryStream<Byte>    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                        locale::global (prevLocale);
                    }
#if     !qCompilerAndStdLib_Locale_Buggy
                    {
                        // Verify change of locale has no effect on results
                        Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
                        Streams::MemoryStream<Byte>    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        VerifyTestResult (jsonExampleWithUpdatedMaxFilesReference == tmpStrm.As<string> ());
                    }
#endif
                }
            }
        }


        namespace Test_06_ParseRegressionTest_2_ {
            void    DoIt ()
            {
                auto f = [] () {
                    map<wstring, VariantValue>  mv;
                    mv[L"MaxFiles"] = VariantValue (405);
                    VariantValue    v   =    VariantValue (mv);

                    string  encoded;
                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        encoded = tmpStrm.str ();
                    }
                    stringstream    tnmStrStrm (encoded);
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tnmStrStrm);
                    VerifyTestResult (v1 == v);
                };
                f ();
#if     !qCompilerAndStdLib_Locale_Buggy
                {
                    Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
                    f ();
                }
#endif
            }
        }


        namespace Test_05_ParseRegressionTest_3_ {
            void    DoIt ()
            {
                {
                    const char  kJSONExample_[] =
                        "{"
                        "        \"T1\" : \"\","
                        "        \"T2\" : null,"
                        "        \"T3\" : {"
                        "            \"DefaultBackupDirectory\" : true,"
                        "            \"PrintName\" : \"{Default Backup Directory}\""
                        "        }"
                        "    }"
                        "}"
                        ;
                    VariantValue v = DataExchange::JSON::Reader ().Read (Streams::ExternallyOwnedMemoryInputStream<Byte> (reinterpret_cast<const Byte*> (std::begin (kJSONExample_)), reinterpret_cast<const Byte*> (std::begin (kJSONExample_)) + strlen (kJSONExample_)));
                    Mapping<String, VariantValue>  mv  =   v.As<Mapping<String, VariantValue>> ();
                    VerifyTestResult (mv[L"T1"].GetType () == VariantValue::Type::eString);
                    VerifyTestResult (mv[L"T1"] == String ());
                    VerifyTestResult (mv[L"T2"].GetType () == VariantValue::Type::eNull);
                    VerifyTestResult (mv[L"T3"].GetType () == VariantValue::Type::eMap);
                }
            }
        }



        namespace Test_07_ParserTestReadWriteBasictypes_ {
            void    DoIt ()
            {
                using   namespace   Time;
                auto f = [] (VariantValue v) {
                    string  encoded;
                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (v, tmpStrm);
                        encoded = tmpStrm.str ();
                    }
                    stringstream    tnmStrStrm (encoded);
                    VariantValue    v1  =   DataExchange::JSON::Reader ().Read (tnmStrStrm);
                    // JSON reader comes back with strings - because date/datetime are not native types
                    if (v.GetType () == VariantValue::Type::eDate and v1.GetType () == VariantValue::Type::eString) {
                        v1 = VariantValue (v1.As<Time::Date> ());
                    }
                    if (v.GetType () == VariantValue::Type::eDateTime and v1.GetType () == VariantValue::Type::eString) {
                        v1 = VariantValue (v1.As<Time::DateTime> ());
                    }
                    if (v.GetType () == VariantValue::Type::eFloat) {
                        VerifyTestResult (Math::NearlyEquals (v1.As<double> (), v.As<double> (), 0.001));
                    }
                    else {
                        VerifyTestResult (v1 == v);
                    }
                };
                auto    doAll = [f] () {
                    f (VariantValue (405));
                    f (VariantValue (4405));
                    f (VariantValue (44905));
                    f (VariantValue (405.1));
                    f (VariantValue (4405.2));
                    f (VariantValue (44905.3));
                    f (VariantValue (L"'"));
                    f (VariantValue (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12)));
                    f (VariantValue (DateTime (Date (Year (1933), MonthOfYear::eFebruary, DayOfMonth::e12), TimeOfDay (432))));

                    {
                        stringstream    tmpStrm;
                        DataExchange::JSON::Writer ().Write (VariantValue (44905.3), tmpStrm);
                        string tmp = tmpStrm.str ();
                        VerifyTestResult (tmp.find (",") == string::npos);
                    }
                };
                {
                    doAll ();
#if     !qCompilerAndStdLib_Locale_Buggy
                    Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
                    doAll ();
#endif
                }
            }
        }


        namespace Test_08_ReadEmptyStreamShouldNotFail_ {
            void    DoIt ()
            {
                try {
                    VariantValue    vOut    =   DataExchange::JSON::Reader ().Read (Streams::MemoryStream<Byte> (nullptr, nullptr));
                    VerifyTestResult (false);
                }
                catch (const DataExchange::BadFormatException&) {
                    // Good - this should fail
                }
            }
        }

        namespace Test_09_ReadWriteNANShouldNotFail_ {

            void    CheckRoundtrip_encode_decode_unchanged (const VariantValue& v)
            {
                string  encodedRep;
                {
                    Streams::MemoryStream<Byte>    out;
                    DataExchange::JSON::Writer ().Write (v, out);
                    encodedRep = out.As<string> ();
                }
                {
                    stringstream    tmp;
                    tmp << encodedRep;
                    VariantValue    vOut = DataExchange::JSON::Reader ().Read (tmp);
                    VerifyTestResult (vOut == v);
                }
            }

            void    DoIt ()
            {
                CheckRoundtrip_encode_decode_unchanged (VariantValue (Math::nan<double> ()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (-numeric_limits<double>::infinity()));
                CheckRoundtrip_encode_decode_unchanged (VariantValue (numeric_limits<double>::infinity()));
            }
        }




        void    DoAll_ ()
        {
            Test_01_BasicWriterTests_::DoIt ();
            Test_02_BasicReaderTests_::DoIt ();
            Test_03_CheckCanReadFromSmallBadSrc_::DoIt ();
            Test_04_CheckStringQuoting_::DoIt ();
            Test_05_ParseRegressionTest_1_::DoIt ();
            Test_06_ParseRegressionTest_2_::DoIt ();
            Test_05_ParseRegressionTest_3_::DoIt ();
            Test_07_ParserTestReadWriteBasictypes_::DoIt ();
            Test_08_ReadEmptyStreamShouldNotFail_::DoIt ();
            Test_09_ReadWriteNANShouldNotFail_::DoIt ();
        }
    }
}







namespace   {
    namespace XML_ONLY_ {
        /*
         *  This section is for using the direct - XML-only APIs, and verifying the results look like good XML
         */



        namespace Test_01_FirstPlayingAroundTest_ {
            void    DoIt ()
            {
                {
                    DataExchange::XML::Writer w;
                    VariantValue    v   =   VariantValue (44905.3);
                    Streams::MemoryStream<Byte>    out;
                    w.Write (v, out);
                    string x = out.As<string> ();
                    int breakhere = 1;
                }
                {
                    DataExchange::XML::Writer w;
                    map<wstring, VariantValue>  mv;
                    mv[L"MaxFiles"] = VariantValue (405);
                    VariantValue    v   =    VariantValue (mv);
                    Streams::MemoryStream<Byte>    out;
                    w.Write (v, out);
                    string x = out.As<string> ();
                    int breakhere = 1;
                }
            }
        }



        void    DoAll_ ()
        {
            Test_01_FirstPlayingAroundTest_::DoIt ();
        }
    }
}










namespace   {
    namespace GENERIC_SERIALIZE_DESERIALIZE_ {
        /*
         *  This section is for generic tests - that will be applied to BOTH the default JSON reader/writers
         *  and the default XML readers/writers (with no mods)
         */

        void    DoAll_ ()
        {



        }
    }
}



namespace {
    /// @TODO MOVE ELSEWHERE
    template <typename T>
    void Test3_VariantValue_Helper_MinMax_ ()
    {
        {
            VariantValue v = numeric_limits<T>::lowest ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::min ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
        {
            VariantValue v = numeric_limits<T>::max ();
            VariantValue vs = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            VerifyTestResult (v == vrt);
        }
    }
    void    Test3_VariantValue ()
    {
        using Characters::String;
        {
            VariantValue v;
            VerifyTestResult (v.empty ());
            v = String (L"hi");
            VerifyTestResult (v == L"hi");
        }
        Test3_VariantValue_Helper_MinMax_<int> ();
        Test3_VariantValue_Helper_MinMax_<unsigned int> ();
        Test3_VariantValue_Helper_MinMax_<long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long> ();
        Test3_VariantValue_Helper_MinMax_<long long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long long> ();
        Test3_VariantValue_Helper_MinMax_<float> ();
        Test3_VariantValue_Helper_MinMax_<double> ();
        Test3_VariantValue_Helper_MinMax_<long double> ();
    }

}



namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_7zArchive_::DoAll_ ();
        INI_ONLY_::DoAll_ ();
        JSON_ONLY_::DoAll_();
        XML_ONLY_::DoAll_();
        GENERIC_SERIALIZE_DESERIALIZE_::DoAll_();

        Test3_VariantValue ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

