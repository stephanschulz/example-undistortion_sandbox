#include "ofApp.h"

//https://docs.opencv.org/4.x/db/d56/tutorial_capture_graycode_pattern.html
//https://docs.opencv.org/4.x/dc/da9/tutorial_decode_graycode_pattern.html
//https://www.diva-portal.org/smash/get/diva2:650544/FULLTEXT01.pdf
//https://github.com/punpongsanon/graycode-procam-calibration/blob/master/code/procam-calibration-with-graycode/main.cpp
using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetBackgroundColor(ofColor::pink);
    
    easyCam_2d.removeAllInteractions();
    easyCam_2d.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_XY, OF_MOUSE_BUTTON_LEFT, 'm');
    easyCam_2d.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_Z, OF_MOUSE_BUTTON_RIGHT, 'm');
    
    easyCam_2d.enableOrtho();
    easyCam_2d.setNearClip(-1000000);
    easyCam_2d.setFarClip(1000000);
    easyCam_2d.setVFlip(true);
    viewport_2d = ofRectangle(0,0,1000,1200);
    //    easyCam_2d.move(700,0,500);
    ofxLoadCamera(easyCam_2d, "ofeasyCam_2dSettings");
    
//    easyCam_3d.setNearClip(-1000000);
//    easyCam_3d.setFarClip(1000000);
//    easyCam_3d.setVFlip(true);
    viewport_3d = ofRectangle(1000,0,600,1200);
    ofxLoadCamera(easyCam_3d, "ofeasyCam_3dSettings");
    
    projectorWidth = 1728; //1920;
    projectorHeight = 1117; //1080;
    
    camWidth =  1280; //1344/2;
    camHeight = 720; //376; //
    params.width = projectorWidth;
    params.height = projectorHeight;
//    encodeGraycode();
    encodeGraycode_simple();
    
    gui_main.setup("cameras", "gui_main.json");
    gui_main.setPosition(20,20);
    gui_main.add(bDebug.set("debug",false));
    uidDropdown_1 = make_unique<ofxDropdown>(selectedUid_1);
    uidDropdown_1->disableMultipleSelection();
    uidDropdown_1->enableCollapseOnSelection();
    //    uidDropdown_2 = make_unique<ofxDropdown>(selectedUid_2);
    //    uidDropdown_2->disableMultipleSelection();
    //    uidDropdown_2->enableCollapseOnSelection();
    
    //get back a list of devices.
    vector<ofVideoDevice> devices = cam1.listDevices();
    
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            uidDropdown_1->add( devices[i].serialID, devices[i].deviceName + " _ " + devices[i].serialID);
            //            uidDropdown_2->add( devices[i].serialID, devices[i].deviceName + " _ " + devices[i].serialID);
        }
    }
    
    
    gui_main.add(uidDropdown_1.get());
    //    gui_main.add(uidDropdown_2.get());
    
    // this goes before loading the gui, so if it actually loads ofApp::selectedCameraChanged will get called.
    uidListener_1 = selectedUid_1.newListener(this, &ofApp::selectedCameraChanged_1);
    //    uidListener_2 = selectedUid_2.newListener(this, &ofApp::selectedCameraChanged_2);
    
//    gui_main.add(bShow_encodeFullscreen.set("encode fullscreen",false));
    gui_main.add(white_thresh.set("thresh white",5,0,255));
    gui_main.add(black_thresh.set("thresh black",40,0,255));
    
    
    if(ofFile::doesFileExist("gui_main.json")){
        gui_main.loadFromFile("gui_main.json");
    }
//    bShow_encodeFullscreen = false;
    
    //    listeners.push(white_thresh.newListener( [&](int&) {
    //        graycode_decode->setWhiteThreshold( white_thresh );
    //    }));
    //    listeners.push(black_thresh.newListener( [&](int&) {
    //        graycode_decode->setBlackThreshold( black_thresh );
    //    }));
    
    
    //	calibration.setFillFrame(true); // true by default
    //	calibration.load("mbp-2011-isight.yml");
    
    //    imitate(undistorted, cam);
    captureStage = -1;
    encode_captureCount = 0;
    decode_done = false;
    
}

//--------------------------------------------------------------
void ofApp::selectedCameraChanged_1(string &){
    cam1.close();
    cam1.setUniqueID(selectedUid_1);
    //    cam_1.setup(camWidth, camHeight);
    cam1.setup(camWidth, camHeight);
    ofLog()<<"cam1 res "<<cam1.getWidth()<<" x "<<cam1.getHeight();
}

//--------------------------------------------------------------
//void ofApp::selectedCameraChanged_2(string &){
//    cam_2.close();
//    cam_2.setUniqueID(selectedUid_2);
//    cam_2.setup(camWidth, camHeight);
//}

