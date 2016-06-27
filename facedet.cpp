#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <math.h>
#include <string>

using namespace dlib;
using namespace std;

/*
    Algorithm:
    1. Detect the bounding box of image, i.e., the face.
    2. save sub-image containing the bounding box.
    3. The bounding box by itself is a 200 x 200 image.
    4. Detect facial features in new image.
    5. Extract distance in terms of pixels.
*/

class FaceDet
{
    
    frontal_face_detector detector;
    shape_predictor sp;
    image_window win;
    array2d<rgb_pixel> img;
    std::vector<rectangle> dets;
    std::vector<full_object_detection> shapes;

    unsigned long eyeDist;
    unsigned long eyeSize;
    unsigned long leftEyeEar;
    unsigned long rightEyeEar;
    unsigned long noseHeight;
    unsigned long noseWidth;
    unsigned long noseSize;
    unsigned long foreheadWidth;
    unsigned long lipLength;
    unsigned long lipWidth;
    unsigned long lipUpWidth;
    unsigned long lipDownWidth;
    unsigned long eyeEyebrow;
    string lowerOrupper;
    dlib::vector<double, 2> leftEyeTilt, rightEyeTilt, eyeTilt;

    double driver, amiable, analytical, expressive;

    public:

        FaceDet(array2d<rgb_pixel> &img)
        {
            /*
                Initialize all objects..
            */
            
            //A frontal face detector to detect faces..
            detector = get_frontal_face_detector();

            //Facial landmarks predictor which is pre-trained..
            deserialize ("shape_predictor_68_face_landmarks.dat") >> sp;

            win.clear_overlay();
            assign_image(this->img, img);
            
            //Detect the bounding boxes, i.e., faces in the image and assign it to dets[]..
            dets = detector(img);
        }

        void SetPersonality()
        {
            int dCount = 0, eCount = 0, amCount = 0, anCount = 0, total = 0;

            int FWavg = 112, ESavg = 291, ENSavg = 80, eyeTiltX = 31, eyeTiltY = -11, EBSet = 23, NSavg = 1244, NWavg = 41, LTavg = 11;
            int NHavg = 62;

            cout<<"\nCharacteristics: \n";

            if(foreheadWidth > FWavg + 3)
            {
                eCount++;
                dCount++;
                cout<<"\nWide forehead.";
            }
            else if(foreheadWidth <= FWavg + 3 && foreheadWidth >= FWavg - 3)
            {
                anCount++;
                cout<<"\nAverage forehead width.";
            }

            if(eyeSize > ESavg + 10)
            {
                eCount++;
                cout<<"\nLarge eyes.";
            }
            else if(eyeSize < ESavg)
            {
                anCount++;
                cout<<"\nSmall eyes.";
            }

            if(eyeDist > 80)
            {
                amCount++;
                cout<<"\nEyes widely set.";
            }
            else
            {
                anCount++;
                cout<<"\nEyes closely set";
            }

            if(eyeTilt.y() < eyeTiltY)
            {
                amCount++;
                eCount++;
                cout<<"\nCurved eyebrows.";
            }
            else
            {
                dCount++;
                anCount++;
                cout<<"\nStraight eyebrows";
            }

            if(eyeEyebrow > EBSet)
            {
                anCount++;
                cout<<"\nHigh set eyebrows.";
            }
            else
            {
                dCount++;
                cout<<"\nLow set eyebrows.";
            }

            if(noseSize > NSavg)
            {
                dCount++;
                cout<<"\nLarge nose.";
            }
            else
            {
                amCount++;
                cout<<"\nSmall nose.";
            }

            if(noseWidth > NWavg)
            {
                eCount++;
                cout<<"\nWide nose.";
            }
            else
            {
                dCount++;
                cout<<"\nNarrow nose.";
            }

            if(noseHeight > NHavg)
            {
                dCount++;
                cout<<"\nLong nose.";
            }
            else
            {
                eCount++;
                cout<<"\nShort nose.";
            }

            if(lipWidth > LTavg)
            {
                eCount++;
                cout<<"\nThick lips.";
            }
            else
            {
                anCount++;
                cout<<"\nThin lips."<<endl;
            }

            if(lowerOrupper == "lower")
                amCount++;
            else if(lowerOrupper == "equal")
                amCount++;

            total = dCount + amCount + anCount + eCount;

            float driverPerc = (((float) dCount * 100) / total);
            float amiablePerc = (((float) amCount * 100) / total);
            float analyticalPerc = (((float) anCount * 100) / total);
            float expressivePerc = (((float) eCount * 100) / total);

            cout<<"\nPersonality Traits: ";
            cout<<"\nDriver    : "<<driverPerc;
            cout<<"\nExpressive: "<<expressivePerc;
            cout<<"\nAnalytical: "<<analyticalPerc;
            cout<<"\nAmiable   : "<<amiablePerc<<endl; 

        }

