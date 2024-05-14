#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "opencv2/structured_light.hpp"
#include <opencv2/imgcodecs.hpp>

#include "ofxVideoGrabber.h"

#include "ofxGui.h"
#include "ofxDropdown.h"
#include "ofxCameraSaveLoad.h"

using namespace cv;

#define WINDOWWIDTH 1280
#define WINDOWHEIGHT 800
#define GRAYCODEWIDTHSTEP 1 //5
#define GRAYCODEHEIGHTSTEP 1 //5
#define GRAYCODEWIDTH WINDOWWIDTH / GRAYCODEWIDTHSTEP
#define GRAYCODEHEIGHT WINDOWHEIGHT / GRAYCODEHEIGHTSTEP
//#define WHITETHRESHOLD 5
//#define BLACKTHRESHOLD 40

// Camera to Projector
struct C2P {
    int cx;
    int cy;
    int px;
    int py;
    C2P(int camera_x, int camera_y, int proj_x, int proj_y)
    {
        cx = camera_x;
        cy = camera_y;
        px = proj_x;
        py = proj_y;
    }
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    
    ofEventListeners listeners;
    
    ofEasyCam easyCam_2d;
    ofEasyCam easyCam_3d;
    ofRectangle viewport_2d;
    ofRectangle viewport_3d;
    void drawViewportOutline(const ofRectangle & viewport);
    
    ofxPanel gui_main;
    
    bool bShowGui = true;
    
    int camWidth, camHeight;
    int projectorWidth, projectorHeight;
    
    //camera grabber
    ofxVideoGrabber cam1;
    ofImage cam1_img;
    ofImage cam2_img;
    
    //    ofxVideoGrabber cam_2;
    
    ofParameter<string> selectedUid_1 = {"Selected Cam 1", ""};
    unique_ptr<ofxDropdown> uidDropdown_1;
    ofEventListener uidListener_1;
    void selectedCameraChanged_1(string &);
    
    //    ofParameter<string> selectedUid_2 = {"Selected Cam 2", ""};
    //    unique_ptr<ofxDropdown> uidDropdown_2;
    //    ofEventListener uidListener_2;
    //    void selectedCameraChanged_2(string &);
    
    //graycode calibration
    ofImage undistorted;
    //	ofxCv::Calibration calibration;
    
    //encode graycode pattern
    bool encodeGraycode();
    bool encodeGraycode_simple();
    
    cv::Mat c2pX , c2pY;
    cv::Mat result_viz;
    
    structured_light::GrayCodePattern::Params params;
    vector<Mat> graycode_patterns;
    vector<vector<string>> capturedImages; // Store filenames for each camera
    
//    int patternCounter = 0;
    bool encode_capture = false;
    int encode_captureCount = 0;
    int captureStage = 0;
    float captureStage_timer = 0;
    float captureStage_duration = 0;
    ofParameter<bool> bDebug;
    
    //decode graycode
    bool decodeGraycode_simple();
    bool decodeGraycode();
    bool decode_done = false;
//    bool readStringList( const string& filename, vector<string>& l );
    bool readStringList_oneCam( string filename, vector<string>& l );
    bool readStringList_twoCams( string filename, vector<string>& l );
    cv::Ptr<structured_light::GrayCodePattern> graycode_decode;
    ofParameter<int> white_thresh;
    ofParameter<int> black_thresh;
    
    vector<Mat> capturedMats;
    vector<ofImage> capturedOfImages;
    // Loading calibration parameters
//    Mat cam1_intrinsics, cam1_distCoeffs, cam2_intrinsics, cam2_distCoeffs, R, T;
    
    //homogrpahy sandbox
    bool hasHomography = false;
    cv::Mat homography;
    
    ofImage cm_disp_img;
    ofImage disp_otsu_img;
    
    ofMesh pointCloud_mesh;
    ofMesh pointCloud_mesh_zeored;
    ofBoxPrimitive boundingBox;
    float mesh_minY = INT_MAX;
    float mesh_maxY = -INT_MAX;
    float mesh_minX = INT_MAX;
    float mesh_maxX = -INT_MAX;
    float mesh_minZ = INT_MAX;
    float mesh_maxZ = -INT_MAX;
    
    cv::Mat white_capturedMat, black_capturedMat;
    ofImage white_capturedImg, black_capturedImg;
    void pointCloudToOFMesh(const cv::Mat& pointcloud, ofMesh& mesh, const cv::Mat& colorImage = cv::Mat()) {
        // Set up the mesh
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_POINTS);
        bool useColor = !colorImage.empty();

        for (int y = 0; y < pointcloud.rows; ++y) {
            for (int x = 0; x < pointcloud.cols; ++x) {
                // Get the 3D point
                cv::Vec3f point = pointcloud.at<cv::Vec3f>(y, x);
                // Check if the point is valid
                if (cv::checkRange(point)) {
                    ofVec3f ofPoint(point[0], point[1], point[2]);
                    mesh.addVertex(ofPoint);

                    // Add color if provided
                    if (useColor) {
                        cv::Vec3b color = colorImage.at<cv::Vec3b>(y, x);
                        ofColor ofColor(color[2], color[1], color[0]);  // Convert BGR to RGB
                        mesh.addColor(ofColor);
                    }
                }
            }
        }
    }
    
    ofBoxPrimitive getExtremes(ofMesh & mesh){
        
 
        ofRectangle boundingRect;
        ofBoxPrimitive boundingBox;

        for(auto & v : mesh.getVertices()){
            mesh_minX = min(mesh_minX,v.x);
            mesh_maxX = max(mesh_maxX,v.x);
            mesh_minY = min(mesh_minY,v.y);
            mesh_maxY = max(mesh_maxY,v.y);
            mesh_minZ = min(mesh_minZ,v.z);
            mesh_maxZ = max(mesh_maxZ,v.z);
        }
        ofLog()<<"mesh_minX "<<mesh_minX<<" mesh_maxX "<<mesh_maxX;
        ofLog()<<"mesh_minY "<<mesh_minY<<" mesh_maxY "<<mesh_maxY;
        ofLog()<<"mesh_minZ "<<mesh_minZ<<" mesh_maxZ "<<mesh_maxZ;
        
        float w = mesh_maxX-mesh_minX;
        float h = mesh_maxY-mesh_minY;
        float d = mesh_maxZ-mesh_minZ;
//        boundingBox.setPosition(minX+w/2,minY+h/2, minZ+d/2);
        boundingBox.setWidth(w);
        boundingBox.setHeight(h);
        boundingBox.setDepth(d);
        boundingBox.setResolution(1);
        ofLog()<<"boundingBox "<<w<<"x"<<h<<"x"<<d;

        
        boundingRect.setX(mesh_minX);
        boundingRect.setWidth(mesh_maxX-mesh_minX);
        boundingRect.setY(mesh_minY);
        boundingRect.setHeight(mesh_maxY-mesh_minY);
        
        //        activeArea_width = boundingRect.getWidth();
        //        activeArea_height = boundingRect.getHeight();
        //TODO: set bounding box not rect
        ofLog()<<"getExtremes(vector<ofVec3f> & _points new boundingRect "<<boundingRect;
        
        return boundingBox;
    }
};
