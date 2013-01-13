/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2012 Ivan Kelly <ivan@ivankelly.net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-plugin TODO
 *
 * FIXME:Describe plugin here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! plugin ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "gst_testaudio.h"

GST_DEBUG_CATEGORY (testaudio_gst_src_debug);
#define GST_CAT_DEFAULT testaudio_gst_src_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define FORMAT_STR "{ S32LE }"
#else
#define FORMAT_STR "{ S32BE }"
#endif

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, "
        "format = (string) " FORMAT_STR ", "
        "layout = (string) interleaved, "
        "rate = (int) [ 1, MAX ], "
	"channels = (int) 2")
    );

#define testaudio_gst_src_parent_class parent_class
G_DEFINE_TYPE (TestAudioGstSrc, testaudio_gst_src, GST_TYPE_AUDIO_SRC);

static void testaudio_gst_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void testaudio_gst_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

/* open the device with given specs */
static gboolean testaudio_gst_open(GstAudioSrc *src);
/* prepare resources and state to operate with the given specs */
static gboolean testaudio_gst_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec);
/* undo anything that was done in prepare() */
static gboolean testaudio_gst_unprepare(GstAudioSrc *src);
/* close the device */
static gboolean testaudio_gst_close(GstAudioSrc *src);
/* read samples from the device */
static guint testaudio_gst_read(GstAudioSrc *src, gpointer data, guint length,
			      GstClockTime *timestamp);
/* get number of samples queued in the device */
static guint testaudio_gst_delay (GstAudioSrc *src);
/* reset the audio device, unblock from a write */
static void testaudio_gst_reset(GstAudioSrc *src);

/* GObject vmethod implementations */

/* initialize the src's class */
static void
testaudio_gst_src_class_init (TestAudioGstSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstAudioSrcClass *gstaudiosrc_class;

  GST_INFO_OBJECT(klass, "class init");

  gobject_class = G_OBJECT_CLASS(klass);
  gstelement_class = GST_ELEMENT_CLASS(klass);
  gstaudiosrc_class = GST_AUDIO_SRC_CLASS(klass);

  gobject_class->set_property = testaudio_gst_src_set_property;
  gobject_class->get_property = testaudio_gst_src_get_property;

  gstaudiosrc_class->open = GST_DEBUG_FUNCPTR (testaudio_gst_open);
  gstaudiosrc_class->prepare = GST_DEBUG_FUNCPTR (testaudio_gst_prepare);
  gstaudiosrc_class->unprepare = GST_DEBUG_FUNCPTR (testaudio_gst_unprepare);
  gstaudiosrc_class->close = GST_DEBUG_FUNCPTR (testaudio_gst_close);
  gstaudiosrc_class->read = GST_DEBUG_FUNCPTR (testaudio_gst_read);
  gstaudiosrc_class->delay = GST_DEBUG_FUNCPTR (testaudio_gst_delay);
  gstaudiosrc_class->reset = GST_DEBUG_FUNCPTR (testaudio_gst_reset);
  
  gst_element_class_set_details_simple(gstelement_class,
    "Plugin",
    "FIXME:Generic",
    "FIXME:Generic Template Element",
    "AUTHOR_NAME AUTHOR_EMAIL");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
testaudio_gst_src_init (TestAudioGstSrc * klass)
{
  int err;
  GST_INFO_OBJECT(klass, "src init");
}

/* open the device with given specs */
static gboolean
testaudio_gst_open(GstAudioSrc *src) {
  return TRUE;
}

/* prepare resources and state to operate with the given specs */
static gboolean
testaudio_gst_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec) {
  GST_INFO_OBJECT(src, "prepare");
  return TRUE;
}

/* undo anything that was done in prepare() */
static gboolean
testaudio_gst_unprepare(GstAudioSrc *src) {
  GST_INFO_OBJECT(src, "unprepare");
  return TRUE;
}

/* close the device */
static gboolean
testaudio_gst_close(GstAudioSrc *src) {
  TestAudioGstSrc *sp_src = TEST_AUDIO_GST_SRC (src);

  return TRUE;
}

/* read samples from the device */
static guint
testaudio_gst_read(GstAudioSrc *src, gpointer data, guint length,
		 GstClockTime *timestamp) {
  int i = 0;
  int samples = (length/sizeof(int));

  int* idata = (int*)data;
  for (i = 0; i < samples; i++) {
    idata[i] = (int)g_random_int_range(-1.0, 1.0) * 2147483647.0;
  }

  return samples*sizeof(int);
}

/* get number of samples queued in the device */
static guint
testaudio_gst_delay (GstAudioSrc *src) {
  return 0;
}

/* reset the audio device, unblock from a write */
static void
testaudio_gst_reset(GstAudioSrc *src) {
  GST_INFO_OBJECT(src, "reset");
}

static void
testaudio_gst_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  TestAudioGstSrc *src = TEST_AUDIO_GST_SRC (object);

  switch (prop_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
testaudio_gst_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  TestAudioGstSrc *filter = TEST_AUDIO_GST_SRC (object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
testaudio_plugin_init (GstPlugin * plugin)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template plugin' with your description
   */
  GST_DEBUG_CATEGORY_INIT (testaudio_gst_src_debug, "testaudio",
      0, "Template plugin");
  GST_INFO_OBJECT(plugin, "plugin_init");
  return gst_element_register (plugin, "testaudio", GST_RANK_NONE,
			       TEST_AUDIO_GST_TYPE_SRC);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "testaudio_gst"
#endif

/* gstreamer looks for this structure to register plugins
 *
 * exchange the string 'Template plugin' with your plugin description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    testaudio,
    "TestAudio plugi",
    testaudio_plugin_init,
    "0.0.1",
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