        void calculate(full_object_detection shape, rectangle dets)
        {
            dlib::vector<double,2> l, r, one, two, three;
            double cnt = 0;
            unsigned long s, t, s1, t1, s2;

            //Area covered by the face..
            cout<<"\nThe bounding box coordinates: ";
            cout<<"\nTop left:     "<<dets.tl_corner();
            cout<<"\nTop right:    "<<dets.tr_corner();
            cout<<"\nBottom left:  "<<dets.bl_corner();
            cout<<"\nBottom right: "<<dets.br_corner();
            cout<<endl;
            cout<<"\nNumber of parts: "<<shape.num_parts()<<endl;
           
            //Width of forehead..
            r = shape.part(45) - shape.part(36);

            foreheadWidth = pow(r.x(), 2) + pow(r.y(), 2);
            foreheadWidth = sqrt(foreheadWidth);
            cout<<"\nThe width of the forehead is: "<<foreheadWidth<<endl;

            //size of eye..
            r = shape.part(39) - shape.part(36);
            l = shape.part(45) - shape.part(42);
            
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            t = sqrt(t);
            
            one = shape.part(41) - shape.part(37);
            two = shape.part(47) - shape.part(43);
            
            s1 = pow(one.x(), 2) + pow(one.y(), 2);
            s1 = sqrt(s1);
            t1 = pow(two.x(), 2) + pow(two.y(), 2);
            t1 = sqrt(t1);
            
            s = s * s1;
            t = t * t1;

            eyeSize = (s + t)/2;
            cout<<"\nThe eye size is: "<<eyeSize;

            //detecting distance between eyes..
            for (unsigned long j = 36; j <= 41; ++j) 
            {
                l += shape.part(j);
                ++cnt;
            }
            l /= cnt;
            cnt = 0;
            for (unsigned long j = 42; j <= 47; ++j) 
            {
                r += shape.part(j);
                ++cnt;
            }
            r /= cnt;
            
            eyeDist = pow((l.x() - r.x()), 2) + pow((l.y() - r.y()),2);
            eyeDist = sqrt(eyeDist);
            cout<<"\nThe distance between the eyes is: "<<eyeDist<<endl;
            
            //right eyebrow : 17 - 21
            //left  eyebrow : 22 - 26
            rightEyeTilt = shape.part(19) - shape.part(17);
            leftEyeTilt = shape.part(24) - shape.part(22);
            cout<<"\nThe right eyebrow tilt: "<<rightEyeTilt;
            cout<<"\nThe left eyebrow tilt: "<<leftEyeTilt;
            eyeTilt = (leftEyeTilt + rightEyeTilt) / 2;
            cout<<"\nThe average eye tilt is: "<<eyeTilt;

            //eyebrow low set or high set..
            r = shape.part(37) - shape.part(19);
            l = shape.part(43) - shape.part(23);
            
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            t = sqrt(t);

            eyeEyebrow = (s + t)/2;
            cout<<"\nthe distance between eyebrow and eye is: "<<eyeEyebrow<<endl;
            
            //height of nose
            r = shape.part(33) - shape.part(27);
            
            noseHeight = pow(r.x(),2) + pow(r.y(),2);
            noseHeight = sqrt(noseHeight);
            cout<<"\nThe height of the nose is: "<<noseHeight;

            //Nose size..
            r = shape.part(33) - shape.part(27);
            l = shape.part(35) - shape.part(31);
            
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            t = sqrt(t);
            
            noseSize = 0.5 * t * s;
            noseWidth = t;
            cout<<"\nThe size of the nose is: "<<noseSize;
            cout<<"\nThe width of the nose is: "<<noseWidth<<endl;

            //Length of lips
            r = shape.part(54) - shape.part(48);
            
            lipLength = pow(r.x(),2) + pow(r.y(),2);
            lipLength = sqrt(lipLength);
            cout<<"\nThe Length of lip is: "<<lipLength;
       
            //lower and upper lip comparison
            r = shape.part(61) - shape.part(49);
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            
            r = shape.part(61) - shape.part(50);
            t = pow(r.x(), 2) + pow(r.y(), 2);
            t = sqrt(t);
            
            r = shape.part(62) - shape.part(51);
            s1 = pow(r.x(), 2) + pow(r.y(), 2);
            s1 = sqrt(s1);
            
            r = shape.part(63) - shape.part(52);
            t1 = pow(r.x(), 2) + pow(r.y(), 2);
            t1 = sqrt(t1);
            
            r = shape.part(63) - shape.part(53);
            s2 = pow(r.x(), 2) + pow(r.y(), 2);
            s2 = sqrt(s2);

            lipUpWidth = s + t + s1 + s2 + t1;
            lipUpWidth /= 5;
            
            r = shape.part(65) - shape.part(55);
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            
            r = shape.part(65) - shape.part(56);
            t = pow(r.x(), 2) + pow(r.y(), 2);
            t = sqrt(t);
            
            r = shape.part(66) - shape.part(57);
            s1 = pow(r.x(), 2) + pow(r.y(), 2);
            s1 = sqrt(s1);
            
            r = shape.part(67) - shape.part(58);
            t1 = pow(r.x(), 2) + pow(r.y(), 2);
            t1 = sqrt(t1);
            
            r = shape.part(67) - shape.part(59);
            s2 = pow(r.x(), 2) + pow(r.y(), 2);
            s2 = sqrt(s2);
            
            lipDownWidth = s + t + s1 + s2 + t1;
            lipDownWidth /= 5;
            
            if(lipUpWidth > lipDownWidth)
            {
                cout<<"\nUpper lip is wider.";
                lowerOrupper = "upper";
            }    
            else if(lipDownWidth > lipUpWidth)
            {
                cout<<"\nLower lip is wider.";
                lowerOrupper = "lower";
            }
            else
            {
                lowerOrupper = "equal";
                cout<<"\nEqual thickness of upper and lower lip.";
            }
            
            lipWidth = lipUpWidth + lipDownWidth;
            lipWidth /= 2;

            cout<<"\nThe width of the lip is: "<<lipWidth<<endl;

            //distance between eyes and ears
            r = shape.part(36) - shape.part(0);
            l = shape.part(45) - shape.part(16);

            rightEyeEar = pow(r.x(), 2) + pow(r.y(), 2);
            leftEyeEar = pow(l.x(), 2) + pow(l.y(), 2);
            rightEyeEar = sqrt(rightEyeEar);
            leftEyeEar = sqrt(leftEyeEar);
            cout<<"\nThe approximate distance between left ear and left eye: "<<leftEyeEar;
            cout<<"\nThe approximate distance between right ear and right eye: "<<rightEyeEar;
            cout<<"\nThe average distance between ears and eyes is: "<<(leftEyeEar + rightEyeEar)/2<<endl;

            SetPersonality();

        }