void ofApp::update() {
    cam1.update();
    //    cam_2.update();
    if(cam1.isFrameNew()) {
        
        cam1_img.setFromPixels(cam1.getPixels());
//        cam1_img.crop(0,0,cam1.getWidth()/2,cam1.getHeight());
//        cam2_img.setFromPixels(cam1.getPixels());
//        cam2_img.crop(cam1.getWidth()/2,0,cam1.getWidth()/2,cam1.getHeight());
        
//        if(ofGetElapsedTimef() - captureStage_timer > captureStage_duration){
//            captureStage_timer = ofGetElapsedTimef();
//
//            if(captureStage == 0){
//                ofSetFullscreen(true);
//                ofHideCursor();
//            }
//        }
        if(encode_capture == true){
            encode_capture = false;
          
//            Mat tmp;
            
            // Resizing images to avoid issues for high resolution images, visualizing them as grayscale
//            cv::resize( toCv(cam1_img), tmp, cv::Size( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT);
//            cvtColor( tmp, tmp, COLOR_RGB2GRAY );
//            cv::resize(toCv(cam2_img), tmp, cvSize( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT);
//            cvtColor( tmp, tmp, COLOR_RGB2GRAY );
            
            
            
//            if(encode_captureCount < pattern.size()){
            if(captureStage == 0){
                //MARK: in captureStage == 1 show live cam video to help point camera
                captureStage = 1;
//                if(bShow_encodeFullscreen == true){
                    ofSetFullscreen(true);
                    
                    ofHideCursor();
//                }
            }else if(captureStage == 1){
                //MARK: captureStage == 1, in captureStage == 2 show black image
                captureStage = 2;
            }else if(captureStage == 2){
                //MARK: captureStage == 2, capture graycode images via camera
                string name = ofToString(encode_captureCount+1);
                string temp_path = "captures/pattern_cam1_im"+name+".png";
                ofLog()<<"save images "<<temp_path<<" , "<<encode_captureCount;
                capturedImages[0].push_back(ofToDataPath(temp_path,true));
//                capturedImages[1].push_back(ofToDataPath("captures/pattern_cam2_im"+name+".png",true));
                
                cam1_img.save(capturedImages[0].back());
//                cam2_img.save(capturedImages[1].back());
                //            } else if(encode_captureCount == pattern.size()){
                ofLog()<<"capturedImages[0].size "<<capturedImages[0].size()<<" / graycode_patterns.size() "<<graycode_patterns.size();
                if(capturedImages[0].size() >= graycode_patterns.size()){
                    captureStage_timer = ofGetElapsedTimef();
                    captureStage = 3;
                } else {
                    encode_captureCount++;
                }
                
//                encode_captureCount = ofClamp(encode_captureCount,0,graycode_patterns.size()-1);
            }else if(captureStage == 3 && ofGetElapsedTimef() - captureStage_timer > 1){
                //MARK: captureStage == 3, save yml file list
                //If color represents the scene illuminated with a full white projection, it can be used directly as the white image:
                //means i should not need to separatly capture color images, since i can use the white ones
//                capturedImages[0].push_back(ofToDataPath("captures/pattern_cam1_color.png",true));
//                capturedImages[1].push_back(ofToDataPath("captures/pattern_cam2_color.png",true));
                
//                cam1_img.save(capturedImages[0].back());
//                cam2_img.save(capturedImages[1].back());
                // Generate YAML file
                FileStorage fs(ofToDataPath("captured_images.yml",true), FileStorage::WRITE);
                fs << "cam1" << capturedImages[0];
                //                fs << "cam2" << capturedImages[1];
                fs.release();
                
                ofLog()<<"saved captured_images.yml";
                captureStage = 4;
//                if(bShow_encodeFullscreen == true){
                    ofSetFullscreen(false);
                    ofShowCursor();
//                }
                
            } else if(captureStage == 4){
                captureStage = -1;
            }
           
            captureStage_timer = ofGetElapsedTimef();
        }//if(encode_capture == true)
//        ofLog()<<"captureStage "<<captureStage;
    }//end if(cam1.isFrameNew())

}

