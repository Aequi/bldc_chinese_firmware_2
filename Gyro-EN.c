 
/ * Includes ----------------------------------------------- ------------------- * /
#include "stm32f10x.h"
// # include "l3g4200d_driver.h" // xx0410
// # include "ADXL345_driver.h" // xx0410
#include "l3g4200d_LIS331DLH_driver.h" // 0410
#include "MC_Globals.h"

Float eRPM_Integ = 0;
S16 eRPM_LPF = 0;
// # define Flash_START_ADDRESS (uint32) 0x0803E800 // Flash page address, page 125
/ * Private define ---------------------------------------------- -------------- * /
// LPF parameter setup:
// # define i_LPF 1;
// # define K_LPF 2;
// # define i_LPF 1;
// # define K_LPF 4;
// # define i_LPF 3;
// # define K_LPF 4;
/ *
Program Size: Code = 21904 RO-data = 336 RW-data = 1232 ZI-data = 2024
Program Size: Code = 21904 RO-data = 336 RW-data = 716 ZI-data = 2020
RW-data = 1232-716 = 516 form occupied 2012-10-10 lihewen
* /
// s16 ASIN875Tab [257] = {0,26,51,77,102,128,153,179,205,230,256,281,307,333,358,384,
// 410,435,461,486,512,538,563,589,615,640,666,692,718,743,769,795,
// 821,846,872,898,924,950,976,1001,1027,1053,1079,1105,1131,1157,1183,1209,
// 1235,1261,1287,1313,1339,1366,1392,1418,1444,1470,1497,1523,1549,1575,1602,1628,
// 1655,1681,1707,1734,1760,1787,1814,1840,1867,1894,1920,1947,1974,2001,2027,2054,
// 2081,2108,2135,2162,2189,2216,2243,2271,2298,2325,2352,2380,2407,2434,2462,2489,
// 2517,2545,2572,2600,2628,2656,2683,2711,2739,2767,2795,2824,2852,2880,2908,2937,
// 2965,2994,3022,3051,3079,3108,3137,3166,3195,3224,3253,3282,3311,3340,3370,3399,
// 3429,3458,3488,3518,3547,3577,3607,3637,3667,3698,3728,3758,3789,3820,3850,3881,
// 3912,3943,3974,4005,4036,4068,4099,4131,4163,4195,4227,4259,4291,4323,4356,4388,
// 4421,4454,4487,4520,4553,4586,4620,4654,4687,4721,4755,4790,4824,4859,4894,4929,
// 4964,4999,5035,5070,5106,5142,5178,5215,5252,5288,5326,5363,5401,5438,5476,5515,
Lt; / RTI & gt; 5553,5592,5631,5670,5710,5750,5790,5831,5871,5913,5954,5996,6038,6081,6123,6167,
// 6210,6254,6299,6344,6389,6435,6482,6528,6576,6624,6672,6721,6771,6821,6872,6924,
// 6977,7030,7084,7139,7195,7251,7309,7368,7428,7489,7551,7615,7680,7747,7816,7886,
// 7958,8033,8110,8190,8273,8359,8449,8544,8644,8751,8865,8989,9127,9282,9467,9707,
// 10286};
// s16 ASIN875Tab [331] = {
// 0, 13, 26, 38, 51, 64, 77, 90, 102, 115, 128, 141, 153, 166, 179, 192,
// 205, 217, 230, 243, 256, 269, 281, 294, 307, 320, 333, 345, 358, 371, 384, 397,
// 410, 422, 435, 448, 461, 474, 486, 499, 512, 525, 538, 551, 563, 576, 589, 602,
// 615, 628, 640, 653, 666, 679, 692, 705, 718, 731, 743, 756, 769, 782, 795, 808,
// 821, 834, 846, 859, 872, 885, 898, 911, 924, 937, 950, 963, 976, 989,
// 1027,1040,1053,1066,1079,1092,1105,1118,1131,1144,1157,1170,1183,1196,1209,1222,
// 1235,1248,1261,1274,1287,1300,1313,1326,1339,1352,1366,1379,1392,1405,1418,1431,
// 1444,1457,1470,1483,1497,1510,1523,1536,1549,1562,1575,1589,1602,1615,1628,1641,
// 1655,1668,1681,1694,1707,1721,1734,1747,1760,1774,1787,1800,1814,1827,1840,1854,
// 1867,1880,1894,1907,1920,1934,1947,1960,1974,1987,2001,2014,2027,2041,2054,2068,
// 2081,2095,2108,2122,2135,2149,2162,2176,2189,2203,2216,2230,2243,2257,2271,2284,
// 2298,2311,2325,2339,2352,2366,2380,2393,2407,2421,2434,2448,2462,2476,2489,2503,
// 2517,2531,2545,2559,2572,2586,2600,2614,2628,2642,2656,2670,2683,2697,2711,2725,
// 2739,2753,2767,2781,2795,2810,2824,2838,2852,2866,2880,2894,2908,2922,2937,2951,
// 2965,2979,2994,3008,3022,3036,3051,3065,3079,3094,3108,3122,3137,3151,3166,3180,
// 3195,3209,3224,3238,3253,3267,3282,3296,3311,3326,3340,3355,3370,3384,3399,3414,
// 3429,3443,3458,3473,3488,3503,3518,3532,3547,3562,3577,3592,3607,3622,3637,3652,
// 3667,3683,3698,3713,3728,3743,3758,3774,3789,3804,3820,3835,3850,3866,3881,3896,
// 3912,3927,3943,3958,3974,3990,4005,4021,4036,4052,4068,4084,4099,4115,4131,4147,
// 4163,4179,4195,4211,4227,4243,4259,4275,4291,4307,4323,4339,4356,4372,4388,4404,
// 4421,4437,4454,4470,4487,4503,4520,4536,4553,4570,4586};
S16 ASIN875Tab [661] = {
0, 6, 13, 19, 26, 32, 38, 45, 51, 58, 64, 70, 77, 83, 90,
102, 129, 115, 122, 128, 134, 141, 147, 153, 160, 166, 173, 179, 185, 192,
205, 211, 217, 224, 230, 237, 243, 249, 256, 262, 269, 275, 281, 288, 294, 301,
Lt; / RTI & gt; 337,384,390,397,403, & lt; RTI ID = 0.0 & gt;
4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
Lt; / RTI & gt; 53,552,531,538,544,551,557,563,570,576,583,589,596,602,608,
6,166,679,679,685,692,698,705,711, & lt; RTI ID = 0.0 & gt;
718, 780, 757,
818, 880, 894, 890, 905, 911, 917,
918, 930, 953, 966, 969, 976, 982, 989, 995, 2001, 1008, 1014, 1021, // 10
Lt; / RTI & gt;
1137, 1144, 1145,
1268, 1224, 1220, 1320, 1326, 1333,
1335, 1392, 1398, 1405, 1411, 1418, 1424, 1431, 1438,
1444, 1464, 1470, 1477, 1490, 1490, 1497, 1503, 1510, 1516, 1523, 1529, 1536, 1543, // 15
1515, 1635, 1635, 1635, 1641, 1648,
1616, 1741, 1734, 1741, 1747, 1747,
1780, 1787, 1794, 1800, 1807, 1814, 1820, 1827, 1834, 1840, 1847, 1854, 1860,
1867, 1880, 1887, 1894, 1900, 1907, 1914, 1920, 1927, 1934, 1940, 1947, 1954, 1960,
Lt; / RTI & gt; 20,209,204,204,204,2041,2048,2054,2061,2068,2074, // 20
2081,2088,2095,2101,2108,2115,2122,2128,2135,2142,2149,2155,2162,2169,2176,2182,
2129, 2196, 2203, 2209, 2216, 2223, 2230, 2237, 2243, 2250, 2257, 2264, 2271, 2277, 2284,
2219, 2323, 2325, 2387, 2393, 2300,
247, 2424, 2424, 2434, 2441, 2448, 2455, 2462, 2469, 2476, 2483, 2489, 2496, 2503,
2517,253,255,255,255,255,255,257,2579,2586,2593,2600,2607,2614,2621, // 25
2628, 2626, 266, 266, 267, 276, 2725, 2732,
2786, 2760, 2767, 2774, 2781, 2788, 2795, 2802, 2810, 2817, 2824, 2831, 2838, 2845,
2885, 2887, 2880, 2887, 2894, 2901, 2908, 2915, 2922, 2930, 2937, 2942, 2951,
2965, 2972, 2979, 2986, 2934, 3100, 3015, 3022, 3029, 3036, 3044, 3051, 3058, 3065, 3072,
Lt; / RTI & gt; 307,309,3094,3101,3108,3115,3122,3130,3137,3144,3151,3159,3166,3173,3180,3187, // 30
3185, 329, 3226, 3228, 3231, 3238, 3245, 3253, 3260, 3267, 3275, 3282, 3289, 3296,
3333, 338, 334, 334, 334,
3436, 3443, 3443, 345, 342,
3567, 3555, 356, 3577, 3585, 3595, 3592, 3600, 3607, 3615, 3622, 3630, 3637, 3645, 3652, 3660,
3667, 3675, 368, 3698, 3705, 3713, 3720, 3728, 3736, 3743, 3751, 3758, 3766, 3774, 3781, // 35
3785, 3884, 3886, 3827, 3886, 3827, 3835, 3843, 3850, 3858, 3866, 3873, 3881, 3889, 3896,
3929, 3920, 3990, 397, 395, 4013, 4021, 4029,
4036, 4044, 4060, 4068, 4076, 4084, 4091, 4099, 4107, 4115, 4123, 4131, 4139, 417,
4163,4179,4187,4195,4203,4211,4218,4227,4235,4243,4251,4259,4267,4275,4283,
423, 433, 433, 433, 433, 433,
4445, 4426, 4478, 4478, 447, 4495, 453, 4511, 4520, 4528, 4536, 4545,
4531, 4570, 4578, 4586};
// s16 ASIN875Tab [1318] = {
// 0,
//2	,
// 3,
// 5,
// 6,
//8	,
// 10,
// 11,
// 13,
// 14,
// 16,
// 18,
// 19,
//twenty one	,
//twenty two	,
//twenty four	,
// 26,
// 27,
// 29,
// 30,
// 32,
// 34,
// 35,
// 37,
// 38,
// 40,
// 42,
// 43,
// 45,
// 46,
// 48,
// 50,
// 51,
// 53,
// 54,
// 56,
// 58,
// 59,
// 61,
// 62,
// 64,
// 66,
// 67,
// 69,
// 70,
// 72,
// 74,
// 75,
// 77,
// 78,
// 80,
// 82,
// 83,
// 85,
// 86,
// 88,
// 90,
// 91,
// 93,
// 94,
// 96,
// 98,
// 99,
// 101,
// 102,
// 104,
// 106,
// 107,
// 109,
// 110,
// 112,
// 114,
// 115,
// 117,
// 118,
// 120,
// 122,
// 123,
// 125,
// 126,
// 128,
// 130,
// 131,
// 133,
// 134,
// 136,
// 138,
// 139,
// 141,
// 142,
// 144,
// 146,
// 147,
// 149,
// 150,
// 152,
// 154,
// 155,
// 157,
// 158,
// 160,
// 162,
// 163,
// 165,
// 166,
// 168,
// 170,
// 171,
// 173,
// 174,
// 176,
// 178,
// 179,
// 181,
// 182,
// 184,
// 186,
// 187,
// 189,
// 190,
// 192,
// 194,
// 195,
// 197,
// 198,
// 200,
// 202,
// 203,
// 205,
// 206,
// 208,
// 210,
// 211,
// 213,
// 214,
// 216,
// 218,
// 219,
// 221,
// 222,
// 224,
// 226,
// 227,
// 229,
// 230,
// 232,
// 234,
// 235,
// 237,
// 238,
// 240,
// 242,
// 243,
// 245,
// 246,
// 248,
// 250,
// 251,
// 253,
// 254,
// 256,
// 258,
// 259,
// 261,
// 262,
// 264,
// 266,
// 267,
// 269,
// 270,
// 272,
// 274,
// 275,
// 277,
// 279,
// 280,
// 282,
// 283,
// 285,
// 287,
// 288,
// 290,
// 291,
// 293,
// 295,
// 296,
// 298,
// 299,
// 301,
// 303,
// 304,
// 306,
// 307,
// 309,
// 311,
// 312,
// 314,
// 315,
// 317,
// 319,
// 320,
// 322,
// 323,
// 325,
// 327,
// 328,
// 330,
// 331,
// 333,
// 335,
// 336,
// 338,
// 340,
// 341,
// 343,
// 344,
// 346,
// 348,
// 349,
// 351,
// 352,
// 354,
// 356,
// 357,
// 359,
// 360,
// 362,
// 364,
// 365,
// 367,
// 368,
// 370,
// 372,
// 373,
// 375,
// 377,
// 378,
// 380,
// 381,
// 383,
// 385,
// 386,
// 388,
// 389,
// 391,
// 393,
// 394,
// 396,
// 397,
// 399,
// 401,
// 402,
// 404,
// 405,
// 407,
// 409,
// 410,
// 412,
// 414,
// 415,
// 417,
// 418,
// 420,
// 422,
// 423,
// 425,
// 426,
// 428,
// 430,
// 431,
// 433,
// 435,
// 436,
// 438,
// 439,
// 441,
// 443,
// 444,
// 446,
// 447,
// 449,
// 451,
// 452,
// 454,
// 455,
// 457,
// 459,
// 460,
// 462,
// 464,
// 465,
// 467,
// 468,
// 470,
// 472,
// 473,
// 475,
// 476,
// 478,
// 480,
// 481,
// 483,
// 485,
// 486,
// 488,
// 489,
// 491,
// 493,
// 494,
// 496,
// 497,
// 499,
// 501,
// 502,
// 504,
// 506,
// 507,
// 509,
// 510,
// 512,
// 514,
// 515,
// 517,
// 519,
// 520,
// 522,
// 523,
// 525,
// 527,
// 528,
// 530,
// 531,
// 533,
// 535,
// 536,
// 538,
// 540,
// 541,
// 543,
// 544,
// 546,
// 548,
// 549,
// 551,
// 553,
// 554,
// 556,
// 557,
// 559,
// 561,
// 562,
// 564,
// 566,
// 567,
// 569,
// 570,
// 572,
// 574,
// 575,
// 577,
// 579,
// 580,
// 582,
// 583,
// 585,
// 587,
// 588,
// 590,
// 592,
// 593,
// 595,
// 596,
// 598,
// 600,
// 601,
// 603,
// 605,
// 606,
// 608,
// 609,
// 611,
// 613,
// 614,
// 616,
// 618,
// 619,
// 621,
// 622,
// 624,
// 626,
// 627,
// 629,
// 631,
// 632,
// 634,
// 635,
// 637,
// 639,
// 640,
// 642,
// 644,
// 645,
// 647,
// 648,
// 650,
// 652,
// 653,
// 655,
// 657,
// 658,
// 660,
// 662,
// 663,
// 665,
// 666,
// 668,
// 670,
// 671,
// 673,
// 675,
// 676,
// 678,
// 679,
// 681,
// 683,
// 684,
// 686,
// 688,
// 689,
// 691,
// 693,
// 694,
// 696,
// 697,
// 699,
// 701,
// 702,
// 704,
// 706,
// 707,
// 709,
// 711,
// 712,
// 714,
// 715,
// 717,
// 719,
// 720,
// 722,
// 724,
// 725,
// 727,
// 729,
// 730,
// 732,
// 734,
// 735,
// 737,
// 738,
// 740,
// 742,
// 743,
// 745,
// 747,
// 748,
// 750,
// 752,
// 753,
// 755,
// 756,
// 758,
// 760,
// 761,
// 763,
// 765,
// 766,
// 768,
// 770,
// 771,
// 773,
// 775,
// 776,
// 778,
// 779,
// 781,
// 783,
// 784,
// 786,
// 788,
// 789,
// 791,
// 793,
// 794,
// 796,
// 798,
// 799,
// 801,
// 803,
// 804,
// 806,
// 807,
// 809,
// 811,
// 812,
// 814,
// 816,
// 817,
// 819,
// 821,
// 822,
// 824,
// 826,
// 827,
// 829,
// 831,
// 832,
// 834,
// 836,
// 837,
// 839,
// 841,
// 842,
// 844,
// 845,
// 847,
// 849,
// 850,
// 852,
// 854,
// 855,
// 857,
// 859,
// 860,
// 862,
// 864,
// 865,
// 867,
// 869,
// 870,
// 872,
// 874,
// 875,
// 877,
// 879,
// 880,
// 882,
// 884,
// 885,
// 887,
// 889,
// 890,
// 892,
// 894,
// 895,
// 897,
// 898,
// 900,
// 902,
// 903,
// 905,
// 907,
// 908,
// 910,
// 912,
// 913,
// 915,
// 917,
// 918,
// 920,
// 922,
// 923,
// 925,
// 927,
// 928,
// 930,
// 932,
// 933,
// 935,
// 937,
// 938,
// 940,
// 942,
// 943,
// 945,
// 947,
// 948,
// 950,
// 952,
// 953,
// 955,
// 957,
// 958,
// 960,
// 962,
// 963,
// 965,
// 967,
// 968,
// 970,
// 972,
// 973,
// 975,
// 977,
// 978,
// 980,
// 982,
// 984,
// 985,
// 987,
// 989,
// 990,
// 992,
// 994,
// 995,
// 997,
// 999,
// 1000,
// 1002,
// 1004,
// 1005,
// 1007,
// 1009,
// 1010,
// 1012,
// 1014,
// 1015,
// 1017,
// 1019,
// 1020,
// 1022,
// 1024,
// 1025,
// 1027,
// 1029,
// 1030,
// 1032,
// 1034,
// 1036,
// 1037,
// 1039,
// 1041,
// 1042,
// 1044,
// 1046,
// 1047,
// 1049,
// 1051,
// 1052,
// 1054,
// 1056,
// 1057,
// 1059,
// 1061,
// 1062,
// 1064,
// 1066,
// 1068,
// 1069,
// 1071,
// 1073,
// 1074,
// 1076,
// 1078,
// 1079,
// 1081,
// 1083,
// 1084,
// 1086,
// 1088,
// 1089,
// 1091,
// 1093,
// 1095,
// 1096,
// 1098,
// 1100,
// 1101,
// 1103,
// 1105,
// 1106,
// 1108,
// 1110,
// 1112,
// 1113,
// 1115,
// 1117,
// 1118,
// 1120,
// 1122,
// 1123,
// 1125,
// 1127,
// 1128,
// 1130,
// 1132,
// 1134,
// 1135,
// 1137,
// 1139,
// 1140,
// 1142,
// 1144,
// 1145,
// 1147,
// 1149,
// 1151,
// 1152,
// 1154,
// 1156,
// 1157,
// 1159,
// 1161,
// 1163,
// 1164,
// 1166,
// 1168,
// 1169,
// 1171,
// 1173,
// 1174,
// 1176,
// 1178,
// 1180,
// 1181,
// 1183,
// 1185,
// 1186,
// 1188,
// 1190,
// 1192,
// 1193,
// 1195,
// 1197,
// 1198,
// 1200,
// 1202,
// 1204,
// 1205,
// 1207,
// 1209,
// 1210,
// 1212,
// 1214,
// 1216,
// 1217,
// 1219,
// 1221,
// 1222,
// 1224,
// 1226,
// 1228,
// 1229,
// 1231,
// 1233,
// 1234,
// 1236,
// 1238,
// 1240,
// 1241,
// 1243,
// 1245,
// 1246,
// 1248,
// 1250,
// 1252,
// 1253,
// 1255,
// 1257,
// 1259,
// 1260,
// 1262,
// 1264,
// 1265,
// 1267,
// 1269,
// 1271,
// 1272,
// 1274,
// 1276,
// 1278,
// 1279,
// 1281,
// 1283,
// 1284,
// 1286,
// 1288,
// 1290,
// 1291,
// 1293,
// 1295,
// 1297,
// 1298,
// 1300,
// 1302,
// 1303,
// 1305,
// 1307,
// 1309,
// 1310,
// 1312,
// 1314,
// 1316,
// 1317,
// 1319,
// 1321,
// 1323,
// 1324,
// 1326,
// 1328,
// 1330,
// 1331,
// 1333,
// 1335,
// 1337,
// 1338,
// 1340,
// 1342,
// 1343,
// 1345,
// 1347,
// 1349,
// 1350,
// 1352,
// 1354,
// 1356,
// 1357,
// 1359,
// 1361,
// 1363,
// 1364,
// 1366,
// 1368,
// 1370,
// 1371,
// 1373,
// 1375,
// 1377,
// 1378,
// 1380,
// 1382,
// 1384,
// 1385,
// 1387,
// 1389,
// 1391,
// 1392,
// 1394,
// 1396,
// 1398,
// 1399,
// 1401,
// 1403,
// 1405,
// 1407,
// 1408,
// 1410,
// 1412,
// 1414,
// 1415,
// 1417,
// 1419,
// 1421,
// 1422,
// 1424,
// 1426,
// 1428,
// 1429,
// 1431,
// 1433,
// 1435,
// 1436,
// 1438,
// 1440,
// 1442,
// 1444,
// 1445,
// 1447,
// 1449,
// 1451,
// 1452,
// 1454,
// 1456,
// 1458,
// 1459,
// 1461,
// 1463,
// 1465,
// 1467,
// 1468,
// 1470,
// 1472,
// 1474,
// 1475,
// 1477,
// 1479,
// 1481,
// 1483,
// 1484,
// 1486,
// 1488,
// 1490,
// 1491,
// 1493,
// 1495,
// 1497,
// 1499,
// 1500,
// 1502,
// 1504,
// 1506,
// 1507,
// 1509,
// 1511,
// 1513,
// 1515,
// 1516,
// 1518,
// 1520,
// 1522,
// 1524,
// 1525,
// 1527,
// 1529,
// 1531,
// 1533,
// 1534,
// 1536,
// 1538,
// 1540,
// 1541,
// 1543,
// 1545,
// 1547,
// 1549,
// 1550,
// 1552,
// 1554,
// 1556,
// 1558,
// 1559,
// 1561,
// 1563,
// 1565,
// 1567,
// 1568,
// 1570,
// 1572,
// 1574,
// 1576,
// 1577,
// 1579,
// 1581,
// 1583,
// 1585,
// 1586,
// 1588,
// 1590,
// 1592,
// 1594,
// 1596,
// 1597,
// 1599,
// 1601,
// 1603,
// 1605,
// 1606,
// 1608,
// 1610,
// 1612,
// 1614,
// 1615,
// 1617,
// 1619,
// 1621,
// 1623,
// 1625,
// 1626,
// 1628,
// 1630,
// 1632,
// 1634,
// 1635,
// 1637,
// 1639,
// 1641,
// 1643,
// 1645,
// 1646,
// 1648,
// 1650,
// 1652,
// 1654,
// 1656,
// 1657,
// 1659,
// 1661,
// 1663,
// 1665,
// 1666,
// 1668,
// 1670,
// 1672,
// 1674,
// 1676,
// 1677,
// 1679,
// 1681,
// 1683,
// 1685,
// 1687,
// 1689,
// 1690,
// 1692,
// 1694,
// 1696,
// 1698,
// 1700,
// 1701,
// 1703,
// 1705,
// 1707,
// 1709,
// 1711,
// 1712,
// 1714,
// 1716,
// 1718,
// 1720,
// 1722,
// 1724,
// 1725,
// 1727,
// 1729,
// 1731,
// 1733,
// 1735,
// 1736,
// 1738,
// 1740,
// 1742,
// 1744,
// 1746,
// 1748,
// 1749,
// 1751,
// 1753,
// 1755,
// 1757,
// 1759,
// 1761,
// 1762,
// 1764,
// 1766,
// 1768,
// 1770,
// 1772,
// 1774,
// 1776,
// 1777,
// 1779,
// 1781,
// 1783,
// 1785,
// 1787,
// 1789,
// 1790,
// 1792,
// 1794,
// 1796,
// 1798,
// 1800,
// 1802,
// 1804,
// 1805,
// 1807,
// 1809,
// 1811,
// 1813,
// 1815,
// 1817,
// 1819,
// 1821,
// 1822,
// 1824,
// 1826,
// 1828,
// 1830,
// 1832,
// 1834,
// 1836,
// 1837,
// 1839,
// 1841,
// 1843,
// 1845,
// 1847,
// 1849,
// 1851,
// 1853,
// 1855,
// 1856,
// 1858,
// 1860,
// 1862,
// 1864,
// 1866,
// 1868,
// 1870,
// 1872,
// 1873,
// 1875,
// 1877,
// 1879,
// 1881,
// 1883,
// 1885,
// 1887,
// 1889,
// 1891,
// 1893,
// 1894,
// 1896,
// 1898,
// 1900,
// 1902,
// 1904,
// 1906,
// 1908,
// 1910,
// 1912,
// 1914,
// 1915,
// 1917,
// 1919,
// 1921,
// 1923,
// 1925,
// 1927,
// 1929,
// 1931,
// 1933,
// 1935,
// 1937,
// 1939,
// 1940,
// 1942,
// 1944,
// 1946,
// 1948,
// 1950,
// 1952,
// 1954,
// 1956,
// 1958,
// 1960,
// 1962,
// 1964,
// 1966,
// 1968,
// 1969,
// 1971,
// 1973,
// 1975,
// 1977,
// 1979,
// 1981,
// 1983,
// 1985,
// 1987,
// 1989,
// 1991,
// 1993,
// 1995,
// 1997,
// 1999,
// 2001,
// 2003,
// 2005,
// 2006,
// 2008,
// 2010,
// 2012,
// 2014,
// 2016,
// 2018,
// 2020,
// 2022,
// 2024,
// 2026,
// 2028,
// 2030,
// 2032,
// 2034,
// 2036,
// 2038,
// 2040,
// 2042,
// 2044,
// 2046,
// 2048,
// 2050,
// 2052,
// 2054,
// 2056,
// 2058,
// 2060,
// 2062,
// 2064,
// 2065,
// 2067,
// 2069,
// 2071,
// 2073,
// 2075,
// 2077,
// 2079,
// 2081,
// 2083,
// 2085,
// 2087,
// 2089,
// 2091,
// 2093,
// 2095,
// 2097,
// 2099,
// 2101,
// 2103,
// 2105,
// 2107,
// 2109,
// 2111,
// 2113,
// 2115,
// 2117,
// 2119,
// 2121,
// 2123,
// 2125,
// 2127,
// 2129,
// 2131,
// 2133,
// 2135,
// 2137,
// 2139,
// 2141,
// 2143,
// 2145,
// 2147,
// 2149,
// 2151,
// 2153,
// 2155,
// 2158,
// 2160,
// 2162,
// 2164,
// 2166,
// 2168,
// 2170,
// 2172,
// 2174,
// 2176,
// 2178,
// 2180,
// 2182,
// 2184,
// 2186,
// 2188,
// 2190,
// 2192,
// 2194,
// 2196,
// 2198,
// 2200,
// 2202,
// 2204,
// 2206,
// 2208,
// 2210,
// 2212,
// 2215,
// 2217,
// 2219,
// 2221,
// 2223,
// 2225,
// 2227,
// 2229,
// 2231,
// 2233,
// 2235,
// 2237,
// 2239,
// 2241,
// 2243,
// 2245,
// 2247,
// 2250,
// 2252,
// 2254,
// 2256,
// 2258,
// 2260,
// 2262,
// 2264,
// 2266,
// 2268,
// 2270,
// 2272,
// 2274,
// 2276,
// 2279,
// 2281,
// 2283,
// 2285,
// 2287
//};

