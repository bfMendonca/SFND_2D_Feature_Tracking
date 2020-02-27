#include <numeric>
#include "matching2D.hpp"

using namespace std;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        int normType = descriptorType.compare( "DES_BINARY" ) == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        matcher = cv::DescriptorMatcher::create( cv::DescriptorMatcher::FLANNBASED );
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        //As now we cannot modified the image, or we will endup modifying the ring buffer, let's make a copy, if needed
        //in order to overcome the FLANN BUG
        if( ( (matcherType.compare("MAT_FLANN") == 0) ) && descSource.type() != CV_32F ) {
            
            cv::Mat modifiedSource = descSource.clone();
            cv::Mat modifiedRef = descRef.clone();

            descSource.convertTo( modifiedSource, CV_32F );
            descRef.convertTo( modifiedRef, CV_32F );

            matcher->match(modifiedSource, modifiedRef, matches); // Finds the best match for each descriptor in desc1
        }else {
            matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        }

        
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        int k = 2;
        std::vector< std::vector <cv::DMatch> > rawKnnMatches;
        
        double t = (double)cv::getTickCount();
        matcher->knnMatch(descSource, descRef, rawKnnMatches, 2); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (KNN) with n=" << rawKnnMatches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;


        double minDescDistRatio = 0.8;
        for (auto it = rawKnnMatches.begin(); it != rawKnnMatches.end(); ++it) {

            if ((*it)[0].distance < minDescDistRatio * (*it)[1].distance) {
                matches.push_back((*it)[0]);
            }
        }

        cout << "# keypoints removed = " << rawKnnMatches.size() - matches.size() << endl;
    }
}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    cv::Ptr<cv::DescriptorExtractor> extractor;
    if (descriptorType.compare("BRISK") == 0)
    {

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = cv::BRISK::create(threshold, octaves, patternScale);
    }
    else 
    {
        if( descriptorType.compare("BRIEF") == 0 ) {
            extractor = cv::xfeatures2d::BriefDescriptorExtractor::create();
        }else if( descriptorType.compare("ORB") == 0 ) {
            extractor = cv::ORB::create( );
        }else if( descriptorType.compare("FREAK") == 0 ) {
            extractor = cv::xfeatures2d::FREAK::create();
        }else if( descriptorType.compare("AKAZE") == 0 ) {
            extractor = cv::AKAZE::create();
        }else if( descriptorType.compare("SIFT") == 0 ) { 
            extractor = cv::xfeatures2d::SIFT::create();
        }

    }

    // perform feature description
    double t = (double)cv::getTickCount();
    extractor->compute(img, keypoints, descriptors);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;

    
}


void detKeypointsHarris(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, true, k);
    //-------------------------------------------------------------------------------------------------AAAA---//

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Harris detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Harris Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
    // std::cout << "number of keypoints for harris: " << keyPoints.size() << std::endl;
}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}


void detKeypointsModern(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, std::string detectorType, bool bVis)
{

    if( detectorType.compare("SIFT") != 0 ) {
        cv::Ptr<cv::FeatureDetector> detector;

        cout << "Using detector " << detectorType << ",n = ";

        if( detectorType.compare("FAST") == 0) {
            detector = cv::FastFeatureDetector::create();
        }else if( detectorType.compare("BRISK") == 0  ) {
            detector = cv::BRISK::create();
        }else if( detectorType.compare("ORB") == 0  ) {
            detector = cv::ORB::create();
        }else if( detectorType.compare("AKAZE") == 0  ) {
            detector = cv::AKAZE::create();
        }

        double t = (double)cv::getTickCount();
        detector->detect( img, keypoints );
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

        cout << keypoints.size() << " keypoints in " << 1000 * t /1.0 << "ms" << endl;
    }else {
        cv::Ptr<cv::xfeatures2d::SIFT> sift;
        sift = cv::xfeatures2d::SIFT::create();

        double t = (double)cv::getTickCount();
        sift->detect( img, keypoints );
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

        cout << "Using detector " << detectorType << ",n = " << keypoints.size() << " keypoints in " << 1000 * t /1.0 << "ms" << endl; 
    }
}