void ofApp::draw() {
    
    if(captureStage >=0 && captureStage <= 3){
        ofSetColor(0);
        ofFill();
        ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
        
        ofPushMatrix();
        ofTranslate(-graycode_patterns[encode_captureCount].cols/2,-graycode_patterns[encode_captureCount].rows/2);
        ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
        ofSetColor(255);
       
        drawMat(graycode_patterns[encode_captureCount],0,0);
   
        ofSetLineWidth(3);
        ofSetColor(100);
        ofNoFill();
        ofDrawRectangle(1,1,graycode_patterns[encode_captureCount].cols-2,graycode_patterns[encode_captureCount].rows-2);
        ofSetLineWidth(1);
        ofPopMatrix();
        
        if(captureStage == 0 || captureStage == 1){
            ofSetColor(255);
            ofPushMatrix();
            ofTranslate(100,100);
            ofScale(0.5);
            cam1_img.draw(0, 0);
            ofPopMatrix();
        }
        
    } else {
        //MARK: draw 2d
        drawViewportOutline(viewport_2d);
        // keep a copy of your viewport and transform matrices for later
        ofPushView();
        // tell OpenGL to change your viewport. note that your transform matrices will now need setting up
        ofViewport(viewport_2d);
        easyCam_2d.begin();
        ofSetColor(255);
        cam1_img.draw(0, 0);
//        cam2_img.draw(camWidth, 0);
        
        if(hasHomography){
//            cv::Mat imageToProject = toCv(cam1_img);  // Load or prepare the image you want to project
//            cv::Mat warpedImage;
//            cv::warpPerspective(imageToProject, warpedImage, homography, cv::Size(projectorWidth, projectorHeight));
            
//            drawMat(warpedImage,camWidth,0);
            
            drawMat(result_viz,camWidth,0);
        }
        
       
        if(capturedMats.size() > 0){
            ofPushMatrix();
            ofTranslate(0,camHeight);
            ofScale(0.2);
            int cnt = 0;
            for(auto & m : capturedMats){
                drawMat(m,m.cols*(cnt%5),m.rows*int(cnt/5));

                cnt++;
            }
//            cnt = 0;
//            for(auto & m : capturedOfImages){
//                m.draw(cnt*m.getWidth(),0);
//                cnt++;
//            }
            
            ofPopMatrix();
        }
    
        ofSetColor(255);
        drawMat(white_capturedMat,-camWidth,0);
        drawMat(black_capturedMat,-camWidth,white_capturedMat.rows);
        ofSetColor(ofColor::blue);
        ofDrawBitmapString("white graycode", -camWidth+2,25);
        ofDrawBitmapString("black graycode", -camWidth+2,white_capturedMat.rows+25);
        
//        white_capturedImg.draw(-camWidth*2,0);
//        black_capturedImg.draw(-camWidth*2,white_capturedImg.getHeight());
        
        if(captureStage == -1){
//            ofDrawBitmapStringHighlight("use key c to capture frames", camWidth/2,camHeight * 2);
        }else if(captureStage >= 0 && captureStage <=4){
            if(ofInRange(encode_captureCount,0,graycode_patterns.size()-1)) drawMat(graycode_patterns[encode_captureCount],0,camHeight);
        }else if(captureStage == 3){
            ofDrawBitmapStringHighlight("capture done "+ofToString(capturedImages[0].size()), camWidth/2,camHeight * 2);
        }
        
        if(decode_done == true){
           if(cm_disp_img.isAllocated()) cm_disp_img.draw(0,-camHeight);
            if(disp_otsu_img.isAllocated()) disp_otsu_img.draw(camWidth,-camHeight);
        }
        
        
        easyCam_2d.end();
        ofPopView();
        
        //MARK: draw 3d
        drawViewportOutline(viewport_3d);
        // keep a copy of your viewport and transform matrices for later
        ofPushView();
        // tell OpenGL to change your viewport. note that your transform matrices will now need setting up
        ofViewport(viewport_3d);
        easyCam_3d.begin();
        ofSetColor(255);
        if(pointCloud_mesh.getNumVertices() > 0) pointCloud_mesh.drawVertices();
        if(pointCloud_mesh_zeored.getNumVertices() > 0) pointCloud_mesh_zeored.drawVertices();
        ofSetColor(ofColor::blue);
        boundingBox.drawWireframe();
        //    ofDrawAxis(10);
        ofDrawGrid(400);
        easyCam_3d.end();
        ofPopView();
        
        //	undistorted.draw(640, 0);
        
        //TODO: draw pattern fullscreen on projector
     
            
        stringstream tt;
        tt<<"press key n to enter first stage of capture"<<endl;
        tt<<"press key c to capture all 44 graycodes one by one"<<endl;
        tt<<"press key 1 to decode"<<endl;
        
        ofDrawBitmapStringHighlight(tt.str(), ofGetWidth()/2, 100);
    }
    
    if(bDebug == true){
        stringstream ss;
        ss<<"captureStage "<<captureStage<<endl;
        ss<<"encode_captureCount: "<<encode_captureCount<<" / "<<graycode_patterns.size()<<endl;
//        if(captureStage == 0)
        ss<<"dim: "<<graycode_patterns[encode_captureCount].cols<<" , "<<graycode_patterns[encode_captureCount].rows<<endl;
        ss<<"since last capture: "<<(ofGetElapsedTimef() - captureStage_timer)<<endl;
        ofDrawBitmapString(ss.str(), ofGetWidth()-300,10);

    }
    
    if(bShowGui == true){
        gui_main.draw();
    }
   
}