/// * Global variables -------------------------------------------- -------------- * /
GyroAcceDataStruct GyroData, AcceData, * pGyroData = & GyroData, * pAcceData = & AcceData;
// float offset = 0;
S16 GyroData_Self_offset = 0;

S16 UpAngleOffset = 0;
 
U8 Offset_Calibration_Flag = 0; // = 1 Allow calibration, = 0; No calibration allowed
/ * Private variables ---------------------------------------------- ------------ * /
S16 L3G4200D_state, ADXL345_state; // communication status bit
S16 L3G4200D_buff [3] = {0,0,0}; // store L3G4200D 3 sets of data
S16 ADXL345_buff [3] = {0,0,0}; // store ADXL345 3 sets of data ADXL345 data high 8 bits in ADXL345_buff low 8 bits high 8 bits and lower 8 bits need to exchange position

// s16 Acce_offset_x, Acce_offset_y, Acce_offset_z; // acceleration once offset value; (need to be read from ROM after power-on, rewrite write acceleration IC calibration register)
/ ********** function statement ********* /
Void delay_us (u32 nus) // delay function
{
 Int i;
 For (; nus> 0; nus--)
  {
   For (i = 7; i> 0; i--);
  }
}
// KECH-01 Motherboard SPI1 Connect Gyro board
// PB3 --------------------------- SPC clock signal
// PB5 --------------------------- SDI data entry
// PB4 --------------------------- SDO data output
// PE15 -------------------------- CS L3G4200D communication chip selection
// PG1 --------------------------- CS1 ADXL345 communication chip selection
// PG0 --------------------------- INT2 The ADXL345 data is ready for interrupts
/ * Configure the clock * / // xx0410
// void Gyro_RCC_Configuration (void)
// {
//// SystemInit ();
//// RCC_PCLK2Config (RCC_HCLK_Div1);
// / * Enable peripheral clocks ------------------------------------------- ------- * /
// / * GPIOA, GPIOB and SPI1 clock enable * /
// RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG |
// RCC_APB2Periph_SPI1, ENABLE); // Turn on the IO port SPI clock
// RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE); // Turn on remapping clock
// GPIO_PinRemapConfig (GPIO_Remap_SPI1, ENABLE); // SPI1 pin remapping
// GPTAG_dg enablement / SW-DP enable release of PB3, PB4 as common IO port (GPIO_Remap_SWJ_JTAGDisable, ENABLE)
//
//} // xx0410
/ * Configure GPIO * / // xx0410
// void Gyro_GPIO_Configuration (void)
// {
// GPIO_InitTypeDef GPIO_InitStructure;
//
// / * Configure SPI1 pins: SCK, MISO and MOSI --------------------------------- *
// / * Confugure SCK and MOSI pins as Alternate Function Push Pull * /
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
// GPIO_InitStructure.GPIO_Speed ​​= GPIO_Speed_50MHz; // IO speed is 50MHZ
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Multiplex push-pull output
// GPIO_Init (GPIOB, & GPIO_InitStructure);
// / * Confugure MISO pin as Input Floating * /
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
// GPIO_Init (GPIOB, & GPIO_InitStructure);
// / * Confugure L3G4200D INT and CS * /
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // Gyro_INT
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // GPIO_Mode_IPD;
// GPIO_Init (GPIOE, & GPIO_InitStructure);
//
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // L3G4200D chip select cs
// GPIO_InitStructure.GPIO_Speed ​​= GPIO_Speed_50MHz; // IO speed is 10MHZ
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Universal push-pull output
// GPIO_Init (GPIOE, & GPIO_InitStructure);
//
// / * Confugure ADXL345 INT and CS * /
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // Acce_INT
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
// GPIO_Init (GPIOG, & GPIO_InitStructure);
// / * Confugure CS pin as Function Push Pull * /
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4; // ADXL345 Chip Select cs1
// GPIO_InitStructure.GPIO_Speed ​​= GPIO_Speed_50MHz; // IO speed is 50MHZ
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Universal push-pull output
// GPIO_Init (GPIOG, & GPIO_InitStructure);
//
//} // xx0410

