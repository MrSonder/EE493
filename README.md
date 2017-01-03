## EE493

### Jan 2, 2017
#### Important Updates
* Line guides are added to indicate boundaries of direction decision.
* Status Bar to replace terminal outputs that can currently display:
	* Location of the image (Left, Mid, Right)
	* Angle information (degrees)
	* Distance from the camera (z axis (cm))
	* Distance from the origin (x axis (pixels))
* New function getAngle(), 
	* returns the angle of the object with respect to the normal of the camera when the object is within the boundaries.
* New function getDistance() returns the distance of the object similar to getAngle()
* New function setColor(int color) sets the threshold values for the color choice.
	* ex. setColor('B') used for Blue.
* Area threshold added to prevent false tracking under selectObject(), if no object is present:
	* trackObject() returns void 
	* Status Bar displays "Searching!".
	
#### Small Tweaks and Configurations
* Trackbars are added to control the boundary limits: 
	* direction boundary (x-axis limits)
	* offset (to compansate the offset camera on robot)
* Unnecessary function initializations are removed or merged.
* Status Bar will be used as the output window rather than terminal from now on.
* Window type set to WINDOW_NORMAL for control bar.
* WiringPi functions are removed.
* resizeRatio parameter to control all resizing functions.
* many commented out lines are deleted.

#### To do
* Search function
