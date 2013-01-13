add_library(gst_testaudio SHARED gst_testaudio.c)
target_link_libraries(gst_testaudio ${GST_LIBRARIES} ${GSTAUDIO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})