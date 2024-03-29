/**
  *  \file test/afl/io/msexpandtransformtest.cpp
  *  \brief Test for afl::io::MsExpandTransform
  */

#include "afl/io/msexpandtransform.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    /*
     *  Almost Random Data (a VGAP registration key)
     */
    const uint8_t ORIGINAL_RANDOM[] = {
        0x2a, 0x5d, 0xd8, 0x46, 0x28, 0xa8, 0xe5, 0x04,  0x5b, 0x42, 0x56, 0x6c, 0x57, 0xfa, 0xc7, 0x49,
        0x70, 0x4c, 0xa4, 0x57, 0x3a, 0xa4, 0x0e, 0x0a,  0x6e, 0x8e, 0x31, 0x4b, 0x2c, 0x78, 0x9e, 0x11,
        0x3f, 0xd6, 0x90, 0x25, 0x07, 0xa8, 0x71, 0x0b,  0x91, 0xc5, 0x98, 0x46, 0xbf, 0x2a, 0x84, 0x3e,
        0xe2, 0xd2, 0xba, 0x4d, 0xb7, 0x8a, 0x51, 0x6f,  0x11, 0xa6, 0x2f, 0x5d, 0xec, 0x01, 0x6c, 0x0b,
        0x92, 0x84, 0x99, 0x5b, 0x36, 0xf6, 0xfc, 0x28,  0x76, 0x86, 0x91, 0x3e, 0x40, 0xa3, 0x92, 0x17,
        0xf1, 0xfb, 0xab, 0x55, 0xf5, 0x1b, 0x2f, 0x74,  0x45, 0x0c, 0x1c, 0x34, 0x00, 0xc4, 0x4e, 0x23,
        0x67, 0x24, 0x00, 0x14, 0x01, 0xa7, 0xd3, 0x4c,  0xfb, 0x08, 0xc4, 0x4a, 0x32, 0x17, 0x01, 0x2d,
        0x1c, 0x6b, 0x17, 0x1a, 0xf1, 0xaf, 0xff, 0x65,  0xf2, 0xf4, 0xe6, 0x11, 0x3b, 0xe0, 0x3e, 0x46,
        0xc9, 0x0d, 0xa5, 0x3a, 0x31, 0x69, 0xdf, 0x42,  0x41, 0x83, 0xaa, 0x54, 0xae, 0x26, 0x98, 0x0f,
        0x8d, 0x82, 0xda, 0x35, 0x63, 0xda, 0x93, 0x2c,  0x0a, 0x00, 0x43, 0x49, 0x23, 0x03, 0xec, 0x2e,
        0x49, 0xff, 0x7f, 0x75, 0x92, 0x43, 0x70, 0x66,  0xc3, 0x1c, 0x4e, 0x74, 0xcb, 0x18, 0x05, 0x2a,
        0x8a, 0x2a, 0xb6, 0x1b, 0xf7, 0x24, 0x23, 0x77,  0x43, 0xfa, 0xad, 0x13, 0x76, 0x21, 0x86, 0x18,
        0xd7, 0x59, 0x99, 0x4e, 0xbc, 0xda, 0x12, 0x01,  0xd0, 0x30, 0x7e, 0x3d, 0xfb, 0x96, 0x01, 0x00,
        0x04, 0x0a, 0xa9, 0x2e, 0xad, 0x5b, 0xd6, 0x26,  0xd8, 0x9a, 0xf7, 0x20, 0xa5, 0xba, 0xf4, 0x0b,
        0xaa, 0x5c, 0x03, 0x5e, 0x54, 0xa6, 0x4b, 0x51,  0xc5, 0x08, 0xfa, 0x60, 0x43, 0x1b, 0xe6, 0x6d,
        0x5f, 0xcc, 0xc5, 0x20, 0x84, 0xbb, 0xce, 0x49,  0xc7, 0xc5, 0xba, 0x07, 0xff, 0x17, 0x6f, 0x1b,
        0xa6, 0xec, 0x7f, 0x31, 0xbf, 0x40, 0x4a, 0x1e,  0x22, 0x25, 0xe6, 0x1c, 0xae, 0x4f, 0x7e, 0x53,
        0x51, 0x5d, 0x2c, 0x1d, 0x2c, 0x42, 0x14, 0x23,  0xcb, 0xc7, 0xb5, 0x5d, 0x85, 0x0d, 0xe4, 0x26,
        0xf2, 0xcc, 0xda, 0x64, 0x74, 0x17, 0x36, 0x24,  0xcc, 0x72, 0x87, 0x43, 0x35, 0x2d, 0x65, 0x60,
        0x60, 0x2b, 0x41, 0x15, 0x44, 0xe3, 0x11, 0x1a,  0x2f, 0x05, 0x40, 0x28, 0x19, 0x39, 0x92, 0x17,
        0x0d, 0xe8, 0x5e, 0x12, 0xb1, 0x35, 0x73, 0x0d,  0xce, 0x15, 0x88, 0x34, 0x2b, 0x0f, 0x42, 0x5d,
        0x61, 0xe5, 0x75, 0x73, 0x2a, 0xce, 0x6b, 0x19,  0x64, 0xad, 0x35, 0x5a, 0xa7, 0x41, 0xa0, 0x1b,
        0x6b, 0x03, 0xc2, 0x50, 0xb8, 0xfe, 0xcb, 0x13,  0x3c, 0x20, 0x82, 0x6c, 0x87, 0x79, 0x70, 0x75,
        0xeb, 0x41, 0xef, 0x02, 0x78, 0x2b, 0xc8, 0x41,  0x6a, 0x07, 0x0c, 0x56, 0xfa, 0x97, 0x84, 0x6a,
        0x7d, 0x8d, 0x08, 0x3e, 0x4c, 0x70, 0xa8, 0x4d,  0xa9, 0x59, 0xa5, 0x6c, 0x02, 0xdd, 0x06, 0x0d,
        0xa8, 0xaf, 0xd1, 0x5b, 0x87, 0x5f, 0x06, 0x37,  0x5d, 0xaa, 0x2b, 0x06, 0xdd, 0x3a, 0x0c, 0x1b,
        0xf7, 0xd2, 0x40, 0x06, 0x7d, 0xe3, 0x75, 0x72,  0x5a, 0xf9, 0xe0, 0x5b, 0x01, 0x4e, 0x61, 0x50,
        0xf4, 0x29, 0x63, 0x11, 0x45, 0x46, 0x38, 0x4a,  0xa4, 0xbc, 0x88, 0x18, 0x60, 0xec, 0x15, 0x2a,
        0x49, 0x41, 0xba, 0x30, 0xda, 0x4b, 0x3f, 0x16,  0xc4, 0xe8, 0x1a, 0x54, 0xa4, 0x52, 0x05, 0x3f,
        0xe6, 0xe5, 0xb8, 0x0c, 0x5f, 0xf7, 0xa8, 0x4f,  0xb8, 0x39, 0x79, 0x5d, 0x90, 0xfc, 0xd6, 0x73,
        0xc0, 0x2a, 0x0c, 0x76, 0xa8, 0xf1, 0xd6, 0x4a,  0x5c, 0x84, 0x97, 0x26, 0x5b, 0x96, 0x8d, 0x29,
        0xac, 0x9e, 0xcb, 0x0e, 0xc0, 0x83, 0xa5, 0x51,  0x51, 0x31, 0x4d, 0x03, 0x04, 0x07, 0xa4, 0x66,
        0x0d, 0x84, 0xde, 0x72, 0x44, 0xbe, 0xea, 0x21,  0x20, 0x59, 0xc2, 0x0e, 0xb6, 0x2f, 0x80, 0x10,
        0xc9, 0x4f, 0xa1, 0x51, 0x66, 0xb6, 0xe4, 0x54,  0x50, 0x7e, 0xd9, 0x25, 0xb4, 0x2e, 0xea, 0x2c,
        0xc9, 0xa1, 0xb2, 0x5a, 0xc1, 0xef, 0xef, 0x2a,  0x68, 0xe5, 0x95, 0x76, 0xe4, 0x5d, 0x17, 0x57,
        0x32, 0xf9, 0x53, 0x5e, 0x39, 0xd5, 0xee, 0x1c,  0x23, 0x13, 0x8c, 0x50, 0xd2, 0xde, 0x96, 0x71,
        0xf1, 0x7a, 0x1a, 0x45, 0x57, 0xe0, 0xc8, 0x5a,  0xae, 0xde, 0x71, 0x55, 0x7f, 0xce, 0x17, 0x22,
        0x31, 0xdb, 0x0a, 0x37, 0xe6, 0xdc, 0x16, 0x56,  0xfb, 0xb3, 0x95, 0x1f, 0x37, 0x92, 0xbd, 0x47,
        0x0f, 0xe8, 0x50, 0x69, 0x8e, 0x45, 0x5d, 0x47,  0xd3, 0x66, 0xd6, 0x36, 0xce, 0xd2, 0x8b, 0x5e,
        0x32, 0x37, 0x62, 0x1b, 0xf4, 0xc9, 0xef, 0x05,  0xaf, 0x32, 0xe3, 0x69, 0xa5, 0x08, 0x53, 0x31,
        0xbb, 0xe5, 0x70, 0x6b, 0x1f, 0x6c, 0x93, 0x51,  0x8e, 0xd3, 0x09, 0x22, 0x29, 0xac, 0xd7, 0x43,
        0x9f, 0x63, 0x08, 0x44, 0x24, 0x24, 0x84, 0x56,  0x3d, 0xb5, 0xed, 0x42, 0x72, 0x8e, 0xca, 0x01,
        0x00, 0x79, 0xe2, 0x2e, 0x96, 0x0f, 0xa0, 0x30,  0xe9, 0x6f, 0x67, 0x93, 0x05, 0x6f, 0x5a, 0x85,
        0xa6, 0x3a, 0x67, 0x8a, 0x67, 0x28, 0x0f, 0xd7,  0xc8, 0x8a, 0x3d, 0x72, 0xc6, 0x91, 0x76, 0x8f,
        0x19, 0x59, 0x7d, 0x62, 0x96, 0xb9, 0x00, 0x2c,  0xdf, 0xaa, 0xf8, 0x68, 0x74, 0x65, 0x97, 0x63,
        0x79, 0x9b, 0x42, 0x52, 0xb1, 0x02, 0x6f, 0x30,  0xae, 0x0e, 0x8f, 0x0c, 0x7d, 0x24, 0x0a, 0x30,
        0x16, 0xcf, 0x2f, 0x3d, 0x8c, 0xbd, 0xb0, 0x48,  0x9b, 0xfc, 0x79, 0x56, 0x8a, 0x96, 0xf7, 0x0b,
        0x68, 0xde, 0xea, 0x76, 0xba, 0x7e, 0xc1, 0x50,  0x9f, 0x04, 0x10, 0x19, 0xc0, 0xbe, 0x47, 0x63,
        0x45, 0x46, 0x5f, 0x3a, 0x4b, 0x40, 0x93, 0x2d,  0xee, 0xfd, 0xb6, 0x5f, 0x5e, 0x38, 0xca, 0x15,
        0xbb, 0x46, 0x6e, 0x24, 0x26, 0xa2, 0x1b, 0x46,  0xec, 0x4f, 0x99, 0x38, 0x4f, 0xfb, 0x7c, 0x41,
        0x5c, 0x74, 0xe4, 0x24, 0x85, 0x50, 0x6b, 0x3d,  0x47, 0x40, 0x2f, 0x0f, 0x63, 0xdc, 0x66, 0x40,
        0x3d, 0x5f, 0x5e, 0x58, 0xd3, 0x01, 0x84, 0x1e,  0xd0, 0xf2, 0x7e, 0x05, 0xfa, 0x15, 0xe5, 0x48,
        0x21, 0x60, 0x37, 0x23, 0x0e, 0x19, 0x69, 0x40,  0x52, 0x82, 0xea, 0x47, 0x85, 0xfd, 0x40, 0x72,
        0x50, 0x7b, 0x67, 0x35, 0x0c, 0x6b, 0x9b, 0x59,  0xf8, 0xbd, 0x38, 0x19, 0xec, 0x56, 0x42, 0x4e,
        0x91, 0x73, 0x48, 0x04, 0x67, 0x9e, 0xf2, 0x16,  0xb0, 0x7a, 0xbd, 0x3e, 0x9e, 0x42, 0x1f, 0x06,
        0x2d, 0xec, 0x0b, 0x0a, 0x52, 0x29, 0x73, 0x47,  0x54, 0x96, 0x38, 0x6b, 0x2b, 0xd0, 0xa6, 0x2f,
        0xc2, 0xb7, 0x79, 0x69, 0x41, 0x7c, 0x6c, 0x6c,  0xe1, 0x8e, 0xd4, 0x09, 0x62, 0x35, 0xa6, 0x5a,
        0x08, 0xc5, 0xaa, 0x26, 0xeb, 0x3e, 0xe7, 0x20,  0xe5, 0x35, 0x52, 0x15, 0xcc, 0x9c, 0x0c, 0x10,
        0xf1, 0x1d, 0x14, 0x37, 0x90, 0x2e, 0xaf, 0x27,  0x1b, 0x94, 0x1b, 0x65, 0x10, 0x92, 0x0b, 0x66,
        0x86, 0x1d, 0xb7, 0x2a, 0x5e, 0xa7, 0xd5, 0x54,  0x00, 0x61, 0x76, 0x5a, 0xc3, 0xa1, 0xbd, 0x1b,
        0x25, 0x33, 0x33, 0x12, 0x86, 0x43, 0x23, 0x60,  0x5d, 0x18, 0xb3, 0x22, 0x0f, 0xfb, 0x01, 0x34,
        0xa1, 0x4d, 0xb1, 0x2e, 0x92, 0x1b, 0x25, 0x5e,  0x02, 0x36, 0x66, 0x3a, 0xb3, 0x4e, 0xbb, 0x4c,
        0x99, 0xe7, 0x6a, 0x48, 0x98, 0x29, 0xa5, 0x68,  0x65, 0x94, 0x0c, 0x20, 0x66, 0x3b, 0xf0, 0x32,
        0x1e, 0x30, 0x5e, 0x11, 0x09, 0xcc, 0xee, 0x64,  0x45, 0x5f, 0xcf, 0x66, 0xc9, 0x5e, 0x26, 0x2f,
        0x3d, 0xd1, 0xfe, 0x1c, 0x6d, 0xe6, 0x0f, 0x5f,  0x6e, 0x36, 0x2f, 0x3f, 0xe2, 0x0a, 0xf8, 0x09,
        0x17, 0xe0, 0x8f, 0x2a, 0x01, 0x25, 0x85, 0x14,  0xdd, 0xdd, 0x90, 0x3c, 0x94, 0x14, 0x38, 0x0b,
        0x99, 0xe2, 0x6c, 0x58, 0xfc, 0xda, 0x43, 0x02,  0xb3, 0x9f, 0x80, 0x19, 0x47, 0xc8, 0x1f, 0x3a,
        0x16, 0x7d, 0x2c, 0x21, 0xa7, 0x88, 0x9d, 0x3c,  0x7e, 0xc0, 0x45, 0x53, 0x82, 0x7c, 0x79, 0x32,
        0x8c, 0x35, 0xb2, 0x41, 0xc4, 0x7d, 0x50, 0x53,  0x52, 0x20, 0xbe, 0x29, 0xc7, 0x40, 0xf8, 0x05,
        0xfc, 0x66, 0x27, 0x15, 0x34, 0x98, 0xc5, 0x41,  0xf2, 0xea, 0xea, 0x64, 0x5c, 0xaf, 0xfa, 0x4c,
        0x3a, 0xc2, 0xd5, 0x2d, 0x32, 0xa7, 0x88, 0x17,  0x0f, 0xfd, 0xd2, 0x2a, 0xe6, 0x66, 0x27, 0x01,
        0xcc, 0xfe, 0xe8, 0x63, 0x78, 0x6c, 0xbd, 0x35,  0x99, 0xd3, 0xa7, 0x00, 0xc9, 0x8f, 0x1b, 0x0c,
        0x27, 0x23, 0xef, 0x55, 0xef, 0x41, 0xcf, 0x44,  0x37, 0xab, 0xd1, 0x41, 0x43, 0x98, 0x3e, 0x6f,
        0x79, 0xec, 0x24, 0x07, 0x59, 0xd8, 0xae, 0x24,  0xa3, 0x3d, 0xf8, 0x2b, 0x2e, 0x06, 0x85, 0x54,
        0xb7, 0xd7, 0x5d, 0x24, 0xe6, 0x18, 0x7b, 0x0c,  0x17, 0x13, 0x2b, 0x4b, 0xbd, 0x61, 0xc1, 0x19,
        0x80, 0xd2, 0xc5, 0x12, 0xfb, 0xa7, 0xc5, 0x50,  0xcd, 0x03, 0xf1, 0x5b, 0x69, 0xcb, 0x6d, 0x37,
        0xbb, 0x7c, 0x23, 0x4d, 0xe7, 0x8a, 0x42, 0x59,  0x2d, 0xcf, 0x46, 0x16, 0x64, 0xb1, 0x41, 0x45,
        0x4d, 0x9a, 0x01, 0x37, 0xe6, 0x64, 0x36, 0x08,  0x98, 0xd2, 0x69, 0x16, 0x70, 0x1c, 0x86, 0x70,
        0x89, 0x15, 0x59, 0x3e, 0x08, 0xd7, 0x4f, 0x51,  0xa9, 0xe9, 0x4b, 0x18, 0x2c, 0x26, 0x13, 0x45,
        0x6f, 0x2e, 0x83, 0x61, 0xeb, 0x0c, 0xc9, 0x34,  0x2a, 0x64, 0xf6, 0x1b, 0x0e, 0x85, 0x3c, 0x5f,
        0x8b, 0x44, 0x89, 0x38, 0xe5, 0x51, 0xfc, 0x5c,  0x90, 0xa0, 0x27, 0x42, 0xd9, 0xc6, 0xd5, 0x0a,
        0x1a, 0xbd, 0xa1, 0x0a, 0xe7, 0xdc, 0x1d, 0x55,  0xb8, 0x47, 0x8c, 0x4f, 0x71, 0x18, 0x18, 0x1f,
        0x64, 0x0f, 0xb7, 0x24, 0x16, 0x2d, 0x76, 0x0e,  0xe9, 0xa4, 0x25, 0x3c, 0x28, 0x41, 0x0e, 0x62,
        0xae, 0xf0, 0xba, 0x4d, 0xf4, 0x0a, 0xd7, 0x1d,  0x13, 0x9f, 0x4b, 0x69, 0xff, 0x31, 0x20, 0x4c,
        0x70, 0x9c, 0x26, 0x47, 0xb7, 0xaf, 0x1a, 0x3a,  0x43, 0xd8, 0xb8, 0x18, 0xe8, 0x32, 0x9b, 0x60,
        0x24, 0x3c, 0x47, 0x6b, 0xf3, 0x83, 0x61, 0x2c,  0xcd, 0x5d, 0x17, 0x32, 0x1f, 0xbb, 0xb2, 0x61,
        0xec, 0xf6, 0x63, 0x15, 0x86, 0x7f, 0xeb, 0x66,  0x87, 0x0d, 0xe3, 0x18, 0x3c, 0x5f, 0x70, 0x6d,
        0xf3, 0x91, 0x64, 0x58, 0x55, 0x31, 0xcb, 0x65,  0xc5, 0x06, 0x5e, 0x36, 0x6f, 0xe5, 0xad, 0x5b,
        0x81, 0x47, 0x9d, 0x4e, 0x18, 0x5b, 0xb8, 0x6f,  0xcd, 0xc9, 0x9c, 0x70, 0x09, 0x76, 0x3c, 0x28,
        0x89, 0xa6, 0x0d, 0x0c, 0x44, 0x38, 0x5b, 0x32,  0xeb, 0xf1, 0xef, 0x24, 0x41, 0x6d, 0xa7, 0x03,
        0x5c, 0x1d, 0x18, 0x6c, 0xaa, 0xda, 0x86, 0x47,  0x7c, 0x08, 0xdc, 0x1c, 0x31, 0xce, 0x92, 0x0a,
        0xd8, 0xad, 0xe2, 0x31, 0xdc, 0xb3, 0xbb, 0x1e,  0x78, 0x06, 0x29, 0x70, 0x76, 0xd5, 0xe5, 0x1a,
        0x69, 0x28, 0x5b, 0x42, 0x51, 0xaf, 0x2a, 0x35,  0xe7, 0xee, 0x7f, 0x61, 0xb0, 0x29, 0xf2, 0x3d,
        0xba, 0x22, 0xcf, 0x11, 0xf5, 0xfa, 0xcf, 0x3a,  0x60, 0x82, 0x96, 0x35, 0xf1, 0x2e, 0x06, 0x42,
        0x7a, 0x78, 0xc6, 0x3c, 0x72, 0xe5, 0x7c, 0x20,  0xfe, 0x1b, 0x04, 0x5d, 0x72, 0x76, 0x2c, 0x60,
        0xe7, 0x7d, 0x94, 0x1f, 0x4f, 0xdf, 0x4c, 0x55,  0x94, 0x2f, 0xb2, 0x44, 0x74, 0x54, 0xf5, 0x22,
        0x6c, 0x41, 0x9a, 0x33, 0x4b, 0xa2, 0x24, 0x45,  0x91, 0xdd, 0x3b, 0x0f, 0xd9, 0x8b, 0x2f, 0x36,
        0xb3, 0x7b, 0xa0, 0x59, 0xbc, 0xec, 0x50, 0x37,  0xbb, 0x2e, 0x27, 0x18, 0x25, 0xa8, 0xb8, 0x23,
        0xad, 0x98, 0xbf, 0x1e, 0x0f, 0xe8, 0x9d, 0x08,  0x26, 0xf1, 0xb9, 0x1d, 0x41, 0xe8, 0xcd, 0x5e,
        0xe8, 0xdc, 0x74, 0x0d, 0xa7, 0x26, 0x2d, 0x57,  0x23, 0xae, 0x5c, 0x65, 0xaf, 0xd7, 0x74, 0x3c,
    };

    const uint8_t COMPRESSED_RANDOM[] = {
        0x53, 0x5a, 0x44, 0x44, 0x88, 0xf0, 0x27, 0x33,  0x41, 0x79, 0x40, 0x06, 0x00, 0x00, 0xff, 0x2a,
        0x5d, 0xd8, 0x46, 0x28, 0xa8, 0xe5, 0x04, 0xff,  0x5b, 0x42, 0x56, 0x6c, 0x57, 0xfa, 0xc7, 0x49,
        0xff, 0x70, 0x4c, 0xa4, 0x57, 0x3a, 0xa4, 0x0e,  0x0a, 0xff, 0x6e, 0x8e, 0x31, 0x4b, 0x2c, 0x78,
        0x9e, 0x11, 0xff, 0x3f, 0xd6, 0x90, 0x25, 0x07,  0xa8, 0x71, 0x0b, 0xff, 0x91, 0xc5, 0x98, 0x46,
        0xbf, 0x2a, 0x84, 0x3e, 0xff, 0xe2, 0xd2, 0xba,  0x4d, 0xb7, 0x8a, 0x51, 0x6f, 0xff, 0x11, 0xa6,
        0x2f, 0x5d, 0xec, 0x01, 0x6c, 0x0b, 0xff, 0x92,  0x84, 0x99, 0x5b, 0x36, 0xf6, 0xfc, 0x28, 0xff,
        0x76, 0x86, 0x91, 0x3e, 0x40, 0xa3, 0x92, 0x17,  0xff, 0xf1, 0xfb, 0xab, 0x55, 0xf5, 0x1b, 0x2f,
        0x74, 0xff, 0x45, 0x0c, 0x1c, 0x34, 0x00, 0xc4,  0x4e, 0x23, 0xff, 0x67, 0x24, 0x00, 0x14, 0x01,
        0xa7, 0xd3, 0x4c, 0xff, 0xfb, 0x08, 0xc4, 0x4a,  0x32, 0x17, 0x01, 0x2d, 0xff, 0x1c, 0x6b, 0x17,
        0x1a, 0xf1, 0xaf, 0xff, 0x65, 0xff, 0xf2, 0xf4,  0xe6, 0x11, 0x3b, 0xe0, 0x3e, 0x46, 0xff, 0xc9,
        0x0d, 0xa5, 0x3a, 0x31, 0x69, 0xdf, 0x42, 0xff,  0x41, 0x83, 0xaa, 0x54, 0xae, 0x26, 0x98, 0x0f,
        0xff, 0x8d, 0x82, 0xda, 0x35, 0x63, 0xda, 0x93,  0x2c, 0xff, 0x0a, 0x00, 0x43, 0x49, 0x23, 0x03,
        0xec, 0x2e, 0xff, 0x49, 0xff, 0x7f, 0x75, 0x92,  0x43, 0x70, 0x66, 0xff, 0xc3, 0x1c, 0x4e, 0x74,
        0xcb, 0x18, 0x05, 0x2a, 0xff, 0x8a, 0x2a, 0xb6,  0x1b, 0xf7, 0x24, 0x23, 0x77, 0xff, 0x43, 0xfa,
        0xad, 0x13, 0x76, 0x21, 0x86, 0x18, 0xff, 0xd7,  0x59, 0x99, 0x4e, 0xbc, 0xda, 0x12, 0x01, 0xff,
        0xd0, 0x30, 0x7e, 0x3d, 0xfb, 0x96, 0x01, 0x00,  0xff, 0x04, 0x0a, 0xa9, 0x2e, 0xad, 0x5b, 0xd6,
        0x26, 0xff, 0xd8, 0x9a, 0xf7, 0x20, 0xa5, 0xba,  0xf4, 0x0b, 0xff, 0xaa, 0x5c, 0x03, 0x5e, 0x54,
        0xa6, 0x4b, 0x51, 0xff, 0xc5, 0x08, 0xfa, 0x60,  0x43, 0x1b, 0xe6, 0x6d, 0xff, 0x5f, 0xcc, 0xc5,
        0x20, 0x84, 0xbb, 0xce, 0x49, 0xff, 0xc7, 0xc5,  0xba, 0x07, 0xff, 0x17, 0x6f, 0x1b, 0xff, 0xa6,
        0xec, 0x7f, 0x31, 0xbf, 0x40, 0x4a, 0x1e, 0xff,  0x22, 0x25, 0xe6, 0x1c, 0xae, 0x4f, 0x7e, 0x53,
        0xff, 0x51, 0x5d, 0x2c, 0x1d, 0x2c, 0x42, 0x14,  0x23, 0xff, 0xcb, 0xc7, 0xb5, 0x5d, 0x85, 0x0d,
        0xe4, 0x26, 0xff, 0xf2, 0xcc, 0xda, 0x64, 0x74,  0x17, 0x36, 0x24, 0xff, 0xcc, 0x72, 0x87, 0x43,
        0x35, 0x2d, 0x65, 0x60, 0xff, 0x60, 0x2b, 0x41,  0x15, 0x44, 0xe3, 0x11, 0x1a, 0xff, 0x2f, 0x05,
        0x40, 0x28, 0x19, 0x39, 0x92, 0x17, 0xff, 0x0d,  0xe8, 0x5e, 0x12, 0xb1, 0x35, 0x73, 0x0d, 0xff,
        0xce, 0x15, 0x88, 0x34, 0x2b, 0x0f, 0x42, 0x5d,  0xff, 0x61, 0xe5, 0x75, 0x73, 0x2a, 0xce, 0x6b,
        0x19, 0xff, 0x64, 0xad, 0x35, 0x5a, 0xa7, 0x41,  0xa0, 0x1b, 0xff, 0x6b, 0x03, 0xc2, 0x50, 0xb8,
        0xfe, 0xcb, 0x13, 0xff, 0x3c, 0x20, 0x82, 0x6c,  0x87, 0x79, 0x70, 0x75, 0xff, 0xeb, 0x41, 0xef,
        0x02, 0x78, 0x2b, 0xc8, 0x41, 0xff, 0x6a, 0x07,  0x0c, 0x56, 0xfa, 0x97, 0x84, 0x6a, 0xff, 0x7d,
        0x8d, 0x08, 0x3e, 0x4c, 0x70, 0xa8, 0x4d, 0xff,  0xa9, 0x59, 0xa5, 0x6c, 0x02, 0xdd, 0x06, 0x0d,
        0xff, 0xa8, 0xaf, 0xd1, 0x5b, 0x87, 0x5f, 0x06,  0x37, 0xff, 0x5d, 0xaa, 0x2b, 0x06, 0xdd, 0x3a,
        0x0c, 0x1b, 0xff, 0xf7, 0xd2, 0x40, 0x06, 0x7d,  0xe3, 0x75, 0x72, 0xff, 0x5a, 0xf9, 0xe0, 0x5b,
        0x01, 0x4e, 0x61, 0x50, 0xff, 0xf4, 0x29, 0x63,  0x11, 0x45, 0x46, 0x38, 0x4a, 0xff, 0xa4, 0xbc,
        0x88, 0x18, 0x60, 0xec, 0x15, 0x2a, 0xff, 0x49,  0x41, 0xba, 0x30, 0xda, 0x4b, 0x3f, 0x16, 0xff,
        0xc4, 0xe8, 0x1a, 0x54, 0xa4, 0x52, 0x05, 0x3f,  0xff, 0xe6, 0xe5, 0xb8, 0x0c, 0x5f, 0xf7, 0xa8,
        0x4f, 0xff, 0xb8, 0x39, 0x79, 0x5d, 0x90, 0xfc,  0xd6, 0x73, 0xff, 0xc0, 0x2a, 0x0c, 0x76, 0xa8,
        0xf1, 0xd6, 0x4a, 0xff, 0x5c, 0x84, 0x97, 0x26,  0x5b, 0x96, 0x8d, 0x29, 0xff, 0xac, 0x9e, 0xcb,
        0x0e, 0xc0, 0x83, 0xa5, 0x51, 0xff, 0x51, 0x31,  0x4d, 0x03, 0x04, 0x07, 0xa4, 0x66, 0xff, 0x0d,
        0x84, 0xde, 0x72, 0x44, 0xbe, 0xea, 0x21, 0xff,  0x20, 0x59, 0xc2, 0x0e, 0xb6, 0x2f, 0x80, 0x10,
        0xff, 0xc9, 0x4f, 0xa1, 0x51, 0x66, 0xb6, 0xe4,  0x54, 0xff, 0x50, 0x7e, 0xd9, 0x25, 0xb4, 0x2e,
        0xea, 0x2c, 0xff, 0xc9, 0xa1, 0xb2, 0x5a, 0xc1,  0xef, 0xef, 0x2a, 0xff, 0x68, 0xe5, 0x95, 0x76,
        0xe4, 0x5d, 0x17, 0x57, 0xff, 0x32, 0xf9, 0x53,  0x5e, 0x39, 0xd5, 0xee, 0x1c, 0xff, 0x23, 0x13,
        0x8c, 0x50, 0xd2, 0xde, 0x96, 0x71, 0xff, 0xf1,  0x7a, 0x1a, 0x45, 0x57, 0xe0, 0xc8, 0x5a, 0xff,
        0xae, 0xde, 0x71, 0x55, 0x7f, 0xce, 0x17, 0x22,  0xff, 0x31, 0xdb, 0x0a, 0x37, 0xe6, 0xdc, 0x16,
        0x56, 0xff, 0xfb, 0xb3, 0x95, 0x1f, 0x37, 0x92,  0xbd, 0x47, 0xff, 0x0f, 0xe8, 0x50, 0x69, 0x8e,
        0x45, 0x5d, 0x47, 0xff, 0xd3, 0x66, 0xd6, 0x36,  0xce, 0xd2, 0x8b, 0x5e, 0xff, 0x32, 0x37, 0x62,
        0x1b, 0xf4, 0xc9, 0xef, 0x05, 0xff, 0xaf, 0x32,  0xe3, 0x69, 0xa5, 0x08, 0x53, 0x31, 0xff, 0xbb,
        0xe5, 0x70, 0x6b, 0x1f, 0x6c, 0x93, 0x51, 0xff,  0x8e, 0xd3, 0x09, 0x22, 0x29, 0xac, 0xd7, 0x43,
        0xff, 0x9f, 0x63, 0x08, 0x44, 0x24, 0x24, 0x84,  0x56, 0xff, 0x3d, 0xb5, 0xed, 0x42, 0x72, 0x8e,
        0xca, 0x01, 0xff, 0x00, 0x79, 0xe2, 0x2e, 0x96,  0x0f, 0xa0, 0x30, 0xff, 0xe9, 0x6f, 0x67, 0x93,
        0x05, 0x6f, 0x5a, 0x85, 0xff, 0xa6, 0x3a, 0x67,  0x8a, 0x67, 0x28, 0x0f, 0xd7, 0xff, 0xc8, 0x8a,
        0x3d, 0x72, 0xc6, 0x91, 0x76, 0x8f, 0xff, 0x19,  0x59, 0x7d, 0x62, 0x96, 0xb9, 0x00, 0x2c, 0xff,
        0xdf, 0xaa, 0xf8, 0x68, 0x74, 0x65, 0x97, 0x63,  0xff, 0x79, 0x9b, 0x42, 0x52, 0xb1, 0x02, 0x6f,
        0x30, 0xff, 0xae, 0x0e, 0x8f, 0x0c, 0x7d, 0x24,  0x0a, 0x30, 0xff, 0x16, 0xcf, 0x2f, 0x3d, 0x8c,
        0xbd, 0xb0, 0x48, 0xff, 0x9b, 0xfc, 0x79, 0x56,  0x8a, 0x96, 0xf7, 0x0b, 0xff, 0x68, 0xde, 0xea,
        0x76, 0xba, 0x7e, 0xc1, 0x50, 0xff, 0x9f, 0x04,  0x10, 0x19, 0xc0, 0xbe, 0x47, 0x63, 0xff, 0x45,
        0x46, 0x5f, 0x3a, 0x4b, 0x40, 0x93, 0x2d, 0xff,  0xee, 0xfd, 0xb6, 0x5f, 0x5e, 0x38, 0xca, 0x15,
        0xff, 0xbb, 0x46, 0x6e, 0x24, 0x26, 0xa2, 0x1b,  0x46, 0xff, 0xec, 0x4f, 0x99, 0x38, 0x4f, 0xfb,
        0x7c, 0x41, 0xff, 0x5c, 0x74, 0xe4, 0x24, 0x85,  0x50, 0x6b, 0x3d, 0xff, 0x47, 0x40, 0x2f, 0x0f,
        0x63, 0xdc, 0x66, 0x40, 0xff, 0x3d, 0x5f, 0x5e,  0x58, 0xd3, 0x01, 0x84, 0x1e, 0xff, 0xd0, 0xf2,
        0x7e, 0x05, 0xfa, 0x15, 0xe5, 0x48, 0xff, 0x21,  0x60, 0x37, 0x23, 0x0e, 0x19, 0x69, 0x40, 0xff,
        0x52, 0x82, 0xea, 0x47, 0x85, 0xfd, 0x40, 0x72,  0xff, 0x50, 0x7b, 0x67, 0x35, 0x0c, 0x6b, 0x9b,
        0x59, 0xff, 0xf8, 0xbd, 0x38, 0x19, 0xec, 0x56,  0x42, 0x4e, 0xff, 0x91, 0x73, 0x48, 0x04, 0x67,
        0x9e, 0xf2, 0x16, 0xff, 0xb0, 0x7a, 0xbd, 0x3e,  0x9e, 0x42, 0x1f, 0x06, 0xff, 0x2d, 0xec, 0x0b,
        0x0a, 0x52, 0x29, 0x73, 0x47, 0xff, 0x54, 0x96,  0x38, 0x6b, 0x2b, 0xd0, 0xa6, 0x2f, 0xff, 0xc2,
        0xb7, 0x79, 0x69, 0x41, 0x7c, 0x6c, 0x6c, 0xff,  0xe1, 0x8e, 0xd4, 0x09, 0x62, 0x35, 0xa6, 0x5a,
        0xff, 0x08, 0xc5, 0xaa, 0x26, 0xeb, 0x3e, 0xe7,  0x20, 0xff, 0xe5, 0x35, 0x52, 0x15, 0xcc, 0x9c,
        0x0c, 0x10, 0xff, 0xf1, 0x1d, 0x14, 0x37, 0x90,  0x2e, 0xaf, 0x27, 0xff, 0x1b, 0x94, 0x1b, 0x65,
        0x10, 0x92, 0x0b, 0x66, 0xff, 0x86, 0x1d, 0xb7,  0x2a, 0x5e, 0xa7, 0xd5, 0x54, 0xff, 0x00, 0x61,
        0x76, 0x5a, 0xc3, 0xa1, 0xbd, 0x1b, 0xff, 0x25,  0x33, 0x33, 0x12, 0x86, 0x43, 0x23, 0x60, 0xff,
        0x5d, 0x18, 0xb3, 0x22, 0x0f, 0xfb, 0x01, 0x34,  0xff, 0xa1, 0x4d, 0xb1, 0x2e, 0x92, 0x1b, 0x25,
        0x5e, 0xff, 0x02, 0x36, 0x66, 0x3a, 0xb3, 0x4e,  0xbb, 0x4c, 0xff, 0x99, 0xe7, 0x6a, 0x48, 0x98,
        0x29, 0xa5, 0x68, 0xff, 0x65, 0x94, 0x0c, 0x20,  0x66, 0x3b, 0xf0, 0x32, 0xff, 0x1e, 0x30, 0x5e,
        0x11, 0x09, 0xcc, 0xee, 0x64, 0xff, 0x45, 0x5f,  0xcf, 0x66, 0xc9, 0x5e, 0x26, 0x2f, 0xff, 0x3d,
        0xd1, 0xfe, 0x1c, 0x6d, 0xe6, 0x0f, 0x5f, 0xff,  0x6e, 0x36, 0x2f, 0x3f, 0xe2, 0x0a, 0xf8, 0x09,
        0xff, 0x17, 0xe0, 0x8f, 0x2a, 0x01, 0x25, 0x85,  0x14, 0xff, 0xdd, 0xdd, 0x90, 0x3c, 0x94, 0x14,
        0x38, 0x0b, 0xff, 0x99, 0xe2, 0x6c, 0x58, 0xfc,  0xda, 0x43, 0x02, 0xff, 0xb3, 0x9f, 0x80, 0x19,
        0x47, 0xc8, 0x1f, 0x3a, 0xff, 0x16, 0x7d, 0x2c,  0x21, 0xa7, 0x88, 0x9d, 0x3c, 0xff, 0x7e, 0xc0,
        0x45, 0x53, 0x82, 0x7c, 0x79, 0x32, 0xff, 0x8c,  0x35, 0xb2, 0x41, 0xc4, 0x7d, 0x50, 0x53, 0xff,
        0x52, 0x20, 0xbe, 0x29, 0xc7, 0x40, 0xf8, 0x05,  0xff, 0xfc, 0x66, 0x27, 0x15, 0x34, 0x98, 0xc5,
        0x41, 0xff, 0xf2, 0xea, 0xea, 0x64, 0x5c, 0xaf,  0xfa, 0x4c, 0xff, 0x3a, 0xc2, 0xd5, 0x2d, 0x32,
        0xa7, 0x88, 0x17, 0xff, 0x0f, 0xfd, 0xd2, 0x2a,  0xe6, 0x66, 0x27, 0x01, 0xff, 0xcc, 0xfe, 0xe8,
        0x63, 0x78, 0x6c, 0xbd, 0x35, 0xff, 0x99, 0xd3,  0xa7, 0x00, 0xc9, 0x8f, 0x1b, 0x0c, 0xff, 0x27,
        0x23, 0xef, 0x55, 0xef, 0x41, 0xcf, 0x44, 0xff,  0x37, 0xab, 0xd1, 0x41, 0x43, 0x98, 0x3e, 0x6f,
        0xff, 0x79, 0xec, 0x24, 0x07, 0x59, 0xd8, 0xae,  0x24, 0xff, 0xa3, 0x3d, 0xf8, 0x2b, 0x2e, 0x06,
        0x85, 0x54, 0xff, 0xb7, 0xd7, 0x5d, 0x24, 0xe6,  0x18, 0x7b, 0x0c, 0xff, 0x17, 0x13, 0x2b, 0x4b,
        0xbd, 0x61, 0xc1, 0x19, 0xff, 0x80, 0xd2, 0xc5,  0x12, 0xfb, 0xa7, 0xc5, 0x50, 0xff, 0xcd, 0x03,
        0xf1, 0x5b, 0x69, 0xcb, 0x6d, 0x37, 0xff, 0xbb,  0x7c, 0x23, 0x4d, 0xe7, 0x8a, 0x42, 0x59, 0xff,
        0x2d, 0xcf, 0x46, 0x16, 0x64, 0xb1, 0x41, 0x45,  0xff, 0x4d, 0x9a, 0x01, 0x37, 0xe6, 0x64, 0x36,
        0x08, 0xff, 0x98, 0xd2, 0x69, 0x16, 0x70, 0x1c,  0x86, 0x70, 0xff, 0x89, 0x15, 0x59, 0x3e, 0x08,
        0xd7, 0x4f, 0x51, 0xff, 0xa9, 0xe9, 0x4b, 0x18,  0x2c, 0x26, 0x13, 0x45, 0xff, 0x6f, 0x2e, 0x83,
        0x61, 0xeb, 0x0c, 0xc9, 0x34, 0xff, 0x2a, 0x64,  0xf6, 0x1b, 0x0e, 0x85, 0x3c, 0x5f, 0xff, 0x8b,
        0x44, 0x89, 0x38, 0xe5, 0x51, 0xfc, 0x5c, 0xff,  0x90, 0xa0, 0x27, 0x42, 0xd9, 0xc6, 0xd5, 0x0a,
        0xff, 0x1a, 0xbd, 0xa1, 0x0a, 0xe7, 0xdc, 0x1d,  0x55, 0xff, 0xb8, 0x47, 0x8c, 0x4f, 0x71, 0x18,
        0x18, 0x1f, 0xff, 0x64, 0x0f, 0xb7, 0x24, 0x16,  0x2d, 0x76, 0x0e, 0xff, 0xe9, 0xa4, 0x25, 0x3c,
        0x28, 0x41, 0x0e, 0x62, 0xff, 0xae, 0xf0, 0xba,  0x4d, 0xf4, 0x0a, 0xd7, 0x1d, 0xff, 0x13, 0x9f,
        0x4b, 0x69, 0xff, 0x31, 0x20, 0x4c, 0xff, 0x70,  0x9c, 0x26, 0x47, 0xb7, 0xaf, 0x1a, 0x3a, 0xff,
        0x43, 0xd8, 0xb8, 0x18, 0xe8, 0x32, 0x9b, 0x60,  0xff, 0x24, 0x3c, 0x47, 0x6b, 0xf3, 0x83, 0x61,
        0x2c, 0xff, 0xcd, 0x5d, 0x17, 0x32, 0x1f, 0xbb,  0xb2, 0x61, 0xff, 0xec, 0xf6, 0x63, 0x15, 0x86,
        0x7f, 0xeb, 0x66, 0xff, 0x87, 0x0d, 0xe3, 0x18,  0x3c, 0x5f, 0x70, 0x6d, 0xff, 0xf3, 0x91, 0x64,
        0x58, 0x55, 0x31, 0xcb, 0x65, 0xff, 0xc5, 0x06,  0x5e, 0x36, 0x6f, 0xe5, 0xad, 0x5b, 0xff, 0x81,
        0x47, 0x9d, 0x4e, 0x18, 0x5b, 0xb8, 0x6f, 0xff,  0xcd, 0xc9, 0x9c, 0x70, 0x09, 0x76, 0x3c, 0x28,
        0xff, 0x89, 0xa6, 0x0d, 0x0c, 0x44, 0x38, 0x5b,  0x32, 0xff, 0xeb, 0xf1, 0xef, 0x24, 0x41, 0x6d,
        0xa7, 0x03, 0xff, 0x5c, 0x1d, 0x18, 0x6c, 0xaa,  0xda, 0x86, 0x47, 0xff, 0x7c, 0x08, 0xdc, 0x1c,
        0x31, 0xce, 0x92, 0x0a, 0xff, 0xd8, 0xad, 0xe2,  0x31, 0xdc, 0xb3, 0xbb, 0x1e, 0xff, 0x78, 0x06,
        0x29, 0x70, 0x76, 0xd5, 0xe5, 0x1a, 0xff, 0x69,  0x28, 0x5b, 0x42, 0x51, 0xaf, 0x2a, 0x35, 0xff,
        0xe7, 0xee, 0x7f, 0x61, 0xb0, 0x29, 0xf2, 0x3d,  0xff, 0xba, 0x22, 0xcf, 0x11, 0xf5, 0xfa, 0xcf,
        0x3a, 0xff, 0x60, 0x82, 0x96, 0x35, 0xf1, 0x2e,  0x06, 0x42, 0xff, 0x7a, 0x78, 0xc6, 0x3c, 0x72,
        0xe5, 0x7c, 0x20, 0xff, 0xfe, 0x1b, 0x04, 0x5d,  0x72, 0x76, 0x2c, 0x60, 0xff, 0xe7, 0x7d, 0x94,
        0x1f, 0x4f, 0xdf, 0x4c, 0x55, 0xff, 0x94, 0x2f,  0xb2, 0x44, 0x74, 0x54, 0xf5, 0x22, 0xff, 0x6c,
        0x41, 0x9a, 0x33, 0x4b, 0xa2, 0x24, 0x45, 0xff,  0x91, 0xdd, 0x3b, 0x0f, 0xd9, 0x8b, 0x2f, 0x36,
        0xff, 0xb3, 0x7b, 0xa0, 0x59, 0xbc, 0xec, 0x50,  0x37, 0xff, 0xbb, 0x2e, 0x27, 0x18, 0x25, 0xa8,
        0xb8, 0x23, 0xff, 0xad, 0x98, 0xbf, 0x1e, 0x0f,  0xe8, 0x9d, 0x08, 0xff, 0x26, 0xf1, 0xb9, 0x1d,
        0x41, 0xe8, 0xcd, 0x5e, 0xff, 0xe8, 0xdc, 0x74,  0x0d, 0xa7, 0x26, 0x2d, 0x57, 0xff, 0x23, 0xae,
        0x5c, 0x65, 0xaf, 0xd7, 0x74, 0x3c,
    };

    /*
     *  Compressible Data (a BMP file)
     */
    const uint8_t ORIGINAL_COMPRESSIBLE[] = {
        0x42, 0x4d, 0x5e, 0x06, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00,
        0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x36, 0x00,  0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
        0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00,  0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80,
        0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xc0, 0xc0,  0xc0, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00,  0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff,
        0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x44, 0x44,  0x44, 0x44, 0x00, 0x04, 0x00, 0x00, 0x04, 0x44,
        0x44, 0x44, 0x40, 0x04, 0x44, 0x44, 0x44, 0x40,  0x44, 0x44, 0x44, 0x44, 0x00, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x40, 0x40, 0x00, 0x04,  0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x40, 0x04, 0x44, 0x44, 0x44, 0x40,  0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x04, 0x44,
        0x44, 0x44, 0x44, 0x00, 0x00, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x40, 0x00, 0x04, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44,  0x40, 0x04, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00,
        0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,  0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x40, 0x40, 0x00, 0x00, 0x04, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x40, 0x00, 0x00, 0x40, 0x04,  0x44, 0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x04, 0x00,  0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x00, 0x00, 0x04, 0x40, 0x04, 0x40, 0x00, 0x00,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x40, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x44,  0x44, 0x44, 0x44, 0x44, 0x40, 0x00, 0x00, 0x40,
        0x00, 0x04, 0x40, 0x00, 0x04, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x40,
        0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x00, 0x00, 0x40, 0x00, 0x00, 0x04, 0x40,
        0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00,  0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00, 0x40,  0x00, 0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,  0x40, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
        0x00, 0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,  0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
        0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44,  0x44, 0x44, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00,  0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00,
        0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x40,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x44,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x44, 0x44, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x44, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40,  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x44, 0x44, 0x44, 0x40, 0x04, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,
        0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x44, 0x44, 0x44, 0x04, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x40, 0x00, 0x04, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x44,
        0x04, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,  0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x04, 0x40, 0x44, 0x44, 0x40, 0x44,
        0x44, 0x44, 0x44, 0x04, 0x00, 0x00, 0x00, 0x44,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x44, 0x44, 0x44, 0x40, 0x04, 0x00, 0x00,  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
        0x00, 0x00, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x00, 0x00,  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x40, 0x00,  0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x04, 0x44,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x04,  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x44, 0x40, 0x44, 0x44, 0x40, 0x40, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x44, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00,  0x00, 0x00, 0x00, 0x04, 0x44, 0x40, 0x44, 0x44,
        0x04, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x04, 0x44, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,  0x44, 0x04, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x04,  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x04, 0x40,  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x44, 0x44, 0x40, 0x44, 0x44, 0x40, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44,
        0x40, 0x04, 0x40, 0x04, 0x44, 0x40, 0x00, 0x00,  0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x04, 0x44,
        0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x04, 0x40, 0x04, 0x44, 0x40, 0x44,
        0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x44, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x04,  0x44, 0x44, 0x40, 0x44, 0x40, 0x00, 0x00, 0x00,
        0x00, 0x04, 0x44, 0x44, 0x44, 0x40, 0x44, 0x44,  0x44, 0x04, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x40, 0x44,  0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x44, 0x44,
        0x40, 0x04, 0x44, 0x44, 0x40, 0x44, 0x44, 0x44,  0x40, 0x00, 0x00, 0x00, 0x04, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,  0x44, 0x44, 0x44, 0x40, 0x04, 0x44, 0x44, 0x40,
        0x04, 0x44, 0x44, 0x44, 0x04, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00,
        0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40, 0x04,  0x44, 0x44, 0x40, 0x04, 0x44, 0x44, 0x44, 0x40,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x40, 0x00, 0x00, 0x00, 0x04, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x04, 0x44,  0x44, 0x44, 0x40, 0x04, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00,  0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x40, 0x44, 0x44, 0x44, 0x40, 0x04, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00,
        0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00, 0x00,  0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x40, 0x44, 0x40, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,  0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x04, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x04, 0x00, 0x00, 0x04, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x00, 0x44, 0x44, 0x44, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x04,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x44,
        0x44, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00, 0x40,  0x00, 0x00, 0x04, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x00, 0x44, 0x44, 0x44, 0x44, 0x04, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x40, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x40, 0x04, 0x44, 0x44,  0x40, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x44,
        0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,  0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40,
        0x04, 0x44, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00,  0x00, 0x44, 0x44, 0x44, 0x44, 0x44,
    };

    const uint8_t COMPRESSED_COMPRESSIBLE[] = {
        0x53, 0x5a, 0x44, 0x44, 0x88, 0xf0, 0x27, 0x33,  0x41, 0x70, 0x5e, 0x06, 0x00, 0x00, 0x5f, 0x42,
        0x4d, 0x5e, 0x06, 0x00, 0xf4, 0xf2, 0x76, 0xf4,  0xf0, 0xd5, 0x28, 0xf4, 0xf0, 0x38, 0xf4, 0xf0,
        0x36, 0xf4, 0xf0, 0x01, 0x00, 0xd1, 0x04, 0xf4,  0xf3, 0x12, 0x0d, 0x1c, 0x06, 0x80, 0x2a, 0x02,
        0x00, 0x80, 0xe1, 0x80, 0x2e, 0x03, 0x39, 0x01,  0x2c, 0x01, 0x33, 0x00, 0xc0, 0xc0, 0xc0, 0x3a,
        0xf4, 0xf0, 0xff, 0x4a, 0x02, 0x00, 0xff, 0xff,  0x4e, 0x03, 0x59, 0x01, 0xa4, 0x4c, 0x01, 0x53,
        0x00, 0x44, 0x66, 0x00, 0x0b, 0x01, 0x04, 0x66,  0x00, 0x40, 0x10, 0x6e, 0x02, 0x66, 0x02, 0x66,
        0x01, 0x6f, 0x01, 0x40, 0x6a, 0x05, 0x80, 0x02,  0x6e, 0x02, 0x4e, 0x7c, 0x05, 0x00, 0x40, 0x00,
        0xa0, 0x01, 0x8c, 0x08, 0x44, 0xa5, 0x06, 0x00,  0x0b, 0x01, 0xa4, 0x05, 0xad, 0x03, 0xa1, 0x00,
        0x8a, 0x03, 0x85, 0x03, 0x88, 0x07, 0x82, 0x02,  0xc2, 0xd0, 0x06, 0x00, 0x72, 0x00, 0xdd, 0x09,
        0x0b, 0x02, 0x9a, 0x04, 0x00, 0x04, 0x03, 0x40,  0x04, 0xa4, 0x06, 0xb9, 0x04, 0xec, 0x08, 0x11,
        0x10, 0x89, 0x06, 0xf4, 0xf0, 0x10, 0xa1, 0x00,  0x09, 0x16, 0xce, 0x01, 0x97, 0x04, 0x40, 0x4a,
        0x12, 0x23, 0x18, 0x3f, 0x11, 0x00, 0x8a, 0x03,  0x23, 0x11, 0x3c, 0x1b, 0x3f, 0x11, 0xf4, 0xf3,
        0x3f, 0x11, 0x58, 0x14, 0xc2, 0x04, 0x00, 0x83,  0x17, 0x22, 0x12, 0x07, 0x11, 0xa5, 0x12, 0x9b,
        0x1b, 0xb0, 0x12, 0xc6, 0x15, 0x65, 0x02, 0x00,  0xbb, 0x18, 0xc9, 0x17, 0xb3, 0x15, 0x1c, 0x0d,
        0xdb, 0x16, 0xf1, 0x1d, 0x01, 0x2b, 0xe7, 0x15,  0x00, 0x32, 0x2d, 0x0b, 0x22, 0x0f, 0x2d, 0xe8,
        0x19, 0x0c, 0x0d, 0x22, 0x27, 0x64, 0x14, 0x54,  0x2c, 0x00, 0x8a, 0x04, 0x20, 0x13, 0x84, 0x2c,
        0x94, 0x26, 0xe6, 0x16, 0xa1, 0x16, 0x77, 0x00,  0x77, 0x02, 0x00, 0xad, 0x16, 0x54, 0x2c, 0x62,
        0x16, 0xd7, 0x2d, 0x0a, 0x16, 0x6a, 0x2d, 0x5c,  0x29, 0x48, 0x2d, 0x00, 0xa5, 0x15, 0x4a, 0x14,
        0x47, 0x3d, 0x82, 0x13, 0x22, 0x12, 0x83, 0x12,  0x2a, 0x2a, 0xd6, 0x13, 0x00, 0x83, 0x16, 0x92,
        0x21, 0x03, 0x28, 0xc6, 0x23, 0xe9, 0x15, 0xcb,  0x21, 0x98, 0x36, 0xd6, 0x13, 0x00, 0x45, 0x34,
        0x9f, 0x32, 0x86, 0x35, 0xac, 0x35, 0x7d, 0x38,  0x85, 0x36, 0x91, 0x21, 0xbb, 0x19, 0x00, 0x69,
        0x00, 0x77, 0x00, 0xa7, 0x36, 0xd5, 0x18, 0x9f,  0x30, 0x12, 0x10, 0xd6, 0x14, 0x91, 0x24, 0x00,
        0xc2, 0x27, 0x9f, 0x32, 0xff, 0x28, 0x76, 0x00,  0x86, 0x15, 0xff, 0x32, 0x59, 0x33, 0x74, 0x04,
        0x00, 0x13, 0x45, 0x50, 0x44, 0x5b, 0x27, 0x75,  0x03, 0x67, 0x17, 0x9e, 0x13, 0x6d, 0x45, 0x90,
        0x28, 0x00, 0xd4, 0x15, 0x88, 0x49, 0x77, 0x02,  0x98, 0x47, 0x8a, 0x05, 0x6e, 0x06, 0xb3, 0x4d,
        0x72, 0x44, 0x00, 0xcb, 0x48, 0xce, 0x16, 0xf9,  0x4d, 0xee, 0x4d, 0x19, 0x5d, 0x0e, 0x5d, 0x1f,
        0x58, 0x4c, 0x46, 0x00, 0x4d, 0x5d, 0xd0, 0x24,  0x39, 0x36, 0x37, 0x5c, 0x5a, 0x46, 0x47, 0x5d,
        0x57, 0x49, 0x87, 0x5d, 0x00, 0x7b, 0x02, 0x3f,  0x48, 0xa7, 0x5d, 0x76, 0x35, 0xbf, 0x5d, 0xcf,
        0x22, 0x5b, 0x48, 0x36, 0x5d, 0x00, 0x2a, 0x25,  0xf9, 0x5d, 0x6e, 0x44, 0xf2, 0x5c, 0xc1, 0x09,
        0x86, 0x32, 0x7d, 0x02,
    };

    void doTest(afl::test::Assert a, const afl::base::ConstBytes_t compressed, const afl::base::ConstBytes_t original)
    {
        // Test 1: decompress in one go (test basic functionality)
        {
            uint8_t output[8192];
            afl::base::ConstBytes_t inputDescriptor(compressed);
            afl::base::Bytes_t outputDescriptor(output);
            afl::io::MsExpandTransform().transform(inputDescriptor, outputDescriptor);

            a.check("input empty", inputDescriptor.empty());

            afl::base::ConstBytes_t resultDescriptor(output);
            resultDescriptor.removeEnd(outputDescriptor.size());
            a.check("result content", resultDescriptor.equalContent(original));
        }

        // Test 2: decompress bytewise
        {
            afl::io::MsExpandTransform tx;
            afl::base::ConstBytes_t inputDescriptor(compressed);
            afl::base::ConstBytes_t outputDescriptor(original);
            while (1) {
                // Try to extract a byte
                uint8_t byte[1];
                afl::base::ConstBytes_t in;
                afl::base::Bytes_t out(byte);
                tx.transform(in, out);
                if (out.empty()) {
                    // Got a byte!
                    const uint8_t* p = outputDescriptor.eat();
                    a.checkNonNull("got byte", p);
                    a.checkEqual("byte value", byte[0], *p);
                } else if (const uint8_t* p = inputDescriptor.eat()) {
                    // We have another input byte. Push that in.
                    byte[0] = *p;
                    in = byte;
                    out = afl::base::Bytes_t();
                    tx.transform(in, out);
                    a.check("input pushed", in.empty());
                } else {
                    // No more input, test passed.
                    break;
                }
            }
        }
    }
}

AFL_TEST("afl.io.MsExpandTransform:random", a)
{
    doTest(a, COMPRESSED_RANDOM, ORIGINAL_RANDOM);
}

AFL_TEST("afl.io.MsExpandTransform:compressible", a)
{
    doTest(a, COMPRESSED_COMPRESSIBLE, ORIGINAL_COMPRESSIBLE);
}
