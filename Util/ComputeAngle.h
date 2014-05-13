/*

Copyright (c) 2013, Project OSRM, Dennis Luxen, others
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef COMPUTE_ANGLE_H
#define COMPUTE_ANGLE_H

#include "../Util/MercatorUtil.h"
#include <osrm/Coordinate.h>

#include <boost/assert.hpp>
#include <cmath>

constexpr unsigned short atan_table[4096] = {
0x0000,  0x0014,  0x0028,  0x003d,  0x0051,  0x0065,
0x007a,  0x008e,  0x00a3,  0x00b7,  0x00cb,  0x00e0,
0x00f4,  0x0108,  0x011d,  0x0131,  0x0146,  0x015a,
0x016e,  0x0183,  0x0197,  0x01ab,  0x01c0,  0x01d4,
0x01e9,  0x01fd,  0x0211,  0x0226,  0x023a,  0x024e,
0x0263,  0x0277,  0x028c,  0x02a0,  0x02b4,  0x02c9,
0x02dd,  0x02f1,  0x0306,  0x031a,  0x032f,  0x0343,
0x0357,  0x036c,  0x0380,  0x0394,  0x03a9,  0x03bd,
0x03d2,  0x03e6,  0x03fa,  0x040f,  0x0423,  0x0437,
0x044c,  0x0460,  0x0475,  0x0489,  0x049d,  0x04b2,
0x04c6,  0x04da,  0x04ef,  0x0503,  0x0517,  0x052c,
0x0540,  0x0555,  0x0569,  0x057d,  0x0592,  0x05a6,
0x05ba,  0x05cf,  0x05e3,  0x05f8,  0x060c,  0x0620,
0x0635,  0x0649,  0x065d,  0x0672,  0x0686,  0x069b,
0x06af,  0x06c3,  0x06d8,  0x06ec,  0x0700,  0x0715,
0x0729,  0x073d,  0x0752,  0x0766,  0x077b,  0x078f,
0x07a3,  0x07b8,  0x07cc,  0x07e0,  0x07f5,  0x0809,
0x081d,  0x0832,  0x0846,  0x085b,  0x086f,  0x0883,
0x0898,  0x08ac,  0x08c0,  0x08d5,  0x08e9,  0x08fd,
0x0912,  0x0926,  0x093b,  0x094f,  0x0963,  0x0978,
0x098c,  0x09a0,  0x09b5,  0x09c9,  0x09dd,  0x09f2,
0x0a06,  0x0a1a,  0x0a2f,  0x0a43,  0x0a58,  0x0a6c,
0x0a80,  0x0a95,  0x0aa9,  0x0abd,  0x0ad2,  0x0ae6,
0x0afa,  0x0b0f,  0x0b23,  0x0b37,  0x0b4c,  0x0b60,
0x0b75,  0x0b89,  0x0b9d,  0x0bb2,  0x0bc6,  0x0bda,
0x0bef,  0x0c03,  0x0c17,  0x0c2c,  0x0c40,  0x0c54,
0x0c69,  0x0c7d,  0x0c91,  0x0ca6,  0x0cba,  0x0cce,
0x0ce3,  0x0cf7,  0x0d0b,  0x0d20,  0x0d34,  0x0d48,
0x0d5d,  0x0d71,  0x0d86,  0x0d9a,  0x0dae,  0x0dc3,
0x0dd7,  0x0deb,  0x0e00,  0x0e14,  0x0e28,  0x0e3d,
0x0e51,  0x0e65,  0x0e7a,  0x0e8e,  0x0ea2,  0x0eb7,
0x0ecb,  0x0edf,  0x0ef4,  0x0f08,  0x0f1c,  0x0f31,
0x0f45,  0x0f59,  0x0f6e,  0x0f82,  0x0f96,  0x0fab,
0x0fbf,  0x0fd3,  0x0fe8,  0x0ffc,  0x1010,  0x1025,
0x1039,  0x104d,  0x1062,  0x1076,  0x108a,  0x109e,
0x10b3,  0x10c7,  0x10db,  0x10f0,  0x1104,  0x1118,
0x112d,  0x1141,  0x1155,  0x116a,  0x117e,  0x1192,
0x11a7,  0x11bb,  0x11cf,  0x11e4,  0x11f8,  0x120c,
0x1221,  0x1235,  0x1249,  0x125d,  0x1272,  0x1286,
0x129a,  0x12af,  0x12c3,  0x12d7,  0x12ec,  0x1300,
0x1314,  0x1329,  0x133d,  0x1351,  0x1365,  0x137a,
0x138e,  0x13a2,  0x13b7,  0x13cb,  0x13df,  0x13f4,
0x1408,  0x141c,  0x1431,  0x1445,  0x1459,  0x146d,
0x1482,  0x1496,  0x14aa,  0x14bf,  0x14d3,  0x14e7,
0x14fb,  0x1510,  0x1524,  0x1538,  0x154d,  0x1561,
0x1575,  0x1589,  0x159e,  0x15b2,  0x15c6,  0x15db,
0x15ef,  0x1603,  0x1617,  0x162c,  0x1640,  0x1654,
0x1669,  0x167d,  0x1691,  0x16a5,  0x16ba,  0x16ce,
0x16e2,  0x16f7,  0x170b,  0x171f,  0x1733,  0x1748,
0x175c,  0x1770,  0x1784,  0x1799,  0x17ad,  0x17c1,
0x17d6,  0x17ea,  0x17fe,  0x1812,  0x1827,  0x183b,
0x184f,  0x1863,  0x1878,  0x188c,  0x18a0,  0x18b4,
0x18c9,  0x18dd,  0x18f1,  0x1905,  0x191a,  0x192e,
0x1942,  0x1957,  0x196b,  0x197f,  0x1993,  0x19a8,
0x19bc,  0x19d0,  0x19e4,  0x19f9,  0x1a0d,  0x1a21,
0x1a35,  0x1a49,  0x1a5e,  0x1a72,  0x1a86,  0x1a9a,
0x1aaf,  0x1ac3,  0x1ad7,  0x1aeb,  0x1b00,  0x1b14,
0x1b28,  0x1b3c,  0x1b51,  0x1b65,  0x1b79,  0x1b8d,
0x1ba2,  0x1bb6,  0x1bca,  0x1bde,  0x1bf2,  0x1c07,
0x1c1b,  0x1c2f,  0x1c43,  0x1c58,  0x1c6c,  0x1c80,
0x1c94,  0x1ca8,  0x1cbd,  0x1cd1,  0x1ce5,  0x1cf9,
0x1d0e,  0x1d22,  0x1d36,  0x1d4a,  0x1d5e,  0x1d73,
0x1d87,  0x1d9b,  0x1daf,  0x1dc3,  0x1dd8,  0x1dec,
0x1e00,  0x1e14,  0x1e28,  0x1e3d,  0x1e51,  0x1e65,
0x1e79,  0x1e8d,  0x1ea2,  0x1eb6,  0x1eca,  0x1ede,
0x1ef2,  0x1f07,  0x1f1b,  0x1f2f,  0x1f43,  0x1f57,
0x1f6c,  0x1f80,  0x1f94,  0x1fa8,  0x1fbc,  0x1fd1,
0x1fe5,  0x1ff9,  0x200d,  0x2021,  0x2035,  0x204a,
0x205e,  0x2072,  0x2086,  0x209a,  0x20ae,  0x20c3,
0x20d7,  0x20eb,  0x20ff,  0x2113,  0x2127,  0x213c,
0x2150,  0x2164,  0x2178,  0x218c,  0x21a0,  0x21b5,
0x21c9,  0x21dd,  0x21f1,  0x2205,  0x2219,  0x222e,
0x2242,  0x2256,  0x226a,  0x227e,  0x2292,  0x22a6,
0x22bb,  0x22cf,  0x22e3,  0x22f7,  0x230b,  0x231f,
0x2333,  0x2348,  0x235c,  0x2370,  0x2384,  0x2398,
0x23ac,  0x23c0,  0x23d5,  0x23e9,  0x23fd,  0x2411,
0x2425,  0x2439,  0x244d,  0x2461,  0x2476,  0x248a,
0x249e,  0x24b2,  0x24c6,  0x24da,  0x24ee,  0x2502,
0x2517,  0x252b,  0x253f,  0x2553,  0x2567,  0x257b,
0x258f,  0x25a3,  0x25b7,  0x25cb,  0x25e0,  0x25f4,
0x2608,  0x261c,  0x2630,  0x2644,  0x2658,  0x266c,
0x2680,  0x2694,  0x26a9,  0x26bd,  0x26d1,  0x26e5,
0x26f9,  0x270d,  0x2721,  0x2735,  0x2749,  0x275d,
0x2771,  0x2785,  0x279a,  0x27ae,  0x27c2,  0x27d6,
0x27ea,  0x27fe,  0x2812,  0x2826,  0x283a,  0x284e,
0x2862,  0x2876,  0x288a,  0x289e,  0x28b3,  0x28c7,
0x28db,  0x28ef,  0x2903,  0x2917,  0x292b,  0x293f,
0x2953,  0x2967,  0x297b,  0x298f,  0x29a3,  0x29b7,
0x29cb,  0x29df,  0x29f3,  0x2a07,  0x2a1b,  0x2a2f,
0x2a43,  0x2a58,  0x2a6c,  0x2a80,  0x2a94,  0x2aa8,
0x2abc,  0x2ad0,  0x2ae4,  0x2af8,  0x2b0c,  0x2b20,
0x2b34,  0x2b48,  0x2b5c,  0x2b70,  0x2b84,  0x2b98,
0x2bac,  0x2bc0,  0x2bd4,  0x2be8,  0x2bfc,  0x2c10,
0x2c24,  0x2c38,  0x2c4c,  0x2c60,  0x2c74,  0x2c88,
0x2c9c,  0x2cb0,  0x2cc4,  0x2cd8,  0x2cec,  0x2d00,
0x2d14,  0x2d28,  0x2d3c,  0x2d50,  0x2d64,  0x2d78,
0x2d8c,  0x2da0,  0x2db4,  0x2dc8,  0x2ddc,  0x2df0,
0x2e04,  0x2e18,  0x2e2c,  0x2e40,  0x2e54,  0x2e68,
0x2e7c,  0x2e90,  0x2ea3,  0x2eb7,  0x2ecb,  0x2edf,
0x2ef3,  0x2f07,  0x2f1b,  0x2f2f,  0x2f43,  0x2f57,
0x2f6b,  0x2f7f,  0x2f93,  0x2fa7,  0x2fbb,  0x2fcf,
0x2fe3,  0x2ff7,  0x300b,  0x301e,  0x3032,  0x3046,
0x305a,  0x306e,  0x3082,  0x3096,  0x30aa,  0x30be,
0x30d2,  0x30e6,  0x30fa,  0x310e,  0x3122,  0x3135,
0x3149,  0x315d,  0x3171,  0x3185,  0x3199,  0x31ad,
0x31c1,  0x31d5,  0x31e9,  0x31fd,  0x3210,  0x3224,
0x3238,  0x324c,  0x3260,  0x3274,  0x3288,  0x329c,
0x32b0,  0x32c3,  0x32d7,  0x32eb,  0x32ff,  0x3313,
0x3327,  0x333b,  0x334f,  0x3363,  0x3376,  0x338a,
0x339e,  0x33b2,  0x33c6,  0x33da,  0x33ee,  0x3401,
0x3415,  0x3429,  0x343d,  0x3451,  0x3465,  0x3479,
0x348c,  0x34a0,  0x34b4,  0x34c8,  0x34dc,  0x34f0,
0x3504,  0x3517,  0x352b,  0x353f,  0x3553,  0x3567,
0x357b,  0x358e,  0x35a2,  0x35b6,  0x35ca,  0x35de,
0x35f2,  0x3605,  0x3619,  0x362d,  0x3641,  0x3655,
0x3668,  0x367c,  0x3690,  0x36a4,  0x36b8,  0x36cb,
0x36df,  0x36f3,  0x3707,  0x371b,  0x372f,  0x3742,
0x3756,  0x376a,  0x377e,  0x3791,  0x37a5,  0x37b9,
0x37cd,  0x37e1,  0x37f4,  0x3808,  0x381c,  0x3830,
0x3844,  0x3857,  0x386b,  0x387f,  0x3893,  0x38a6,
0x38ba,  0x38ce,  0x38e2,  0x38f5,  0x3909,  0x391d,
0x3931,  0x3944,  0x3958,  0x396c,  0x3980,  0x3993,
0x39a7,  0x39bb,  0x39cf,  0x39e2,  0x39f6,  0x3a0a,
0x3a1e,  0x3a31,  0x3a45,  0x3a59,  0x3a6d,  0x3a80,
0x3a94,  0x3aa8,  0x3abb,  0x3acf,  0x3ae3,  0x3af7,
0x3b0a,  0x3b1e,  0x3b32,  0x3b45,  0x3b59,  0x3b6d,
0x3b81,  0x3b94,  0x3ba8,  0x3bbc,  0x3bcf,  0x3be3,
0x3bf7,  0x3c0b,  0x3c1e,  0x3c32,  0x3c46,  0x3c59,
0x3c6d,  0x3c81,  0x3c94,  0x3ca8,  0x3cbc,  0x3ccf,
0x3ce3,  0x3cf7,  0x3d0a,  0x3d1e,  0x3d32,  0x3d45,
0x3d59,  0x3d6d,  0x3d80,  0x3d94,  0x3da8,  0x3dbb,
0x3dcf,  0x3de3,  0x3df6,  0x3e0a,  0x3e1e,  0x3e31,
0x3e45,  0x3e59,  0x3e6c,  0x3e80,  0x3e93,  0x3ea7,
0x3ebb,  0x3ece,  0x3ee2,  0x3ef6,  0x3f09,  0x3f1d,
0x3f30,  0x3f44,  0x3f58,  0x3f6b,  0x3f7f,  0x3f93,
0x3fa6,  0x3fba,  0x3fcd,  0x3fe1,  0x3ff5,  0x4008,
0x401c,  0x402f,  0x4043,  0x4057,  0x406a,  0x407e,
0x4091,  0x40a5,  0x40b8,  0x40cc,  0x40e0,  0x40f3,
0x4107,  0x411a,  0x412e,  0x4142,  0x4155,  0x4169,
0x417c,  0x4190,  0x41a3,  0x41b7,  0x41ca,  0x41de,
0x41f2,  0x4205,  0x4219,  0x422c,  0x4240,  0x4253,
0x4267,  0x427a,  0x428e,  0x42a1,  0x42b5,  0x42c9,
0x42dc,  0x42f0,  0x4303,  0x4317,  0x432a,  0x433e,
0x4351,  0x4365,  0x4378,  0x438c,  0x439f,  0x43b3,
0x43c6,  0x43da,  0x43ed,  0x4401,  0x4414,  0x4428,
0x443b,  0x444f,  0x4462,  0x4476,  0x4489,  0x449d,
0x44b0,  0x44c4,  0x44d7,  0x44eb,  0x44fe,  0x4512,
0x4525,  0x4539,  0x454c,  0x4560,  0x4573,  0x4586,
0x459a,  0x45ad,  0x45c1,  0x45d4,  0x45e8,  0x45fb,
0x460f,  0x4622,  0x4636,  0x4649,  0x465c,  0x4670,
0x4683,  0x4697,  0x46aa,  0x46be,  0x46d1,  0x46e5,
0x46f8,  0x470b,  0x471f,  0x4732,  0x4746,  0x4759,
0x476c,  0x4780,  0x4793,  0x47a7,  0x47ba,  0x47cd,
0x47e1,  0x47f4,  0x4808,  0x481b,  0x482e,  0x4842,
0x4855,  0x4869,  0x487c,  0x488f,  0x48a3,  0x48b6,
0x48ca,  0x48dd,  0x48f0,  0x4904,  0x4917,  0x492a,
0x493e,  0x4951,  0x4965,  0x4978,  0x498b,  0x499f,
0x49b2,  0x49c5,  0x49d9,  0x49ec,  0x49ff,  0x4a13,
0x4a26,  0x4a39,  0x4a4d,  0x4a60,  0x4a73,  0x4a87,
0x4a9a,  0x4aad,  0x4ac1,  0x4ad4,  0x4ae7,  0x4afb,
0x4b0e,  0x4b21,  0x4b35,  0x4b48,  0x4b5b,  0x4b6f,
0x4b82,  0x4b95,  0x4ba8,  0x4bbc,  0x4bcf,  0x4be2,
0x4bf6,  0x4c09,  0x4c1c,  0x4c2f,  0x4c43,  0x4c56,
0x4c69,  0x4c7d,  0x4c90,  0x4ca3,  0x4cb6,  0x4cca,
0x4cdd,  0x4cf0,  0x4d03,  0x4d17,  0x4d2a,  0x4d3d,
0x4d50,  0x4d64,  0x4d77,  0x4d8a,  0x4d9d,  0x4db1,
0x4dc4,  0x4dd7,  0x4dea,  0x4dfe,  0x4e11,  0x4e24,
0x4e37,  0x4e4b,  0x4e5e,  0x4e71,  0x4e84,  0x4e97,
0x4eab,  0x4ebe,  0x4ed1,  0x4ee4,  0x4ef7,  0x4f0b,
0x4f1e,  0x4f31,  0x4f44,  0x4f57,  0x4f6b,  0x4f7e,
0x4f91,  0x4fa4,  0x4fb7,  0x4fcb,  0x4fde,  0x4ff1,
0x5004,  0x5017,  0x502a,  0x503e,  0x5051,  0x5064,
0x5077,  0x508a,  0x509d,  0x50b1,  0x50c4,  0x50d7,
0x50ea,  0x50fd,  0x5110,  0x5123,  0x5137,  0x514a,
0x515d,  0x5170,  0x5183,  0x5196,  0x51a9,  0x51bc,
0x51d0,  0x51e3,  0x51f6,  0x5209,  0x521c,  0x522f,
0x5242,  0x5255,  0x5268,  0x527c,  0x528f,  0x52a2,
0x52b5,  0x52c8,  0x52db,  0x52ee,  0x5301,  0x5314,
0x5327,  0x533a,  0x534e,  0x5361,  0x5374,  0x5387,
0x539a,  0x53ad,  0x53c0,  0x53d3,  0x53e6,  0x53f9,
0x540c,  0x541f,  0x5432,  0x5445,  0x5458,  0x546b,
0x547e,  0x5491,  0x54a5,  0x54b8,  0x54cb,  0x54de,
0x54f1,  0x5504,  0x5517,  0x552a,  0x553d,  0x5550,
0x5563,  0x5576,  0x5589,  0x559c,  0x55af,  0x55c2,
0x55d5,  0x55e8,  0x55fb,  0x560e,  0x5621,  0x5634,
0x5647,  0x565a,  0x566d,  0x5680,  0x5693,  0x56a6,
0x56b9,  0x56cb,  0x56de,  0x56f1,  0x5704,  0x5717,
0x572a,  0x573d,  0x5750,  0x5763,  0x5776,  0x5789,
0x579c,  0x57af,  0x57c2,  0x57d5,  0x57e8,  0x57fb,
0x580e,  0x5820,  0x5833,  0x5846,  0x5859,  0x586c,
0x587f,  0x5892,  0x58a5,  0x58b8,  0x58cb,  0x58de,
0x58f0,  0x5903,  0x5916,  0x5929,  0x593c,  0x594f,
0x5962,  0x5975,  0x5988,  0x599a,  0x59ad,  0x59c0,
0x59d3,  0x59e6,  0x59f9,  0x5a0c,  0x5a1f,  0x5a31,
0x5a44,  0x5a57,  0x5a6a,  0x5a7d,  0x5a90,  0x5aa2,
0x5ab5,  0x5ac8,  0x5adb,  0x5aee,  0x5b01,  0x5b13,
0x5b26,  0x5b39,  0x5b4c,  0x5b5f,  0x5b72,  0x5b84,
0x5b97,  0x5baa,  0x5bbd,  0x5bd0,  0x5be2,  0x5bf5,
0x5c08,  0x5c1b,  0x5c2e,  0x5c40,  0x5c53,  0x5c66,
0x5c79,  0x5c8c,  0x5c9e,  0x5cb1,  0x5cc4,  0x5cd7,
0x5ce9,  0x5cfc,  0x5d0f,  0x5d22,  0x5d34,  0x5d47,
0x5d5a,  0x5d6d,  0x5d7f,  0x5d92,  0x5da5,  0x5db8,
0x5dca,  0x5ddd,  0x5df0,  0x5e03,  0x5e15,  0x5e28,
0x5e3b,  0x5e4d,  0x5e60,  0x5e73,  0x5e86,  0x5e98,
0x5eab,  0x5ebe,  0x5ed0,  0x5ee3,  0x5ef6,  0x5f09,
0x5f1b,  0x5f2e,  0x5f41,  0x5f53,  0x5f66,  0x5f79,
0x5f8b,  0x5f9e,  0x5fb1,  0x5fc3,  0x5fd6,  0x5fe9,
0x5ffb,  0x600e,  0x6021,  0x6033,  0x6046,  0x6059,
0x606b,  0x607e,  0x6091,  0x60a3,  0x60b6,  0x60c8,
0x60db,  0x60ee,  0x6100,  0x6113,  0x6126,  0x6138,
0x614b,  0x615d,  0x6170,  0x6183,  0x6195,  0x61a8,
0x61ba,  0x61cd,  0x61e0,  0x61f2,  0x6205,  0x6217,
0x622a,  0x623d,  0x624f,  0x6262,  0x6274,  0x6287,
0x6299,  0x62ac,  0x62bf,  0x62d1,  0x62e4,  0x62f6,
0x6309,  0x631b,  0x632e,  0x6340,  0x6353,  0x6366,
0x6378,  0x638b,  0x639d,  0x63b0,  0x63c2,  0x63d5,
0x63e7,  0x63fa,  0x640c,  0x641f,  0x6431,  0x6444,
0x6456,  0x6469,  0x647b,  0x648e,  0x64a0,  0x64b3,
0x64c5,  0x64d8,  0x64ea,  0x64fd,  0x650f,  0x6522,
0x6534,  0x6547,  0x6559,  0x656c,  0x657e,  0x6591,
0x65a3,  0x65b5,  0x65c8,  0x65da,  0x65ed,  0x65ff,
0x6612,  0x6624,  0x6637,  0x6649,  0x665b,  0x666e,
0x6680,  0x6693,  0x66a5,  0x66b8,  0x66ca,  0x66dc,
0x66ef,  0x6701,  0x6714,  0x6726,  0x6738,  0x674b,
0x675d,  0x6770,  0x6782,  0x6794,  0x67a7,  0x67b9,
0x67cc,  0x67de,  0x67f0,  0x6803,  0x6815,  0x6827,
0x683a,  0x684c,  0x685e,  0x6871,  0x6883,  0x6896,
0x68a8,  0x68ba,  0x68cd,  0x68df,  0x68f1,  0x6904,
0x6916,  0x6928,  0x693b,  0x694d,  0x695f,  0x6972,
0x6984,  0x6996,  0x69a8,  0x69bb,  0x69cd,  0x69df,
0x69f2,  0x6a04,  0x6a16,  0x6a29,  0x6a3b,  0x6a4d,
0x6a5f,  0x6a72,  0x6a84,  0x6a96,  0x6aa9,  0x6abb,
0x6acd,  0x6adf,  0x6af2,  0x6b04,  0x6b16,  0x6b28,
0x6b3b,  0x6b4d,  0x6b5f,  0x6b71,  0x6b84,  0x6b96,
0x6ba8,  0x6bba,  0x6bcd,  0x6bdf,  0x6bf1,  0x6c03,
0x6c15,  0x6c28,  0x6c3a,  0x6c4c,  0x6c5e,  0x6c70,
0x6c83,  0x6c95,  0x6ca7,  0x6cb9,  0x6ccb,  0x6cde,
0x6cf0,  0x6d02,  0x6d14,  0x6d26,  0x6d39,  0x6d4b,
0x6d5d,  0x6d6f,  0x6d81,  0x6d93,  0x6da6,  0x6db8,
0x6dca,  0x6ddc,  0x6dee,  0x6e00,  0x6e12,  0x6e25,
0x6e37,  0x6e49,  0x6e5b,  0x6e6d,  0x6e7f,  0x6e91,
0x6ea3,  0x6eb6,  0x6ec8,  0x6eda,  0x6eec,  0x6efe,
0x6f10,  0x6f22,  0x6f34,  0x6f46,  0x6f58,  0x6f6b,
0x6f7d,  0x6f8f,  0x6fa1,  0x6fb3,  0x6fc5,  0x6fd7,
0x6fe9,  0x6ffb,  0x700d,  0x701f,  0x7031,  0x7043,
0x7055,  0x7068,  0x707a,  0x708c,  0x709e,  0x70b0,
0x70c2,  0x70d4,  0x70e6,  0x70f8,  0x710a,  0x711c,
0x712e,  0x7140,  0x7152,  0x7164,  0x7176,  0x7188,
0x719a,  0x71ac,  0x71be,  0x71d0,  0x71e2,  0x71f4,
0x7206,  0x7218,  0x722a,  0x723c,  0x724e,  0x7260,
0x7272,  0x7284,  0x7296,  0x72a8,  0x72ba,  0x72cc,
0x72dd,  0x72ef,  0x7301,  0x7313,  0x7325,  0x7337,
0x7349,  0x735b,  0x736d,  0x737f,  0x7391,  0x73a3,
0x73b5,  0x73c7,  0x73d8,  0x73ea,  0x73fc,  0x740e,
0x7420,  0x7432,  0x7444,  0x7456,  0x7468,  0x747a,
0x748b,  0x749d,  0x74af,  0x74c1,  0x74d3,  0x74e5,
0x74f7,  0x7509,  0x751a,  0x752c,  0x753e,  0x7550,
0x7562,  0x7574,  0x7585,  0x7597,  0x75a9,  0x75bb,
0x75cd,  0x75df,  0x75f0,  0x7602,  0x7614,  0x7626,
0x7638,  0x764a,  0x765b,  0x766d,  0x767f,  0x7691,
0x76a3,  0x76b4,  0x76c6,  0x76d8,  0x76ea,  0x76fb,
0x770d,  0x771f,  0x7731,  0x7743,  0x7754,  0x7766,
0x7778,  0x778a,  0x779b,  0x77ad,  0x77bf,  0x77d1,
0x77e2,  0x77f4,  0x7806,  0x7818,  0x7829,  0x783b,
0x784d,  0x785e,  0x7870,  0x7882,  0x7894,  0x78a5,
0x78b7,  0x78c9,  0x78da,  0x78ec,  0x78fe,  0x7910,
0x7921,  0x7933,  0x7945,  0x7956,  0x7968,  0x797a,
0x798b,  0x799d,  0x79af,  0x79c0,  0x79d2,  0x79e4,
0x79f5,  0x7a07,  0x7a19,  0x7a2a,  0x7a3c,  0x7a4e,
0x7a5f,  0x7a71,  0x7a82,  0x7a94,  0x7aa6,  0x7ab7,
0x7ac9,  0x7adb,  0x7aec,  0x7afe,  0x7b0f,  0x7b21,
0x7b33,  0x7b44,  0x7b56,  0x7b67,  0x7b79,  0x7b8b,
0x7b9c,  0x7bae,  0x7bbf,  0x7bd1,  0x7be2,  0x7bf4,
0x7c06,  0x7c17,  0x7c29,  0x7c3a,  0x7c4c,  0x7c5d,
0x7c6f,  0x7c81,  0x7c92,  0x7ca4,  0x7cb5,  0x7cc7,
0x7cd8,  0x7cea,  0x7cfb,  0x7d0d,  0x7d1e,  0x7d30,
0x7d41,  0x7d53,  0x7d64,  0x7d76,  0x7d87,  0x7d99,
0x7daa,  0x7dbc,  0x7dcd,  0x7ddf,  0x7df0,  0x7e02,
0x7e13,  0x7e25,  0x7e36,  0x7e48,  0x7e59,  0x7e6b,
0x7e7c,  0x7e8e,  0x7e9f,  0x7eb0,  0x7ec2,  0x7ed3,
0x7ee5,  0x7ef6,  0x7f08,  0x7f19,  0x7f2b,  0x7f3c,
0x7f4d,  0x7f5f,  0x7f70,  0x7f82,  0x7f93,  0x7fa4,
0x7fb6,  0x7fc7,  0x7fd9,  0x7fea,  0x7ffb,  0x800d,
0x801e,  0x8030,  0x8041,  0x8052,  0x8064,  0x8075,
0x8086,  0x8098,  0x80a9,  0x80bb,  0x80cc,  0x80dd,
0x80ef,  0x8100,  0x8111,  0x8123,  0x8134,  0x8145,
0x8157,  0x8168,  0x8179,  0x818b,  0x819c,  0x81ad,
0x81bf,  0x81d0,  0x81e1,  0x81f3,  0x8204,  0x8215,
0x8226,  0x8238,  0x8249,  0x825a,  0x826c,  0x827d,
0x828e,  0x829f,  0x82b1,  0x82c2,  0x82d3,  0x82e5,
0x82f6,  0x8307,  0x8318,  0x832a,  0x833b,  0x834c,
0x835d,  0x836f,  0x8380,  0x8391,  0x83a2,  0x83b3,
0x83c5,  0x83d6,  0x83e7,  0x83f8,  0x840a,  0x841b,
0x842c,  0x843d,  0x844e,  0x8460,  0x8471,  0x8482,
0x8493,  0x84a4,  0x84b6,  0x84c7,  0x84d8,  0x84e9,
0x84fa,  0x850b,  0x851d,  0x852e,  0x853f,  0x8550,
0x8561,  0x8572,  0x8584,  0x8595,  0x85a6,  0x85b7,
0x85c8,  0x85d9,  0x85ea,  0x85fb,  0x860d,  0x861e,
0x862f,  0x8640,  0x8651,  0x8662,  0x8673,  0x8684,
0x8695,  0x86a7,  0x86b8,  0x86c9,  0x86da,  0x86eb,
0x86fc,  0x870d,  0x871e,  0x872f,  0x8740,  0x8751,
0x8762,  0x8773,  0x8784,  0x8796,  0x87a7,  0x87b8,
0x87c9,  0x87da,  0x87eb,  0x87fc,  0x880d,  0x881e,
0x882f,  0x8840,  0x8851,  0x8862,  0x8873,  0x8884,
0x8895,  0x88a6,  0x88b7,  0x88c8,  0x88d9,  0x88ea,
0x88fb,  0x890c,  0x891d,  0x892e,  0x893f,  0x8950,
0x8961,  0x8972,  0x8983,  0x8994,  0x89a5,  0x89b6,
0x89c6,  0x89d7,  0x89e8,  0x89f9,  0x8a0a,  0x8a1b,
0x8a2c,  0x8a3d,  0x8a4e,  0x8a5f,  0x8a70,  0x8a81,
0x8a92,  0x8aa3,  0x8ab3,  0x8ac4,  0x8ad5,  0x8ae6,
0x8af7,  0x8b08,  0x8b19,  0x8b2a,  0x8b3b,  0x8b4b,
0x8b5c,  0x8b6d,  0x8b7e,  0x8b8f,  0x8ba0,  0x8bb1,
0x8bc1,  0x8bd2,  0x8be3,  0x8bf4,  0x8c05,  0x8c16,
0x8c27,  0x8c37,  0x8c48,  0x8c59,  0x8c6a,  0x8c7b,
0x8c8c,  0x8c9c,  0x8cad,  0x8cbe,  0x8ccf,  0x8ce0,
0x8cf0,  0x8d01,  0x8d12,  0x8d23,  0x8d34,  0x8d44,
0x8d55,  0x8d66,  0x8d77,  0x8d87,  0x8d98,  0x8da9,
0x8dba,  0x8dca,  0x8ddb,  0x8dec,  0x8dfd,  0x8e0d,
0x8e1e,  0x8e2f,  0x8e40,  0x8e50,  0x8e61,  0x8e72,
0x8e83,  0x8e93,  0x8ea4,  0x8eb5,  0x8ec5,  0x8ed6,
0x8ee7,  0x8ef8,  0x8f08,  0x8f19,  0x8f2a,  0x8f3a,
0x8f4b,  0x8f5c,  0x8f6c,  0x8f7d,  0x8f8e,  0x8f9e,
0x8faf,  0x8fc0,  0x8fd0,  0x8fe1,  0x8ff2,  0x9002,
0x9013,  0x9024,  0x9034,  0x9045,  0x9056,  0x9066,
0x9077,  0x9088,  0x9098,  0x90a9,  0x90b9,  0x90ca,
0x90db,  0x90eb,  0x90fc,  0x910c,  0x911d,  0x912e,
0x913e,  0x914f,  0x915f,  0x9170,  0x9181,  0x9191,
0x91a2,  0x91b2,  0x91c3,  0x91d3,  0x91e4,  0x91f5,
0x9205,  0x9216,  0x9226,  0x9237,  0x9247,  0x9258,
0x9268,  0x9279,  0x9289,  0x929a,  0x92aa,  0x92bb,
0x92cc,  0x92dc,  0x92ed,  0x92fd,  0x930e,  0x931e,
0x932f,  0x933f,  0x9350,  0x9360,  0x9370,  0x9381,
0x9391,  0x93a2,  0x93b2,  0x93c3,  0x93d3,  0x93e4,
0x93f4,  0x9405,  0x9415,  0x9426,  0x9436,  0x9447,
0x9457,  0x9467,  0x9478,  0x9488,  0x9499,  0x94a9,
0x94ba,  0x94ca,  0x94da,  0x94eb,  0x94fb,  0x950c,
0x951c,  0x952c,  0x953d,  0x954d,  0x955e,  0x956e,
0x957e,  0x958f,  0x959f,  0x95af,  0x95c0,  0x95d0,
0x95e1,  0x95f1,  0x9601,  0x9612,  0x9622,  0x9632,
0x9643,  0x9653,  0x9663,  0x9674,  0x9684,  0x9694,
0x96a5,  0x96b5,  0x96c5,  0x96d6,  0x96e6,  0x96f6,
0x9707,  0x9717,  0x9727,  0x9738,  0x9748,  0x9758,
0x9768,  0x9779,  0x9789,  0x9799,  0x97aa,  0x97ba,
0x97ca,  0x97da,  0x97eb,  0x97fb,  0x980b,  0x981b,
0x982c,  0x983c,  0x984c,  0x985c,  0x986d,  0x987d,
0x988d,  0x989d,  0x98ad,  0x98be,  0x98ce,  0x98de,
0x98ee,  0x98ff,  0x990f,  0x991f,  0x992f,  0x993f,
0x9950,  0x9960,  0x9970,  0x9980,  0x9990,  0x99a0,
0x99b1,  0x99c1,  0x99d1,  0x99e1,  0x99f1,  0x9a01,
0x9a12,  0x9a22,  0x9a32,  0x9a42,  0x9a52,  0x9a62,
0x9a72,  0x9a83,  0x9a93,  0x9aa3,  0x9ab3,  0x9ac3,
0x9ad3,  0x9ae3,  0x9af3,  0x9b04,  0x9b14,  0x9b24,
0x9b34,  0x9b44,  0x9b54,  0x9b64,  0x9b74,  0x9b84,
0x9b94,  0x9ba4,  0x9bb5,  0x9bc5,  0x9bd5,  0x9be5,
0x9bf5,  0x9c05,  0x9c15,  0x9c25,  0x9c35,  0x9c45,
0x9c55,  0x9c65,  0x9c75,  0x9c85,  0x9c95,  0x9ca5,
0x9cb5,  0x9cc5,  0x9cd5,  0x9ce5,  0x9cf5,  0x9d05,
0x9d15,  0x9d25,  0x9d35,  0x9d45,  0x9d55,  0x9d65,
0x9d75,  0x9d85,  0x9d95,  0x9da5,  0x9db5,  0x9dc5,
0x9dd5,  0x9de5,  0x9df5,  0x9e05,  0x9e15,  0x9e25,
0x9e35,  0x9e45,  0x9e55,  0x9e65,  0x9e74,  0x9e84,
0x9e94,  0x9ea4,  0x9eb4,  0x9ec4,  0x9ed4,  0x9ee4,
0x9ef4,  0x9f04,  0x9f14,  0x9f23,  0x9f33,  0x9f43,
0x9f53,  0x9f63,  0x9f73,  0x9f83,  0x9f93,  0x9fa3,
0x9fb2,  0x9fc2,  0x9fd2,  0x9fe2,  0x9ff2,  0xa002,
0xa012,  0xa021,  0xa031,  0xa041,  0xa051,  0xa061,
0xa071,  0xa080,  0xa090,  0xa0a0,  0xa0b0,  0xa0c0,
0xa0cf,  0xa0df,  0xa0ef,  0xa0ff,  0xa10f,  0xa11e,
0xa12e,  0xa13e,  0xa14e,  0xa15e,  0xa16d,  0xa17d,
0xa18d,  0xa19d,  0xa1ac,  0xa1bc,  0xa1cc,  0xa1dc,
0xa1eb,  0xa1fb,  0xa20b,  0xa21b,  0xa22a,  0xa23a,
0xa24a,  0xa25a,  0xa269,  0xa279,  0xa289,  0xa298,
0xa2a8,  0xa2b8,  0xa2c8,  0xa2d7,  0xa2e7,  0xa2f7,
0xa306,  0xa316,  0xa326,  0xa335,  0xa345,  0xa355,
0xa364,  0xa374,  0xa384,  0xa393,  0xa3a3,  0xa3b3,
0xa3c2,  0xa3d2,  0xa3e2,  0xa3f1,  0xa401,  0xa411,
0xa420,  0xa430,  0xa440,  0xa44f,  0xa45f,  0xa46e,
0xa47e,  0xa48e,  0xa49d,  0xa4ad,  0xa4bc,  0xa4cc,
0xa4dc,  0xa4eb,  0xa4fb,  0xa50a,  0xa51a,  0xa52a,
0xa539,  0xa549,  0xa558,  0xa568,  0xa577,  0xa587,
0xa597,  0xa5a6,  0xa5b6,  0xa5c5,  0xa5d5,  0xa5e4,
0xa5f4,  0xa603,  0xa613,  0xa622,  0xa632,  0xa641,
0xa651,  0xa660,  0xa670,  0xa67f,  0xa68f,  0xa69e,
0xa6ae,  0xa6bd,  0xa6cd,  0xa6dc,  0xa6ec,  0xa6fb,
0xa70b,  0xa71a,  0xa72a,  0xa739,  0xa749,  0xa758,
0xa768,  0xa777,  0xa787,  0xa796,  0xa7a5,  0xa7b5,
0xa7c4,  0xa7d4,  0xa7e3,  0xa7f3,  0xa802,  0xa812,
0xa821,  0xa830,  0xa840,  0xa84f,  0xa85f,  0xa86e,
0xa87d,  0xa88d,  0xa89c,  0xa8ac,  0xa8bb,  0xa8ca,
0xa8da,  0xa8e9,  0xa8f8,  0xa908,  0xa917,  0xa927,
0xa936,  0xa945,  0xa955,  0xa964,  0xa973,  0xa983,
0xa992,  0xa9a1,  0xa9b1,  0xa9c0,  0xa9cf,  0xa9df,
0xa9ee,  0xa9fd,  0xaa0d,  0xaa1c,  0xaa2b,  0xaa3b,
0xaa4a,  0xaa59,  0xaa69,  0xaa78,  0xaa87,  0xaa96,
0xaaa6,  0xaab5,  0xaac4,  0xaad4,  0xaae3,  0xaaf2,
0xab01,  0xab11,  0xab20,  0xab2f,  0xab3e,  0xab4e,
0xab5d,  0xab6c,  0xab7b,  0xab8b,  0xab9a,  0xaba9,
0xabb8,  0xabc7,  0xabd7,  0xabe6,  0xabf5,  0xac04,
0xac14,  0xac23,  0xac32,  0xac41,  0xac50,  0xac60,
0xac6f,  0xac7e,  0xac8d,  0xac9c,  0xacab,  0xacbb,
0xacca,  0xacd9,  0xace8,  0xacf7,  0xad06,  0xad16,
0xad25,  0xad34,  0xad43,  0xad52,  0xad61,  0xad70,
0xad80,  0xad8f,  0xad9e,  0xadad,  0xadbc,  0xadcb,
0xadda,  0xade9,  0xadf8,  0xae08,  0xae17,  0xae26,
0xae35,  0xae44,  0xae53,  0xae62,  0xae71,  0xae80,
0xae8f,  0xae9e,  0xaead,  0xaebd,  0xaecc,  0xaedb,
0xaeea,  0xaef9,  0xaf08,  0xaf17,  0xaf26,  0xaf35,
0xaf44,  0xaf53,  0xaf62,  0xaf71,  0xaf80,  0xaf8f,
0xaf9e,  0xafad,  0xafbc,  0xafcb,  0xafda,  0xafe9,
0xaff8,  0xb007,  0xb016,  0xb025,  0xb034,  0xb043,
0xb052,  0xb061,  0xb070,  0xb07f,  0xb08e,  0xb09d,
0xb0ac,  0xb0bb,  0xb0ca,  0xb0d9,  0xb0e8,  0xb0f6,
0xb105,  0xb114,  0xb123,  0xb132,  0xb141,  0xb150,
0xb15f,  0xb16e,  0xb17d,  0xb18c,  0xb19b,  0xb1aa,
0xb1b8,  0xb1c7,  0xb1d6,  0xb1e5,  0xb1f4,  0xb203,
0xb212,  0xb221,  0xb22f,  0xb23e,  0xb24d,  0xb25c,
0xb26b,  0xb27a,  0xb289,  0xb297,  0xb2a6,  0xb2b5,
0xb2c4,  0xb2d3,  0xb2e2,  0xb2f1,  0xb2ff,  0xb30e,
0xb31d,  0xb32c,  0xb33b,  0xb349,  0xb358,  0xb367,
0xb376,  0xb385,  0xb393,  0xb3a2,  0xb3b1,  0xb3c0,
0xb3cf,  0xb3dd,  0xb3ec,  0xb3fb,  0xb40a,  0xb418,
0xb427,  0xb436,  0xb445,  0xb453,  0xb462,  0xb471,
0xb480,  0xb48e,  0xb49d,  0xb4ac,  0xb4bb,  0xb4c9,
0xb4d8,  0xb4e7,  0xb4f6,  0xb504,  0xb513,  0xb522,
0xb530,  0xb53f,  0xb54e,  0xb55c,  0xb56b,  0xb57a,
0xb588,  0xb597,  0xb5a6,  0xb5b5,  0xb5c3,  0xb5d2,
0xb5e1,  0xb5ef,  0xb5fe,  0xb60d,  0xb61b,  0xb62a,
0xb638,  0xb647,  0xb656,  0xb664,  0xb673,  0xb682,
0xb690,  0xb69f,  0xb6ae,  0xb6bc,  0xb6cb,  0xb6d9,
0xb6e8,  0xb6f7,  0xb705,  0xb714,  0xb722,  0xb731,
0xb740,  0xb74e,  0xb75d,  0xb76b,  0xb77a,  0xb788,
0xb797,  0xb7a6,  0xb7b4,  0xb7c3,  0xb7d1,  0xb7e0,
0xb7ee,  0xb7fd,  0xb80b,  0xb81a,  0xb829,  0xb837,
0xb846,  0xb854,  0xb863,  0xb871,  0xb880,  0xb88e,
0xb89d,  0xb8ab,  0xb8ba,  0xb8c8,  0xb8d7,  0xb8e5,
0xb8f4,  0xb902,  0xb911,  0xb91f,  0xb92e,  0xb93c,
0xb94b,  0xb959,  0xb968,  0xb976,  0xb984,  0xb993,
0xb9a1,  0xb9b0,  0xb9be,  0xb9cd,  0xb9db,  0xb9ea,
0xb9f8,  0xba06,  0xba15,  0xba23,  0xba32,  0xba40,
0xba4f,  0xba5d,  0xba6b,  0xba7a,  0xba88,  0xba97,
0xbaa5,  0xbab3,  0xbac2,  0xbad0,  0xbade,  0xbaed,
0xbafb,  0xbb0a,  0xbb18,  0xbb26,  0xbb35,  0xbb43,
0xbb51,  0xbb60,  0xbb6e,  0xbb7c,  0xbb8b,  0xbb99,
0xbba8,  0xbbb6,  0xbbc4,  0xbbd3,  0xbbe1,  0xbbef,
0xbbfd,  0xbc0c,  0xbc1a,  0xbc28,  0xbc37,  0xbc45,
0xbc53,  0xbc62,  0xbc70,  0xbc7e,  0xbc8c,  0xbc9b,
0xbca9,  0xbcb7,  0xbcc6,  0xbcd4,  0xbce2,  0xbcf0,
0xbcff,  0xbd0d,  0xbd1b,  0xbd29,  0xbd38,  0xbd46,
0xbd54,  0xbd62,  0xbd71,  0xbd7f,  0xbd8d,  0xbd9b,
0xbdaa,  0xbdb8,  0xbdc6,  0xbdd4,  0xbde2,  0xbdf1,
0xbdff,  0xbe0d,  0xbe1b,  0xbe29,  0xbe38,  0xbe46,
0xbe54,  0xbe62,  0xbe70,  0xbe7f,  0xbe8d,  0xbe9b,
0xbea9,  0xbeb7,  0xbec5,  0xbed4,  0xbee2,  0xbef0,
0xbefe,  0xbf0c,  0xbf1a,  0xbf28,  0xbf37,  0xbf45,
0xbf53,  0xbf61,  0xbf6f,  0xbf7d,  0xbf8b,  0xbf99,
0xbfa7,  0xbfb6,  0xbfc4,  0xbfd2,  0xbfe0,  0xbfee,
0xbffc,  0xc00a,  0xc018,  0xc026,  0xc034,  0xc042,
0xc051,  0xc05f,  0xc06d,  0xc07b,  0xc089,  0xc097,
0xc0a5,  0xc0b3,  0xc0c1,  0xc0cf,  0xc0dd,  0xc0eb,
0xc0f9,  0xc107,  0xc115,  0xc123,  0xc131,  0xc13f,
0xc14d,  0xc15b,  0xc169,  0xc177,  0xc185,  0xc193,
0xc1a1,  0xc1af,  0xc1bd,  0xc1cb,  0xc1d9,  0xc1e7,
0xc1f5,  0xc203,  0xc211,  0xc21f,  0xc22d,  0xc23b,
0xc249,  0xc257,  0xc265,  0xc273,  0xc281,  0xc28f,
0xc29d,  0xc2ab,  0xc2b8,  0xc2c6,  0xc2d4,  0xc2e2,
0xc2f0,  0xc2fe,  0xc30c,  0xc31a,  0xc328,  0xc336,
0xc344,  0xc352,  0xc35f,  0xc36d,  0xc37b,  0xc389,
0xc397,  0xc3a5,  0xc3b3,  0xc3c1,  0xc3ce,  0xc3dc,
0xc3ea,  0xc3f8,  0xc406,  0xc414,  0xc422,  0xc42f,
0xc43d,  0xc44b,  0xc459,  0xc467,  0xc475,  0xc482,
0xc490,  0xc49e,  0xc4ac,  0xc4ba,  0xc4c7,  0xc4d5,
0xc4e3,  0xc4f1,  0xc4ff,  0xc50d,  0xc51a,  0xc528,
0xc536,  0xc544,  0xc551,  0xc55f,  0xc56d,  0xc57b,
0xc589,  0xc596,  0xc5a4,  0xc5b2,  0xc5c0,  0xc5cd,
0xc5db,  0xc5e9,  0xc5f7,  0xc604,  0xc612,  0xc620,
0xc62d,  0xc63b,  0xc649,  0xc657,  0xc664,  0xc672,
0xc680,  0xc68d,  0xc69b,  0xc6a9,  0xc6b7,  0xc6c4,
0xc6d2,  0xc6e0,  0xc6ed,  0xc6fb,  0xc709,  0xc716,
0xc724,  0xc732,  0xc73f,  0xc74d,  0xc75b,  0xc768,
0xc776,  0xc784,  0xc791,  0xc79f,  0xc7ad,  0xc7ba,
0xc7c8,  0xc7d6,  0xc7e3,  0xc7f1,  0xc7fe,  0xc80c,
0xc81a,  0xc827,  0xc835,  0xc842,  0xc850,  0xc85e,
0xc86b,  0xc879,  0xc886,  0xc894,  0xc8a2,  0xc8af,
0xc8bd,  0xc8ca,  0xc8d8,  0xc8e5,  0xc8f3,  0xc901,
0xc90e,  0xc91c,  0xc929,  0xc937,  0xc944,  0xc952,
0xc95f,  0xc96d,  0xc97b,  0xc988,  0xc996,  0xc9a3,
0xc9b1,  0xc9be,  0xc9cc,  0xc9d9,  0xc9e7,  0xc9f4,
0xca02,  0xca0f,  0xca1d,  0xca2a,  0xca38,  0xca45,
0xca53,  0xca60,  0xca6e,  0xca7b,  0xca89,  0xca96,
0xcaa4,  0xcab1,  0xcabe,  0xcacc,  0xcad9,  0xcae7,
0xcaf4,  0xcb02,  0xcb0f,  0xcb1d,  0xcb2a,  0xcb37,
0xcb45,  0xcb52,  0xcb60,  0xcb6d,  0xcb7b,  0xcb88,
0xcb95,  0xcba3,  0xcbb0,  0xcbbe,  0xcbcb,  0xcbd8,
0xcbe6,  0xcbf3,  0xcc01,  0xcc0e,  0xcc1b,  0xcc29,
0xcc36,  0xcc43,  0xcc51,  0xcc5e,  0xcc6c,  0xcc79,
0xcc86,  0xcc94,  0xcca1,  0xccae,  0xccbc,  0xccc9,
0xccd6,  0xcce4,  0xccf1,  0xccfe,  0xcd0c,  0xcd19,
0xcd26,  0xcd34,  0xcd41,  0xcd4e,  0xcd5b,  0xcd69,
0xcd76,  0xcd83,  0xcd91,  0xcd9e,  0xcdab,  0xcdb9,
0xcdc6,  0xcdd3,  0xcde0,  0xcdee,  0xcdfb,  0xce08,
0xce15,  0xce23,  0xce30,  0xce3d,  0xce4a,  0xce58,
0xce65,  0xce72,  0xce7f,  0xce8d,  0xce9a,  0xcea7,
0xceb4,  0xcec2,  0xcecf,  0xcedc,  0xcee9,  0xcef6,
0xcf04,  0xcf11,  0xcf1e,  0xcf2b,  0xcf38,  0xcf46,
0xcf53,  0xcf60,  0xcf6d,  0xcf7a,  0xcf87,  0xcf95,
0xcfa2,  0xcfaf,  0xcfbc,  0xcfc9,  0xcfd6,  0xcfe4,
0xcff1,  0xcffe,  0xd00b,  0xd018,  0xd025,  0xd032,
0xd040,  0xd04d,  0xd05a,  0xd067,  0xd074,  0xd081,
0xd08e,  0xd09b,  0xd0a9,  0xd0b6,  0xd0c3,  0xd0d0,
0xd0dd,  0xd0ea,  0xd0f7,  0xd104,  0xd111,  0xd11e,
0xd12b,  0xd139,  0xd146,  0xd153,  0xd160,  0xd16d,
0xd17a,  0xd187,  0xd194,  0xd1a1,  0xd1ae,  0xd1bb,
0xd1c8,  0xd1d5,  0xd1e2,  0xd1ef,  0xd1fc,  0xd209,
0xd216,  0xd223,  0xd230,  0xd23d,  0xd24a,  0xd257,
0xd264,  0xd271,  0xd27e,  0xd28b,  0xd298,  0xd2a5,
0xd2b2,  0xd2bf,  0xd2cc,  0xd2d9,  0xd2e6,  0xd2f3,
0xd300,  0xd30d,  0xd31a,  0xd327,  0xd334,  0xd341,
0xd34e,  0xd35b,  0xd368,  0xd375,  0xd382,  0xd38f,
0xd39c,  0xd3a8,  0xd3b5,  0xd3c2,  0xd3cf,  0xd3dc,
0xd3e9,  0xd3f6,  0xd403,  0xd410,  0xd41d,  0xd42a,
0xd436,  0xd443,  0xd450,  0xd45d,  0xd46a,  0xd477,
0xd484,  0xd491,  0xd49e,  0xd4aa,  0xd4b7,  0xd4c4,
0xd4d1,  0xd4de,  0xd4eb,  0xd4f8,  0xd504,  0xd511,
0xd51e,  0xd52b,  0xd538,  0xd545,  0xd551,  0xd55e,
0xd56b,  0xd578,  0xd585,  0xd591,  0xd59e,  0xd5ab,
0xd5b8,  0xd5c5,  0xd5d1,  0xd5de,  0xd5eb,  0xd5f8,
0xd605,  0xd611,  0xd61e,  0xd62b,  0xd638,  0xd645,
0xd651,  0xd65e,  0xd66b,  0xd678,  0xd684,  0xd691,
0xd69e,  0xd6ab,  0xd6b7,  0xd6c4,  0xd6d1,  0xd6de,
0xd6ea,  0xd6f7,  0xd704,  0xd710,  0xd71d,  0xd72a,
0xd737,  0xd743,  0xd750,  0xd75d,  0xd769,  0xd776,
0xd783,  0xd78f,  0xd79c,  0xd7a9,  0xd7b6,  0xd7c2,
0xd7cf,  0xd7dc,  0xd7e8,  0xd7f5,  0xd802,  0xd80e,
0xd81b,  0xd828,  0xd834,  0xd841,  0xd84e,  0xd85a,
0xd867,  0xd873,  0xd880,  0xd88d,  0xd899,  0xd8a6,
0xd8b3,  0xd8bf,  0xd8cc,  0xd8d8,  0xd8e5,  0xd8f2,
0xd8fe,  0xd90b,  0xd917,  0xd924,  0xd931,  0xd93d,
0xd94a,  0xd956,  0xd963,  0xd970,  0xd97c,  0xd989,
0xd995,  0xd9a2,  0xd9ae,  0xd9bb,  0xd9c8,  0xd9d4,
0xd9e1,  0xd9ed,  0xd9fa,  0xda06,  0xda13,  0xda1f,
0xda2c,  0xda38,  0xda45,  0xda51,  0xda5e,  0xda6a,
0xda77,  0xda84,  0xda90,  0xda9d,  0xdaa9,  0xdab6,
0xdac2,  0xdacf,  0xdadb,  0xdae7,  0xdaf4,  0xdb00,
0xdb0d,  0xdb19,  0xdb26,  0xdb32,  0xdb3f,  0xdb4b,
0xdb58,  0xdb64,  0xdb71,  0xdb7d,  0xdb8a,  0xdb96,
0xdba2,  0xdbaf,  0xdbbb,  0xdbc8,  0xdbd4,  0xdbe1,
0xdbed,  0xdbf9,  0xdc06,  0xdc12,  0xdc1f,  0xdc2b,
0xdc38,  0xdc44,  0xdc50,  0xdc5d,  0xdc69,  0xdc76,
0xdc82,  0xdc8e,  0xdc9b,  0xdca7,  0xdcb3,  0xdcc0,
0xdccc,  0xdcd9,  0xdce5,  0xdcf1,  0xdcfe,  0xdd0a,
0xdd16,  0xdd23,  0xdd2f,  0xdd3b,  0xdd48,  0xdd54,
0xdd60,  0xdd6d,  0xdd79,  0xdd85,  0xdd92,  0xdd9e,
0xddaa,  0xddb7,  0xddc3,  0xddcf,  0xdddc,  0xdde8,
0xddf4,  0xde01,  0xde0d,  0xde19,  0xde25,  0xde32,
0xde3e,  0xde4a,  0xde57,  0xde63,  0xde6f,  0xde7b,
0xde88,  0xde94,  0xdea0,  0xdeac,  0xdeb9,  0xdec5,
0xded1,  0xdedd,  0xdeea,  0xdef6,  0xdf02,  0xdf0e,
0xdf1b,  0xdf27,  0xdf33,  0xdf3f,  0xdf4c,  0xdf58,
0xdf64,  0xdf70,  0xdf7c,  0xdf89,  0xdf95,  0xdfa1,
0xdfad,  0xdfb9,  0xdfc6,  0xdfd2,  0xdfde,  0xdfea,
0xdff6,  0xe003,  0xe00f,  0xe01b,  0xe027,  0xe033,
0xe03f,  0xe04c,  0xe058,  0xe064,  0xe070,  0xe07c,
0xe088,  0xe094,  0xe0a1,  0xe0ad,  0xe0b9,  0xe0c5,
0xe0d1,  0xe0dd,  0xe0e9,  0xe0f5,  0xe102,  0xe10e,
0xe11a,  0xe126,  0xe132,  0xe13e,  0xe14a,  0xe156,
0xe162,  0xe16e,  0xe17b,  0xe187,  0xe193,  0xe19f,
0xe1ab,  0xe1b7,  0xe1c3,  0xe1cf,  0xe1db,  0xe1e7,
0xe1f3,  0xe1ff,  0xe20b,  0xe217,  0xe223,  0xe22f,
0xe23c,  0xe248,  0xe254,  0xe260,  0xe26c,  0xe278,
0xe284,  0xe290,  0xe29c,  0xe2a8,  0xe2b4,  0xe2c0,
0xe2cc,  0xe2d8,  0xe2e4,  0xe2f0,  0xe2fc,  0xe308,
0xe314,  0xe320,  0xe32c,  0xe338,  0xe344,  0xe350,
0xe35c,  0xe368,  0xe374,  0xe380,  0xe38b,  0xe397,
0xe3a3,  0xe3af,  0xe3bb,  0xe3c7,  0xe3d3,  0xe3df,
0xe3eb,  0xe3f7,  0xe403,  0xe40f,  0xe41b,  0xe427,
0xe433,  0xe43f,  0xe44a,  0xe456,  0xe462,  0xe46e,
0xe47a,  0xe486,  0xe492,  0xe49e,  0xe4aa,  0xe4b6,
0xe4c1,  0xe4cd,  0xe4d9,  0xe4e5,  0xe4f1,  0xe4fd,
0xe509,  0xe515,  0xe520,  0xe52c,  0xe538,  0xe544,
0xe550,  0xe55c,  0xe567,  0xe573,  0xe57f,  0xe58b,
0xe597,  0xe5a3,  0xe5af,  0xe5ba,  0xe5c6,  0xe5d2,
0xe5de,  0xe5ea,  0xe5f5,  0xe601,  0xe60d,  0xe619,
0xe625,  0xe630,  0xe63c,  0xe648,  0xe654,  0xe660,
0xe66b,  0xe677,  0xe683,  0xe68f,  0xe69a,  0xe6a6,
0xe6b2,  0xe6be,  0xe6ca,  0xe6d5,  0xe6e1,  0xe6ed,
0xe6f9,  0xe704,  0xe710,  0xe71c,  0xe727,  0xe733,
0xe73f,  0xe74b,  0xe756,  0xe762,  0xe76e,  0xe77a,
0xe785,  0xe791,  0xe79d,  0xe7a8,  0xe7b4,  0xe7c0,
0xe7cb,  0xe7d7,  0xe7e3,  0xe7ef,  0xe7fa,  0xe806,
0xe812,  0xe81d,  0xe829,  0xe835,  0xe840,  0xe84c,
0xe858,  0xe863,  0xe86f,  0xe87b,  0xe886,  0xe892,
0xe89e,  0xe8a9,  0xe8b5,  0xe8c0,  0xe8cc,  0xe8d8,
0xe8e3,  0xe8ef,  0xe8fb,  0xe906,  0xe912,  0xe91d,
0xe929,  0xe935,  0xe940,  0xe94c,  0xe958,  0xe963,
0xe96f,  0xe97a,  0xe986,  0xe991,  0xe99d,  0xe9a9,
0xe9b4,  0xe9c0,  0xe9cb,  0xe9d7,  0xe9e3,  0xe9ee,
0xe9fa,  0xea05,  0xea11,  0xea1c,  0xea28,  0xea33,
0xea3f,  0xea4a,  0xea56,  0xea62,  0xea6d,  0xea79,
0xea84,  0xea90,  0xea9b,  0xeaa7,  0xeab2,  0xeabe,
0xeac9,  0xead5,  0xeae0,  0xeaec,  0xeaf7,  0xeb03,
0xeb0e,  0xeb1a,  0xeb25,  0xeb31,  0xeb3c,  0xeb48,
0xeb53,  0xeb5f,  0xeb6a,  0xeb76,  0xeb81,  0xeb8d,
0xeb98,  0xeba3,  0xebaf,  0xebba,  0xebc6,  0xebd1,
0xebdd,  0xebe8,  0xebf4,  0xebff,  0xec0a,  0xec16,
0xec21,  0xec2d,  0xec38,  0xec44,  0xec4f,  0xec5a,
0xec66,  0xec71,  0xec7d,  0xec88,  0xec93,  0xec9f,
0xecaa,  0xecb6,  0xecc1,  0xeccc,  0xecd8,  0xece3,
0xecef,  0xecfa,  0xed05,  0xed11,  0xed1c,  0xed27,
0xed33,  0xed3e,  0xed4a,  0xed55,  0xed60,  0xed6c,
0xed77,  0xed82,  0xed8e,  0xed99,  0xeda4,  0xedb0,
0xedbb,  0xedc6,  0xedd2,  0xeddd,  0xede8,  0xedf4,
0xedff,  0xee0a,  0xee15,  0xee21,  0xee2c,  0xee37,
0xee43,  0xee4e,  0xee59,  0xee65,  0xee70,  0xee7b,
0xee86,  0xee92,  0xee9d,  0xeea8,  0xeeb3,  0xeebf,
0xeeca,  0xeed5,  0xeee1,  0xeeec,  0xeef7,  0xef02,
0xef0e,  0xef19,  0xef24,  0xef2f,  0xef3a,  0xef46,
0xef51,  0xef5c,  0xef67,  0xef73,  0xef7e,  0xef89,
0xef94,  0xef9f,  0xefab,  0xefb6,  0xefc1,  0xefcc,
0xefd7,  0xefe3,  0xefee,  0xeff9,  0xf004,  0xf00f,
0xf01b,  0xf026,  0xf031,  0xf03c,  0xf047,  0xf052,
0xf05e,  0xf069,  0xf074,  0xf07f,  0xf08a,  0xf095,
0xf0a1,  0xf0ac,  0xf0b7,  0xf0c2,  0xf0cd,  0xf0d8,
0xf0e3,  0xf0ef,  0xf0fa,  0xf105,  0xf110,  0xf11b,
0xf126,  0xf131,  0xf13c,  0xf147,  0xf153,  0xf15e,
0xf169,  0xf174,  0xf17f,  0xf18a,  0xf195,  0xf1a0,
0xf1ab,  0xf1b6,  0xf1c2,  0xf1cd,  0xf1d8,  0xf1e3,
0xf1ee,  0xf1f9,  0xf204,  0xf20f,  0xf21a,  0xf225,
0xf230,  0xf23b,  0xf246,  0xf251,  0xf25c,  0xf267,
0xf272,  0xf27d,  0xf288,  0xf293,  0xf29f,  0xf2aa,
0xf2b5,  0xf2c0,  0xf2cb,  0xf2d6,  0xf2e1,  0xf2ec,
0xf2f7,  0xf302,  0xf30d,  0xf318,  0xf323,  0xf32e,
0xf339,  0xf344,  0xf34f,  0xf35a,  0xf364,  0xf36f,
0xf37a,  0xf385,  0xf390,  0xf39b,  0xf3a6,  0xf3b1,
0xf3bc,  0xf3c7,  0xf3d2,  0xf3dd,  0xf3e8,  0xf3f3,
0xf3fe,  0xf409,  0xf414,  0xf41f,  0xf42a,  0xf435,
0xf43f,  0xf44a,  0xf455,  0xf460,  0xf46b,  0xf476,
0xf481,  0xf48c,  0xf497,  0xf4a2,  0xf4ad,  0xf4b7,
0xf4c2,  0xf4cd,  0xf4d8,  0xf4e3,  0xf4ee,  0xf4f9,
0xf504,  0xf50f,  0xf519,  0xf524,  0xf52f,  0xf53a,
0xf545,  0xf550,  0xf55b,  0xf565,  0xf570,  0xf57b,
0xf586,  0xf591,  0xf59c,  0xf5a6,  0xf5b1,  0xf5bc,
0xf5c7,  0xf5d2,  0xf5dd,  0xf5e7,  0xf5f2,  0xf5fd,
0xf608,  0xf613,  0xf61d,  0xf628,  0xf633,  0xf63e,
0xf649,  0xf653,  0xf65e,  0xf669,  0xf674,  0xf67f,
0xf689,  0xf694,  0xf69f,  0xf6aa,  0xf6b4,  0xf6bf,
0xf6ca,  0xf6d5,  0xf6e0,  0xf6ea,  0xf6f5,  0xf700,
0xf70b,  0xf715,  0xf720,  0xf72b,  0xf736,  0xf740,
0xf74b,  0xf756,  0xf760,  0xf76b,  0xf776,  0xf781,
0xf78b,  0xf796,  0xf7a1,  0xf7ab,  0xf7b6,  0xf7c1,
0xf7cc,  0xf7d6,  0xf7e1,  0xf7ec,  0xf7f6,  0xf801,
0xf80c,  0xf816,  0xf821,  0xf82c,  0xf836,  0xf841,
0xf84c,  0xf856,  0xf861,  0xf86c,  0xf876,  0xf881,
0xf88c,  0xf896,  0xf8a1,  0xf8ac,  0xf8b6,  0xf8c1,
0xf8cc,  0xf8d6,  0xf8e1,  0xf8ec,  0xf8f6,  0xf901,
0xf90b,  0xf916,  0xf921,  0xf92b,  0xf936,  0xf941,
0xf94b,  0xf956,  0xf960,  0xf96b,  0xf976,  0xf980,
0xf98b,  0xf995,  0xf9a0,  0xf9aa,  0xf9b5,  0xf9c0,
0xf9ca,  0xf9d5,  0xf9df,  0xf9ea,  0xf9f4,  0xf9ff,
0xfa0a,  0xfa14,  0xfa1f,  0xfa29,  0xfa34,  0xfa3e,
0xfa49,  0xfa53,  0xfa5e,  0xfa69,  0xfa73,  0xfa7e,
0xfa88,  0xfa93,  0xfa9d,  0xfaa8,  0xfab2,  0xfabd,
0xfac7,  0xfad2,  0xfadc,  0xfae7,  0xfaf1,  0xfafc,
0xfb06,  0xfb11,  0xfb1b,  0xfb26,  0xfb30,  0xfb3b,
0xfb45,  0xfb50,  0xfb5a,  0xfb65,  0xfb6f,  0xfb7a,
0xfb84,  0xfb8f,  0xfb99,  0xfba4,  0xfbae,  0xfbb8,
0xfbc3,  0xfbcd,  0xfbd8,  0xfbe2,  0xfbed,  0xfbf7,
0xfc02,  0xfc0c,  0xfc16,  0xfc21,  0xfc2b,  0xfc36,
0xfc40,  0xfc4b,  0xfc55,  0xfc5f,  0xfc6a,  0xfc74,
0xfc7f,  0xfc89,  0xfc93,  0xfc9e,  0xfca8,  0xfcb3,
0xfcbd,  0xfcc7,  0xfcd2,  0xfcdc,  0xfce7,  0xfcf1,
0xfcfb,  0xfd06,  0xfd10,  0xfd1a,  0xfd25,  0xfd2f,
0xfd3a,  0xfd44,  0xfd4e,  0xfd59,  0xfd63,  0xfd6d,
0xfd78,  0xfd82,  0xfd8c,  0xfd97,  0xfda1,  0xfdab,
0xfdb6,  0xfdc0,  0xfdca,  0xfdd5,  0xfddf,  0xfde9,
0xfdf4,  0xfdfe,  0xfe08,  0xfe13,  0xfe1d,  0xfe27,
0xfe32,  0xfe3c,  0xfe46,  0xfe50,  0xfe5b,  0xfe65,
0xfe6f,  0xfe7a,  0xfe84,  0xfe8e,  0xfe98,  0xfea3,
0xfead,  0xfeb7,  0xfec1,  0xfecc,  0xfed6,  0xfee0,
0xfeeb,  0xfef5,  0xfeff,  0xff09,  0xff14,  0xff1e,
0xff28,  0xff32,  0xff3c,  0xff47,  0xff51,  0xff5b,
0xff65,  0xff70,  0xff7a,  0xff84,  0xff8e,  0xff98,
0xffa3,  0xffad,  0xffb7,  0xffc1,  0xffcc,  0xffd6,
0xffe0,  0xffea,  0xfff4,  0xffff
};

// max value is pi/4
constexpr double SCALING_FACTOR = 4 / M_PI * 0xFFFF;

inline double atan2_lookup(double y, double x)
{
    if ((x > 0.) && (x < std::numeric_limits<double>::epsilon()) ||
        (x < 0.) && (x > -std::numeric_limits<double>::epsilon()))
    {
        if (y >= 0.)
        {
            return M_PI / 2.;
        }
        else
        {
            return -M_PI / 2.;
        }
    }

    unsigned octant = 0;

    if (x < 0.)
    {
        octant = 1;
        x = -x;
    }
    if (y < 0.)
    {
        octant |= 2;
        y = -y;
    }

    double t = y / x;
    if (t > 1.0)
    {
        octant |= 4;
        t = 1.0 / t;
    }

    double angle = atan_table[(unsigned)(t * 4095)] / SCALING_FACTOR;

    switch (octant)
    {
    case 0:
        break;
    case 1:
        angle = M_PI - angle;
        break;
    case 2:
        angle = -angle;
        break;
    case 3:
        angle = -M_PI + angle;
        break;
    case 4:
        angle = M_PI / 2.0 - angle;
        break;
    case 5:
        angle = M_PI / 2.0 + angle;
        break;
    case 6:
        angle = -M_PI / 2.0 + angle;
        break;
    case 7:
        angle = -M_PI / 2.0 - angle;
        break;
    }

    return angle;
}

/* Get angle of line segment (A,C)->(C,B), atan2 magic, formerly cosine theorem*/
template <class CoordinateT>
inline static double GetAngleBetweenThreeFixedPointCoordinates(const CoordinateT &A,
                                                               const CoordinateT &C,
                                                               const CoordinateT &B)
{
    const double v1x = (A.lon - C.lon) / COORDINATE_PRECISION;
    const double v1y = lat2y(A.lat / COORDINATE_PRECISION) - lat2y(C.lat / COORDINATE_PRECISION);
    const double v2x = (B.lon - C.lon) / COORDINATE_PRECISION;
    const double v2y = lat2y(B.lat / COORDINATE_PRECISION) - lat2y(C.lat / COORDINATE_PRECISION);

    double angle = (atan2_lookup(v2y, v2x) - atan2_lookup(v1y, v1x)) * 180 / M_PI;
    while (angle < 0)
        angle += 360;
    return angle;
}


#endif // COMPUTE_ANGLE_H