/ * Configuration SPI1 * /
Void Gyro_SPI1_Configuration (void)
{
 SPI_InitTypeDef SPI_InitStructure;
 // --------------------- SPI1 configuration ------------------
  
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // 128
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
 SPI_InitStructure.SPI_CRCPolynomial = 7;
 SPI_Init (SPI1, & SPI_InitStructure);
 // ----- Enable SPI1 ----
 SPI_Cmd (SPI1, ENABLE);
}

/ * Send half word * /
 
S16 SPI1_SendHalfWord (u16 k)
{
 S16 t;
 While (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_TXE) == RESET); // Check if the specified SPI flag is set to 0,
 SPI_I2S_SendData (SPI1, k); // send a byte of data via the peripheral SPI1
 While (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_RXNE) == RESET);
 T = SPI1-> DR;
 Return (t);
}

Void WriteReg (u16 Reg, u8 Data) // Write data to the corresponding register
{
 U16 a;
 A = Reg | Data;
 SPI1_SendHalfWord (a);
 Delay_us (100);
}
S16 Byte_ReadReg (u16 Reg) // Read the corresponding register data
{
  U16 b;
  S16 t;
  B = Reg | 0x8000;
  T = SPI1_SendHalfWord (b);
  Return (t);
}

S16 Multiple_Byte_ReadReg (u16 Reg) // Multi-byte Reads the corresponding register data
{
 U16 c;
 S16 t;
 C = Reg | 0xC000;
 T = SPI1_SendHalfWord (c);
 Return (t);
}

