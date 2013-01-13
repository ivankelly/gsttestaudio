gst-launch-1.0  -v --gst-debug-level=3 --gst-debug=testaudio:5,audiobasesrc:5,audiosrc:5 --gst-plugin-path=src -v -m testaudio ! audioconvert ! pulsesink