bool ofApp::encodeGraycode_simple(){
    
    ofLog()<<"encodeGraycode_simple()";
    
    cv::structured_light::GrayCodePattern::Params params;
    params.width = GRAYCODEWIDTH;
    params.height = GRAYCODEHEIGHT;
    auto pattern = cv::structured_light::GrayCodePattern::create(params);
    
    // 用途:decode時にpositiveとnegativeの画素値の差が
    //      常にwhiteThreshold以上である画素のみdecodeする
    pattern->setWhiteThreshold(white_thresh);
    // 用途:ShadowMask計算時に white - black > blackThreshold
    //      ならば前景（グレイコードを認識した）と判別する
    // 今回はこれを設定しても参照されることはないが一応セットしておく
    pattern->setBlackThreshold(black_thresh);
    
    graycode_patterns.clear();
    pattern->generate(graycode_patterns);
    
    cv::Mat blackCode, whiteCode;
    pattern->getImagesForShadowMasks(blackCode, whiteCode);
    graycode_patterns.push_back(blackCode);
    graycode_patterns.push_back(whiteCode);
    
    ofLog()<<"graycode_patterns.size "<<graycode_patterns.size();
    capturedImages.resize(2);
    encode_captureCount = 0;
    captureStage = 0;
    
    return true;
}

bool ofApp::encodeGraycode(){
    //https://docs.opencv.org/4.x/db/d56/tutorial_capture_graycode_pattern.html
   
    // Set up GraycodePattern with params
    cv::Ptr<structured_light::GrayCodePattern> graycode_encode = structured_light::GrayCodePattern::create( params );
    // Storage for pattern
    graycode_encode->generate( graycode_patterns );
    cout << graycode_patterns.size() << " pattern images + 2 images for shadows mask computation to acquire with both cameras"<< endl;
    
    // Generate the all-white and all-black images needed for shadows mask computation
    Mat whiteCode;
    Mat blackCode;
    graycode_encode->getImagesForShadowMasks( blackCode, whiteCode );
    
    graycode_patterns.push_back( whiteCode );
    graycode_patterns.push_back( blackCode );
    
    capturedImages.resize(2);
    encode_captureCount = 0;
    captureStage = 0;
//    patternCounter = 0;
    return true;
}

