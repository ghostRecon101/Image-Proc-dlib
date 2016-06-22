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

int main(int argc, char const *argv[])
{
	try
	{
		//declare a frontal face detector object..
        frontal_face_detector detector = get_frontal_face_detector();
		
		//We need a shape predictor information provider. This dat file contains trained set of facial features..
        shape_predictor sp;
		deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

		image_window win;
		array2d<rgb_pixel> img;
		
        //Load image into array2d of RGB pixels..
        load_image(img, argv[1]);

		//Detect bounding boxes(many faces possible) of the image and store in a vector of rectangles.. 
        std::vector<rectangle> dets = detector(img);

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

		std::vector<full_object_detection> shapes;
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

            cout<<"\nThe bounding box coordinates: ";
            cout<<"\nTop left:     "<<dets[i].tl_corner();
            cout<<"\nTop right:    "<<dets[i].tr_corner();
            cout<<"\nBottom left:  "<<dets[i].bl_corner();
            cout<<"\nBottom right: "<<dets[i].br_corner();
            cout<<endl;
            cout<<"\nNumber of parts: "<<shape.num_parts()<<endl;
            dlib::vector<double,2> l, r, one, two;
            double cnt = 0;

            //detecting distance between eyes
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
            unsigned long s;
            s = pow((l.x() - r.x()), 2) + pow((l.y() - r.y()),2);
            s = sqrt(s);
            cout<<"\nThe distance between the eyes is: "<<s<<endl;
            
            //distance between eyes and ears
            r = shape.part(36) - shape.part(0);
            l = shape.part(45) - shape.part(16);
            unsigned long t;
            s = pow(r.x(), 2) + pow(r.y(), 2);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            s = sqrt(s);
            t = sqrt(t);
            cout<<"\nThe approximate distance between left ear and left eye: "<<t;
            cout<<"\nThe approximate distance between right ear and right eye: "<<s;
            cout<<"\n The average distance between ears and eyes is: "<<(s+t)/2;

            //height of nose
            r = shape.part(33) - shape.part(27);
            s = pow(r.x(),2) + pow(r.y(),2);
            s = sqrt(s);
            cout<<"\nThe height of the nose is: "<<s;


            //48 54 Width of lips
            r = shape.part(54) - shape.part(48);
            s = pow(r.x(),2) + pow(r.y(),2);
            s = sqrt(s);
            cout<<"\nThe width of lip is: "<<s<<endl;

            //Width of forehead..
            r = shape.part(45) - shape.part(36);
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            cout<<"\nThe width of the forehead is: "<<s;

            //size of eye
            unsigned long s1, t1;
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
            s = (s + t)/2;
            cout<<"\nThe eye size is: "<<s;

            //right eyebrow : 17 - 21
            //left  eyebrow : 22 - 26
            r = shape.part(19) - shape.part(17);
            l = shape.part(24) - shape.part(22);
            cout<<"\nThe right eyebrow tilt: "<<r;
            cout<<"\nThe left eyebrow tilt: "<<l;

            //eyebrow low set or high set..
            r = shape.part(37) - shape.part(19);
            l = shape.part(43) - shape.part(23);
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            t = sqrt(t);
            s = (s + t)/2;
            cout<<"\nthe distance between eyebrow and eye is: "<<s;

            //Nose size..
            r = shape.part(33) - shape.part(27);
            l = shape.part(35) - shape.part(31);
            s = pow(r.x(), 2) + pow(r.y(), 2);
            s = sqrt(s);
            t = pow(l.x(), 2) + pow(l.y(), 2);
            t = sqrt(t);
            s = 0.5 * t * s;
            cout<<"\nThe nose size is: "<<s;
            cout<<"\nThe width of nose is: "<<t;

        }
        cin.get();
	}
	catch (exception &e)
	{
		cout<<"Exception Occured !!!\n";
	}

	return 0;
}