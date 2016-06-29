import sys
import os
import dlib
import glob
import math
from PIL import Image
from skimage import io

class FaceDetection: 

    def inti(self, img_path): 
        self.detector = dlib.get_frontal_face_detector()
        self.predictor = dlib.shape_predictor("shape_predictor_68_face_landmarks.dat")
        self.win = dlib.image_window()
        self.img = io.imread(img_path)
        self.dets = self.detector(self.img)

    def SetPersonality(self):
        dCount, eCount, amCount, anCount, total = 0, 0, 0, 0, 0
        FWavg, ESavg, ENSavg, eyeTiltX, eyeTiltY = 112, 291, 80, 31, -11
        EBSet, NSavg, NWavg, LTavg, NHavg = 23, 1244, 41, 11, 62
        self.dCount, self.eCount, self.amCount, self.anCount = 0, 0, 0, 0

        print "\nCharacteristics: \n"

        if self.foreheadWidth > FWavg + 3:
            self.eCount+= 1
            self.dCount+= 1
            print "\nWide forehead."
        elif self.foreheadWidth <= FWavg + 3 and self.foreheadWidth >= FWavg - 3:
            self.anCount+= 1
            print "\nAverage forehead width"
        
        if self.eyeSize > ESavg + 10:
            self.eCount+= 1
            print "\nLarge eyes."
        elif self.eyeSize < ESavg:
            self.anCount+= 1
            print "\nSmall eyes."
        
        if self.eyeDist > 80:
            self.amCount+= 1
            print "\nEyes widely set."
        else:
            self.anCount+= 1
            print "\nEyes closely set"
        
        if eyeTilt.y() < eyeTiltY:
            self.amCount+= 1
            self.eCount+= 1
            print "\nCurved eyebrows."
        else:
            self.dCount+= 1
            self.anCount+= 1
            print "\nStraight eyebrows"
        
        if self.eyeEyebrow > EBSet:
            self.anCount+= 1
            print "\nHigh set eyebrows."
        else:
            self.dCount+= 1
            print "\nLow set eyebrows."
        
        if self.noseSize > NSavg:
            self.dCount+= 1
            print "\nLarge nose."
        else:
            self.amCount+= 1
            print "\nSmall nose."
        
        if self.noseWidth > NWavg:
            self.eCount+= 1
            print "\nWide nose."
        else:
            self.dCount+= 1
            print "\nNarrow nose."
        
        if self.noseHeight > NHavg:
            self.dCount+= 1
            print "\nLong nose."
        else:
            self.eCount+= 1
            print "\nShort nose."
        
        if self.lipWidth > LTavg:
            self.eCount+= 1
            print "\nThick lips."
        else:
            self.anCount+= 1
            print "\nThin lips."
        
        if self.lowerOrupper == "lower":
            self.amCount+= 1
        elif self.lowerOrupper == "equal":
            self.amCount+= 1

        self.total = self.dCount + self.amCount + self.anCount + self.eCount

        driverPerc = (self.dCount * 100) / self.total
        amiablePerc = (self.amCount * 100) / self.total
        analyticalPerc = (self.anCount * 100) / self.total
        expressivePerc = (self.eCount * 100) / self.total

        print "\nPersonality Traits: "
        print "\nDriver    : ",driverPerc
        print "\nExpressive: ",expressivePerc
        print "\nAnalytical: ",analyticalPerc
        print "\nAmiable   : ",amiablePerc 

    def calculate(self):
        #Area covered by the face..
        print "\nThe bounding box coordinates: "
        print "\nTop left:     ", self.dets.tl_corner()
        print "\nTop right:    ", self.dets.tr_corner()
        print "\nBottom left:  ", self.dets.bl_corner()
        print "\nBottom right: ", self.dets.br_corner()
        print "\nNumber of parts: ", self.shape.num_parts()
           
        #Width of forehead.. 
        r = self.shape.part(45) - self.shape.part(36)

        self.foreheadWidth = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        self.foreheadWidth = math.sqrt(self.foreheadWidth)
        print "\nThe width of the forehead is: ", self.foreheadWidth

        #size of eye..
        r = self.shape.part(39) - self.shape.part(36)
        l = self.shape.part(45) - self.shape.part(42)
            
        s = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s = math.sqrt(s)
        t = math.pow(l.x(), 2) + math.pow(l.y(), 2)
        t = math.sqrt(t)
            
        one = self.shape.part(41) - self.shape.part(37)
        two = self.shape.part(47) - self.shape.part(43)
            
        s1 = math.pow(one.x(), 2) + math.pow(one.y(), 2)
        s1 = math.sqrt(s1)
        t1 = math.pow(two.x(), 2) + math.pow(two.y(), 2)
        t1 = math.sqrt(t1)
            
        s = s * s1
        t = t * t1

        self.eyeSize = (s + t)/2
        print "\nThe eye size is: ", self.eyeSize

        #detecting distance between eyes..
        for j in range(36, 41):
            l += self.shape.part(j)
            cnt+= 1
            
        l /= cnt
        cnt = 0
        
        for j in range(42, 47):  
            r += self.shape.part(j)
            cnt+= 1
        
        r /= cnt
            
        self.eyeDist = math.pow((l.x() - r.x()), 2) + math.pow((l.y() - r.y()),2)
        self.eyeDist = math.sqrt(self.eyeDist)
        print "\nThe distance between the eyes is: ", self.eyeDist
            
        #right eyebrow : 17 - 21
        #left  eyebrow : 22 - 26
        self.rightEyeTilt = self.shape.part(19) - self.shape.part(17)
        self.leftEyeTilt = self.shape.part(24) - self.shape.part(22)
        print "\nThe right eyebrow tilt: ", self.rightEyeTilt
        print "\nThe left eyebrow tilt: ", self.leftEyeTilt
        self.eyeTilt = (self.leftEyeTilt + self.rightEyeTilt) / 2
        print "\nThe average eye tilt is: ", eyeTilt

        #eyebrow low set or high set..
        r = self.shape.part(37) - self.shape.part(19)
        l = self.shape.part(43) - self.shape.part(23)
            
        s = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s = math.sqrt(s)
        t = math.pow(l.x(), 2) + math.pow(l.y(), 2)
        t = math.sqrt(t)

        self.eyeEyebrow = (s + t)/2
        print "\nthe distance between eyebrow and eye is: ", self.eyeEyebrow
            
        #height of nose
        r = self.shape.part(33) - self.shape.part(27)
            
        self.noseHeight = math.pow(r.x(),2) + math.pow(r.y(),2)
        self.noseHeight = math.sqrt(self.noseHeight)
        print "\nThe height of the nose is: ", self.noseHeight

        #Nose size..
        r = self.shape.part(33) - self.shape.part(27)
        l = self.shape.part(35) - self.shape.part(31)
           
        s = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s = math.sqrt(s)
        t = math.pow(l.x(), 2) + math.pow(l.y(), 2)
        t = math.sqrt(t)
            
        self.noseSize = 0.5 * t * s
        self.noseWidth = t
        print "\nThe size of the nose is: ", self.noseSize
        print "\nThe width of the nose is: ", self.noseWidth

        #Length of lips
        r = self.shape.part(54) - self.shape.part(48)
          
        self.lipLength = math.pow(r.x(),2) + math.pow(r.y(),2)
        self.lipLength = math.sqrt(self.lipLength)
        print "\nThe Length of lip is: ", self.lipLength
       
        #lower and upper lip comparison
        r = self.shape.part(61) - self.shape.part(49)
        s = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s = math.sqrt(s)
          
        r = self.shape.part(61) - self.shape.part(50)
        t = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        t = math.sqrt(t)
            
        r = self.shape.part(62) - self.shape.part(51)
        s1 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s1 = math.sqrt(s1)
           
        r = self.shape.part(63) - self.shape.part(52)
        t1 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        t1 = math.sqrt(t1)
            
        r = self.shape.part(63) - self.shape.part(53)
        s2 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s2 = math.sqrt(s2)

        self.lipUpWidth = s + t + s1 + s2 + t1
        self.lipUpWidth /= 5
           
        r = self.shape.part(65) - self.shape.part(55)
        s = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s = math.sqrt(s)
            
        r = self.shape.part(65) - self.shape.part(56)
        t = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        t = math.sqrt(t)
           
        r = self.shape.part(66) - self.shape.part(57)
        s1 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s1 = math.sqrt(s1)
            
        r = self.shape.part(67) - self.shape.part(58)
        t1 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        t1 = math.sqrt(t1)
            
        r = self.shape.part(67) - self.shape.part(59)
        s2 = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        s2 = math.sqrt(s2)
           
        self.lipDownWidth = s + t + s1 + s2 + t1
        self.lipDownWidth /= 5
            
        if self.lipUpWidth > self.lipDownWidth:
            print "\nUpper lip is wider."
            self.lowerOrupper = "upper"
        elif self.lipDownWidth > self.lipUpWidth:
            print "\nLower lip is wider."
            self.lowerOrupper = "lower"
        else:
            self.lowerOrupper = "equal"
            print "\nEqual thickness of upper and lower lip."
            
            
        self.lipWidth = self.lipUpWidth + self.lipDownWidth
        self.lipWidth /= 2

        print "\nThe width of the lip is: ", self.lipWidth

        #distance between eyes and ears
        r = self.shape.part(36) - self.shape.part(0)
        l = self.shape.part(45) - self.shape.part(16)

        self.rightEyeEar = math.pow(r.x(), 2) + math.pow(r.y(), 2)
        self.leftEyeEar = math.pow(l.x(), 2) + math.pow(l.y(), 2)
        self.rightEyeEar = math.sqrt(self.rightEyeEar)
        self.leftEyeEar = math.sqrt(self.leftEyeEar)
        print "\nThe approximate distance between left ear and left eye: ", self.leftEyeEar
        print "\nThe approximate distance between right ear and right eye: ", self.rightEyeEar
        print "\nThe average distance between ears and eyes is: ", (self.leftEyeEar + self.rightEyeEar)/2

        self.SetPersonality()


    def DetectLandmarks(self):
            #Pass the image and the face's bounding box to the self.shape predictor dat file..
            #It returns the self.shape of the face along with 68 point facial feature coordinates..
            self.dets = detector(image)
            self.shape = predictor(self.image, self.dets[0])
        
            #Set up the window to display face, bounding box in red and facial features in green..
            title = "face 1"
            self.win.set_title(title)
            self.win.clear_overlay()
            self.win.set_image(self.image)
            self.win.add_overlay(self.shape)
            self.win.add_overlay(self.dets[0], rgb_pixel(255,0,0))

            #Call the diplay function..
            calculate(self.shape, self.dets[0])


    def Detect(self):
        if len(self.dets) == 0:
            #If the image was too small to detect a face, zoom in and find out..
            self.dets = detector(self.img, 1)
            if len(self.dets) == 0:
                self.dets = detector(self.img, 2)
    
        if len(self.dets) == 0:
            #If the face was too large in the image, zoom out and find the face..
            self.dets = detector(self.img, -1)
        
        if len(self.dets) > 0:
            crop_rectangle = (self.dets[0].left, self.dets[0].top, self.dets[0].right, self.dets[0].bottom)
            i = Image.open(self.img)
            self.image = i.crop(crop_rectangle);
            print "\nNumber of faces detected: ", format(len(self.dets))
            #Call function to detect landmarks..
            DetectLandmarks()
        else:
            print "\nNo faces detected !!"
            


if len(sys.argv) != 2:
    print(
        "Give the path to the trained self.shape predictor model as the first "
        "argument and then the directory containing the facial images.\n"
        "For example, if you are in the python_examples folder then "
        "execute this program by running:\n"
        "    ./face_landmark_detection.py self.shape_predictor_68_face_landmarks.dat ../examples/faces\n"
        "You can download a trained facial self.shape predictor from:\n"
        "    http:#dlib.net/files/self.shape_predictor_68_face_landmarks.dat.bz2")
    exit()

image_name = sys.argv[1]
#img = io.imread(image_name)

detectFace = FaceDetection()
detectFace.inti(image_name)
detectFace.Detect()

#detector = dlib.get_frontal_face_detector()
#predictor = dlib.shape_predictor(predictor_path)
#win = dlib.image_window()
#dets = detector(img)
#if len(dets) == 0:
#    print "No faces detected"
#else:
#    print "Number of faces detected: ", format(len(dets))
#shape = predictor(img, dets[0])
#win.clear_overlay()
#win.set_image(img)
#win.add_overlay(shape)
#print shape.part(0)

dlib.hit_enter_to_continue()