bool ofApp::decodeGraycode_simple(){
    ofLog()<<"decodeGraycode_simple()";
//https://github.com/punpongsanon/graycode-procam-calibration/blob/master/code/procam-calibration-with-graycode/main.cpp
      vector<string> imagelist;
      string temp_path = ofToDataPath("captured_images.yml", true);
      bool ok = readStringList_oneCam(temp_path, imagelist);
      ofLog()<<"imagelist.size() " << imagelist.size();
      if(!ok || imagelist.empty()) {
          cout << "Cannot open " << temp_path << " or the string list is empty. ok " << (ok ? "is ok" : "is not ok") << endl;
          return false;
      }

  

      capturedMats.clear();
    capturedOfImages.clear();
    
    ofImage temp_img;
    for(size_t i = 0; i < imagelist.size(); i++) {
        string path = imagelist[i];
                  ofLog() << i << ", path " << path;
        if (!temp_img.load(path)) {
            ofLog() << "Failed to load image at: " << path;
            return false;  // Return false if any image fails to load
        }
        temp_img.setImageType(OF_IMAGE_GRAYSCALE);
        capturedOfImages.push_back(temp_img);
//        capturedOfImages.back().update();
        cv::Mat tempMat = toCv(temp_img);      // Convert to cv::Mat
        capturedMats.push_back(tempMat.clone());  // Clone the data to ensure it's independent

        if (capturedMats.back().empty()) {
            cout << "Image at index " << i << " is empty or not loaded properly." << endl;
            return false;
        }
    }
    
    ofLog()<<"capturedMats.size "<<capturedMats.size();
    
    cv::structured_light::GrayCodePattern::Params params;
    params.width = GRAYCODEWIDTH;
    params.height = GRAYCODEHEIGHT;

    auto pattern = cv::structured_light::GrayCodePattern::create(params);
    pattern->setWhiteThreshold(white_thresh);
    // 用途:ShadowMask計算時に white - black > blackThreshold
    //      ならば前景（グレイコードを認識した）と判別する
    // 今回はこれを設定しても参照されることはないが一応セットしておく
    pattern->setBlackThreshold(black_thresh);
    
    int capHeight = capturedMats[0].rows;
    int capWidth = capturedMats[0].cols;
    ofLog()<<"capWidth "<<capWidth<<" capHeight "<<capHeight;
    
    white_capturedMat = capturedMats[capturedMats.size()-1].clone();// capturedMats.back();
    toOf(white_capturedMat,white_capturedImg);
    white_capturedImg.update();
//    capturedMats.pop_back();
    
    black_capturedMat = capturedMats[capturedMats.size()-2].clone(); //capturedMats.back();
    toOf(black_capturedMat,black_capturedImg);
    black_capturedImg.update();
//    capturedMats.pop_back();
    
    ofLog()<<"capturedMats.size "<<capturedMats.size();
    
    c2pX = cv::Mat::zeros(camHeight, capWidth, CV_16U);
    c2pY = cv::Mat::zeros(camHeight, capWidth, CV_16U);
    std::vector<C2P> c2pList;
    int cnt = 0;
    for (int y = 0; y < capHeight; y++) {
        for (int x = 0; x < capWidth; x++) {
            cv::Point pixel;
            if(int(white_capturedMat.at<cv::uint8_t>(y, x) - black_capturedMat.at<cv::uint8_t>(y, x)) >= black_thresh){
                if(cnt % 1000 == 0){
                    cout
                    <<"x "<<x<<" x "<<y
                    <<", white "<<int(white_capturedMat.at<cv::uint8_t>(y, x))
                    <<" - black "<<int(black_capturedMat.at<cv::uint8_t>(y, x))
                    <<" = "<<int(white_capturedMat.at<cv::uint8_t>(y, x) - black_capturedMat.at<cv::uint8_t>(y, x))
                    <<" > black_thresh "<<black_thresh<<endl;
                }
                cnt++;
            }
            if (white_capturedMat.at<cv::uint8_t>(y, x) - black_capturedMat.at<cv::uint8_t>(y, x) > black_thresh &&  !pattern->getProjPixel(capturedMats, x, y, pixel))
            {
                c2pX.at<cv::uint16_t>(y, x) = pixel.x;
                c2pY.at<cv::uint16_t>(y, x) = pixel.y;
                c2pList.push_back(C2P(x, y, pixel.x * GRAYCODEWIDTHSTEP, pixel.y * GRAYCODEHEIGHTSTEP));
            }
        }
    }
    ofLog()<<"above threshold "<<cnt <<" / "<<(capHeight*capWidth)<<" == "<<ofMap(cnt,0,(capHeight*capWidth),0,100)<<" %";
    ofLog()<<"c2pList "<<c2pList.size()<<" == "<<ofMap(c2pList.size(),0,(capHeight*capWidth),0,100)<<" %";
    std::ofstream os(ofToDataPath("c2p.csv"));
    for (auto elem : c2pList) {
        os << elem.cx << ", " << elem.cy << ", " << elem.px << ", " << elem.py
        << std::endl;
    }
    os.close();
    
    result_viz = cv::Mat::zeros(camHeight, camWidth, CV_8UC3);
    for (int y = 0; y < camHeight; y++) {
        for (int x = 0; x < camWidth; x++) {
            result_viz.at<cv::Vec3b>(y, x)[0] = (unsigned char)c2pX.at<cv::uint16_t>(y, x);
            result_viz.at<cv::Vec3b>(y, x)[1] = (unsigned char)c2pY.at<cv::uint16_t>(y, x);
        }
    }
    
    hasHomography = true;
    
    //
    //      ofLog() << "capturedMats[0].size() " << capturedMats[0].size();
    //
    //      cv::Mat disparityMap, mask;
////      bool decoded = graycode_decode->decode(capturedMats, disparityMap, mask); // Ensure the correct vector of Mats is passed
//
//    bool decoded = false;
//    if (graycode_decode && !capturedMats.empty() && !capturedMats[0].empty()) {
//        cout << "Starting decode with " << capturedMats[0].size() << " images." << endl;
//        try {
//            bool decoded = graycode_decode->decode(capturedMats, disparityMap, mask);
//            if (decoded) {
//                cout << "Decoding successful." << endl;
//                decoded = true;
//            } else {
//                cout << "Decoding failed." << endl;
//            }
//        } catch (const std::exception& e) {
//            cout << "Exception during decoding: " << e.what() << endl;
//        }
//    } else {
//        cout << "Decoding setup is not correct." << endl;
//    }
    bool decoded = false;
    if( decoded ) {
        cout << endl << "pattern decoded" << endl;
        /*
        // To better visualize the result, apply a colormap to the computed disparity
        double min;
        double max;
        minMaxIdx(disparityMap, &min, &max);
        Mat cm_disp, scaledDisparityMap;
        cout << "disp min " << min << endl << "disp max " << max << endl;
        convertScaleAbs( disparityMap, scaledDisparityMap, 255 / ( max - min ) );
        applyColorMap( scaledDisparityMap, cm_disp, COLORMAP_JET );
        
        // Show the result
        cv::resize( cm_disp, cm_disp, cv::Size( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT );
        //    imshow( "cm disparity m", cm_disp );
        toOf(cm_disp,cm_disp_img);
        cm_disp_img.update();
        ofLog()<<"ofSaveImage cm_disp_img "<<ofSaveImage(cm_disp_img.getPixels(),ofToDataPath("cm_disp_img.png"));
        
        // Compute a mask to remove background
        Mat dst, thresholded_disp;
        threshold( scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY );
        cv::resize( thresholded_disp, dst, cv::Size( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT );
        //    imshow( "threshold disp otsu", dst );
        toOf(dst,disp_otsu_img);
        disp_otsu_img.update();
        ofLog()<<"ofSaveImage disp_otsu_img "<<ofSaveImage(disp_otsu_img.getPixels(),ofToDataPath("disp_otsu_img.png"));

        
        std::vector<cv::Point2f> cameraPoints, projectorPoints;
        for (int y = 0; y < disparityMap.rows; ++y) {
            for (int x = 0; x < disparityMap.cols; ++x) {
                float disparity = disparityMap.at<float>(y, x);
                if (disparity >= 0 && mask.at<uchar>(y, x) == 255) {
                    cameraPoints.emplace_back(x, y);
                    projectorPoints.emplace_back(x + disparity, y);
                }
            }
        }

        cv::Mat homography = cv::findHomography(cameraPoints, projectorPoints);
        hasHomography = true;
         */
        return true;
    } else {
        
        return false;
    }
}