        void DetectLandmarks()
        {
            
            //For each face detected..
            for(unsigned long i = 0 ; i < dets.size() ; ++i)
            {
                //Pass the image and one face's bounding box to the shape predictor dat file..
                //It returns the shape of the face along with 68 point facial feature coordinates..
                full_object_detection shape = sp(img, dets[i]);
                //Store the shape coordinates in a vector(in case there are more than one face)..
                shapes.push_back(shape);            
            }
        
            //Extract the face chips from the image, i.e., the bounding box and identified features. Store it in a 2d array, i.e., as separate image..
            dlib::array<array2d<rgb_pixel>> face_chips;
            extract_image_chips(img, get_face_chip_details(shapes), face_chips);

            array2d<rgb_pixel> face_chip;
            string title;

            for(unsigned long i = 0; i < dets.size(); i++)
            {

                //Assign each face to a single 2d image..
                assign_image(face_chip, face_chips[i]);
            
                //Same process again..
                std::vector<rectangle> det = detector(face_chip);

                full_object_detection shape = sp(face_chip, det[0]);

                //Set up the window to display face, bounding box in red and facial features in green..
                title = "face " + std::to_string(i+1);
                win.set_title(title);
                win.clear_overlay();
                win.set_image(face_chip);
                win.add_overlay(render_face_detections(shape));
                win.add_overlay(det, rgb_pixel(255,0,0));

                //Call the diplay function..
                calculate(shape, det[0]);

            }
          
        }

        void Detect()
        {
            if(dets.size() == 0)
            {
                //If the image was too small to detect a face, zoom in and find out..
                pyramid_up(img);
                dets = detector(img);
                if(dets.size() == 0)
                {
                    pyramid_up(img);
                    dets = detector(img);
                }
            }
    
            if(dets.size() == 0)
            {
                //If the face was too large in the image, zoom out and find the face..
                pyramid_down<3> a;
                a(img);
                dets = detector(img);
            }
        
            cout<<"\nNumber of faces detected: "<<dets.size();

            if(dets.size() == 0)
            cout<<"\nNo faces detected !!";
            
            //Call function to detect landmarks..
            DetectLandmarks();

        }

};

int main(int argc, char const *argv[])
{
	try
	{
        array2d<rgb_pixel> img;
        
        //Load image into array2d of RGB pixels..
        load_image(img, argv[1]);

        FaceDet obj(img);
        obj.Detect();

        cin.get();
	}
	catch (exception &e)
	{
		cout<<"Exception Occured !!!\n";
	}

	return 0;
}