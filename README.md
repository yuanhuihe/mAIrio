![mAIrio](http://i.imgur.com/EB7P2hW.png)

## Overview
This is a basic AI that can play through certain levels of Super Mario Brothers on an NES emulator. As storing the levels within the program would not be intelligent by any stretch of the imagination, we instead tried to distill Super Mario Brothers down to a
set of basic rules to be implemented. We are using OpenCV to track objects within the emulator and sending keypresses after our decisions have been made about running and jumping.

We used a Windows PC running OpenCV and FCEUX Emulator.

## Demo
[![Demo Video](https://img.youtube.com/vi/zMH7UoMoxjU/hqdefault.jpg)](https://www.youtube.com/watch?v=zMH7UoMoxjU)
## [Full Paper](https://drive.google.com/file/d/0B0-iA2D4gMmXZW04MktvWl9reXc/view?usp=sharing)
## Template Matching
![Template Matching](http://opencv-python-tutroals.readthedocs.io/en/latest/_images/template_ccoeff_1.jpg)

**Template matching** is a common image processing technique used to match small parts of an image with a reference template. Since our input is noise free, or “pixel perfect”, it is a good way to detect objects with a high degree of accuracy. There are several popular methods to determine how close a grouping of pixels is to your specified template. We used sum of square differences, which assigns a sum of square differences between the template and the actual image to each eligible pixel. To find the best match, simply find the minimum value on the corresponding result matrix.

![Goomba Example Raw](http://imgur.com/PnP3o3i.png)
![Goomba Example](http://imgur.com/KuU1zUI.png)

Template matching results.  Each goomba location is circled in red.

## Implementation
### Extract Frame From Emulator
* Get HWND (a handle to a window in the GUI)
* Use hwnd2mat() to convert the bitmap from the window’s contents into a cv::Mat
### Update Mario's State
* Mario's state is represented by an on screen position, a template, and a true game bounding box
* Use template matching to find pixel location on screen
* Use a state change truth table to switch templates if Mario's appearance changes

![Mario Templates](http://i.imgur.com/pr2IsWb.png)
* Use Region of Interest to increase performance to real time requirements
 * Frame over frame pixel velocity predicts future Mario location

![Full Mario](http://i.imgur.com/28x2qro.png)
![ROI](http://i.imgur.com/I1tfTa9.png)

### Update Existing Entity States
![Goomba Full](http://i.imgur.com/9xLynDg.png)
![Goomba Template](http://i.imgur.com/FmZqVHy.png)
![Koopa Full](http://i.imgur.com/7tb7LSJ.png)
![Koopa Template](http://i.imgur.com/Y3n3LbQ.png)

Example Entities and Templates

* Very similar to tracking Mario except Entities can leave the screen!
* ROI of an entity is smaller and more predictable than Mario

![ROI Goomba Full](http://i.imgur.com/28x2qro.png)
![ROI Goomba](http://i.imgur.com/CHmE9XX.png)

Example ROI of a Goomba
### Finding new Entities
![New Entity Full](http://i.imgur.com/NilcAjy.png)
![New Entity Zone](http://i.imgur.com/AmpkXxE.png)

New entity zone plus existing entity masking

* All new entities can be expected to come from the right 40 pixels
* Template match for every kind of entity.  If pixel value > threshold, we have found a new entity 

### Sending Commands
* The Keyboard class sends raw character keypresses to a HWND window
* The Controller class wraps Keyboard to obfuscate generic Mario commands

### Decision Making
Decision making can be generalized into a few bullet points.  There are several edge cases that are not mentioned for jumping and stopping.  Read the full paper for an in depth description!
* If no entities within X pixel's of Mario's location, run right
* If pit detected, jump
* If staircase detected, jump
* If enemy detected, jump