Void L3G4200D_init (void)
{
// the following general purpose sequence can be used to configure the device:
// 1. Write CTRL_REG2
// 2. Write CTRL_REG3
// 3. Write CTRL_REG4
// 4. Write CTRL_REG6
// 5. Write Reference
// Write INT1_THS
// 7. Write INT1_DUR
// 8. Write INT1_CFG
// 9. Write CTRL_REG5
// Write CTRL_REG1
/ * Configuration data output rate 200Hz, range 250dps 2012-10-10 lihewen * /
 
 ACC_CS_PU; / / determine the ADXL345 chip selected high
 Delay_us (1);
 GYRO_CS_PD;
 Delay_us (1);
 WriteReg (CTRL_REG2,0x09); //// set high-pass filter 0.02HZ
 GYRO_CS_PU; //
 Delay_us (1);
 
  GYRO_CS_PD;
  Delay_us (1);
 WriteReg (CTRL_REG3,0x08); // set the I2_DRDY interrupt
 GYRO_CS_PU;
 Delay_us (1);
 
  GYRO_CS_PD;
  Delay_us (1);
 // WriteReg (CTRL_REG4,0x40); // BDU is 0 BLE is 1 high data at low address range 250dps SPI4 line system
 WriteReg (CTRL_REG4,0xC0); // BDU is 1:
 // output registers not updated until MSB and LSB reading

 // WriteReg (CTRL_REG4,0x50); // FS = 500dps
 GYRO_CS_PU; // CTRL_REG4_MSB = 1: output registers not updated until MSB and LSB reading
 Delay_us (1);
  
   GYRO_CS_PD;
   Delay_us (1);
 // WriteReg (REFERENCE, 0x88); // set reference
 WriteReg (REFERENCE, 0x00); // set reference Reference value for Interrupt generation. Default value: 0
 GYRO_CS_PU;
 Delay_us (1);
 
   GYRO_CS_PD;
   Delay_us (1);
 // WriteReg (CTRL_REG5,0x15);
 WriteReg (CTRL_REG5,0x5F); // follow these steps for bypass mode configuration, AN3393 p.31

 GYRO_CS_PU;
 Delay_us (1);
 
  GYRO_CS_PD;
  Delay_us (1);
  WriteReg (FIFO_CTRL_REG, 0x00); // follow these steps for bypass mode configuration, AN3393 p.31
  GYRO_CS_PU;
  Delay_us (1);

 GYRO_CS_PD;
 Delay_us (1);
 // WriteReg (CTRL_REG1,0x9F); // Output rate 400HZ X, Y, Z axis enable measurement // Cut-Off = 25Hz
 // WriteReg (CTRL_REG1,0x4F); // Output rate 200HZ X, Y, Z axis enable measurement //Cut-Off=12.5Hz
 WriteReg (CTRL_REG1,0x5F); // Output rate 200HZ X, Y, Z axis enable measurement // Cut-Off = 25Hz
 GYRO_CS_PU;
 Delay_us (1);
  
}
/ ************************************************* *****************************
ADXL345_Offset_init
Description: Acceleration sensor 0 bias calibration;
Uses: used in the factory, or user balance point calibration.
Conditions of use: 1) the acceleration sensor is called after initialization;
   2) the machine remains stationary at the specified level;
   3) start the initialization state, receive calibration instructions, allowing calibration;
   4) when running, prohibit calibration;
   5)
ADXL345 biased calibration register due to the accuracy of the need for secondary calibration!
Input parameters
OffsetAddr: offset register address, OFSX - x axis; OFSY - y axis; OFSZ - z axis;
OffsetValue: calibration value, chip save static level state read a set of values, seeking (integral) average, the symbol is reversed.
Note: The calibration data is stored in nonvolatile memory and is called when the self-test is started.
************************************************** ***************************** /
// void ADXL345_Offset_init (u16 OffsetAddr, s8 OffsetValue) // xx0410
// {
// ACC_CS_PD;
// WriteReg (OffsetAddr, OffsetValue); // Write the corresponding axis offset // Each signal board offset value (possible) is different, the offset value needs to be tested after the calibration is written
// ACC_CS_PU;
//} // xx0410
// read and write within Flash to be completed
S16 Read_OffsetROM (u32 addr)
{
To be completed
 Return (0xFFFF);
}
S16 Write_OffsetROM (u32 addr, s16 data)
{
To be completed
 Return (0);
}

// void ADXL345_init (void) // xx0410
// {
// // minimum initialization sequence
// // START
// // VS = ON; VDDI / O = ON;
// // WAIT 1.1ms;
// // INITIALIZE COMMAND SEQUENCE:
// // DATA_FORMAT; // ± 2 g, full resolution mode (10-BIT MODE)
// // POWER_CTL; // START MEASUREMENT
// // INT_ENABLE; // ENABLE DATA_READY INTERRUPT.
// // END
// / * configuration 200Hz data output rate, ± 2 g 2012-10-10 lihewen * /
// GYRO_CS_PU; // Make sure the L3G4200D chip is pulled high
// ACC_CS_PD;
// writeReg (DATA_FORMAT, 0x08); // set the full resolution of 10 bits, the data right-aligned, range + -2g
// ACC_CS_PU;
//
// ACC_CS_PD;
// // WriteReg (BW_RATE, 0x0C); // Set the data output rate F3200 E1600 D800 C400 B200
// WriteReg (BW_RATE, 0x0B); // Set the data output rate B200, BW = 100Hz
// ACC_CS_PU;
// ACC_CS_PD;
// WriteReg (INT_ENABLE, 0x80); // enable interrupt DATA READY
// ACC_CS_PU;
//
//
// ACC_CS_PD;
// WriteReg (POWER_CTL, 0x08); // Select the measurement mode
// ACC_CS_PU;
// ACC_CS_PD;
// WriteReg (INT_ENABLE, 0x80); // enable interrupt DATA READY
// ACC_CS_PU;
//
//}
Void LIS331DLH_init (void)
{
GYRO_CS_PU; / / determine the L3G4200D chip selected high
Delay_us (100);
ACC_CS_PD;
Delay_us (1);
WriteReg (A_CTRL_REG2,0x03); // 0x07 set the high-pass
ACC_CS_PU; // 0410 // change to 0x03 filter bypass

Delay_us (1);
ACC_CS_PD;
Delay_us (1);
WriteReg (A_CTRL_REG3,0x06); // interrupt control
ACC_CS_PU;

Delay_us (1);
ACC_CS_PD;
Delay_us (1);
WriteReg (A_CTRL_REG4,0xC0); // 0410 // 0x40 to 0xC0 BDU = 1: output registers not updated between MSB and LSB reading
ACC_CS_PU; // Full-scale selection: ± 2g;

Delay_us (1);
ACC_CS_PD;
Delay_us (1);
WriteReg (A_CTRL_REG5,0x00);
ACC_CS_PU;

Delay_us (1);
ACC_CS_PD;
Delay_us (1);
WriteReg (A_CTRL_REG1,0x2F); // 0x37 Output rate 400HZ X, Y, Z axis enable measurement
ACC_CS_PU; // 0x2F Output rate 100HZ X, Y, Z axis enable measurement
 
}
 
 

Void Read_LIS331DLH_Data (void)
{// read ADXL345 data
U8 t = 0;
If (Get_Acce_RDY == 1) // judge if the ADXL345 data is ready
{
ACC_CS_PD;
Delay_us (1);
ADXL345_state = Multiple_Byte_ReadReg (A_STATUS_REG); // read from address 31, address automatically add one
For (t = 0; t <3; t ++)
{
ADXL345_buff [t] = SPI1_SendHalfWord (0x8000);} // read data
ACC_CS_PU;
}
}

Void Read_L3G4200D_Data (void)
{
 // read L3G4200D data
// read angular rate data
// 1. using the status register: Read STATUS_REG;
// 2. using the data-ready (DRY) signal;
// 3. using the block dat a update (BDU) feature.

 U8 i;
 If (Get_Gyro_RDY == 1)
 {
  ACC_CS_PU; / / determine the ADXL345 chip selected high
  GYRO_CS_PD; // select L3G4200D
  Delay_us (1);
    L3G4200D_state = Multiple_Byte_ReadReg (STATUS_REG); // read from address 27, address automatically add one
//
// L3G4200D_state = (L3G4200D_state & 0x0088);
// if (L3G4200D_state == 0x0088) / / determine whether the data has been updated / / query may be missing data or data can not be updated! To study lihewen 2012-09-09
// {
// L3G4200D_state = 0;
   For (i = 0; i <3; i ++)
   {
     // read out the data
    L3G4200D_buff [i] = SPI1_SendHalfWord (0X8000);
   }
//}
  GYRO_CS_PU;
 }
}

// Read_ADXL345_Data () // xx0410
// read ADXL345 data
// u8 i;
// if (GPIO_ReadInputDataBit (GPIOG, GPIO_Pin_0) == 1) // judge if the ADXL345 data is ready
// {
// GYRO_CS_PU; // release L3G4200D chip select (set high)
// delay_us (1);
// ACC_CS_PD; // select ADXL345 (set low)
// ADXL345_state = Multiple_Byte_ReadReg (DATA_FORMAT); // read from address 31, address automatically add one
//
// for (i = 0; i <3; i ++)
// {
// // read the data
// ADXL345_buff [i] = SPI1_SendHalfWord (0x8000);
//}
// ACC_CS_PU;
//}
//}

Gyro_init (void)
{
    // Gyro board resources are initialized
 Delay_us (5000); // L3G4200D and then delay 5ms
 L3G4200D_init ();
// delay_us (1000);
 // Communication confirmed successfully
 ACC_CS_PU; //// release ADXL345 chip select (set high)
 Delay_us (1);
 GYRO_CS_PD; // select L3G4200D (low)
   L3G4200D_state = Byte_ReadReg (WHO_AM_I); // read the device address
 
 GYRO_CS_PU; // release L3G4200D chip select (set high)
 L3G4200D_state = L3G4200D_state & 0x00FF;
 Delay_us (1000);
}

Acce_init (void)
{
 Delay_us (5000); // power-up delay
// ADXL345_init ();
 LIS331DLH_init ();
 GYRO_CS_PU; / / determine the L3G4200D chip selected high
 ACC_CS_PU;
 Delay_us (1);
 ACC_CS_PD; // check ADXL345
 Delay_us (1);
 // ADXL345_state = Byte_ReadReg (DEVID); // read the device address
 ADXL345_state = Byte_ReadReg (A_WHO_AM_I); // read device address
 ACC_CS_PU;
 ADXL345_state = ADXL345_state & 0x00FF;
 Delay_us (30);
}
 

U8 STM32_Gyro_init (void)
{// STM32 chip resources are initialized
    U8 i, state = 0;
// Gyro_RCC_Configuration ();
// Gyro_GPIO_Configuration (); // xx0410
 Gyro_SPI1_Configuration ();
 ACC_CS_PU;
 GYRO_CS_PU; // film picks high
 Delay_us (15000);
 For (i = 0; i <3; i ++) // gyro sensor is initialized and unsuccessful
 {
  Gyro_init ();
  If (L3G4200D_state == 0x00D3)
  {
   State = 1;
   Break
  }
  Else
  {
   State = 0;
  }
 }
 For (i = 0; i <3; i ++) // acceleration sensor initialization, unsuccessful exit within 3 times
 {
  Acce_init ();
  If (ADXL345_state == 0x0032)
  {
   State | = 2;
   Break
  }
  Else
  {
   State & = 1;
  }
 }
 Return (state);
}
// void Read_Gyro_Data (void)
// {
// read_L3G4200D_Data ();
// Read_ADXL345_Data ();
//}
/ * Acceleration read data, high and low byte exchange * /
S16 swap_HLbyte (u16 swapdata)
{
 U16 tmp1, tmp2;
 Tmp1 = swapdata;
 Tmp1 >> = 8;
 Tmp1 & = 0x00FF;
 Tmp2 = swapdata;
 Tmp2 << = 8;
 Tmp2 & = 0xFF00;
 Return (s16) (tmp1 | tmp2);
}
Void Get_Gyro_Value (void)
{
 Read_L3G4200D_Data ();
// pGyroData -> x_Data = (L3G4200D_buff [0] >> 1) + pGyroData -> offset_x;
// pGyroData -> y_Data = (L3G4200D_buff [1] >> 1) + pGyroData -> offset_y;
// pGyroData -> z_Data = (L3G4200D_buff [2] >> 1) + pGyroData -> offset_z;
 PGyroData -> x_Data = (L3G4200D_buff [0]) + pGyroData -> offset_x;
 PGyroData -> y_Data = (L3G4200D_buff [1]) + pGyroData -> offset_y;
 PGyroData -> z_Data = (L3G4200D_buff [2]) + pGyroData -> offset_z;
}
Void Get_Acce_Value (void)
{
 // Read_ADXL345_Data ();
 Read_LIS331DLH_Data ();
 // test: ok
// ADXL345_buff [0] = 0xFFDD;
// ADXL345_buff [1] = 0x1234;
// ADXL345_buff [2] = 0xABCD;
// pAcceData -> x_Data = swap_HLbyte (ADXL345_buff [0]) + pAcceData -> offset_x;
// pAcceData -> y_Data = swap_HLbyte (ADXL345_buff [1]) + pAcceData -> offset_y;
// pAcceData -> z_Data = swap_HLbyte (ADXL345_buff [2]) + pAcceData -> offset_z;
// pAcceData -> x_Data = (ADXL345_buff [0] >> 3) + pAcceData -> offset_x;
// pAcceData -> y_Data = (ADXL345_buff [1] >> 3) + pAcceData -> offset_y;
// pAcceData -> z_Data = (ADXL345_buff [2] >> 3) + pAcceData -> offset_z;
// pAcceData -> x_Data = (ADXL345_buff [0] >> 5) + pAcceData -> offset_x;
// pAcceData -> y_Data = (ADXL345_buff [1] >> 5) + pAcceData -> offset_y;
// pAcceData -> z_Data = (ADXL345_buff [2] >> 5) + pAcceData -> offset_z;
 PAcceData -> x_Data = (ADXL345_buff [0] >> 4) + pAcceData -> offset_x;
 pAcceData -> y_Data = (ADXL345_buff[1]>>4) + pAcceData -> offset_y;
 pAcceData -> z_Data = (ADXL345_buff[2]>>4) + pAcceData -> offset_z;
}

