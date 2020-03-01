

##M1.1

The data buffer optmization was done at "matching2D_student.cpp", line 40 using an std::deque for building an circular buffer.
As a DataFrame is pushed back, the elemtns at the front are poped until the size is adequate.

## MP.2

The Keypoints dectors were implemented at the file "matchin2D_student.cpp". For the HARRIs detector the same method, "goodFeaturesToTrack" was used, but with the true argument
that defines the usage of the Harris algorithm. 

For the "modern" detectors the function "detKeypointsModern" contains tall of the detectors using an "cv::Ptr<cv::FeatureDetector>", with the exception of the SIFT, that uses an pointer of type cv::xfeatures2d::SIFT. The string detectorType is then used as a "selector switch" for instanciating the correct feature detector type.

## MP.3 

As the preceding vehicle is described as an cv::Rect, the keypoint vector is iterated and the keypoint argument "pt" is tested with the contains method of the cv::Rect.
If the rectangle contains the point, then this keypoint is pushed onto a temp keypoint vector. After the iteration procces ends, the temporary keypoint vector is copyied over
the keypoints vector, as the following code uses this for keypoint description and matching.

## MP.4

For implementing the descriptors, at file "matching2d_Student.cpp" the method descKeypoints was completed using an pointer of type cv::DescriptorExtractor. Depending of the
descriptor argument, descriptorType, one of the descriptors is instanciated with it's own craete function. One importante note for the AKAZE. As it depends on having the
class_id property of the keypoint set to zero before match, an iterations was added when using this type of descriptor for enabling the code to run as expected

## MP.5

FLANN implemented at "matching2d_Student.cpp" on method matchDescriptors. It was just instancied on the cv::DescriptorMatcher using the argument cv::DescriptorMatcher::FLANNBASED. 

Also, on this method, a checking was added for the argument descriptorType. If this type contains "DES_BINARY, the HAMMING norm is used at the matcher, otherwise, an L2 norm is used. This allows the usage of gradient descriptors, as the SIFT, and the binary ones properly. 

Also, when using the FLANN matching, a checking was added to verify if the type of the variable contained at the cv::Mat source. If it is not of type CV_32F, an temporary cv::Mat with this type is generated in order to FLANN run properly.

## MP.6

For the distance ratio testing, at the KNN performance matching, an evaluation was added veryfing if the ratio was above the min specified value.

## MP.7 

Number of keypoints:

ShiTomasi	Harris	Fast	Brisk	Orb		Akaze	Sift
						
1370		492		5063	2757	500		1351	1437
1301		502		4952	2777	500		1327	1371
1361		516		4863	2741	500		1311	1381
1358		524		4840	2735	500		1351	1336
1333		523		4856	2757	500		1360	1303
1284		511		4899	2695	500		1347	1370
1322		505		4870	2715	500		1363	1396
1366		510		4868	2628	500		1331	1382
1389		529		4996	2639	500		1357	1462
1339		520		4997	2672	500		1331	1422

Regarding the Size around the keypoints: 
For shi-tomasi and Harris: mean size is 4
FAST: mean size is 7
BRISK: mean size is 19.3
ORB: mean size is 53.
AKAZE: mean size is 7.50526
SIFT: mean size is 4.9395


## MP.8

The following tables have all the number of matched keypoitns using the described criterea for all images and all possible combinations


					Detector: Shi Tomasi					
Descriptor	Brisk	Orb		Freak	Akaze	Sift	
			95		106		86		112		112	
			88		102		90		111		109	
			80		99		86		101		104	
			90		102		88		106		103	
			82		103		86		105		99	
			79		97		80		101		101	
			85		98		81		99		96	
			86		104		86		108		106	
			82		97		85		96		97	
									
					Detector: Harris					
Descriptor	Brisk	Orb		Freak	Akaze	Sift	
			43		46		43		48		46	
			39		44		43		48		46	
			45		48		42		47		50	
			44		51		39		53		52	
			37		49		41		50		51	
			41		51		48		51		48	
			44		50		45		53		52	
			52		57		47		57		59	
			48		55		48		54		55	
						
					Detector: Fast					
Descriptor	Brisk	Orb		Freak	Akaze	Sift	
			256		307		251		332		316	
			243		308		247		334		325	
			241		298		233		319		297	
			239		321		255		330		311	
			215		283		231		295		291	
			251		315		265		336		326	
			248		323		251		331		315	
			243		302		253		330		300	
			247		311		247		314		301	
						
					Detector: Brisk					
Descriptor	Brisk	Orb		Freak	Akaze	Sift	
			171		162		160		148		182	
			176		175		177		157		193		
			157		158		155		147		169	
			176		167		173		142		183	
			174		160		161		146		171	
			188		182		183		150		195	
			173		167		169		152		194	
			171		171		178		146		176	
			184		172		168		151		183	
								
					Detector: ORB					
Descriptor	Brisk	Orb		Freak	Akaze	Sift	
			73		67		42		42		67	
			74		70		36		54		79	
			79		72		44		56		78	
			85		84		47		56		79	
			79		91		44		49		82	
			92		101		51		54		95	
			90		92		52		74		95	
			88		93		48		61		94	
			91		93		56		61		94	
								
					Detector: Akaze					
Descriptor	Brisk 	Orb		Freak	Akaze	Sift	
			137		131		126		138		134	
			125		129		129		137		134	
			129		127		127		131		130	
			129		117		121		128		136	
			131		130		122		128		137	
			132		131		133		145		147	
			142		137		144		147		147	
			146		135		147		149		154	
			144		145		138		149		151	
							
					Detector: Sift					
Descriptor	Brisk 	Orb		Freak	Akaze 	Sift	
			64					
			66					
			63					
			67					
			59					
			64					
			64					
			67					
			80					



Note, for some reason, when using the Sift detector, it presented some problems regarding memory allocation with the descriptos, with exception of the SIFT descriptr.


## MP. 8

Using the tme took for detection and description as criteria for choosing, as well as the number of keypoints, the top 3 combinations are:

Detector: FAST and Descriptor ORB
Detector: FAST and Descriptor BRISK
Detector: HARRIs and Descriptor BRISK