bool ofApp::decodeGraycode(){
    //https://docs.opencv.org/4.x/dc/da9/tutorial_decode_graycode_pattern.html
    
    graycode_decode = structured_light::GrayCodePattern::create( params );
    
    graycode_decode->setWhiteThreshold( white_thresh );
    graycode_decode->setBlackThreshold( black_thresh );
    
    vector<string> imagelist;
    //    ofDirectory capture_dir;
    //    capture_dir.allowExt("jpg");
    //    capture_dir.listDir(ofToDataPath("captures"));
    //    capture_dir.sort();
    //    for(int i=0; i<capture_dir.size(); i++){
    //        imagelist.push_back();
    //    }
    string temp_path = ofToDataPath("captured_images.yml",true);
    bool ok = readStringList_twoCams(temp_path,imagelist);
    ofLog()<<"imagelist.size() "<<imagelist.size();
    if( !ok || imagelist.empty() ) {
        cout << "can not open " << temp_path << " or the string list is empty ok "<<(ok ? "is ok":"is not ok")<< endl;
        //     help();
        return false;
    }
    
    cv::FileStorage fs(ofToDataPath("stereo_calibration.yml",true), cv::FileStorage::READ);
    if( !fs.isOpened() ){
        cout << "Failed to open Calibration Data File." << endl;
        return false;
    }
    Mat cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, R, T;
    fs["cam1_intrinsics"] >> cam1intrinsics;
    fs["cam1_distorsion"] >> cam1distCoeffs;
    fs["cam2_intrinsics"] >> cam2intrinsics;
    fs["cam2_distorsion"] >> cam2distCoeffs;
    fs["R"] >> R;
    fs["T"] >> T;
   
    fs.release();
    
    cout << "cam1intrinsics" << endl << cam1intrinsics << endl;
    cout << "cam1distCoeffs" << endl << cam1distCoeffs << endl;
    cout << "cam2intrinsics" << endl << cam2intrinsics << endl;
    cout << "cam2distCoeffs" << endl << cam2distCoeffs << endl;
    cout << "T" << endl << T << endl << "R" << endl << R << endl;
    
    if (!R.data || !T.data || !cam1intrinsics.data || !cam2intrinsics.data || !cam1distCoeffs.data || !cam2distCoeffs.data) {
        ofLogError() << "Failed to load cameras calibration parameters";
        return false;
    }
    
    //returns the number of pattern images needed for the Gray Code pattern based on the specified width and height of the projection.
    size_t numberOfPatternImages = graycode_decode->getNumberOfPatternImages();
    ofLog()<<"numberOfPatternImages "<<numberOfPatternImages;
    vector<vector<Mat> > captured_pattern;
    captured_pattern.resize( 2 );
    captured_pattern[0].resize( numberOfPatternImages );
    captured_pattern[1].resize( numberOfPatternImages );
    
    
    // loads a reference color image taken after the Gray Code pattern images.
    //read a reference color image from a list of image filenames (imagelist). The reference image is often taken after the series of structured light images and serves as a normal color image of the scene.
    ofImage temp_img;
    temp_img.load(imagelist[numberOfPatternImages]);
    ofLog()<<"ofSaveImage whiteImages[0]. "<<ofSaveImage(temp_img.getPixels(), ofToDataPath("whiteImages[0].png"));
    ofLog()<<"imagelist[numberOfPatternImages] "<<imagelist[numberOfPatternImages];
    ofLog()<<(numberOfPatternImages)<<" imagelist.size "<<imagelist.size();
    Mat color = toCv(temp_img);
//    Mat color = imread( imagelist[numberOfPatternImages], cv::IMREAD_COLOR );
    cv::Size imagesSize = color.size();
  
    // Stereo rectify
    cout << "Rectifying images..." << endl;
    Mat R1, R2, P1, P2, Q;
    cv::Rect validRoi[2];
    stereoRectify( cam1intrinsics, cam1distCoeffs, cam2intrinsics, cam2distCoeffs, imagesSize, R, T, R1, R2, P1, P2, Q, 0,
                  -1, imagesSize, &validRoi[0], &validRoi[1] );

    Mat map1x, map1y, map2x, map2y;
    initUndistortRectifyMap( cam1intrinsics, cam1distCoeffs, R1, P1, imagesSize, CV_32FC1, map1x, map1y );
    initUndistortRectifyMap( cam2intrinsics, cam2distCoeffs, R2, P2, imagesSize, CV_32FC1, map2x, map2y );
    
    // Loading pattern images
    for( size_t i = 0; i < numberOfPatternImages; i++ )
    {
//        captured_pattern[0][i] = imread( imagelist[i], IMREAD_GRAYSCALE );
//        captured_pattern[1][i] = imread( imagelist[i + numberOfPatternImages + 2], IMREAD_GRAYSCALE );
    
        temp_img.load(imagelist[i]);
        captured_pattern[0][i] = toCv(temp_img);
        temp_img.load(imagelist[i+ (numberOfPatternImages + 2)]);
        captured_pattern[1][i] = toCv(temp_img);
        
        if( (!captured_pattern[0][i].data) || (!captured_pattern[1][i].data) )
        {
            cout << "Empty images" << endl;
            return false;
        }
        
        remap( captured_pattern[1][i], captured_pattern[1][i], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
        remap( captured_pattern[0][i], captured_pattern[0][i], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
        
    }
    
    cout << "done" << endl;
    
    vector<Mat> blackImages;
    vector<Mat> whiteImages;
    
    blackImages.resize( 2 );
    whiteImages.resize( 2 );
    
    // Loading images (all white + all black) needed for shadows computation
    //imagelist[numberOfPatternImages]
    cvtColor( color, whiteImages[0], COLOR_RGB2GRAY );
    
//    whiteImages[1] = imread( imagelist[2 * numberOfPatternImages + 2], IMREAD_GRAYSCALE );
//    blackImages[0] = imread( imagelist[numberOfPatternImages + 1], IMREAD_GRAYSCALE );
//    blackImages[1] = imread( imagelist[2 * numberOfPatternImages + 2 + 1], IMREAD_GRAYSCALE );
    
    ofLog()<<"load whiteImages[1] "<< temp_img.load(imagelist[2 * numberOfPatternImages + 2]);
    ofLog()<<"imagelist[2 * numberOfPatternImages + 2] "<<imagelist[2 * numberOfPatternImages + 2];
    ofLog()<<(2 * numberOfPatternImages + 2)<<" imagelist.size "<<imagelist.size();
    ofSaveImage(temp_img.getPixels(), ofToDataPath("whiteImages[1].png"));
    whiteImages[1] = toCv(temp_img);
    
    ofLog()<<"load blackImages[0] "<<temp_img.load(imagelist[numberOfPatternImages + 1]);
    ofLog()<<"imagelist[numberOfPatternImages + 1 "<<imagelist[numberOfPatternImages + 1];
    ofLog()<<(numberOfPatternImages + 1);
    ofSaveImage(temp_img.getPixels(), ofToDataPath("blackImages[0].png"));
    blackImages[0] = toCv(temp_img);
    
    ofLog()<<"load blackImages[1] "<<temp_img.load(imagelist[2 * numberOfPatternImages + 2 + 1]); //numberOfPatternImages plus a black and a white
    ofLog()<<"imagelist[2 * numberOfPatternImages + 2 + 1] "<<imagelist[2 * numberOfPatternImages + 2 + 1];
    ofLog()<<(2 * numberOfPatternImages + 2 + 1);
    ofSaveImage(temp_img.getPixels(), ofToDataPath("blackImages[1].png"));
    blackImages[1] = toCv(temp_img);

    remap( color, color, map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
    
    remap( whiteImages[0], whiteImages[0], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
    remap( whiteImages[1], whiteImages[1], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
    
    remap( blackImages[0], blackImages[0], map2x, map2y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
    remap( blackImages[1], blackImages[1], map1x, map1y, INTER_NEAREST, BORDER_CONSTANT, Scalar() );
    
    cout << endl << "Decoding pattern ..." << endl;
    Mat disparityMap;
    bool decoded = graycode_decode->decode( captured_pattern, disparityMap, blackImages, whiteImages,
                                    structured_light::DECODE_3D_UNDERWORLD );
    
   
    if( decoded ) {
        cout << endl << "pattern decoded" << endl;
        
        // To better visualize the result, apply a colormap to the computed disparity
        double min;
        double max;
        minMaxIdx(disparityMap, &min, &max);
        Mat cm_disp, scaledDisparityMap;
        cout << "disp min " << min << endl << "disp max " << max << endl;
        convertScaleAbs( disparityMap, scaledDisparityMap, 255 / ( max - min ) );
        applyColorMap( scaledDisparityMap, cm_disp, COLORMAP_JET );
        
        // Show the result
        cv::resize( cm_disp, cm_disp, cv::Size( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT );
        //    imshow( "cm disparity m", cm_disp );
        toOf(cm_disp,cm_disp_img);
        cm_disp_img.update();
        ofLog()<<"ofSaveImage cm_disp_img "<<ofSaveImage(cm_disp_img.getPixels(),ofToDataPath("cm_disp_img.png"));
        
        // Compute the point cloud
        Mat pointcloud;
        disparityMap.convertTo( disparityMap, CV_32FC1 );
        reprojectImageTo3D( disparityMap, pointcloud, Q, true, -1 );
        
        
        pointCloudToOFMesh(pointcloud, pointCloud_mesh); //, colorImage); // Optionally provide a color image
        ofLog()<<"pointCloud_mesh "<<pointCloud_mesh.getNumVertices();
        boundingBox = getExtremes(pointCloud_mesh);
        
        pointCloud_mesh_zeored.clear();
        float midX = (mesh_maxX - mesh_minX) / 2.0;
        float midY = (mesh_maxY - mesh_minY) / 2.0;
        float midZ = (mesh_maxZ - mesh_minZ) / 2.0;
        for(auto & v : pointCloud_mesh.getVertices()){
            pointCloud_mesh_zeored.addVertex(ofVec3f(v.x - mesh_maxX + midX, v.y - mesh_maxY + midY, v.z - mesh_maxZ + midZ));
            pointCloud_mesh_zeored.addColor(ofColor::white);
        }
        // Compute a mask to remove background
        Mat dst, thresholded_disp;
        threshold( scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY );
        cv::resize( thresholded_disp, dst, cv::Size( camWidth, camHeight ), 0, 0, INTER_LINEAR_EXACT );
        //    imshow( "threshold disp otsu", dst );
        toOf(dst,disp_otsu_img);
        disp_otsu_img.update();
        ofLog()<<"ofSaveImage disp_otsu_img "<<ofSaveImage(disp_otsu_img.getPixels(),ofToDataPath("disp_otsu_img.png"));

    
        return true;
    }
  
    return true;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'g'){
        bShowGui = !bShowGui;
        if(bShowGui == false){
            gui_main.saveToFile("gui_main.json");
            ofxSaveCamera(easyCam_2d, "ofeasyCam_2dSettings");
            ofxSaveCamera(easyCam_3d, "ofeasyCam_3Settings");
        }
    }
}


void ofApp::keyReleased(int key){
    if(key == 'c'){
//        patternCounter++;
//        patternCounter = ofClamp(patternCounter,0,pattern.size());
//        if(patternCounter >= pattern.size()){
//            ofLog()<<"patternCounter >= pattern.size() "<<pattern.size();
//
//            patternDone = true;
//        } else {
//
//        }
//        captureStage_timer = ofGetElapsedTimef();
//        captureStage_duration = 0;
        encode_capture = true;
    }
    if(key == 'n'){
        bShowGui = false;
        captureStage = 0;
        encode_captureCount = 0;
        ofSetFullscreen(true);
    }
    
//    if(key == 'd') decode_done = decodeGraycode();
    if(key == '1') decode_done = decodeGraycode_simple();
    if(key == 'f') ofToggleFullscreen();
    //    if(key == 'c'){
    //
    //    }
}

bool ofApp::readStringList_oneCam( string filename, vector<string>& l )
{
    ofLog()<<"readStringList() "<<filename;
    l.resize( 0 );
    FileStorage fs( filename, FileStorage::READ );
    if( !fs.isOpened() )
    {
        cerr << "failed to open " << filename << endl;
        return false;
    }
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
    {
        cerr << "cam 1 images are not a sequence! FAIL" << endl;
        return false;
    }
    
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
    {
        l.push_back( ( string ) *it );
    }
    
    return true;
}

bool ofApp::readStringList_twoCams( string filename, vector<string>& l )
{
    ofLog()<<"readStringList() "<<filename;
    l.resize( 0 );
    FileStorage fs( filename, FileStorage::READ );
    if( !fs.isOpened() )
    {
        cerr << "failed to open " << filename << endl;
        return false;
    }
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
    {
        cerr << "cam 1 images are not a sequence! FAIL" << endl;
        return false;
    }
    
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
    {
        l.push_back( ( string ) *it );
    }
    
    n = fs["cam2"];
    if( n.type() != FileNode::SEQ )
    {
        cerr << "cam 2 images are not a sequence! FAIL" << endl;
        return false;
    }
    
    it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
    {
        l.push_back( ( string ) *it );
    }
    
    if( l.size() % 2 != 0 )
    {
        cout << "Error: the image list contains odd (non-even) number of elements\n";
        return false;
    }
    return true;
}

//--------------------------------------------------------------
void ofApp::drawViewportOutline(const ofRectangle & viewport){
    ofPushStyle();
    ofFill();
    ofSetColor(50);
    ofSetLineWidth(0);
    ofDrawRectangle(viewport);
    ofNoFill();
    ofSetColor(25);
    ofSetLineWidth(1.0f);
    ofDrawRectangle(viewport);
    ofPopStyle();
}