/ * Private functions ---------------------------------------------- ----------- * /
/*******************************************************************************
* Function Name  : Pitch_Yaw_Calc()
* Description    : Routine to be performed Pitch_Yaw calculation ...
* Input          : L3G4200D_buff[],ADXL345_buff[];
* Output         : calculated results of pitch_rate,yaw_rate,roll_rate,pitch,roll; 
* Return         : 
* Author              : lihewen
* Date of build    : //2012.09.07
* Date of modified   : //2012 
* Date First Issued   : //2012
* Note : yaw can not be calculated by integration method without reference of Acceeleration 
************************************************** ***************************** /
//GyroAcceDataStruct  GyroData, AcceData, *pGyroData = &GyroData, *pAcceData = &AcceeData;
void Pitch_Yaw_Calc(void) //滤波，换算成角度值，再kalman滤波
{
 U16 i;
 s32 temp=0;
 static s16 GyroDataXk_1=0;
 static s16 AcceDataXk_1=0;
 //Gyro:
 if(pGyroData -> x_Data > 28571) //14285) //28571) //
  pGyroData -> xdatapu875 = 28571;  //14285;
 else if(pGyroData -> x_Data < -28571) //14285) //28571)
  pGyroData -> xdatapu875 = -28571;  //14285;
 Else
  pGyroData -> xdatapu875 = pGyroData -> x_Data;
 if(pGyroData -> y_Data > 28571) //14285) //28571)
  pGyroData -> ydatapu875 = 28571;  //14285;
 else if(pGyroData -> y_Data < -28571) //14285) //28571)
  pGyroData -> ydatapu875 = -28571;  //14285;
 Else
  pGyroData -> ydatapu875 = pGyroData -> y_Data;
 if(pGyroData -> z_Data > 28571) //14285) //28571)
  pGyroData -> zdatapu875 = 28571;  //14285;
 else if(pGyroData -> z_Data < -28571) //14285) //28571)
  pGyroData -> zdatapu875 = -28571;  //14285;	 //28571;  //
 Else
  pGyroData -> zdatapu875 = pGyroData -> z_Data;
 //GyroLPF:  //2012-12-08
 //0点噪声去除：
// if((pGyroData -> xdatapu875 > -50)&&(pGyroData -> xdatapu875 < 50))
// if((pGyroData -> xdatapu875 > -100)&&(pGyroData -> xdatapu875 < 100))
// 	pGyroData -> xdatapu875 = 0;
	pGyroData-> x_LPF_Previous = pGyroData -> xdatapu875; //test
	//0点噪声去除修改：100修改为60
	if(pGyroData -> xdatapu875 > 60)
		pGyroData -> xdatapu875 -=60;
	else if(pGyroData -> xdatapu875 < -60)
		pGyroData -> xdatapu875 +=60;
Else
		pGyroData -> xdatapu875 =0;

 temp = GyroDataXk_1*3; //7;//15;   // *3;	  //滤波截止频率进一步降低2012-12-22
 temp += pGyroData -> xdatapu875;
 temp >>=2;  //3;  //4;//2;
 pGyroData -> xdatapu875 = (s16)temp;
 GyroDataXk_1 = pGyroData -> xdatapu875;
 //Acce LPF:  //2倍超取样
// pAcceData -> x_Data += AcceDataXk_1*3;
// pAcceData -> x_Data +=1; //四舍五入
// pAcceData -> x_Data >>=1;
// AcceDataXk_1 = pAcceData -> x_Data>>1;
 //Acce LPF:  //2倍超取样 //滤波截止频率进一步降低2012-12-22
 //0点噪声去除：
// if((pAcceData -> x_Data >= -2)&&(pAcceData -> x_Data <= 2))
// if((pAcceData -> x_Data >= -6)&&(pAcceData -> x_Data <= 6))
// 	pAcceData -> x_Data = 0;

	//0点噪声去除修改：	5改为4，再改为2
	if(pAcceData -> x_Data > 2)
 		pAcceData -> x_Data -= 2;
	else if(pAcceData -> x_Data < -2)
 		pAcceData -> x_Data += 2;
Else
 		pAcceData -> x_Data = 0;

 temp = AcceDataXk_1*3;  //15;
// temp >>=1;	   //2倍超取样
 temp += pAcceData -> x_Data; 
// temp >>=1;	   //2倍超取样
 temp >>=2;  //;
 AcceDataXk_1 =(s16)temp;
 pAcceData -> x_Data = AcceDataXk_1;

 //Acce:
 if(pAcceData -> x_Data < 0)
 {
  i = -pAcceData -> x_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317; //330; //
  pAcceData -> xdatapu875 = -ASIN875Tab[i];
 }
 Else
 {
  i =  pAcceData -> x_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317;  //330;
  pAcceData -> xdatapu875 = ASIN875Tab[i];
 }
 if(pAcceData -> y_Data < 0)
 {
  i = -pAcceData -> y_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317;  //330;
  pAcceData -> ydatapu875 = -ASIN875Tab[i];
 }
 Else
 {
  i =  pAcceData -> y_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317;  //330;
  pAcceData -> ydatapu875 = ASIN875Tab[i];
 }
 if(pAcceData -> z_Data < 0)
 {
  i = -pAcceData -> z_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317;  //330;
  pAcceData -> zdatapu875 = -ASIN875Tab[i];
 }
 Else
 {
  i =  pAcceData -> z_Data;
  if(i > 660)  //330) //1317)  //330)
   i = 660;  //330; //1317;  //330;
  pAcceData -> zdatapu875 = ASIN875Tab[i];
 }
// pAcceData -> xdatapu875 = pAcceData -> x_Data * 26;
// pAcceData -> ydatapu875 = pAcceData -> y_Data * 26;
// pAcceData -> zdatapu875 = pAcceData -> z_Data * 26;
}
/ *
Low pass filter
Yk = K * Xn + (1-K) * Yk_1; fL ≈ K / (2πT)
// K = i / k; K takes 1/4, 1/2, 5/8, 3/4, 7/8 ....
* /
void LowPassFilterK1_2(GyroAcceDataStruct *pxstruct)  //fL = 1/(4PiT)
{
 //输出滤波值:
 pxstruct -> x_Data += pxstruct -> x_LPF_Previous;
 pxstruct -> x_Data >>= 1;
 pxstruct -> y_Data += pxstruct -> y_LPF_Previous;
 pxstruct -> y_Data >>= 1; 
 pxstruct -> z_Data += pxstruct -> z_LPF_Previous;
 pxstruct -> z_Data >>= 1;
 //保存Yk-1:
 pxstruct -> x_LPF_Previous = pxstruct -> x_Data;
 pxstruct -> y_LPF_Previous = pxstruct -> y_Data;
 pxstruct -> z_LPF_Previous = pxstruct -> z_Data;
}
//void LPF_KS2(GyroAcceDataStruct *pxstruct)  //低通滤波超取样,2倍精度输出
//{            //若精度不足时采用
// //输出滤波值:
// pxstruct -> x_Data2 = pxstruct -> x_Data + pxstruct -> x_LPF_Previous;
// //pxstruct -> x_Data >>= 1;
// pxstruct -> y_Data2 = pxstruct -> y_Data + pxstruct -> y_LPF_Previous;
// //pxstruct -> y_Data >>= 1; 
// pxstruct -> z_Data2 = pxstruct -> z_Data + pxstruct -> z_LPF_Previous;
// //pxstruct -> z_Data >>= 1;
// //保存Yk-1:
// pxstruct -> x_LPF_Previous = pxstruct -> x_Data2 >> 1;
// pxstruct -> y_LPF_Previous = pxstruct -> y_Data2 >> 1;
// pxstruct -> z_LPF_Previous = pxstruct -> z_Data2 >> 1;
//
//}
/ *
求积分
Yk = K * Xn + (1-K) * Yk_1; fL ≈ K / (2πT)
// K = i / k; K takes 1/4, 1/2, 5/8, 3/4, 7/8 ....
* /
void Integral(GyroAcceDataStruct *pxstruct)
{
 pxstruct -> x_Integral += pxstruct -> x_Data;
 pxstruct -> y_Integral += pxstruct -> y_Data;
 pxstruct -> z_Integral += pxstruct -> z_Data;
}

//void Acce_Offset_init(s8 x, s8 y, s8 z) //xx0410
// {
// ACC_CS_PD;
// WriteReg(OFSX,x);     //X轴偏移量 //每块信号板偏移值(可能)不同，偏移值需要测试校准后写入
// ACC_CS_PU;
//
//  ACC_CS_PD;
// WriteReg(OFSY,y);     //Y轴偏移量//每块信号板偏移值(可能)不同，偏移值需要测试校准后写入
// ACC_CS_PU;
//
// ACC_CS_PD;
// WriteReg(OFSZ,z);     //Z轴偏移量  //每块信号板偏移值(可能)不同，偏移值需要测试校准后写入
// ACC_CS_PU;
// delay_us(1000);
//}
/****************************************************************************** 
Gyro_Acce_Offset_Calibration()
描述：陀螺仪、加速度传感器0偏置校准；
用途：用于出厂，或用户平衡点校准。
使用条件： 1)陀螺仪、加速度传感器初始化后调用；
   2)机器保持静止在规定的水平状态；
   3)开机初始化状态，接收到校准指令时，允许校准；
   4)运行状态时，禁止校准；
   5)
备注：校准数据存储在非易失存储器中，开机自检时调用。
self-test & offset Calibration
//偏移具体限制值，待试验确定！
************************************************** ***************************** /
void Gyro_Acce_Offset_Calibration(s16 *buff)
{
 s16 x,y,z;
 //加速度一次偏置值:(需在上电后从ROM读出，重新写入写入加速度IC校准寄存器)
// s8 Acce_offset_x,Acce_offset_y,Acce_offset_z;  
// s32 xtmp,ytmp,ztmp;
 s16 i; 
// if (Offset_Calibration_Flag)
// {
  //Accelerometer 1st  offset reset:
//  Acce_offset_x=0;
//  Acce_offset_y=0;
//  Acce_offset_z=0;
  //Accelerometer 2nd  offset reset:
  pAcceData -> offset_x=0;
  pAcceData -> offset_y=0;
  pAcceData -> offset_z=512; //-2048;  //Z+1g = 256;
  //Gyroscope offset reset:
  pGyroData -> offset_x=0;
  pGyroData -> offset_y=0;
  pGyroData -> offset_z=0;
  //Acce_Offset_reset:
//  Acce_Offset_init(0, 0, 0); //xx0410
  //加速度一次校准:
//     for(i=0;i<512;i++)
//     {
//   while(Get_Acce_RDY!=1) //等待数据有效
//   {
//    ; //死循环保护,故障标志输出
//   }
//   Get_Acce_Value();
//
//   //LowPassFilterK1_2(pAcceData);
//   Integral(pAcceData);
//   //是否会积分饱和判别:...........MAX=127*256=32512, MIN=-127*256=-32512//
//   if((pAcceData -> x_Integral > 51200)||(pAcceData -> x_Integral < -51200))
//    break;
//   if((pAcceData -> y_Integral > 51200)||(pAcceData -> y_Integral < -51200))
//    break;
//   if((pAcceData -> z_Integral > 51200)||(pAcceData -> z_Integral < -51200))
//    break;
//   //Integral(pAcceData);
////   while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)==1) //等待管脚变低
////   {
////    ;
////   }
//   delay_us(1000);
//   
//}
//  x =(s16)(pAcceData -> x_Integral/i);
//  y =(s16)(pAcceData -> y_Integral/i);
//  z =(s16)(pAcceData -> z_Integral/i);
  // test:
//  pAcceData -> offset_x = -1*x;
//  pAcceData -> offset_y = -1*y;
//  pAcceData -> offset_z = -1*z;

//  x >>= 2;   //*1/4
//  y >>= 2;
//  z >>= 2;
//  x *= -1;   //符号取反
//  y *= -1;
//  z *= -1;
//  //判断校准值是否超标，超标时进行限定，或者要求用户检查是否放置水平并保持静止,重新校准!
//  //偏移具体限制值，待试验确定！下同...
//  if(x>127)
//   x=127;    //超标,或者输出显示"要求用户检查是否放置水平并保持静止"； 下同
//  else if(x<-128)
//   x=-128;
// else else
//   Acce_offset_x=(s8)x;
//  if(y>127)
//   y=127;
//  else if(x<-128)
//   y=-128;
// else else
//   Acce_offset_y=(s8)y;
//  if(z>127)
//   z=127;
//  else if(x<-128)
//   z=-128;
// else else
//   Acce_offset_z=(s8)z;
//  //Acce_Offset_set:
//  buff[2] = Acce_offset_x;
//  Acce_Offset_init(Acce_offset_x, Acce_offset_y, Acce_offset_z); //xx0410
  //保存加速度一次校准值到非易失存储器：
  //Save_Acce1st_offset();  
  //delay();
  // AcceData Integral Reset:
  pAcceData -> x_Integral = 0;
  pAcceData -> y_Integral = 0;
  pAcceData -> z_Integral = 0;
  
  //加速度二次校准：
  for(i=0;i<512;i++)
     {
   while(Get_Acce_RDY!=1) //等待数据有效
   {
    ;  //死循环保护,故障标志输出
   }
   Get_Acce_Value ();
 
   // LowPassFilterK1_2 (pAcceData);
   Integral(pAcceData);
   //是否会积分饱和判别:...........MAX=512*256=131072  //MAX=512*1024=524288//
   if((pAcceData -> x_Integral > 131072)||(pAcceData -> x_Integral < -131072))
    Break
   if((pAcceData -> y_Integral > 131072)||(pAcceData -> y_Integral < -131072))
    Break
   if((pAcceData -> z_Integral > 131072)||(pAcceData -> z_Integral < -131072))
    Break
   //Integral(pAcceData);
//   while(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)==1) //等待管脚变低
//   {
//    ;
//   }
   Delay_us (1000);
   
  }
  x =(s16)(pAcceData -> x_Integral/i);
  y =(s16)(pAcceData -> y_Integral/i);
  z =(s16)(pAcceData -> z_Integral/i);
  x *= -1;   //符号取反
  y *= -1;
  z *= -1;
  //判断校准值是否超标，超标时进行限定，或者要求用户检查是否放置水平并保持静止,重新校准!
  if(x>256)
   x=256;    //超标,或者输出显示"要求用户检查是否放置水平并保持静止"； 下同
  else if(x<-256)
   x=-256;
  if(y>256)
   y=256;
  else if(x<-256)
   y=-256;
  if(z>256)
   z=256;
  else if(x<-256)
   z=-256;

  //保存加速度二次校准值到非易失存储器：
  //Save_Acce2st_offset();  
  buff[3]= x+46;	//46=脚撑偏短补偿值
  buff[7]= y;
  buff[8]= z;
//  pAcceData -> offset_x = x;
//  pAcceData -> offset_y = y;
//  pAcceData -> offset_z = z;
     
  //L3G4200校准:
  for(i=0;i<512;i++)
     {
   while(Get_Gyro_RDY!=1) //等待数据有效
   {
    ;//死循环保护,故障标志输出
   }
//   while(!Gyro_Data_ready);
   Get_Gyro_Value ();
   LowPassFilterK1_2(pGyroData);
   Integral(pGyroData);
   //是否会积分饱和判别:...........MAX=127*256=, MIN=-128*256//
   if((pGyroData -> x_Integral > 65536)||(pGyroData -> x_Integral < -65536))
    Break
   if((pGyroData -> y_Integral > 65536)||(pGyroData -> y_Integral < -65536))
    Break
   if((pGyroData -> z_Integral > 65536)||(pGyroData -> z_Integral < -65536))
    Break
   //Integral(pGyroData);
//   while(Get_Gyro_RDY==1) //等待管脚变低
//   {
//    ;
//   }
   Delay_us (1000);
   Gyro_Data_ready=0;
   
  }
  x =(s16)(pGyroData -> x_Integral/i);
  y =(s16)(pGyroData -> y_Integral/i);
  z =(s16)(pGyroData -> z_Integral/i);
  x *= -1;   //符号取反
  y *= -1;
  z *= -1;
  //判断校准值是否超标，超标时进行限定，或者要求用户检查是否放置水平并保持静止,重新校准!
//  if(x>127)
//   pGyroData -> offset_x=127;    //超标,或者输出显示"要求用户检查是否放置水平并保持静止"； 下同
//  else if(x<-128)
//   pGyroData -> offset_x=-128;
// else else
//   pGyroData -> offset_x =(s8)x;
//
//  if(y>127)
//   pGyroData -> offset_y=127;
//  else if(x<-128)
//   pGyroData -> offset_y=-128;
// else else
//   pGyroData -> offset_y=(s8)y;
//
//  if(z>127)
//   pGyroData -> offset_z=127;
//  else if(x<-128)
//   pGyroData -> offset_z=-128;
// else else
//   pGyroData -> offset_z=(s8)z;

  pGyroData -> offset_x=x;
  pGyroData -> offset_y=y;
  pGyroData -> offset_z=z;
  buff[4]= x;
  buff[5]= z;

 
  //保存陀螺仪校准值到非易失存储器，供以后上电初始化调用
  //Save_Gyro_offset();
  //;
  //读取非易失存储器中的offset校准数据，比较确认写入是否正确
  //............
//  Offset_Calibration_Flag = 0;
//}
// else  //不要求校准时，直接读出以前保存的偏置值，供运算使用
// {
//   //由非易失存储器读取一次校准值，写入ADXL345偏置寄存器
//  Acce_offset_x = (s8)Read_OffsetROM(Acce_offset_x_addr1);
//  Acce_offset_y = (s8)Read_OffsetROM(Acce_offset_y_addr1);
//  Acce_offset_z = (s8)Read_OffsetROM(Acce_offset_z_addr1);
//  ADXL345_Offset_init(OFSX, Acce_offset_x);
//  ADXL345_Offset_init(OFSY, Acce_offset_y);
//  ADXL345_Offset_init(OFSZ, Acce_offset_z);
//
//  //由非易失存储器读取一次校准值，运算使用
//  pAcceData -> offset_x = Read_OffsetROM(Acce_offset_x_addr2);
//  pAcceData -> offset_y = Read_OffsetROM(Acce_offset_y_addr2);
//  pAcceData -> offset_z = Read_OffsetROM(Acce_offset_z_addr2);
//
//  //由非易失存储器读取L3G4200D校准值，运算使用
//  pGyroData -> offset_z = Read_OffsetROM(Gyro_offset_x_addr);
//  pGyroData -> offset_z = Read_OffsetROM(Gyro_offset_y_addr);
//  pGyroData -> offset_z = Read_OffsetROM(Gyro_offset_z_addr); 
//}
 
}
 
//void Gyro_Offset_Calibration(void)
// {
// s16 x,y,z;
// s16 static i; 
//
//
//  //L3G4200校准:
//
//
// Integral(pGyroData);
//
// i++;
//
// if(i==2048)
// {
//
//  x =(s16)(pGyroData -> x_Integral/i);
//  y =(s16)(pGyroData -> y_Integral/i);
//  z =(s16)(pGyroData -> z_Integral/i);
//  x *= -1;   //符号取反
//  y *= -1;
//  z *= -1;
//  //判断校准值是否超标，超标时进行限定，或者要求用户检查是否放置水平并保持静止,重新校准!
//  if(x>127)
//   pGyroData -> offset_x=127;    //超标,或者输出显示"要求用户检查是否放置水平并保持静止"； 下同
//  else if(x<-128)
//   pGyroData -> offset_x=-128;
// else else
//   pGyroData -> offset_x =(s8)x;
//
//  if(y>127)
//   pGyroData -> offset_y=127;
//  else if(x<-128)
//   pGyroData -> offset_y=-128;
// else else
//   pGyroData -> offset_y=(s8)y;
//
//  if(z>127)
//   pGyroData -> offset_z=127;
//  else if(x<-128)
//   pGyroData -> offset_z=-128;
// else else
//   pGyroData -> offset_z=(s8)z;
//
//  i=0;
//
//}
//
//}
//void Gyro_Acce_process(void)
// {
////data transform
////1. Gyro (8.75/1000) dps/1LSB
////2. Acce 
//;
//
//}
 

/*Kalman Filter 2012-10-10*/
//http://bbs.21ic.com/icview-221319-1-1.html 

