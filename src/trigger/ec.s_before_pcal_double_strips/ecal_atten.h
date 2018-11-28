/* ecal_atten.h */

/* table containing exp(-path[axis]/atten), where axis can be [0..2] so we have 3 tables; arguments are 'u' and 'v' strip number */

static int filecontent[1296][6] = {
 1, 36, 36, 990, 323, 986,
 2, 35, 36, 990, 333, 955,
 2, 36, 35, 970, 333, 986,
 2, 36, 36, 981, 328, 975,
 3, 34, 36, 991, 344, 925,
 3, 35, 35, 967, 344, 955,
 3, 35, 36, 977, 338, 944,
 3, 36, 34, 939, 344, 986,
 3, 36, 35, 949, 338, 973,
 4, 33, 36, 991, 354, 897,
 4, 34, 35, 967, 354, 925,
 4, 34, 36, 977, 349, 913,
 4, 35, 34, 939, 354, 955,
 4, 35, 35, 949, 349, 944,
 4, 36, 33, 912, 354, 986,
 4, 36, 34, 921, 349, 975,
 5, 32, 36, 991, 367, 869,
 5, 33, 35, 967, 367, 897,
 5, 33, 36, 977, 361, 884,
 5, 34, 34, 939, 367, 925,
 5, 34, 35, 949, 361, 913,
 5, 35, 33, 912, 367, 957,
 5, 35, 34, 921, 361, 944,
 5, 36, 32, 885, 367, 986,
 5, 36, 33, 896, 361, 975,
 6, 31, 36, 991, 379, 841,
 6, 32, 35, 967, 379, 869,
 6, 32, 36, 977, 372, 856,
 6, 33, 34, 939, 379, 897,
 6, 33, 35, 949, 372, 884,
 6, 34, 33, 912, 379, 925,
 6, 34, 34, 921, 372, 913,
 6, 35, 32, 885, 379, 957,
 6, 35, 33, 896, 372, 944,
 6, 36, 31, 860, 379, 986,
 6, 36, 32, 870, 372, 975,
 7, 30, 36, 991, 391, 814,
 7, 31, 35, 967, 391, 841,
 7, 31, 36, 977, 384, 829,
 7, 32, 34, 939, 391, 869,
 7, 32, 35, 949, 384, 856,
 7, 33, 33, 912, 391, 897,
 7, 33, 34, 921, 384, 884,
 7, 34, 32, 885, 391, 926,
 7, 34, 33, 896, 384, 913,
 7, 35, 31, 860, 391, 957,
 7, 35, 32, 870, 384, 944,
 7, 36, 30, 834, 391, 986,
 7, 36, 31, 845, 384, 975,
 8, 29, 36, 991, 404, 788,
 8, 30, 35, 967, 404, 814,
 8, 30, 36, 977, 397, 804,
 8, 31, 34, 939, 404, 841,
 8, 31, 35, 949, 398, 829,
 8, 32, 33, 912, 404, 869,
 8, 32, 34, 921, 398, 856,
 8, 33, 32, 885, 404, 897,
 8, 33, 33, 896, 398, 884,
 8, 34, 31, 860, 404, 926,
 8, 34, 32, 870, 398, 913,
 8, 35, 30, 834, 404, 957,
 8, 35, 31, 845, 398, 944,
 8, 36, 29, 810, 404, 986,
 8, 36, 30, 819, 398, 975,
 9, 28, 36, 991, 416, 763,
 9, 29, 35, 967, 416, 788,
 9, 29, 36, 977, 409, 778,
 9, 30, 34, 939, 416, 814,
 9, 30, 35, 949, 409, 804,
 9, 31, 33, 912, 416, 841,
 9, 31, 34, 921, 411, 829,
 9, 32, 32, 885, 416, 869,
 9, 32, 33, 896, 411, 857,
 9, 33, 31, 860, 416, 897,
 9, 33, 32, 870, 411, 885,
 9, 34, 30, 834, 416, 926,
 9, 34, 31, 845, 411, 915,
 9, 35, 29, 810, 416, 957,
 9, 35, 30, 819, 411, 944,
 9, 36, 28, 787, 416, 986,
 9, 36, 29, 796, 411, 975,
10, 27, 36, 991, 430, 740,
10, 28, 35, 967, 430, 763,
10, 28, 36, 977, 423, 754,
10, 29, 34, 939, 430, 788,
10, 29, 35, 949, 423, 778,
10, 30, 33, 912, 430, 814,
10, 30, 34, 921, 423, 804,
10, 31, 32, 885, 430, 842,
10, 31, 33, 896, 423, 829,
10, 32, 31, 860, 430, 869,
10, 32, 32, 870, 423, 857,
10, 33, 30, 834, 430, 897,
10, 33, 31, 845, 423, 885,
10, 34, 29, 810, 430, 926,
10, 34, 30, 819, 423, 915,
10, 35, 28, 787, 430, 957,
10, 35, 29, 796, 423, 944,
10, 36, 27, 764, 430, 986,
10, 36, 28, 773, 423, 975,
11, 26, 36, 990, 444, 718,
11, 27, 35, 967, 444, 740,
11, 27, 36, 977, 437, 730,
11, 28, 34, 939, 444, 763,
11, 28, 35, 949, 437, 754,
11, 29, 33, 912, 444, 788,
11, 29, 34, 922, 437, 778,
11, 30, 32, 885, 444, 814,
11, 30, 33, 896, 437, 804,
11, 31, 31, 860, 444, 842,
11, 31, 32, 870, 437, 830,
11, 32, 30, 834, 444, 869,
11, 32, 31, 845, 437, 857,
11, 33, 29, 810, 444, 898,
11, 33, 30, 819, 437, 885,
11, 34, 28, 787, 444, 926,
11, 34, 29, 796, 437, 915,
11, 35, 27, 762, 444, 961,
11, 35, 28, 773, 437, 944,
11, 36, 26, 740, 444, 990,
11, 36, 27, 750, 437, 975,
12, 25, 36, 990, 458, 697,
12, 26, 35, 967, 458, 717,
12, 26, 36, 977, 451, 707,
12, 27, 34, 939, 458, 740,
12, 27, 35, 949, 451, 730,
12, 28, 33, 912, 458, 763,
12, 28, 34, 922, 451, 754,
12, 29, 32, 885, 458, 788,
12, 29, 33, 896, 451, 778,
12, 30, 31, 860, 458, 814,
12, 30, 32, 870, 451, 804,
12, 31, 30, 834, 458, 842,
12, 31, 31, 845, 451, 830,
12, 32, 29, 810, 458, 869,
12, 32, 30, 819, 451, 857,
12, 33, 28, 787, 458, 898,
12, 33, 29, 796, 451, 885,
12, 34, 27, 762, 458, 930,
12, 34, 28, 773, 451, 915,
12, 35, 26, 740, 458, 961,
12, 35, 27, 750, 451, 944,
12, 36, 25, 718, 458, 990,
12, 36, 26, 730, 451, 976,
13, 24, 36, 990, 474, 675,
13, 25, 35, 967, 474, 694,
13, 25, 36, 977, 467, 685,
13, 26, 34, 939, 474, 717,
13, 26, 35, 949, 467, 707,
13, 27, 33, 912, 474, 740,
13, 27, 34, 922, 467, 730,
13, 28, 32, 885, 474, 763,
13, 28, 33, 896, 467, 754,
13, 29, 31, 860, 474, 788,
13, 29, 32, 870, 467, 778,
13, 30, 30, 836, 474, 814,
13, 30, 31, 845, 467, 804,
13, 31, 29, 810, 474, 842,
13, 31, 30, 819, 467, 830,
13, 32, 28, 785, 474, 873,
13, 32, 29, 796, 467, 857,
13, 33, 27, 762, 474, 901,
13, 33, 28, 773, 467, 885,
13, 34, 26, 740, 474, 930,
13, 34, 27, 750, 467, 915,
13, 35, 25, 718, 474, 961,
13, 35, 26, 728, 467, 944,
13, 36, 24, 698, 474, 990,
13, 36, 25, 708, 467, 976,
14, 23, 36, 990, 488, 652,
14, 24, 35, 967, 488, 671,
14, 24, 36, 977, 481, 662,
14, 25, 34, 939, 490, 694,
14, 25, 35, 949, 481, 685,
14, 26, 33, 912, 490, 717,
14, 26, 34, 922, 481, 707,
14, 27, 32, 885, 490, 740,
14, 27, 33, 896, 481, 730,
14, 28, 31, 860, 490, 763,
14, 28, 32, 870, 481, 754,
14, 29, 30, 836, 490, 788,
14, 29, 31, 845, 481, 778,
14, 30, 29, 810, 490, 814,
14, 30, 30, 819, 481, 804,
14, 31, 28, 785, 490, 845,
14, 31, 29, 796, 481, 830,
14, 32, 27, 762, 490, 873,
14, 32, 28, 773, 481, 857,
14, 33, 26, 740, 490, 901,
14, 33, 27, 750, 481, 885,
14, 34, 25, 718, 490, 930,
14, 34, 26, 728, 481, 915,
14, 35, 24, 698, 490, 961,
14, 35, 25, 708, 481, 944,
14, 36, 23, 677, 490, 990,
14, 36, 24, 688, 481, 976,
15, 22, 36, 990, 504, 633,
15, 23, 35, 967, 504, 651,
15, 23, 36, 977, 497, 642,
15, 24, 34, 939, 505, 671,
15, 24, 35, 949, 497, 662,
15, 25, 33, 912, 505, 694,
15, 25, 34, 922, 497, 685,
15, 26, 32, 885, 505, 717,
15, 26, 33, 896, 497, 707,
15, 27, 31, 860, 505, 740,
15, 27, 32, 870, 497, 730,
15, 28, 30, 836, 505, 763,
15, 28, 31, 845, 497, 754,
15, 29, 29, 808, 504, 791,
15, 29, 30, 819, 497, 778,
15, 30, 28, 785, 505, 818,
15, 30, 29, 796, 497, 805,
15, 31, 27, 762, 505, 845,
15, 31, 28, 773, 497, 830,
15, 32, 26, 740, 505, 873,
15, 32, 27, 750, 497, 857,
15, 33, 25, 718, 505, 901,
15, 33, 26, 728, 497, 885,
15, 34, 24, 698, 505, 930,
15, 34, 25, 708, 497, 915,
15, 35, 23, 677, 505, 961,
15, 35, 24, 688, 497, 944,
15, 36, 22, 657, 505, 990,
15, 36, 23, 667, 497, 976,
16, 21, 36, 990, 522, 612,
16, 22, 35, 967, 522, 629,
16, 22, 36, 977, 513, 621,
16, 23, 34, 939, 522, 651,
16, 23, 35, 949, 513, 642,
16, 24, 33, 912, 522, 671,
16, 24, 34, 922, 513, 662,
16, 25, 32, 885, 522, 694,
16, 25, 33, 896, 513, 685,
16, 26, 31, 860, 522, 717,
16, 26, 32, 870, 513, 707,
16, 27, 30, 836, 522, 740,
16, 27, 31, 845, 513, 730,
16, 28, 29, 809, 522, 767,
16, 28, 30, 819, 513, 754,
16, 29, 28, 785, 522, 792,
16, 29, 29, 796, 513, 778,
16, 30, 27, 762, 522, 819,
16, 30, 28, 773, 513, 805,
16, 31, 26, 740, 522, 845,
16, 31, 27, 750, 513, 830,
16, 32, 25, 718, 522, 873,
16, 32, 26, 730, 513, 857,
16, 33, 24, 698, 522, 902,
16, 33, 25, 708, 513, 885,
16, 34, 23, 677, 522, 930,
16, 34, 24, 688, 513, 915,
16, 35, 22, 657, 522, 961,
16, 35, 23, 667, 513, 944,
16, 36, 21, 638, 522, 990,
16, 36, 22, 648, 513, 976,
17, 20, 36, 990, 538, 592,
17, 21, 35, 967, 538, 610,
17, 21, 36, 977, 531, 601,
17, 22, 34, 939, 538, 629,
17, 22, 35, 949, 531, 621,
17, 23, 33, 912, 538, 651,
17, 23, 34, 922, 531, 642,
17, 24, 32, 885, 538, 671,
17, 24, 33, 896, 531, 662,
17, 25, 31, 860, 538, 694,
17, 25, 32, 870, 531, 685,
17, 26, 30, 836, 538, 717,
17, 26, 31, 845, 531, 707,
17, 27, 29, 809, 538, 742,
17, 27, 30, 819, 531, 731,
17, 28, 28, 785, 538, 768,
17, 28, 29, 796, 531, 754,
17, 29, 27, 762, 538, 792,
17, 29, 28, 773, 531, 778,
17, 30, 26, 740, 538, 819,
17, 30, 27, 750, 531, 805,
17, 31, 25, 718, 538, 845,
17, 31, 26, 730, 531, 830,
17, 32, 24, 698, 538, 873,
17, 32, 25, 708, 531, 857,
17, 33, 23, 677, 538, 902,
17, 33, 24, 688, 531, 885,
17, 34, 22, 657, 538, 930,
17, 34, 23, 667, 531, 915,
17, 35, 21, 638, 538, 961,
17, 35, 22, 648, 531, 944,
17, 36, 20, 620, 538, 990,
17, 36, 21, 629, 531, 976,
18, 19, 36, 990, 556, 574,
18, 20, 35, 967, 556, 591,
18, 20, 36, 977, 547, 583,
18, 21, 34, 939, 556, 610,
18, 21, 35, 949, 547, 601,
18, 22, 33, 912, 556, 629,
18, 22, 34, 922, 547, 621,
18, 23, 32, 885, 556, 651,
18, 23, 33, 896, 547, 642,
18, 24, 31, 861, 556, 671,
18, 24, 32, 870, 547, 662,
18, 25, 30, 833, 556, 697,
18, 25, 31, 845, 547, 685,
18, 26, 29, 809, 556, 720,
18, 26, 30, 819, 547, 708,
18, 27, 28, 785, 556, 742,
18, 27, 29, 796, 547, 731,
18, 28, 27, 762, 556, 768,
18, 28, 28, 773, 547, 754,
18, 29, 26, 740, 556, 792,
18, 29, 27, 750, 547, 778,
18, 30, 25, 718, 556, 819,
18, 30, 26, 730, 547, 805,
18, 31, 24, 698, 556, 845,
18, 31, 25, 708, 547, 830,
18, 32, 23, 677, 556, 873,
18, 32, 24, 688, 547, 857,
18, 33, 22, 657, 556, 902,
18, 33, 23, 667, 547, 887,
18, 34, 21, 638, 556, 930,
18, 34, 22, 648, 549, 916,
18, 35, 20, 620, 556, 962,
18, 35, 21, 629, 549, 944,
18, 36, 19, 602, 556, 990,
18, 36, 20, 611, 549, 976,
19, 18, 36, 990, 574, 555,
19, 19, 35, 967, 574, 572,
19, 19, 36, 977, 565, 564,
19, 20, 34, 939, 574, 591,
19, 20, 35, 949, 565, 583,
19, 21, 33, 912, 574, 610,
19, 21, 34, 922, 565, 601,
19, 22, 32, 885, 574, 629,
19, 22, 33, 896, 565, 621,
19, 23, 31, 861, 574, 651,
19, 23, 32, 870, 565, 642,
19, 24, 30, 833, 574, 675,
19, 24, 31, 845, 565, 662,
19, 25, 29, 809, 574, 697,
19, 25, 30, 820, 565, 685,
19, 26, 28, 785, 574, 720,
19, 26, 29, 796, 565, 708,
19, 27, 27, 762, 574, 742,
19, 27, 28, 773, 565, 731,
19, 28, 26, 740, 574, 768,
19, 28, 27, 750, 565, 754,
19, 29, 25, 718, 574, 792,
19, 29, 26, 730, 565, 778,
19, 30, 24, 698, 574, 819,
19, 30, 25, 708, 565, 805,
19, 31, 23, 677, 574, 846,
19, 31, 24, 688, 565, 830,
19, 32, 22, 657, 574, 873,
19, 32, 23, 667, 565, 859,
19, 33, 21, 638, 574, 902,
19, 33, 22, 648, 565, 887,
19, 34, 20, 620, 574, 930,
19, 34, 21, 629, 565, 916,
19, 35, 19, 602, 574, 962,
19, 35, 20, 611, 565, 945,
19, 36, 18, 584, 574, 990,
19, 36, 19, 593, 566, 976,
20, 17, 36, 990, 592, 538,
20, 18, 35, 967, 593, 554,
20, 18, 36, 977, 583, 546,
20, 19, 34, 939, 593, 572,
20, 19, 35, 949, 583, 564,
20, 20, 33, 912, 593, 591,
20, 20, 34, 922, 583, 583,
20, 21, 32, 885, 593, 610,
20, 21, 33, 896, 583, 601,
20, 22, 31, 857, 592, 633,
20, 22, 32, 870, 584, 621,
20, 23, 30, 833, 592, 653,
20, 23, 31, 845, 584, 642,
20, 24, 29, 809, 593, 675,
20, 24, 30, 820, 584, 663,
20, 25, 28, 785, 593, 697,
20, 25, 29, 796, 584, 685,
20, 26, 27, 762, 593, 720,
20, 26, 28, 773, 584, 708,
20, 27, 26, 740, 593, 742,
20, 27, 27, 750, 584, 731,
20, 28, 25, 718, 593, 768,
20, 28, 26, 730, 584, 754,
20, 29, 24, 698, 593, 792,
20, 29, 25, 708, 584, 779,
20, 30, 23, 677, 593, 819,
20, 30, 24, 688, 584, 805,
20, 31, 22, 657, 593, 846,
20, 31, 23, 667, 584, 830,
20, 32, 21, 638, 593, 873,
20, 32, 22, 648, 584, 859,
20, 33, 20, 620, 593, 902,
20, 33, 21, 629, 584, 887,
20, 34, 19, 602, 593, 930,
20, 34, 20, 611, 584, 916,
20, 35, 18, 584, 593, 962,
20, 35, 19, 593, 584, 945,
20, 36, 17, 569, 593, 991,
20, 36, 18, 577, 584, 976,
21, 16, 36, 990, 612, 522,
21, 17, 35, 967, 612, 537,
21, 17, 36, 977, 603, 529,
21, 18, 34, 939, 612, 554,
21, 18, 35, 949, 602, 546,
21, 19, 33, 912, 612, 572,
21, 19, 34, 922, 603, 564,
21, 20, 32, 885, 612, 591,
21, 20, 33, 896, 603, 583,
21, 21, 31, 857, 612, 612,
21, 21, 32, 870, 603, 601,
21, 22, 30, 833, 612, 633,
21, 22, 31, 845, 603, 621,
21, 23, 29, 809, 612, 653,
21, 23, 30, 820, 603, 643,
21, 24, 28, 785, 612, 675,
21, 24, 29, 796, 603, 663,
21, 25, 27, 762, 612, 697,
21, 25, 28, 773, 603, 685,
21, 26, 26, 740, 612, 720,
21, 26, 27, 750, 603, 708,
21, 27, 25, 718, 612, 744,
21, 27, 26, 730, 603, 731,
21, 28, 24, 698, 612, 768,
21, 28, 25, 708, 603, 754,
21, 29, 23, 677, 612, 792,
21, 29, 24, 688, 603, 779,
21, 30, 22, 657, 612, 819,
21, 30, 23, 667, 603, 805,
21, 31, 21, 638, 612, 846,
21, 31, 22, 648, 603, 830,
21, 32, 20, 620, 612, 873,
21, 32, 21, 629, 603, 859,
21, 33, 19, 602, 612, 902,
21, 33, 20, 611, 603, 887,
21, 34, 18, 584, 612, 931,
21, 34, 19, 593, 603, 916,
21, 35, 17, 568, 612, 962,
21, 35, 18, 577, 603, 945,
21, 36, 16, 551, 614, 991,
21, 36, 17, 560, 603, 976,
22, 15, 36, 990, 633, 504,
22, 16, 35, 967, 633, 519,
22, 16, 36, 977, 623, 513,
22, 17, 34, 939, 633, 537,
22, 17, 35, 949, 623, 529,
22, 18, 33, 912, 633, 554,
22, 18, 34, 922, 623, 546,
22, 19, 32, 883, 633, 574,
22, 19, 33, 896, 623, 564,
22, 20, 31, 857, 633, 592,
22, 20, 32, 870, 623, 583,
22, 21, 30, 833, 633, 612,
22, 21, 31, 845, 623, 602,
22, 22, 29, 809, 633, 633,
22, 22, 30, 820, 623, 621,
22, 23, 28, 785, 633, 653,
22, 23, 29, 796, 623, 643,
22, 24, 27, 763, 633, 675,
22, 24, 28, 773, 623, 663,
22, 25, 26, 740, 633, 697,
22, 25, 27, 750, 623, 685,
22, 26, 25, 718, 633, 720,
22, 26, 26, 730, 624, 708,
22, 27, 24, 698, 633, 744,
22, 27, 25, 708, 624, 731,
22, 28, 23, 677, 633, 768,
22, 28, 24, 688, 624, 754,
22, 29, 22, 657, 633, 792,
22, 29, 23, 667, 624, 779,
22, 30, 21, 638, 633, 819,
22, 30, 22, 648, 624, 805,
22, 31, 20, 620, 633, 846,
22, 31, 21, 629, 624, 832,
22, 32, 19, 602, 633, 874,
22, 32, 20, 611, 624, 859,
22, 33, 18, 584, 633, 902,
22, 33, 19, 593, 624, 887,
22, 34, 17, 568, 633, 931,
22, 34, 18, 577, 624, 916,
22, 35, 16, 551, 633, 962,
22, 35, 17, 560, 624, 945,
22, 36, 15, 536, 634, 991,
22, 36, 16, 544, 624, 976,
23, 14, 36, 990, 653, 488,
23, 15, 35, 967, 653, 503,
23, 15, 36, 977, 643, 497,
23, 16, 34, 939, 653, 519,
23, 16, 35, 949, 643, 513,
23, 17, 33, 912, 653, 537,
23, 17, 34, 922, 643, 529,
23, 18, 32, 883, 652, 555,
23, 18, 33, 896, 643, 546,
23, 19, 31, 857, 652, 574,
23, 19, 32, 870, 643, 564,
23, 20, 30, 833, 652, 592,
23, 20, 31, 845, 643, 583,
23, 21, 29, 809, 653, 612,
23, 21, 30, 820, 643, 602,
23, 22, 28, 785, 653, 633,
23, 22, 29, 796, 643, 621,
23, 23, 27, 763, 653, 653,
23, 23, 28, 773, 643, 643,
23, 24, 26, 740, 653, 675,
23, 24, 27, 750, 643, 663,
23, 25, 25, 718, 653, 697,
23, 25, 26, 730, 643, 685,
23, 26, 24, 698, 653, 720,
23, 26, 25, 708, 643, 708,
23, 27, 23, 677, 653, 744,
23, 27, 24, 688, 643, 731,
23, 28, 22, 657, 653, 768,
23, 28, 23, 667, 643, 754,
23, 29, 21, 638, 653, 792,
23, 29, 22, 648, 643, 779,
23, 30, 20, 620, 653, 819,
23, 30, 21, 629, 643, 805,
23, 31, 19, 602, 653, 846,
23, 31, 20, 611, 643, 832,
23, 32, 18, 584, 653, 874,
23, 32, 19, 593, 643, 859,
23, 33, 17, 568, 653, 902,
23, 33, 18, 577, 643, 887,
23, 34, 16, 551, 653, 931,
23, 34, 17, 560, 643, 916,
23, 35, 15, 536, 653, 962,
23, 35, 16, 544, 643, 945,
23, 36, 14, 521, 653, 991,
23, 36, 15, 527, 643, 976,
24, 13, 36, 990, 675, 474,
24, 14, 35, 967, 675, 486,
24, 14, 36, 977, 665, 481,
24, 15, 34, 939, 675, 503,
24, 15, 35, 949, 663, 497,
24, 16, 33, 912, 675, 519,
24, 16, 34, 922, 665, 513,
24, 17, 32, 883, 675, 538,
24, 17, 33, 896, 665, 529,
24, 18, 31, 857, 675, 556,
24, 18, 32, 870, 665, 546,
24, 19, 30, 833, 675, 574,
24, 19, 31, 845, 665, 564,
24, 20, 29, 809, 675, 593,
24, 20, 30, 820, 665, 583,
24, 21, 28, 785, 675, 612,
24, 21, 29, 796, 665, 602,
24, 22, 27, 763, 675, 633,
24, 22, 28, 773, 665, 621,
24, 23, 26, 740, 675, 653,
24, 23, 27, 750, 665, 643,
24, 24, 25, 718, 675, 675,
24, 24, 26, 730, 665, 663,
24, 25, 24, 698, 675, 697,
24, 25, 25, 708, 665, 685,
24, 26, 23, 677, 675, 720,
24, 26, 24, 688, 665, 708,
24, 27, 22, 657, 675, 744,
24, 27, 23, 667, 665, 731,
24, 28, 21, 638, 675, 768,
24, 28, 22, 648, 665, 754,
24, 29, 20, 620, 675, 792,
24, 29, 21, 629, 665, 779,
24, 30, 19, 602, 675, 819,
24, 30, 20, 611, 665, 805,
24, 31, 18, 584, 675, 846,
24, 31, 19, 593, 665, 832,
24, 32, 17, 568, 675, 874,
24, 32, 18, 577, 665, 859,
24, 33, 16, 551, 675, 902,
24, 33, 17, 560, 665, 887,
24, 34, 15, 536, 675, 931,
24, 34, 16, 544, 665, 916,
24, 35, 14, 521, 675, 962,
24, 35, 15, 527, 665, 945,
24, 36, 13, 505, 675, 991,
24, 36, 14, 513, 665, 976,
25, 12, 36, 990, 697, 458,
25, 13, 35, 967, 697, 472,
25, 13, 36, 977, 685, 465,
25, 14, 34, 939, 697, 486,
25, 14, 35, 949, 685, 481,
25, 15, 33, 908, 697, 504,
25, 15, 34, 922, 685, 497,
25, 16, 32, 883, 697, 522,
25, 16, 33, 896, 685, 513,
25, 17, 31, 857, 697, 538,
25, 17, 32, 870, 685, 529,
25, 18, 30, 833, 697, 556,
25, 18, 31, 845, 685, 546,
25, 19, 29, 809, 697, 574,
25, 19, 30, 820, 685, 564,
25, 20, 28, 785, 697, 593,
25, 20, 29, 796, 685, 583,
25, 21, 27, 763, 697, 612,
25, 21, 28, 773, 685, 602,
25, 22, 26, 740, 697, 633,
25, 22, 27, 750, 686, 621,
25, 23, 25, 718, 697, 653,
25, 23, 26, 730, 686, 643,
25, 24, 24, 698, 697, 675,
25, 24, 25, 708, 686, 663,
25, 25, 23, 677, 697, 697,
25, 25, 24, 688, 686, 685,
25, 26, 22, 657, 697, 720,
25, 26, 23, 667, 686, 708,
25, 27, 21, 638, 697, 744,
25, 27, 22, 648, 686, 731,
25, 28, 20, 620, 697, 768,
25, 28, 21, 629, 686, 754,
25, 29, 19, 602, 697, 792,
25, 29, 20, 611, 686, 779,
25, 30, 18, 584, 697, 819,
25, 30, 19, 593, 686, 805,
25, 31, 17, 568, 697, 846,
25, 31, 18, 577, 686, 832,
25, 32, 16, 551, 698, 874,
25, 32, 17, 560, 686, 859,
25, 33, 15, 536, 698, 902,
25, 33, 16, 544, 686, 887,
25, 34, 14, 521, 698, 931,
25, 34, 15, 527, 686, 916,
25, 35, 13, 504, 698, 962,
25, 35, 14, 513, 686, 945,
25, 36, 12, 490, 698, 991,
25, 36, 13, 497, 686, 976,
26, 11, 36, 990, 720, 444,
26, 12, 35, 967, 720, 456,
26, 12, 36, 977, 708, 451,
26, 13, 34, 939, 720, 472,
26, 13, 35, 949, 708, 465,
26, 14, 33, 910, 718, 488,
26, 14, 34, 922, 708, 481,
26, 15, 32, 883, 718, 504,
26, 15, 33, 896, 708, 497,
26, 16, 31, 857, 720, 522,
26, 16, 32, 870, 708, 513,
26, 17, 30, 833, 720, 538,
26, 17, 31, 845, 708, 529,
26, 18, 29, 809, 720, 556,
26, 18, 30, 820, 708, 546,
26, 19, 28, 785, 720, 574,
26, 19, 29, 796, 708, 564,
26, 20, 27, 763, 720, 593,
26, 20, 28, 773, 708, 583,
26, 21, 26, 740, 720, 612,
26, 21, 27, 750, 708, 602,
26, 22, 25, 718, 720, 633,
26, 22, 26, 730, 708, 623,
26, 23, 24, 698, 720, 653,
26, 23, 25, 708, 708, 643,
26, 24, 23, 677, 720, 675,
26, 24, 24, 688, 708, 663,
26, 25, 22, 657, 720, 697,
26, 25, 23, 667, 708, 685,
26, 26, 21, 638, 720, 720,
26, 26, 22, 648, 708, 708,
26, 27, 20, 620, 720, 744,
26, 27, 21, 629, 708, 731,
26, 28, 19, 602, 720, 768,
26, 28, 20, 611, 708, 754,
26, 29, 18, 584, 720, 792,
26, 29, 19, 593, 708, 779,
26, 30, 17, 568, 720, 819,
26, 30, 18, 577, 708, 805,
26, 31, 16, 551, 720, 846,
26, 31, 17, 560, 708, 832,
26, 32, 15, 536, 720, 874,
26, 32, 16, 544, 708, 859,
26, 33, 14, 521, 720, 902,
26, 33, 15, 527, 709, 887,
26, 34, 13, 504, 720, 931,
26, 34, 14, 513, 709, 916,
26, 35, 12, 490, 720, 962,
26, 35, 13, 497, 709, 945,
26, 36, 11, 476, 721, 991,
26, 36, 12, 483, 709, 976,
27, 10, 36, 990, 742, 430,
27, 11, 35, 967, 742, 442,
27, 11, 36, 977, 731, 437,
27, 12, 34, 936, 742, 458,
27, 12, 35, 949, 731, 451,
27, 13, 33, 910, 742, 474,
27, 13, 34, 922, 731, 465,
27, 14, 32, 883, 742, 488,
27, 14, 33, 896, 731, 481,
27, 15, 31, 857, 742, 504,
27, 15, 32, 870, 731, 497,
27, 16, 30, 833, 742, 522,
27, 16, 31, 845, 731, 513,
27, 17, 29, 809, 742, 538,
27, 17, 30, 820, 731, 529,
27, 18, 28, 785, 742, 556,
27, 18, 29, 796, 731, 546,
27, 19, 27, 763, 742, 574,
27, 19, 28, 773, 731, 564,
27, 20, 26, 740, 742, 593,
27, 20, 27, 750, 731, 583,
27, 21, 25, 718, 742, 612,
27, 21, 26, 730, 731, 602,
27, 22, 24, 698, 742, 633,
27, 22, 25, 708, 731, 623,
27, 23, 23, 677, 742, 653,
27, 23, 24, 688, 731, 643,
27, 24, 22, 657, 742, 675,
27, 24, 23, 667, 731, 663,
27, 25, 21, 638, 744, 698,
27, 25, 22, 648, 731, 685,
27, 26, 20, 620, 744, 720,
27, 26, 21, 629, 731, 708,
27, 27, 19, 602, 744, 744,
27, 27, 20, 611, 731, 731,
27, 28, 18, 584, 744, 768,
27, 28, 19, 593, 731, 754,
27, 29, 17, 569, 744, 794,
27, 29, 18, 577, 732, 779,
27, 30, 16, 551, 744, 819,
27, 30, 17, 560, 732, 805,
27, 31, 15, 536, 744, 846,
27, 31, 16, 544, 732, 832,
27, 32, 14, 521, 744, 874,
27, 32, 15, 527, 732, 859,
27, 33, 13, 504, 744, 902,
27, 33, 14, 513, 732, 887,
27, 34, 12, 490, 744, 931,
27, 34, 13, 497, 732, 916,
27, 35, 11, 476, 744, 962,
27, 35, 12, 483, 732, 945,
27, 36, 10, 462, 744, 991,
27, 36, 11, 469, 732, 976,
28,  9, 36, 990, 767, 416,
28, 10, 35, 967, 767, 428,
28, 10, 36, 977, 755, 423,
28, 11, 34, 936, 767, 444,
28, 11, 35, 949, 755, 437,
28, 12, 33, 910, 767, 458,
28, 12, 34, 922, 755, 451,
28, 13, 32, 883, 767, 474,
28, 13, 33, 896, 755, 465,
28, 14, 31, 857, 767, 488,
28, 14, 32, 870, 755, 481,
28, 15, 30, 833, 767, 504,
28, 15, 31, 845, 755, 497,
28, 16, 29, 809, 767, 522,
28, 16, 30, 820, 755, 513,
28, 17, 28, 785, 767, 538,
28, 17, 29, 796, 755, 529,
28, 18, 27, 763, 767, 556,
28, 18, 28, 773, 755, 546,
28, 19, 26, 740, 768, 574,
28, 19, 27, 751, 755, 564,
28, 20, 25, 718, 768, 593,
28, 20, 26, 730, 755, 583,
28, 21, 24, 698, 768, 612,
28, 21, 25, 708, 755, 602,
28, 22, 23, 677, 768, 633,
28, 22, 24, 688, 755, 623,
28, 23, 22, 657, 768, 653,
28, 23, 23, 667, 755, 643,
28, 24, 21, 638, 768, 675,
28, 24, 22, 648, 755, 663,
28, 25, 20, 620, 768, 698,
28, 25, 21, 629, 755, 685,
28, 26, 19, 602, 768, 720,
28, 26, 20, 611, 755, 708,
28, 27, 18, 584, 768, 744,
28, 27, 19, 593, 755, 731,
28, 28, 17, 569, 768, 768,
28, 28, 18, 577, 755, 754,
28, 29, 16, 551, 768, 794,
28, 29, 17, 560, 755, 779,
28, 30, 15, 536, 768, 819,
28, 30, 16, 544, 757, 805,
28, 31, 14, 521, 768, 846,
28, 31, 15, 527, 757, 832,
28, 32, 13, 504, 768, 874,
28, 32, 14, 513, 757, 859,
28, 33, 12, 490, 768, 902,
28, 33, 13, 497, 757, 888,
28, 34, 11, 476, 768, 931,
28, 34, 12, 483, 757, 916,
28, 35, 10, 462, 768, 962,
28, 35, 11, 469, 757, 947,
28, 36,  9, 449, 768, 991,
28, 36, 10, 456, 757, 976,
29,  8, 36, 990, 791, 402,
29,  9, 35, 963, 791, 416,
29,  9, 36, 977, 779, 409,
29, 10, 34, 936, 791, 430,
29, 10, 35, 949, 779, 423,
29, 11, 33, 910, 791, 444,
29, 11, 34, 922, 779, 437,
29, 12, 32, 883, 791, 458,
29, 12, 33, 896, 779, 451,
29, 13, 31, 857, 791, 474,
29, 13, 32, 870, 779, 465,
29, 14, 30, 833, 791, 488,
29, 14, 31, 845, 779, 481,
29, 15, 29, 809, 791, 505,
29, 15, 30, 820, 779, 497,
29, 16, 28, 785, 791, 522,
29, 16, 29, 796, 779, 513,
29, 17, 27, 763, 791, 538,
29, 17, 28, 773, 781, 529,
29, 18, 26, 740, 792, 556,
29, 18, 27, 751, 781, 546,
29, 19, 25, 718, 792, 574,
29, 19, 26, 730, 781, 564,
29, 20, 24, 698, 792, 593,
29, 20, 25, 708, 781, 583,
29, 21, 23, 677, 792, 612,
29, 21, 24, 688, 781, 602,
29, 22, 22, 657, 792, 633,
29, 22, 23, 667, 781, 623,
29, 23, 21, 638, 792, 653,
29, 23, 22, 648, 781, 643,
29, 24, 20, 620, 792, 675,
29, 24, 21, 629, 781, 663,
29, 25, 19, 602, 792, 698,
29, 25, 20, 611, 781, 685,
29, 26, 18, 584, 792, 721,
29, 26, 19, 593, 781, 708,
29, 27, 17, 569, 792, 744,
29, 27, 18, 577, 781, 731,
29, 28, 16, 551, 792, 768,
29, 28, 17, 560, 781, 755,
29, 29, 15, 536, 792, 794,
29, 29, 16, 544, 781, 779,
29, 30, 14, 521, 792, 819,
29, 30, 15, 527, 781, 805,
29, 31, 13, 505, 792, 847,
29, 31, 14, 513, 781, 832,
29, 32, 12, 490, 792, 874,
29, 32, 13, 497, 781, 859,
29, 33, 11, 476, 792, 902,
29, 33, 12, 483, 781, 888,
29, 34, 10, 462, 792, 931,
29, 34, 11, 469, 781, 916,
29, 35,  9, 449, 792, 962,
29, 35, 10, 456, 782, 947,
29, 36,  8, 437, 794, 991,
29, 36,  9, 442, 782, 976,
30,  7, 36, 990, 818, 391,
30,  8, 35, 963, 818, 402,
30,  8, 36, 977, 805, 397,
30,  9, 34, 936, 818, 416,
30,  9, 35, 949, 805, 409,
30, 10, 33, 910, 818, 430,
30, 10, 34, 922, 805, 423,
30, 11, 32, 883, 818, 444,
30, 11, 33, 896, 805, 437,
30, 12, 31, 857, 818, 458,
30, 12, 32, 870, 805, 451,
30, 13, 30, 833, 818, 474,
30, 13, 31, 845, 805, 465,
30, 14, 29, 809, 818, 490,
30, 14, 30, 820, 805, 481,
30, 15, 28, 785, 818, 505,
30, 15, 29, 796, 805, 497,
30, 16, 27, 763, 818, 522,
30, 16, 28, 773, 805, 513,
30, 17, 26, 740, 818, 538,
30, 17, 27, 751, 805, 529,
30, 18, 25, 718, 818, 556,
30, 18, 26, 730, 805, 546,
30, 19, 24, 698, 819, 574,
30, 19, 25, 708, 805, 564,
30, 20, 23, 677, 819, 593,
30, 20, 24, 688, 805, 583,
30, 21, 22, 658, 819, 612,
30, 21, 23, 667, 805, 602,
30, 22, 21, 638, 819, 633,
30, 22, 22, 648, 805, 623,
30, 23, 20, 620, 819, 653,
30, 23, 21, 629, 805, 643,
30, 24, 19, 602, 819, 675,
30, 24, 20, 611, 805, 663,
30, 25, 18, 586, 819, 698,
30, 25, 19, 593, 806, 685,
30, 26, 17, 569, 819, 721,
30, 26, 18, 577, 806, 708,
30, 27, 16, 551, 819, 744,
30, 27, 17, 560, 806, 731,
30, 28, 15, 536, 819, 768,
30, 28, 16, 544, 806, 755,
30, 29, 14, 521, 819, 794,
30, 29, 15, 527, 806, 779,
30, 30, 13, 505, 819, 819,
30, 30, 14, 513, 806, 805,
30, 31, 12, 490, 819, 847,
30, 31, 13, 497, 806, 832,
30, 32, 11, 476, 819, 874,
30, 32, 12, 483, 806, 860,
30, 33, 10, 462, 819, 902,
30, 33, 11, 469, 806, 888,
30, 34,  9, 449, 819, 931,
30, 34, 10, 456, 806, 916,
30, 35,  8, 435, 819, 962,
30, 35,  9, 442, 806, 947,
30, 36,  7, 423, 819, 991,
30, 36,  8, 430, 806, 977,
31,  6, 36, 990, 845, 377,
31,  7, 35, 963, 845, 391,
31,  7, 36, 977, 832, 384,
31,  8, 34, 936, 845, 404,
31,  8, 35, 950, 832, 397,
31,  9, 33, 910, 845, 416,
31,  9, 34, 922, 832, 409,
31, 10, 32, 883, 845, 430,
31, 10, 33, 896, 832, 423,
31, 11, 31, 857, 845, 444,
31, 11, 32, 870, 832, 437,
31, 12, 30, 833, 845, 458,
31, 12, 31, 845, 832, 451,
31, 13, 29, 809, 845, 474,
31, 13, 30, 820, 832, 465,
31, 14, 28, 785, 845, 490,
31, 14, 29, 796, 832, 481,
31, 15, 27, 763, 845, 505,
31, 15, 28, 773, 832, 497,
31, 16, 26, 740, 845, 522,
31, 16, 27, 751, 832, 513,
31, 17, 25, 718, 845, 538,
31, 17, 26, 730, 832, 529,
31, 18, 24, 698, 845, 556,
31, 18, 25, 708, 832, 547,
31, 19, 23, 677, 845, 574,
31, 19, 24, 688, 833, 564,
31, 20, 22, 658, 845, 593,
31, 20, 23, 667, 833, 583,
31, 21, 21, 638, 845, 612,
31, 21, 22, 648, 833, 602,
31, 22, 20, 620, 845, 634,
31, 22, 21, 629, 833, 623,
31, 23, 19, 602, 846, 653,
31, 23, 20, 611, 833, 643,
31, 24, 18, 586, 846, 675,
31, 24, 19, 593, 833, 663,
31, 25, 17, 569, 846, 698,
31, 25, 18, 577, 833, 685,
31, 26, 16, 551, 846, 721,
31, 26, 17, 560, 833, 708,
31, 27, 15, 536, 846, 744,
31, 27, 16, 544, 833, 731,
31, 28, 14, 521, 846, 768,
31, 28, 15, 527, 833, 755,
31, 29, 13, 505, 846, 794,
31, 29, 14, 513, 833, 779,
31, 30, 12, 490, 846, 819,
31, 30, 13, 497, 833, 805,
31, 31, 11, 476, 846, 847,
31, 31, 12, 483, 833, 832,
31, 32, 10, 462, 846, 874,
31, 32, 11, 469, 833, 860,
31, 33,  9, 449, 846, 902,
31, 33, 10, 456, 833, 888,
31, 34,  8, 435, 846, 933,
31, 34,  9, 442, 833, 916,
31, 35,  7, 423, 846, 962,
31, 35,  8, 430, 833, 947,
31, 36,  6, 412, 846, 991,
31, 36,  7, 418, 833, 977,
32,  5, 36, 990, 873, 365,
32,  6, 35, 964, 871, 377,
32,  6, 36, 977, 859, 372,
32,  7, 34, 936, 871, 391,
32,  7, 35, 950, 859, 384,
32,  8, 33, 910, 871, 404,
32,  8, 34, 922, 859, 397,
32,  9, 32, 883, 871, 416,
32,  9, 33, 896, 859, 409,
32, 10, 31, 857, 871, 430,
32, 10, 32, 870, 859, 423,
32, 11, 30, 833, 871, 444,
32, 11, 31, 845, 859, 437,
32, 12, 29, 809, 871, 458,
32, 12, 30, 820, 859, 451,
32, 13, 28, 785, 873, 474,
32, 13, 29, 796, 859, 465,
32, 14, 27, 763, 873, 490,
32, 14, 28, 773, 859, 481,
32, 15, 26, 740, 873, 505,
32, 15, 27, 751, 860, 497,
32, 16, 25, 718, 873, 522,
32, 16, 26, 730, 860, 513,
32, 17, 24, 698, 873, 538,
32, 17, 25, 708, 860, 529,
32, 18, 23, 677, 873, 556,
32, 18, 24, 688, 860, 547,
32, 19, 22, 658, 873, 574,
32, 19, 23, 667, 860, 565,
32, 20, 21, 638, 873, 593,
32, 20, 22, 648, 860, 583,
32, 21, 20, 620, 873, 614,
32, 21, 21, 629, 860, 602,
32, 22, 19, 602, 873, 634,
32, 22, 20, 611, 860, 623,
32, 23, 18, 586, 873, 653,
32, 23, 19, 593, 860, 643,
32, 24, 17, 569, 873, 675,
32, 24, 18, 577, 860, 663,
32, 25, 16, 551, 873, 698,
32, 25, 17, 560, 860, 685,
32, 26, 15, 536, 873, 721,
32, 26, 16, 544, 860, 708,
32, 27, 14, 521, 873, 744,
32, 27, 15, 527, 860, 731,
32, 28, 13, 505, 873, 768,
32, 28, 14, 513, 860, 755,
32, 29, 12, 490, 874, 794,
32, 29, 13, 497, 860, 781,
32, 30, 11, 476, 874, 819,
32, 30, 12, 483, 860, 805,
32, 31, 10, 462, 874, 847,
32, 31, 11, 469, 861, 833,
32, 32,  9, 449, 874, 874,
32, 32, 10, 456, 861, 860,
32, 33,  8, 435, 874, 902,
32, 33,  9, 442, 861, 888,
32, 34,  7, 423, 874, 933,
32, 34,  8, 430, 861, 916,
32, 35,  6, 412, 874, 962,
32, 35,  7, 418, 861, 947,
32, 36,  5, 400, 874, 991,
32, 36,  6, 405, 861, 977,
33,  4, 36, 990, 901, 354,
33,  5, 35, 964, 901, 365,
33,  5, 36, 977, 887, 360,
33,  6, 34, 936, 901, 377,
33,  6, 35, 950, 887, 372,
33,  7, 33, 910, 901, 391,
33,  7, 34, 922, 887, 384,
33,  8, 32, 883, 901, 404,
33,  8, 33, 896, 887, 397,
33,  9, 31, 857, 901, 416,
33,  9, 32, 870, 887, 409,
33, 10, 30, 833, 901, 430,
33, 10, 31, 845, 887, 423,
33, 11, 29, 809, 901, 444,
33, 11, 30, 820, 887, 437,
33, 12, 28, 785, 901, 458,
33, 12, 29, 796, 887, 451,
33, 13, 27, 763, 901, 474,
33, 13, 28, 773, 888, 465,
33, 14, 26, 740, 901, 490,
33, 14, 27, 751, 888, 481,
33, 15, 25, 718, 901, 505,
33, 15, 26, 730, 888, 497,
33, 16, 24, 698, 901, 522,
33, 16, 25, 708, 888, 513,
33, 17, 23, 677, 901, 538,
33, 17, 24, 688, 888, 529,
33, 18, 22, 658, 901, 556,
33, 18, 23, 667, 888, 547,
33, 19, 21, 638, 901, 574,
33, 19, 22, 648, 888, 565,
33, 20, 20, 620, 901, 593,
33, 20, 21, 629, 888, 583,
33, 21, 19, 602, 902, 614,
33, 21, 20, 611, 888, 602,
33, 22, 18, 586, 902, 634,
33, 22, 19, 593, 888, 623,
33, 23, 17, 569, 902, 653,
33, 23, 18, 577, 888, 643,
33, 24, 16, 551, 902, 675,
33, 24, 17, 560, 888, 663,
33, 25, 15, 536, 902, 698,
33, 25, 16, 544, 888, 685,
33, 26, 14, 521, 902, 721,
33, 26, 15, 527, 888, 708,
33, 27, 13, 505, 902, 744,
33, 27, 14, 513, 888, 731,
33, 28, 12, 490, 902, 768,
33, 28, 13, 497, 888, 755,
33, 29, 11, 476, 902, 794,
33, 29, 12, 483, 888, 781,
33, 30, 10, 462, 902, 819,
33, 30, 11, 469, 888, 805,
33, 31,  9, 449, 902, 847,
33, 31, 10, 456, 888, 833,
33, 32,  8, 437, 902, 875,
33, 32,  9, 442, 888, 860,
33, 33,  7, 423, 902, 902,
33, 33,  8, 430, 888, 888,
33, 34,  6, 412, 902, 933,
33, 34,  7, 418, 888, 916,
33, 35,  5, 400, 902, 962,
33, 35,  6, 405, 888, 947,
33, 36,  4, 388, 902, 991,
33, 36,  5, 391, 888, 980,
34,  3, 36, 990, 930, 344,
34,  4, 35, 964, 930, 354,
34,  4, 36, 978, 916, 349,
34,  5, 34, 936, 930, 365,
34,  5, 35, 950, 916, 360,
34,  6, 33, 910, 930, 377,
34,  6, 34, 922, 916, 372,
34,  7, 32, 883, 930, 391,
34,  7, 33, 896, 916, 384,
34,  8, 31, 857, 930, 404,
34,  8, 32, 870, 916, 397,
34,  9, 30, 833, 930, 416,
34,  9, 31, 845, 916, 409,
34, 10, 29, 809, 930, 430,
34, 10, 30, 820, 916, 423,
34, 11, 28, 785, 930, 444,
34, 11, 29, 796, 916, 437,
34, 12, 27, 763, 930, 458,
34, 12, 28, 773, 916, 451,
34, 13, 26, 740, 930, 474,
34, 13, 27, 751, 916, 465,
34, 14, 25, 718, 930, 490,
34, 14, 26, 730, 916, 481,
34, 15, 24, 698, 930, 505,
34, 15, 25, 708, 916, 497,
34, 16, 23, 677, 930, 522,
34, 16, 24, 688, 916, 513,
34, 17, 22, 658, 930, 538,
34, 17, 23, 667, 916, 529,
34, 18, 21, 639, 930, 556,
34, 18, 22, 648, 916, 547,
34, 19, 20, 620, 930, 574,
34, 19, 21, 629, 916, 565,
34, 20, 19, 602, 930, 593,
34, 20, 20, 611, 916, 583,
34, 21, 18, 586, 930, 614,
34, 21, 19, 593, 916, 602,
34, 22, 17, 569, 930, 634,
34, 22, 18, 577, 916, 623,
34, 23, 16, 551, 930, 654,
34, 23, 17, 560, 916, 643,
34, 24, 15, 536, 930, 675,
34, 24, 16, 544, 916, 665,
34, 25, 14, 521, 930, 698,
34, 25, 15, 527, 916, 685,
34, 26, 13, 505, 930, 721,
34, 26, 14, 513, 916, 708,
34, 27, 12, 490, 930, 745,
34, 27, 13, 497, 916, 731,
34, 28, 11, 476, 930, 768,
34, 28, 12, 483, 917, 755,
34, 29, 10, 462, 930, 794,
34, 29, 11, 469, 917, 781,
34, 30,  9, 449, 931, 819,
34, 30, 10, 456, 917, 805,
34, 31,  8, 437, 931, 847,
34, 31,  9, 442, 917, 833,
34, 32,  7, 423, 931, 875,
34, 32,  8, 430, 917, 860,
34, 33,  6, 412, 931, 902,
34, 33,  7, 418, 917, 888,
34, 34,  5, 400, 931, 933,
34, 34,  6, 405, 917, 916,
34, 35,  4, 388, 931, 962,
34, 35,  5, 393, 917, 947,
34, 36,  3, 377, 931, 992,
34, 36,  4, 381, 917, 980,
35,  2, 36, 990, 963, 333,
35,  3, 35, 964, 959, 344,
35,  3, 36, 978, 945, 337,
35,  4, 34, 936, 959, 354,
35,  4, 35, 950, 945, 349,
35,  5, 33, 910, 959, 367,
35,  5, 34, 922, 945, 360,
35,  6, 32, 883, 959, 379,
35,  6, 33, 896, 945, 372,
35,  7, 31, 857, 959, 391,
35,  7, 32, 870, 945, 384,
35,  8, 30, 833, 959, 404,
35,  8, 31, 845, 945, 397,
35,  9, 29, 809, 961, 416,
35,  9, 30, 820, 945, 409,
35, 10, 28, 786, 961, 430,
35, 10, 29, 796, 945, 423,
35, 11, 27, 763, 961, 444,
35, 11, 28, 773, 945, 437,
35, 12, 26, 740, 961, 458,
35, 12, 27, 751, 945, 451,
35, 13, 25, 718, 961, 474,
35, 13, 26, 730, 945, 465,
35, 14, 24, 698, 961, 490,
35, 14, 25, 708, 947, 481,
35, 15, 23, 677, 961, 505,
35, 15, 24, 688, 947, 497,
35, 16, 22, 658, 961, 522,
35, 16, 23, 667, 947, 513,
35, 17, 21, 639, 961, 540,
35, 17, 22, 648, 947, 529,
35, 18, 20, 620, 961, 556,
35, 18, 21, 629, 947, 547,
35, 19, 19, 602, 961, 574,
35, 19, 20, 611, 947, 565,
35, 20, 18, 586, 961, 593,
35, 20, 19, 593, 947, 583,
35, 21, 17, 569, 961, 614,
35, 21, 18, 577, 947, 602,
35, 22, 16, 551, 961, 634,
35, 22, 17, 560, 947, 623,
35, 23, 15, 536, 961, 654,
35, 23, 16, 544, 947, 643,
35, 24, 14, 521, 961, 675,
35, 24, 15, 527, 947, 665,
35, 25, 13, 505, 962, 698,
35, 25, 14, 513, 947, 685,
35, 26, 12, 490, 962, 721,
35, 26, 13, 497, 947, 708,
35, 27, 11, 476, 962, 745,
35, 27, 12, 483, 947, 731,
35, 28, 10, 462, 962, 768,
35, 28, 11, 469, 947, 755,
35, 29,  9, 449, 962, 794,
35, 29, 10, 456, 948, 781,
35, 30,  8, 437, 962, 819,
35, 30,  9, 442, 948, 805,
35, 31,  7, 423, 962, 847,
35, 31,  8, 430, 948, 833,
35, 32,  6, 412, 962, 875,
35, 32,  7, 418, 948, 860,
35, 33,  5, 400, 962, 902,
35, 33,  6, 405, 948, 888,
35, 34,  4, 388, 962, 933,
35, 34,  5, 393, 948, 916,
35, 35,  3, 377, 962, 963,
35, 35,  4, 381, 948, 947,
35, 36,  2, 365, 962, 990,
35, 36,  3, 370, 948, 981,
36,  1, 36, 990, 987, 321,
36,  2, 35, 964, 991, 333,
36,  2, 36, 978, 981, 326,
36,  3, 34, 936, 990, 344,
36,  3, 35, 950, 976, 337,
36,  4, 33, 910, 990, 354,
36,  4, 34, 922, 976, 349,
36,  5, 32, 883, 990, 367,
36,  5, 33, 897, 976, 360,
36,  6, 31, 857, 990, 379,
36,  6, 32, 870, 976, 372,
36,  7, 30, 833, 990, 391,
36,  7, 31, 845, 976, 384,
36,  8, 29, 809, 990, 404,
36,  8, 30, 820, 976, 397,
36,  9, 28, 786, 990, 416,
36,  9, 29, 796, 976, 409,
36, 10, 27, 763, 990, 430,
36, 10, 28, 773, 976, 423,
36, 11, 26, 740, 990, 444,
36, 11, 27, 751, 976, 437,
36, 12, 25, 718, 990, 458,
36, 12, 26, 730, 976, 451,
36, 13, 24, 698, 990, 474,
36, 13, 25, 708, 976, 465,
36, 14, 23, 677, 990, 490,
36, 14, 24, 688, 976, 481,
36, 15, 22, 658, 990, 505,
36, 15, 23, 667, 976, 497,
36, 16, 21, 639, 990, 522,
36, 16, 22, 648, 976, 513,
36, 17, 20, 620, 990, 540,
36, 17, 21, 629, 977, 529,
36, 18, 19, 602, 990, 556,
36, 18, 20, 611, 977, 547,
36, 19, 18, 586, 990, 575,
36, 19, 19, 593, 977, 565,
36, 20, 17, 569, 990, 593,
36, 20, 18, 577, 977, 583,
36, 21, 16, 551, 990, 614,
36, 21, 17, 560, 977, 602,
36, 22, 15, 536, 990, 634,
36, 22, 16, 544, 977, 623,
36, 23, 14, 521, 990, 654,
36, 23, 15, 527, 977, 643,
36, 24, 13, 505, 990, 675,
36, 24, 14, 513, 977, 665,
36, 25, 12, 490, 990, 698,
36, 25, 13, 497, 977, 685,
36, 26, 11, 476, 990, 721,
36, 26, 12, 483, 977, 708,
36, 27, 10, 462, 991, 745,
36, 27, 11, 469, 977, 731,
36, 28,  9, 449, 991, 768,
36, 28, 10, 456, 977, 755,
36, 29,  8, 437, 991, 794,
36, 29,  9, 442, 977, 781,
36, 30,  7, 423, 991, 819,
36, 30,  8, 430, 977, 805,
36, 31,  6, 412, 991, 847,
36, 31,  7, 418, 978, 833,
36, 32,  5, 400, 991, 875,
36, 32,  6, 405, 978, 860,
36, 33,  4, 388, 991, 902,
36, 33,  5, 393, 978, 888,
36, 34,  3, 377, 991, 934,
36, 34,  4, 381, 978, 916,
36, 35,  2, 365, 991, 966,
36, 35,  3, 370, 978, 948,
36, 36,  1, 356, 991, 987,
36, 36,  2, 358, 980, 985
};
