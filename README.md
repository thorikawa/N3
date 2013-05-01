#N3

##What's N3
* Yet another SixthSense software
* cf. SixthSense: http://www.pranavmistry.com/projects/sixthsense/

##DEMO
* Gesture Application: http://www.youtube.com/watch?v=W5x79DQyPiE
* PaperDraw Application: http://www.youtube.com/watch?v=iHSEzvneSeU

##How to run N3
###Requirement
* OpenCV (2.4 or later is recomended)

###Build
* Set environment
<pre><code>export OPENCV_HOME=/your/path/to/opencv</code></pre>
* Run make
<pre><code>make</code></pre>

##Code Structure
Each application is defined under  *Apps* folder. Currently the following applications are provided.

* Draw : Simple draw application
* PaperDraw : Improved draw application.
* Gesture1 : Gesture recognition and physical world simulation

##Technology
We use C++ (tessted on Mac OS 10.7.3) with OpenCV

##License
BSD License