// float gyro_m：陀螺仪测得的量（角速度）
//float incAngle:加计测得的角度值
////卡尔曼滤波参数
#define dt               0.0047//卡尔曼滤波采样频率
#define R_angle          0.6667 //测量噪声的协方差（即是测量偏差）
#define Q_angle          0.0001//过程噪声的协方差
#define Q_gyro           0.0003 //过程噪声的协方差  过程噪声协方差为一个一行两列矩阵
//执行时间*/128.0=7.278us;屏蔽*/128.0=6.819us
//float kalmanUpdate(const s16 gyro,const s16 Acce)
s16 kalmanUpdate(const s16 gyro,const s16 Acce)   
{
  float gyro_m, incAngle;         
        float K_0;//含有卡尔曼增益的另外一个函数，用于计算最优估计值
        float K_1;//含有卡尔曼增益的函数，用于计算最优估计值的偏差
        float Y_0;
        float Y_1;
        
        float Rate;//去除偏差后的角速度
        float Pdot[4];//过程协方差矩阵的微分矩阵
        float angle_err;//角度偏量
        float E;//计算的过程量
                
        static float angle = 0;            //下时刻最优估计值角度
        static float q_bias = 0;        //陀螺仪的偏差
        static float P[2][2] = {{ 1, 0 }, { 0, 1 }};//过程协方差矩阵
  //float anglek;
  s16	anglek;
  //gyro=0xFFBF,Acce=0x001A;
  gyro_m = (float)(gyro); ///128.0);   //=-0.5078125 //-23.40625
  incAngle = (float)(Acce); ///128.0);   //= 0.203125 //28.64844
                  
        Rate = gyro_m - q_bias;     //=-0.4900703 //q_bias=-1.954954,Rate=-21.4513 
    //s16Q0=s16Q0-s16Q0
        //计算过程协方差矩阵的微分矩阵//Pdot[]={0.002069033,-0.0007878614,-0.0007878614,0.0003}
        Pdot[0] = Q_angle - P[0][1] - P[1][0];   //s32Q31, Q_angle,P[][]==s32Q31     
        Pdot[1] = - P[1][1];                     //s32Q31    
        Pdot[2] = - P[1][1];                     //s32Q31             
        Pdot[3] = Q_gyro;                        //s32Q31  Q_gyro==s32Q31
  //angle=0.104875
        angle += Rate * dt; //角速度积分得出角度 //=0.1025717 //27.78343
  //s16Q0*s16Q15 = s32Q15

  //P[][]={0.002585493,-0.0009845167,-0.0009845168,0.0007878614}
        P[0][0] += Pdot[0] * dt; //计算协方差矩阵//Q31*Q15
        P[0][1] += Pdot[1] * dt; 
        P[1][0] += Pdot[2] * dt; 
        P[1][1] += Pdot[3] * dt; //P[][]={0.002595218,-0.0009882196,-0.0009882197,0.0007892714}
  

        angle_err = incAngle - angle; //计算角度偏差//=0.1005533 //0.8650036
        E = R_angle + P[0][0];  //=0.6925952 //Q31+Q31=Q31
        K_0 = P[0][0] / E; //计算卡尔曼增益//=0.003747092  //Q31=Q46/Q31
        K_1 = P[1][0] / E; //=-0.001426836
        Y_0 = P[0][0];  //=0.002595218
        Y_1 = P[0][1];  //=-0.0009882196
  
        P[0][0] -= K_0 * Y_0; //更新协方差矩阵
        P[0][1] -= K_0 * Y_1; 
        P[1][0] -= K_1 * Y_0; 
        P[1][1] -= K_1 * Y_1; //={0.002585493,-0.0009845167,-0.0009845168,0.0007878614}

        angle += K_0 * angle_err; //给出最优估计值  //=0.1029485 //27.78667  //s64Q15 = Q31*Q31
        q_bias += K_1 * angle_err;//更新最优估计值偏差 //=0.1024282 //-1.956188
  		anglek = (s16)angle; //*128.0);
        return anglek; 
}
//s32 offset_correction_integral(s16 data)
s16 offset_correction_integral(s16 data)
{
 static u8 i=0;
 static float datasum=0; 
 //static float dt = 0.0047;
 static float offset = 0;
 float x;
 x = (float)(data)-offset;
 
 datasum += x*0.0047;
 GyroData_Self_offset=(s16)x;
 i++;
 if(i==31)  //Ti=1.2sec  //试验测试选择32个累积较好(Ti=0.0047*32=0.15sec)
 {
  i=0;
  offset = datasum/32.0;
//  if(offset>0)    //四舍五入
//   offset +=0.5;
//  else if(offset<0)
//   offset -=0.5;
  //GyroData_Self_offset=(s16)offset;
 }
 return ((s16)(datasum));  // ((s32)(datasum));
}

void Gyro_Data_ready_check(void)
{
	u8 tmp1;//,tmp2,tmp3;

	tmp1=0;
	tmp1 += Get_Gyro_RDY;
	delay_us(1);
	tmp1 += Get_Gyro_RDY;
	delay_us(1);
	tmp1 += Get_Gyro_RDY;
	delay_us(1);
	tmp1 += Get_Gyro_RDY;
	delay_us(1);
	tmp1 += Get_Gyro_RDY;
	//if((tmp1==1)&&(tmp2==1)&&(tmp3==1))
	if(tmp1>=3)	   //变更检测方式2013-04-03
{
		Gyro_Data_ready=1;
}
Else
		Gyro_Data_ready=0;

}
//void Gyro_Data_ready_check(void)
// {
// static u8 Gyro_Data_ready_PinState1=0,Gyro_Data_ready_PinState2=0;
// static u8 Gyro_Data_ready_PinState=0,Gyro_Data_ready_PinState_Prev=0;
//
// Gyro_Data_ready_PinState1=Gyro_Data_ready_PinState2;
// Gyro_Data_ready_PinState2=Get_Gyro_RDY;
//
// if((Gyro_Data_ready_PinState1+Gyro_Data_ready_PinState2)==0)
// {
//  Gyro_Data_ready_PinState = 0;
//}
// else if((Gyro_Data_ready_PinState1+Gyro_Data_ready_PinState2)==2)
// {
//  Gyro_Data_ready_PinState = 1;
//}
//
//
// if((Gyro_Data_ready_PinState_Prev==0)&(Gyro_Data_ready_PinState == 1))//||((Gyro_Data_ready_PinState_Prev+Gyro_Data_ready_PinState) == 2))  //L3G4200D Data ready?
//  Gyro_Data_ready = 1;
//
// Gyro_Data_ready_PinState_Prev=Gyro_Data_ready_PinState;
//}

//void YawCalc(void)
// {
////	static s16 eRPM_LPF;
//	s32 S32temp;
////	s16 sumLRrpm;
//////	static float eRPM_Integ=0;
////
////	sumLRrpm=Lrpm+Rrpm;
////	if(sumLRrpm<0)
////		sumLRrpm = -sumLRrpm;
////	if(sumLRrpm>10400)
////		sumLRrpm=10400;
////	//sumLRrpm滤波:
////	sumLRrpm /=20;
////	Joystick_Value -=sumLRrpm; //此处算法错误//随速度增加，转向灵敏度降低
//	 
////	S32temp=Rrpm-Lrpm + Joystick_Value;
////	S32temp += eRPM_LPF*63;
////	S32temp >>=6;
////	eRPM_LPF = (s16)S32temp;
////	eRPM_Integ += eRPM_LPF*0.005;
////	if(eRPM_Integ>600)
////		eRPM_Integ=600;
////	if(eRPM_Integ<-600)
////		eRPM_Integ=-600;
////	Joystick_Value = (eRPM_LPF>>1) +(s16)eRPM_Integ;
////	Joystick_Value >>=1;
//
//}

void UpAngleOffsetSpeedcompensation(void)
{
	//static u16 count=0;
	static s16 LPF=0,LPF1=0;  // last_LRrpm=0;
	s16 temp; //,Accetemp;
	s32 S32temp;				  //jysPCB:UpAngleOffset=857 ==15°


	//速度相关计算：
	if(rpm_get_ready==1)
{
		rpm_get_ready=0;

		//速度检测死区补偿：
		if(Lrpm>120)	   
			Lrpm -=120;
		else if(Lrpm<-120)
			Lrpm +=120;
		else
			Lrpm =0;

		if(Rrpm>120)
			Rrpm -=120;
		else if(Rrpm<-120)
			Rrpm +=120;
		else
			Rrpm =0;

		//速度相关计算：
		LRrpm = Lrpm + Rrpm;	//两轮速度和
		R_Lrmp = Rrpm - Lrpm;	//两轮速度差
		AcceLRrpm = LRrpm - Last_LRrpm;	//加速度
		Last_LRrpm = LRrpm;	 //计算加速度，保存前次速度
		//滤波:
		LPF_LRrpm += LRrpm*3; //两轮速度和滤波值
		LPF_LRrpm >>= 2;
The same time as the above-
		LPF_R_Lrmp += R_Lrmp*3;	//两轮速度差滤波值
		LPF_R_Lrmp >>=2;
The same time as the above-
		LPF_AcceLRrpm += AcceLRrpm;	//加速度滤波值
		LPF_AcceLRrpm >>=1;
		//以上为速度修正算法，代替下面的速度算法！
//		if(((Lrpm>120)||(Lrpm<-120))&&((Rrpm>120)||(Rrpm<-120)))   //速度大于100时，速度能够稳定检测时开始计算
//		{
//			LRrpm = Lrpm + Rrpm;	//两轮速度和
//			R_Lrmp = Rrpm - Lrpm;	//两轮速度差
//			AcceLRrpm = LRrpm - Last_LRrpm;	//加速度
//			Last_LRrpm = LRrpm;	 //计算加速度，保存前次速度
//			//滤波:
//			LPF_LRrpm += LRrpm*3; //两轮速度和滤波值
//			LPF_LRrpm >>= 2;
//
//			LPF_R_Lrmp += R_Lrmp*3;	//两轮速度差滤波值
//			LPF_R_Lrmp >>=2;
//
//			LPF_AcceLRrpm += AcceLRrpm;	//加速度滤波值
//			LPF_AcceLRrpm >>=1;
//}
//		else if(((Lrpm<100)||(Lrpm>-100))&&((Rrpm<100)||(Rrpm>-100)))
//		{//100-120之间保持原有数据，作为缓冲
//			LRrpm  = 0;
//			R_Lrmp  = 0;
//			AcceLRrpm  = 0;
//			Last_LRrpm  = 0;
//			LPF_LRrpm =0;
//			LPF_R_Lrmp=0;
//			LPF_AcceLRrpm=0;
//}
	//}

//	Accetemp = temp-last_LRrpm;	//起步前倾补偿方案二
//	last_LRrpm = temp;
//	Upcomp += Accetemp;
//	Upcomp >>=1;
	temp = LRrpm;		  //左右轮速度和
	//按低通延迟计算:
	if(temp>10400)	//最值限制
		temp=10400;
	if(temp<-10400)
		temp=-10400;
	//temp>>=2;  //除4,时速18km/h对应转速=转速对应上翘角度
	//temp /=5;  //除5,时速18km/h对应上翘角度10.85°

	S32temp = (s32)(LPF*3) + (s32)temp;   //速度滤波
	S32temp >>=2;
	LPF = (s16)S32temp;

	//高级模式：12.5km/h起翘,20km/h最高
	//10km/h起翘,18km/h最高
	S32temp *=1000;
	S32temp /=2850;	//2567;
	temp = (s16)S32temp;
The same time as the above-
	if(temp>1825)  //2571)
		UpAngleOffset =	temp-1825;  //-2571;
	else if(temp<-1825)  //-2571)
		UpAngleOffset =	temp+1825;  //+2571;
Else
{
		UpAngleOffset =	0;
}
	//初级模式:	 8km/h起翘,12.5km最高
//	S32temp *=1000;
//	S32temp /=1689;
//	temp = (s16)S32temp;
//
//	if(temp>2368)					//
//		UpAngleOffset =	temp-2368;
//	else if(temp<-2368)
//		UpAngleOffset =	temp+2368;
// else else
// {
//		UpAngleOffset =	0;
//}

	if(UpAngleOffset>1250)	 //限制上翘角度=12.25°//10°=1160 //11°=1250
		UpAngleOffset=1250;
	if(UpAngleOffset<-1250)
		UpAngleOffset=-1250;

	//void YawCalc(void):
	//转向灵敏度限制:
	temp=LPF;
	if(LPF<0)				//转向灵敏度限制:转向灵敏度随速度增加而降低
		temp=-LPF;			//Joystick_Value=(1-|(Lrpm+Rrpm)/4|/3250)
	temp/=4;   //原程序中遗漏此条
	if(temp>=300)  //增加转速和大于1200，开始转向灵敏度限制
{
		S32temp = 21000/temp;
		S32temp += 30;
		S32temp *= Joystick_Value;
		S32temp /= 100;
		Joystick_Value -=(s16)S32temp;
}
//	S32temp=Joystick_Value*temp;
//	S32temp/=3250;
//	Joystick_Value -=(s16)S32temp;


	//R_Lrmp滤波:
	S32temp=R_Lrmp;	//Rrpm-Lrpm; // + Joystick_Value;
	S32temp += LPF1*3;	 //error: += eRPM_LPF*63;
	S32temp >>=2;
	temp = (s16)S32temp;
	LPF1 = temp;	   //原程序中漏掉这条2013-05-29


  	//左转：右轮快于左轮LPF1>0; Joystick_Value<0右轮快于左轮;
  	if((LRrpm>300)||(LRrpm<-300))
  {
	  	temp = LPF1*2	+ Joystick_Value; 
	  	if(temp>5)	//转速差大于输入值(Joystick_Value<0左转),转向太快
					//或者输入值大于转速差(Joystick_Value>0右转)--转向不足
			eRPM_LPF++;	  //修正转速差，使左轮转速增加，右轮转速降低；
		else if(temp<-5)	//
			eRPM_LPF--;
		else
{
			if(eRPM_LPF>0)
				eRPM_LPF--;
			else if(eRPM_LPF<0)
				eRPM_LPF++;
}
}
Else
{
		if(eRPM_LPF>0)
			eRPM_LPF--;
		else if(eRPM_LPF<0)
			eRPM_LPF++;
}
  }
}


//void UpAngleOffsetSpeedcompensation(void)
// {
//	//static u16 count=0;
//	static s16 LPF=0,LPF1=0;  // last_LRrpm=0;
//	s16 temp; //,Accetemp;
//	s32 S32temp;				  //jysPCB:UpAngleOffset=857 ==15°
//
//
//	//速度相关计算：
//	if(rpm_get_ready==1)
// {
//		rpm_get_ready=0;
//
//		if(((Lrpm>120)||(Lrpm<-120))&&((Rrpm>120)||(Rrpm<-120)))   //速度大于100时，速度能够稳定检测时开始计算
//		{
//			LRrpm = Lrpm + Rrpm;	//两轮速度和
//			R_Lrmp = Rrpm - Lrpm;	//两轮速度差
//			AcceLRrpm = LRrpm - Last_LRrpm;	//加速度
//			Last_LRrpm = LRrpm;	 //计算加速度，保存前次速度
//			//滤波:
//			LPF_LRrpm += LRrpm*3; //两轮速度和滤波值
//			LPF_LRrpm >>= 2;
//
//			LPF_R_Lrmp += R_Lrmp*3;	//两轮速度差滤波值
//			LPF_R_Lrmp >>=2;
//
//			LPF_AcceLRrpm += AcceLRrpm;	//加速度滤波值
//			LPF_AcceLRrpm >>=1;
//}
//		else if(((Lrpm<100)||(Lrpm>-100))&&((Rrpm<100)||(Rrpm>-100)))
//		{//100-120之间保持原有数据，作为缓冲
//			LRrpm  = 0;
//			R_Lrmp  = 0;
//			AcceLRrpm  = 0;
//			Last_LRrpm  = 0;
//			LPF_LRrpm =0;
//			LPF_R_Lrmp=0;
//			LPF_AcceLRrpm=0;
//}
//	//}
//
////	Accetemp = temp-last_LRrpm;	//起步前倾补偿方案二
////	last_LRrpm = temp;
////	Upcomp += Accetemp;
////	Upcomp >>=1;
//	temp = LRrpm;		  //左右轮速度和
//	//按低通延迟计算:
//	if(temp>10400)	//最值限制
//		temp=10400;
//	if(temp<-10400)
//		temp=-10400;
//	//temp>>=2;  //除4,时速18km/h对应转速=转速对应上翘角度
//	//temp /=5;  //除5,时速18km/h对应上翘角度10.85°
//
//	S32temp = (s32)(LPF*3) + (s32)temp;   //速度滤波
//	S32temp >>=2;
//	LPF = (s16)S32temp;
//
//	//高级模式：12.5km/h起翘,20km/h最高
//	S32temp *=1000;
//	S32temp /=2567;
//	temp = (s16)S32temp;
//
//	if(temp>2571)
//		UpAngleOffset =	temp-2571;
//	else if(temp<-2571)
//		UpAngleOffset =	temp+2571;
// else else
// {
//		UpAngleOffset =	0;
//}
//	//初级模式:	 8km/h起翘,12.5km最高
////	S32temp *=1000;
////	S32temp /=1689;
////	temp = (s16)S32temp;
////
////	if(temp>2368)					//
////		UpAngleOffset =	temp-2368;
////	else if(temp<-2368)
////		UpAngleOffset =	temp+2368;
////	else
////	{
////		UpAngleOffset =	0;
////	}
//
//	if(UpAngleOffset>1200)	 //限制上翘角度=12.25°//10°=1160 //11°=1250
//		UpAngleOffset=1200;
//	if(UpAngleOffset<-1200)
//		UpAngleOffset=-1200;
//
//	//void YawCalc(void):
//
//	temp=LPF;
//	if(LPF<0)				//转向灵敏度限制:转向灵敏度随速度增加而降低
//		temp=-LPF;			//Joystick_Value=(1-|(Lrpm+Rrpm)/4|/3250)
//	temp/=4;   //原程序中遗漏此条
//	if(temp>=300)  //增加转速和大于1200，开始转向灵敏度限制
// {
//		S32temp = 21000/temp;
//		S32temp += 30;
//		S32temp *= Joystick_Value;
//		S32temp /= 100;
//		Joystick_Value -=(s16)S32temp;
//}
////	S32temp=Joystick_Value*temp;
////	S32temp/=3250;
////	Joystick_Value -=(s16)S32temp;
//
//
//	//R_Lrmp滤波:
//	S32temp=R_Lrmp;	//Rrpm-Lrpm; // + Joystick_Value;
//	S32temp += LPF1*3;	 //error: += eRPM_LPF*63;
//	S32temp >>=2;
//	temp = (s16)S32temp;
//	LPF1 = temp;	   //原程序中漏掉这条2013-05-29
//
//
//  	//左转：右轮快于左轮LPF1>0; Joystick_Value<0右轮快于左轮;
//  	if((LRrpm>300)||(LRrpm<-300))
//  	{
//	  	temp = LPF1*2	+ Joystick_Value; 
//	  	if(temp>5)	//转速差大于输入值(Joystick_Value<0左转),转向太快
//					//或者输入值大于转速差(Joystick_Value>0右转)--转向不足
//			eRPM_LPF++;	  //修正转速差，使左轮转速增加，右轮转速降低；
//		else if(temp<-5)	//
//			eRPM_LPF--;
//		else
//		{
//			if(eRPM_LPF>0)
//				eRPM_LPF--;
//			else if(eRPM_LPF<0)
//				eRPM_LPF++;
//}
//}
// else else
// {
//		if(eRPM_LPF>0)
//			eRPM_LPF--;
//		else if(eRPM_LPF<0)
//			eRPM_LPF++;
//}
//}
//}

////	S32temp=Rrpm+Lrpm;
////  if((S32temp>2000)||(S32temp<-2000))
//  if((LRrpm>200)||(LRrpm<-200))
// {
//  	//左转：右轮快于左轮LPF1>0; Joystick_Value<0右轮快于左轮;
//  	temp = LPF1*2	+ Joystick_Value; 
//  	if(temp>5)	//转速差大于输入值(Joystick_Value<0左转),转向太快
//				//或者输入值大于转速差(Joystick_Value>0右转)--转向不足
//		eRPM_LPF++;	  //修正转速差，使左轮转速增加，右轮转速降低；
//	else if(temp<-5)	//
//		eRPM_LPF++;
// else else
// {
//		if(eRPM_LPF>0)
//			eRPM_LPF--;
//		else if(eRPM_LPF<0)
//			eRPM_LPF++;
//}

//	if((Joystick_Value==0)||(Joystick_Value==1)||(Joystick_Value==-1))
// {
//		eRPM_Integ +=temp*0.01;  //*0.001;
////		if(temp>0)
////			eRPM_LPF++;
////		if(temp<0)
////			eRPM_LPF--;
//		eRPM_LPF=(s16)eRPM_Integ;
//		if(eRPM_LPF>50)		//+/50
//		eRPM_LPF=50;
//		if(eRPM_LPF<-50)
//		eRPM_LPF=-50;	
//}
// else else
// {
//		eRPM_Integ=0;
//		//eRPM_LPF=0;
//		if(eRPM_LPF>0)
//			eRPM_LPF--;
//		if(eRPM_LPF<0)
//			eRPM_LPF++;
//}
//}
// else else
// {
//  		eRPM_Integ=0;
//		//eRPM_LPF=0;
//		if(eRPM_LPF>0)
//			eRPM_LPF--;
//		if(eRPM_LPF<0)
//			eRPM_LPF++;
//}

//	if((temp>-10)&&(temp<10))
// {
//		eRPM_Integ=0;
//		eRPM_LPF=0;
//}


//	eRPM_Integ += eRPM_LPF*0.005;
//	if(eRPM_Integ>200)
//		eRPM_Integ=200;
//	if(eRPM_Integ<-200)
//		eRPM_Integ=-200;
//	//Joystick_Value = (eRPM_LPF>>1) +(s16)eRPM_Integ;
//	Joystick_Value = eRPM_LPF +(s16)eRPM_Integ;
//	Joystick_Value >>=1;

The same time as the above-





//	if(temp>=2400)		 //速度>4.7km/h开始上翘	UpAngleOffset=1700 ==15°
// {
//		if(UpAngleOffset < ((temp>>3) - 300)) //((temp>>2) - 600))
//			//UpAngleOffset +=10;	   //增加速度不能大于传感器反应速度！
//			UpAngleOffset ++;	   //+10好像还是太快了
//		else
//			//UpAngleOffset -=15;	   //减小速度略大于传感器反应速度，以便尽快减速复位！
//			UpAngleOffset -=10; //对应于++，-10
//
//		if(UpAngleOffset>850) //1700)	  //限定最高上翘角度15度
//			UpAngleOffset=850; //1700;
//}
//	else if(UpAngleOffset>10)	 //低于初始上翘值时
// {
//		UpAngleOffset-=10;	 //加快上翘角度递减//-50
//}
//	else 
// {
//		UpAngleOffset=0;	 //
//}
//  count++;
//  if(count>=20)	  ////增加速度不能大于传感器反应速度！增加滤波延迟应该较好
// {
//  	count=0;
//	if(temp>=2400)
// {
//		UpAngleOffset = (temp>>3) - 300;	
//}
// else else
// {
//		if(UpAngleOffset>0)
//			UpAngleOffset--;
//}
//
//	if(temp<=-2400)
// {
//		UpAngleOffset = (temp>>3) + 300;
//}
// else else
// {
//		if(UpAngleOffset<0)
//			UpAngleOffset++;
//}
//
//	if(UpAngleOffset>850) //1700)	  //限定最高上翘角度15度
//		UpAngleOffset=850; //1700;
//	if(UpAngleOffset<-570) //限定最高上翘角度-10度
//		UpAngleOffset=-570; //
//}
//
//}
//}

//void UpAngleOffsetSpeedcompensation(void)
// {
//	s16 temp;
//	temp = Lrpm+Rrpm;
//	if(temp>=2400)		 //速度>4.7km/h开始上翘
// {
//		UpAngleOffset = (temp>>2) - 600;
//		if(UpAngleOffset>1700)	  //限定最高上翘角度15度
//			UpAngleOffset=1700;
//}
//	else if(UpAngleOffset>100)	 //>50
// {
//		UpAngleOffset-=100;	 //加快上翘角度递减//-50
//}
//	else if(UpAngleOffset>10)
// {
//		UpAngleOffset-=10;	 //
//}
//	else if(UpAngleOffset>1)
// {
//		UpAngleOffset--;	 //
//}
//
//}
 
 
 
 

