/*
 * Cogl
 *
 * A Low Level GPU Graphics and Utilities API
 *
 * Copyright (C) 2007,2008,2009,2011 Intel Corporation.
 * Copyright (C) 2019 DisplayLink (UK) Ltd.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 *
 * Authors:
 *   Robert Bragg <robert@linux.intel.com>
 */

#pragma once

#include "cogl/cogl-pipeline.h"
#include "cogl/cogl-indices.h"
#include "cogl/cogl-bitmap.h"
#include "cogl/cogl-texture.h"
#include "mtk/mtk.h"

#include <glib-object.h>

#include <graphene.h>

G_BEGIN_DECLS

/**
 * CoglFrameBuffer:
 *
 * A common interface for manipulating framebuffers
 *
 * Framebuffers are a collection of buffers that can be rendered too.
 * A framebuffer may be comprised of one or more color buffers, an
 * optional depth buffer and an optional stencil buffer. Other
 * configuration parameters are associated with framebuffers too such
 * as whether the framebuffer supports multi-sampling (an anti-aliasing
 * technique) or dithering.
 *
 * There are two kinds of framebuffer in Cogl, #CoglOnscreen
 * framebuffers and #CoglOffscreen framebuffers. As the names imply
 * offscreen framebuffers are for rendering something offscreen
 * (perhaps to a texture which is bound as one of the color buffers).
 * The exact semantics of onscreen framebuffers depends on the window
 * system backend that you are using, but typically you can expect
 * rendering to a #CoglOnscreen framebuffer will be immediately
 * visible to the user.
 *
 * If you want to create a new framebuffer then you should start by
 * looking at the #CoglOnscreen and #CoglOffscreen constructor
 * functions, such as cogl_offscreen_new_with_texture() or
 * cogl_onscreen_new(). The #CoglFramebuffer interface deals with
 * all aspects that are common between those two types of framebuffer.
 *
 * Setup of a new CoglFramebuffer happens in two stages. There is a
 * configuration stage where you specify all the options and ancillary
 * buffers you want associated with your framebuffer and then when you
 * are happy with the configuration you can "allocate" the framebuffer
 * using cogl_framebuffer_allocate(). Technically explicitly calling
 * cogl_framebuffer_allocate() is optional for convenience and the
 * framebuffer will automatically be allocated when you first try to
 * draw to it, but if you do the allocation manually then you can
 * also catch any possible errors that may arise from your
 * configuration.
 */

#define COGL_TYPE_FRAMEBUFFER (cogl_framebuffer_get_type ())
COGL_EXPORT
G_DECLARE_DERIVABLE_TYPE (CoglFramebuffer, cogl_framebuffer,
                          COGL, FRAMEBUFFER, GObject)

struct _CoglFramebufferClass
{
  /*< private >*/
  GObjectClass parent_class;

  gboolean (* allocate) (CoglFramebuffer  *framebuffer,
                         GError          **error);
  gboolean (* is_y_flipped) (CoglFramebuffer *framebuffer);
};

/**
 * cogl_framebuffer_allocate:
 * @framebuffer: A #CoglFramebuffer
 * @error: A pointer to a #GError for returning exceptions.
 *
 * Explicitly allocates a configured #CoglFramebuffer allowing developers to
 * check and handle any errors that might arise from an unsupported
 * configuration so that fallback configurations may be tried.
 *
 * Many applications don't support any fallback options at least when
 * they are initially developed and in that case the don't need to use this API
 * since Cogl will automatically allocate a framebuffer when it first gets
 * used.  The disadvantage of relying on automatic allocation is that the
 * program will abort with an error message if there is an error during
 * automatic allocation.
 *
 * Return value: %TRUE if there were no error allocating the framebuffer, else %FALSE.
 */
COGL_EXPORT gboolean
cogl_framebuffer_allocate (CoglFramebuffer *framebuffer,
                           GError **error);

/**
 * cogl_framebuffer_get_width:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the current width of the given @framebuffer.
 *
 * Return value: The width of @framebuffer.
 */
COGL_EXPORT int
cogl_framebuffer_get_width (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_height:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the current height of the given @framebuffer.
 *
 * Return value: The height of @framebuffer.
 */
COGL_EXPORT int
cogl_framebuffer_get_height (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_set_viewport:
 * @framebuffer: A #CoglFramebuffer
 * @x: The top-left x coordinate of the viewport origin (only integers
 *     supported currently)
 * @y: The top-left y coordinate of the viewport origin (only integers
 *     supported currently)
 * @width: The width of the viewport (only integers supported currently)
 * @height: The height of the viewport (only integers supported currently)
 *
 * Defines a scale and offset for everything rendered relative to the
 * top-left of the destination framebuffer.
 *
 * By default the viewport has an origin of (0,0) and width and height
 * that match the framebuffer's size. Assuming a default projection and
 * modelview matrix then you could translate the contents of a window
 * down and right by leaving the viewport size unchanged by moving the
 * offset to (10,10). The viewport coordinates are measured in pixels.
 * If you left the x and y origin as (0,0) you could scale the windows
 * contents down by specify and width and height that's half the real
 * size of the framebuffer.
 *
 * Although the function takes floating point arguments, existing
 * drivers only allow the use of integer values. In the future floating
 * point values will be exposed via a checkable feature.
 *
 */
COGL_EXPORT void
cogl_framebuffer_set_viewport (CoglFramebuffer *framebuffer,
                               float x,
                               float y,
                               float width,
                               float height);

/**
 * cogl_framebuffer_get_viewport_x:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the x coordinate of the viewport origin as set using cogl_framebuffer_set_viewport()
 * or the default value which is 0.
 *
 * Return value: The x coordinate of the viewport origin.
 */
COGL_EXPORT float
cogl_framebuffer_get_viewport_x (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_viewport_y:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the y coordinate of the viewport origin as set using cogl_framebuffer_set_viewport()
 * or the default value which is 0.
 *
 * Return value: The y coordinate of the viewport origin.
 */
COGL_EXPORT float
cogl_framebuffer_get_viewport_y (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_viewport_width:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the width of the viewport as set using cogl_framebuffer_set_viewport()
 * or the default value which is the width of the framebuffer.
 *
 * Return value: The width of the viewport.
 */
COGL_EXPORT float
cogl_framebuffer_get_viewport_width (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_viewport_height:
 * @framebuffer: A #CoglFramebuffer
 *
 * Queries the height of the viewport as set using cogl_framebuffer_set_viewport()
 * or the default value which is the height of the framebuffer.
 *
 * Return value: The height of the viewport.
 */
COGL_EXPORT float
cogl_framebuffer_get_viewport_height (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_viewport4fv:
 * @framebuffer: A #CoglFramebuffer
 * @viewport: (out caller-allocates) (array fixed-size=4): A pointer to an
 *            array of 4 floats to receive the (x, y, width, height)
 *            components of the current viewport.
 *
 * Queries the x, y, width and height components of the current viewport as set
 * using cogl_framebuffer_set_viewport() or the default values which are 0, 0,
 * framebuffer_width and framebuffer_height.  The values are written into the
 * given @viewport array.
 *
 */
COGL_EXPORT void
cogl_framebuffer_get_viewport4fv (CoglFramebuffer *framebuffer,
                                  float *viewport);

/**
 * cogl_framebuffer_push_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 *
 * Copies the current model-view matrix onto the matrix stack. The matrix
 * can later be restored with cogl_framebuffer_pop_matrix().
 */
COGL_EXPORT void
cogl_framebuffer_push_matrix (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_pop_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 *
 * Restores the model-view matrix on the top of the matrix stack.
 */
COGL_EXPORT void
cogl_framebuffer_pop_matrix (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_scale:
 * @framebuffer: A #CoglFramebuffer pointer
 * @x: Amount to scale along the x-axis
 * @y: Amount to scale along the y-axis
 * @z: Amount to scale along the z-axis
 *
 * Multiplies the current model-view matrix by one that scales the x,
 * y and z axes by the given values.
 */
COGL_EXPORT void
cogl_framebuffer_scale (CoglFramebuffer *framebuffer,
                        float x,
                        float y,
                        float z);

/**
 * cogl_framebuffer_translate:
 * @framebuffer: A #CoglFramebuffer pointer
 * @x: Distance to translate along the x-axis
 * @y: Distance to translate along the y-axis
 * @z: Distance to translate along the z-axis
 *
 * Multiplies the current model-view matrix by one that translates the
 * model along all three axes according to the given values.
 */
COGL_EXPORT void
cogl_framebuffer_translate (CoglFramebuffer *framebuffer,
                            float x,
                            float y,
                            float z);

/**
 * cogl_framebuffer_rotate:
 * @framebuffer: A #CoglFramebuffer pointer
 * @angle: Angle in degrees to rotate.
 * @x: X-component of vertex to rotate around.
 * @y: Y-component of vertex to rotate around.
 * @z: Z-component of vertex to rotate around.
 *
 * Multiplies the current model-view matrix by one that rotates the
 * model around the axis-vector specified by @x, @y and @z. The
 * rotation follows the right-hand thumb rule so for example rotating
 * by 10 degrees about the axis-vector (0, 0, 1) causes a small
 * counter-clockwise rotation.
 */
COGL_EXPORT void
cogl_framebuffer_rotate (CoglFramebuffer *framebuffer,
                         float angle,
                         float x,
                         float y,
                         float z);

/**
 * cogl_framebuffer_transform:
 * @framebuffer: A #CoglFramebuffer pointer
 * @matrix: the matrix to multiply with the current model-view
 *
 * Multiplies the current model-view matrix by the given matrix.
 */
COGL_EXPORT void
cogl_framebuffer_transform (CoglFramebuffer         *framebuffer,
                            const graphene_matrix_t *matrix);

/**
 * cogl_framebuffer_get_modelview_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 * @matrix: (out): return location for the model-view matrix
 *
 * Stores the current model-view matrix in @matrix.
 */
COGL_EXPORT void
cogl_framebuffer_get_modelview_matrix (CoglFramebuffer   *framebuffer,
                                       graphene_matrix_t *matrix);

/**
 * cogl_framebuffer_set_modelview_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 * @matrix: the new model-view matrix
 *
 * Sets @matrix as the new model-view matrix.
 */
COGL_EXPORT void
cogl_framebuffer_set_modelview_matrix (CoglFramebuffer         *framebuffer,
                                       const graphene_matrix_t *matrix);

/**
 * cogl_framebuffer_perspective:
 * @framebuffer: A #CoglFramebuffer pointer
 * @fov_y: Vertical field of view angle in degrees.
 * @aspect: The (width over height) aspect ratio for display
 * @z_near: The distance to the near clipping plane (Must be positive,
 *   and must not be 0)
 * @z_far: The distance to the far clipping plane (Must be positive)
 *
 * Replaces the current projection matrix with a perspective matrix
 * based on the provided values.
 *
 * You should be careful not to have to great a @z_far / @z_near
 * ratio since that will reduce the effectiveness of depth testing
 * since there won't be enough precision to identify the depth of
 * objects near to each other.
 */
COGL_EXPORT void
cogl_framebuffer_perspective (CoglFramebuffer *framebuffer,
                              float fov_y,
                              float aspect,
                              float z_near,
                              float z_far);

/**
 * cogl_framebuffer_frustum:
 * @framebuffer: A #CoglFramebuffer pointer
 * @left: X position of the left clipping plane where it
 *   intersects the near clipping plane
 * @right: X position of the right clipping plane where it
 *   intersects the near clipping plane
 * @bottom: Y position of the bottom clipping plane where it
 *   intersects the near clipping plane
 * @top: Y position of the top clipping plane where it intersects
 *   the near clipping plane
 * @z_near: The distance to the near clipping plane (Must be positive)
 * @z_far: The distance to the far clipping plane (Must be positive)
 *
 * Replaces the current projection matrix with a perspective matrix
 * for a given viewing frustum defined by 4 side clip planes that
 * all cross through the origin and 2 near and far clip planes.
 */
COGL_EXPORT void
cogl_framebuffer_frustum (CoglFramebuffer *framebuffer,
                          float left,
                          float right,
                          float bottom,
                          float top,
                          float z_near,
                          float z_far);

/**
 * cogl_framebuffer_orthographic:
 * @framebuffer: A #CoglFramebuffer pointer
 * @x_1: The x coordinate for the first vertical clipping plane
 * @y_1: The y coordinate for the first horizontal clipping plane
 * @x_2: The x coordinate for the second vertical clipping plane
 * @y_2: The y coordinate for the second horizontal clipping plane
 * @near: The *distance* to the near clipping
 *   plane (will be *negative* if the plane is
 *   behind the viewer)
 * @far: The *distance* to the far clipping
 *   plane (will be *negative* if the plane is
 *   behind the viewer)
 *
 * Replaces the current projection matrix with an orthographic projection
 * matrix.
 */
COGL_EXPORT void
cogl_framebuffer_orthographic (CoglFramebuffer *framebuffer,
                               float x_1,
                               float y_1,
                               float x_2,
                               float y_2,
                               float near,
                               float far);

/**
 * cogl_framebuffer_get_projection_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 * @matrix: (out): return location for the projection matrix
 *
 * Stores the current projection matrix in @matrix.
 */
COGL_EXPORT void
cogl_framebuffer_get_projection_matrix (CoglFramebuffer   *framebuffer,
                                        graphene_matrix_t *matrix);

/**
 * cogl_framebuffer_set_projection_matrix:
 * @framebuffer: A #CoglFramebuffer pointer
 * @matrix: the new projection matrix
 *
 * Sets @matrix as the new projection matrix.
 */
COGL_EXPORT void
cogl_framebuffer_set_projection_matrix (CoglFramebuffer         *framebuffer,
                                        const graphene_matrix_t *matrix);

/**
 * cogl_framebuffer_push_rectangle_clip:
 * @framebuffer: A #CoglFramebuffer pointer
 * @x_1: x coordinate for top left corner of the clip rectangle
 * @y_1: y coordinate for top left corner of the clip rectangle
 * @x_2: x coordinate for bottom right corner of the clip rectangle
 * @y_2: y coordinate for bottom right corner of the clip rectangle
 *
 * Specifies a modelview transformed rectangular clipping area for all
 * subsequent drawing operations. Any drawing commands that extend
 * outside the rectangle will be clipped so that only the portion
 * inside the rectangle will be displayed. The rectangle dimensions
 * are transformed by the current model-view matrix.
 *
 * The rectangle is intersected with the current clip region. To undo
 * the effect of this function, call cogl_framebuffer_pop_clip().
 */
COGL_EXPORT void
cogl_framebuffer_push_rectangle_clip (CoglFramebuffer *framebuffer,
                                      float x_1,
                                      float y_1,
                                      float x_2,
                                      float y_2);

COGL_EXPORT void
cogl_framebuffer_push_region_clip (CoglFramebuffer *framebuffer,
                                   MtkRegion       *region);

/**
 * cogl_framebuffer_pop_clip:
 * @framebuffer: A #CoglFramebuffer pointer
 *
 * Reverts the clipping region to the state before the last call to
 * cogl_framebuffer_push_rectangle_clip()
 */
COGL_EXPORT void
cogl_framebuffer_pop_clip (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_red_bits:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Retrieves the number of red bits of @framebuffer
 *
 * Return value: the number of bits
 *
 */
COGL_EXPORT int
cogl_framebuffer_get_red_bits (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_green_bits:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Retrieves the number of green bits of @framebuffer
 *
 * Return value: the number of bits
 *
 */
COGL_EXPORT int
cogl_framebuffer_get_green_bits (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_blue_bits:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Retrieves the number of blue bits of @framebuffer
 *
 * Return value: the number of bits
 *
 */
COGL_EXPORT int
cogl_framebuffer_get_blue_bits (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_alpha_bits:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Retrieves the number of alpha bits of @framebuffer
 *
 * Return value: the number of bits
 *
 */
COGL_EXPORT int
cogl_framebuffer_get_alpha_bits (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_get_dither_enabled:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Returns whether dithering has been requested for the given @framebuffer.
 * See cogl_framebuffer_set_dither_enabled() for more details about dithering.
 *
 * This may return %TRUE even when the underlying @framebuffer
 * display pipeline does not support dithering. This value only represents
 * the user's request for dithering.
 *
 * Return value: %TRUE if dithering has been requested or %FALSE if not.
 */
COGL_EXPORT gboolean
cogl_framebuffer_get_dither_enabled (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_set_dither_enabled:
 * @framebuffer: a pointer to a #CoglFramebuffer
 * @dither_enabled: %TRUE to enable dithering or %FALSE to disable
 *
 * Enables or disabled dithering if supported by the hardware.
 *
 * Dithering is a hardware dependent technique to increase the visible
 * color resolution beyond what the underlying hardware supports by playing
 * tricks with the colors placed into the framebuffer to give the illusion
 * of other colors. (For example this can be compared to half-toning used
 * by some news papers to show varying levels of grey even though their may
 * only be black and white are available).
 *
 * If the current display pipeline for @framebuffer does not support dithering
 * then this has no affect.
 *
 * Dithering is enabled by default.
 *
 */
COGL_EXPORT void
cogl_framebuffer_set_dither_enabled (CoglFramebuffer *framebuffer,
                                     gboolean dither_enabled);

/**
 * cogl_framebuffer_get_depth_write_enabled:
 * @framebuffer: a pointer to a #CoglFramebuffer
 *
 * Queries whether depth buffer writing is enabled for @framebuffer. This
 * can be controlled via cogl_framebuffer_set_depth_write_enabled().
 *
 * Return value: %TRUE if depth writing is enabled or %FALSE if not.
 */
COGL_EXPORT gboolean
cogl_framebuffer_get_depth_write_enabled (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_set_depth_write_enabled:
 * @framebuffer: a pointer to a #CoglFramebuffer
 * @depth_write_enabled: %TRUE to enable depth writing or %FALSE to disable
 *
 * Enables or disables depth buffer writing when rendering to @framebuffer.
 * If depth writing is enabled for both the framebuffer and the rendering
 * pipeline, and the framebuffer has an associated depth buffer, depth
 * information will be written to this buffer during rendering.
 *
 * Depth buffer writing is enabled by default.
 */
COGL_EXPORT void
cogl_framebuffer_set_depth_write_enabled (CoglFramebuffer *framebuffer,
                                          gboolean depth_write_enabled);

/**
 * cogl_framebuffer_get_context:
 * @framebuffer: A #CoglFramebuffer
 *
 * Can be used to query the #CoglContext a given @framebuffer was
 * instantiated within. This is the #CoglContext that was passed to
 * cogl_onscreen_new() for example.
 *
 * Return value: (transfer none): The #CoglContext that the given
 *               @framebuffer was instantiated within.
 */
COGL_EXPORT CoglContext *
cogl_framebuffer_get_context (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_clear:
 * @framebuffer: A #CoglFramebuffer
 * @buffers: A mask of `CoglBufferBit`s identifying which auxiliary
 *   buffers to clear
 * @color: The color to clear the color buffer too if specified in
 *         @buffers.
 *
 * Clears all the auxiliary buffers identified in the @buffers mask, and if
 * that includes the color buffer then the specified @color is used.
 *
 */
COGL_EXPORT void
cogl_framebuffer_clear (CoglFramebuffer *framebuffer,
                        unsigned long buffers,
                        const CoglColor *color);

/**
 * cogl_framebuffer_clear4f:
 * @framebuffer: A #CoglFramebuffer
 * @buffers: A mask of `CoglBufferBit`s identifying which auxiliary
 *   buffers to clear
 * @red: The red component of color to clear the color buffer too if
 *       specified in @buffers.
 * @green: The green component of color to clear the color buffer too if
 *         specified in @buffers.
 * @blue: The blue component of color to clear the color buffer too if
 *        specified in @buffers.
 * @alpha: The alpha component of color to clear the color buffer too if
 *         specified in @buffers.
 *
 * Clears all the auxiliary buffers identified in the @buffers mask, and if
 * that includes the color buffer then the specified @color is used.
 *
 */
COGL_EXPORT void
cogl_framebuffer_clear4f (CoglFramebuffer *framebuffer,
                          unsigned long buffers,
                          float red,
                          float green,
                          float blue,
                          float alpha);

/**
 * cogl_framebuffer_draw_rectangle:
 * @framebuffer: A destination #CoglFramebuffer
 * @pipeline: A #CoglPipeline state object
 * @x_1: X coordinate of the top-left corner
 * @y_1: Y coordinate of the top-left corner
 * @x_2: X coordinate of the bottom-right corner
 * @y_2: Y coordinate of the bottom-right corner
 *
 * Draws a rectangle to @framebuffer with the given @pipeline state
 * and with the top left corner positioned at (@x_1, @y_1) and the
 * bottom right corner positioned at (@x_2, @y_2).
 *
 * The position is the position before the rectangle has been
 * transformed by the model-view matrix and the projection
 * matrix.
 *
 * If you want to describe a rectangle with a texture mapped on
 * it then you can use
 * cogl_framebuffer_draw_textured_rectangle().
 */
COGL_EXPORT void
cogl_framebuffer_draw_rectangle (CoglFramebuffer *framebuffer,
                                 CoglPipeline *pipeline,
                                 float x_1,
                                 float y_1,
                                 float x_2,
                                 float y_2);

/**
 * cogl_framebuffer_draw_textured_rectangle:
 * @framebuffer: A destination #CoglFramebuffer
 * @pipeline: A #CoglPipeline state object
 * @x_1: x coordinate upper left on screen.
 * @y_1: y coordinate upper left on screen.
 * @x_2: x coordinate lower right on screen.
 * @y_2: y coordinate lower right on screen.
 * @s_1: S texture coordinate of the top-left coorner
 * @t_1: T texture coordinate of the top-left coorner
 * @s_2: S texture coordinate of the bottom-right coorner
 * @t_2: T texture coordinate of the bottom-right coorner
 *
 * Draws a textured rectangle to @framebuffer using the given
 * @pipeline state with the top left corner positioned at (@x_1, @y_1)
 * and the bottom right corner positioned at (@x_2, @y_2). The top
 * left corner will have texture coordinates of (@s_1, @t_1) and the
 * bottom right corner will have texture coordinates of (@s_2, @t_2).
 *
 * The position is the position before the rectangle has been
 * transformed by the model-view matrix and the projection
 * matrix.
 *
 * This is a high level drawing api that can handle any kind of
 * #CoglTexture texture such as #CoglTexture2DSliced textures
 * which may internally be comprised of multiple low-level textures.
 * This is unlike low-level drawing apis such as cogl_primitive_draw()
 * which only support low level texture types that are directly
 * supported by GPUs such as #CoglTexture2D.
 *
 * The given texture coordinates will only be used for the first
 * texture layer of the pipeline and if your pipeline has more than
 * one layer then all other layers will have default texture
 * coordinates of @s_1=0.0 @t_1=0.0 @s_2=1.0 @t_2=1.0
 *
 * The given texture coordinates should always be normalized such that
 * (0, 0) corresponds to the top left and (1, 1) corresponds to the
 * bottom right. To map an entire texture across the rectangle pass
 * in @s_1=0, @t_1=0, @s_2=1, @t_2=1.
 */
COGL_EXPORT void
cogl_framebuffer_draw_textured_rectangle (CoglFramebuffer *framebuffer,
                                          CoglPipeline *pipeline,
                                          float x_1,
                                          float y_1,
                                          float x_2,
                                          float y_2,
                                          float s_1,
                                          float t_1,
                                          float s_2,
                                          float t_2);

/**
 * cogl_framebuffer_draw_multitextured_rectangle:
 * @framebuffer: A destination #CoglFramebuffer
 * @pipeline: A #CoglPipeline state object
 * @x_1: x coordinate upper left on screen.
 * @y_1: y coordinate upper left on screen.
 * @x_2: x coordinate lower right on screen.
 * @y_2: y coordinate lower right on screen.
 * @tex_coords: (in) (array) (transfer none): An array containing groups of
 *   4 float values: [s_1, t_1, s_2, t_2] that are interpreted as two texture
 *   coordinates; one for the top left texel, and one for the bottom right
 *   texel. Each value should be between 0.0 and 1.0, where the coordinate
 *   (0.0, 0.0) represents the top left of the texture, and (1.0, 1.0) the
 *   bottom right.
 * @tex_coords_len: The length of the @tex_coords array. (For one layer
 *   and one group of texture coordinates, this would be 4)
 *
 * Draws a textured rectangle to @framebuffer with the given @pipeline
 * state with the top left corner positioned at (@x_1, @y_1) and the
 * bottom right corner positioned at (@x_2, @y_2). As a pipeline may
 * contain multiple texture layers this interface lets you supply
 * texture coordinates for each layer of the pipeline.
 *
 * The position is the position before the rectangle has been
 * transformed by the model-view matrix and the projection
 * matrix.
 *
 * This is a high level drawing api that can handle any kind of
 * #CoglTexture texture for the first layer such as
 * #CoglTexture2DSliced textures which may internally be comprised of
 * multiple low-level textures.  This is unlike low-level drawing apis
 * such as cogl_primitive_draw() which only support low level texture
 * types that are directly supported by GPUs such as #CoglTexture2D.
 *
 * This api can not currently handle multiple high-level meta
 * texture layers. The first layer may be a high level meta texture
 * such as #CoglTexture2DSliced but all other layers much be low
 * level textures such as #CoglTexture2D.
 *
 * The top left texture coordinate for layer 0 of any pipeline will be
 * (tex_coords[0], tex_coords[1]) and the bottom right coordinate will
 * be (tex_coords[2], tex_coords[3]). The coordinates for layer 1
 * would be (tex_coords[4], tex_coords[5]) (tex_coords[6],
 * tex_coords[7]) and so on...
 *
 * The given texture coordinates should always be normalized such that
 * (0, 0) corresponds to the top left and (1, 1) corresponds to the
 * bottom right. To map an entire texture across the rectangle pass
 * in tex_coords[0]=0, tex_coords[1]=0, tex_coords[2]=1,
 * tex_coords[3]=1.
 *
 * The first pair of coordinates are for the first layer (with the
 * smallest layer index) and if you supply less texture coordinates
 * than there are layers in the current source pipeline then default
 * texture coordinates (0.0, 0.0, 1.0, 1.0) are generated.
 */
COGL_EXPORT void
cogl_framebuffer_draw_multitextured_rectangle (CoglFramebuffer *framebuffer,
                                               CoglPipeline *pipeline,
                                               float x_1,
                                               float y_1,
                                               float x_2,
                                               float y_2,
                                               const float *tex_coords,
                                               int tex_coords_len);

/**
 * cogl_framebuffer_draw_rectangles:
 * @framebuffer: A destination #CoglFramebuffer
 * @pipeline: A #CoglPipeline state object
 * @coordinates: (in) (array) (transfer none): an array of coordinates
 *   containing groups of 4 float values: [x_1, y_1, x_2, y_2] that are
 *   interpreted as two position coordinates; one for the top left of
 *   the rectangle (x1, y1), and one for the bottom right of the
 *   rectangle (x2, y2).
 * @n_rectangles: number of rectangles defined in @coordinates.
 *
 * Draws a series of rectangles to @framebuffer with the given
 * @pipeline state in the same way that
 * cogl_framebuffer_draw_rectangle() does.
 *
 * The top left corner of the first rectangle is positioned at
 * (coordinates[0], coordinates[1]) and the bottom right corner is
 * positioned at (coordinates[2], coordinates[3]). The positions for
 * the second rectangle are (coordinates[4], coordinates[5]) and
 * (coordinates[6], coordinates[7]) and so on...
 *
 * The position is the position before the rectangle has been
 * transformed by the model-view matrix and the projection
 * matrix.
 *
 * As a general rule for better performance its recommended to use
 * this this API instead of calling
 * cogl_framebuffer_draw_textured_rectangle() separately for multiple
 * rectangles if all of the rectangles will be drawn together with the
 * same @pipeline state.
 */
COGL_EXPORT void
cogl_framebuffer_draw_rectangles (CoglFramebuffer *framebuffer,
                                  CoglPipeline *pipeline,
                                  const float *coordinates,
                                  unsigned int n_rectangles);

/**
 * cogl_framebuffer_draw_textured_rectangles:
 * @framebuffer: A destination #CoglFramebuffer
 * @pipeline: A #CoglPipeline state object
 * @coordinates: (in) (array) (transfer none): an array containing
 *   groups of 8 float values: [x_1, y_1, x_2, y_2, s_1, t_1, s_2, t_2]
 *   that have the same meaning as the arguments for
 *   cogl_framebuffer_draw_textured_rectangle().
 * @n_rectangles: number of rectangles to @coordinates to draw
 *
 * Draws a series of rectangles to @framebuffer with the given
 * @pipeline state in the same way that
 * cogl_framebuffer_draw_textured_rectangle() does.
 *
 * The position is the position before the rectangle has been
 * transformed by the model-view matrix and the projection
 * matrix.
 *
 * This is a high level drawing api that can handle any kind of
 * #CoglTexture texture such as #CoglTexture2DSliced textures
 * which may internally be comprised of multiple low-level textures.
 * This is unlike low-level drawing apis such as cogl_primitive_draw()
 * which only support low level texture types that are directly
 * supported by GPUs such as #CoglTexture2D.
 *
 * The top left corner of the first rectangle is positioned at
 * (coordinates[0], coordinates[1]) and the bottom right corner is
 * positioned at (coordinates[2], coordinates[3]). The top left
 * texture coordinate is (coordinates[4], coordinates[5]) and the
 * bottom right texture coordinate is (coordinates[6],
 * coordinates[7]). The coordinates for subsequent rectangles
 * are defined similarly by the subsequent coordinates.
 *
 * As a general rule for better performance its recommended to use
 * this this API instead of calling
 * cogl_framebuffer_draw_textured_rectangle() separately for multiple
 * rectangles if all of the rectangles will be drawn together with the
 * same @pipeline state.
 *
 * The given texture coordinates should always be normalized such that
 * (0, 0) corresponds to the top left and (1, 1) corresponds to the
 * bottom right. To map an entire texture across the rectangle pass
 * in tex_coords[0]=0, tex_coords[1]=0, tex_coords[2]=1,
 * tex_coords[3]=1.
 */
COGL_EXPORT void
cogl_framebuffer_draw_textured_rectangles (CoglFramebuffer *framebuffer,
                                           CoglPipeline *pipeline,
                                           const float *coordinates,
                                           unsigned int n_rectangles);

/* XXX: Should we take an n_buffers + buffer id array instead of using
 * the CoglBufferBits type which doesn't seem future proof? */
/**
 * cogl_framebuffer_discard_buffers:
 * @framebuffer: A #CoglFramebuffer
 * @buffers: A #CoglBufferBit mask of which ancillary buffers you want
 *           to discard.
 *
 * Declares that the specified @buffers no longer need to be referenced
 * by any further rendering commands. This can be an important
 * optimization to avoid subsequent frames of rendering depending on
 * the results of a previous frame.
 *
 * For example; some tile-based rendering GPUs are able to avoid allocating and
 * accessing system memory for the depth and stencil buffer so long as these
 * buffers are not required as input for subsequent frames and that can save a
 * significant amount of memory bandwidth used to save and restore their
 * contents to system memory between frames.
 *
 * It is currently considered an error to try and explicitly discard the color
 * buffer by passing %COGL_BUFFER_BIT_COLOR. This is because the color buffer is
 * already implicitly discard when you finish rendering to a #CoglOnscreen
 * framebuffer, and it's not meaningful to try and discard the color buffer of
 * a #CoglOffscreen framebuffer since they are single-buffered.
 *
 *
 */
COGL_EXPORT void
cogl_framebuffer_discard_buffers (CoglFramebuffer *framebuffer,
                                  unsigned long buffers);

/**
 * cogl_framebuffer_finish:
 * @framebuffer: A #CoglFramebuffer pointer
 *
 * This blocks the CPU until all pending rendering associated with the
 * specified framebuffer has completed. It's very rare that developers should
 * ever need this level of synchronization with the GPU and should never be
 * used unless you clearly understand why you need to explicitly force
 * synchronization.
 *
 * One example might be for benchmarking purposes to be sure timing
 * measurements reflect the time that the GPU is busy for not just the time it
 * takes to queue rendering commands.
 */
COGL_EXPORT void
cogl_framebuffer_finish (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_read_pixels_into_bitmap:
 * @framebuffer: A #CoglFramebuffer
 * @x: The x position to read from
 * @y: The y position to read from
 * @source: Identifies which auxiliary buffer you want to read
 *          (only COGL_READ_PIXELS_COLOR_BUFFER supported currently)
 * @bitmap: The bitmap to store the results in.
 *
 * This reads a rectangle of pixels from the given framebuffer where
 * position (0, 0) is the top left. The pixel at (x, y) is the first
 * read, and a rectangle of pixels with the same size as the bitmap is
 * read right and downwards from that point.
 *
 * Currently Cogl assumes that the framebuffer is in a premultiplied
 * format so if the format of @bitmap is non-premultiplied it will
 * convert it. To read the pixel values without any conversion you
 * should either specify a format that doesn't use an alpha channel or
 * use one of the formats ending in PRE.
 *
 * Return value: %TRUE if the read succeeded or %FALSE otherwise. The
 *  function is only likely to fail if the bitmap points to a pixel
 *  buffer and it could not be mapped.
 */
COGL_EXPORT gboolean
cogl_framebuffer_read_pixels_into_bitmap (CoglFramebuffer *framebuffer,
                                          int x,
                                          int y,
                                          CoglReadPixelsFlags source,
                                          CoglBitmap *bitmap);

/**
 * cogl_framebuffer_read_pixels:
 * @framebuffer: A #CoglFramebuffer
 * @x: The x position to read from
 * @y: The y position to read from
 * @width: The width of the region of rectangles to read
 * @height: The height of the region of rectangles to read
 * @format: The pixel format to store the data in
 * @pixels: The address of the buffer to store the data in
 *
 * This is a convenience wrapper around
 * cogl_framebuffer_read_pixels_into_bitmap() which allocates a
 * temporary #CoglBitmap to read pixel data directly into the given
 * buffer. The rowstride of the buffer is assumed to be the width of
 * the region times the bytes per pixel of the format. The source for
 * the data is always taken from the color buffer. If you want to use
 * any other rowstride or source, please use the
 * cogl_framebuffer_read_pixels_into_bitmap() function directly.
 *
 * The implementation of the function looks like this:
 *
 * ```c
 * bitmap = cogl_bitmap_new_for_data (context,
 *                                    width, height,
 *                                    format,
 *                                    /<!-- -->* rowstride *<!-- -->/
 *                                    bpp * width,
 *                                    pixels);
 * cogl_framebuffer_read_pixels_into_bitmap (framebuffer,
 *                                           x, y,
 *                                           COGL_READ_PIXELS_COLOR_BUFFER,
 *                                           bitmap);
 * g_object_unref (bitmap);
 * ```
 *
 * Return value: %TRUE if the read succeeded or %FALSE otherwise.
 */
COGL_EXPORT gboolean
cogl_framebuffer_read_pixels (CoglFramebuffer *framebuffer,
                              int x,
                              int y,
                              int width,
                              int height,
                              CoglPixelFormat format,
                              uint8_t *pixels);

COGL_EXPORT uint32_t
cogl_framebuffer_error_quark (void);

/**
 * COGL_FRAMEBUFFER_ERROR:
 *
 * An error domain for reporting #CoglFramebuffer exceptions
 */
#define COGL_FRAMEBUFFER_ERROR (cogl_framebuffer_error_quark ())

typedef enum /*< prefix=COGL_FRAMEBUFFER_ERROR >*/
{
  COGL_FRAMEBUFFER_ERROR_ALLOCATE
} CoglFramebufferError;

/**
 * cogl_framebuffer_blit:
 * @framebuffer: The source #CoglFramebuffer
 * @dst: The destination #CoglFramebuffer
 * @src_x: Source x position
 * @src_y: Source y position
 * @dst_x: Destination x position
 * @dst_y: Destination y position
 * @width: Width of region to copy
 * @height: Height of region to copy
 * @error: optional error object
 *
 * @return FALSE for an immediately detected error, TRUE otherwise.
 *
 * This blits a region of the color buffer of the source buffer
 * to the destination buffer. This function should only be
 * called if the COGL_FEATURE_ID_BLIT_FRAMEBUFFER feature is
 * advertised.
 *
 * The source and destination rectangles are defined in offscreen
 * framebuffer orientation. When copying between an offscreen and
 * onscreen framebuffers, the image is y-flipped accordingly.
 *
 * The two buffers must have the same value types (e.g. floating-point,
 * unsigned int, signed int, or fixed-point), but color formats do not
 * need to match. This limitation comes from OpenGL ES 3.0 definition
 * of glBlitFramebuffer.
 *
 * Note that this function differs a lot from the glBlitFramebuffer
 * function provided by the GL_EXT_framebuffer_blit extension. Notably
 * it doesn't support having different sizes for the source and
 * destination rectangle. This doesn't seem
 * like a particularly useful feature. If the application wanted to
 * scale the results it may make more sense to draw a primitive
 * instead.
 *
 * The GL function is documented to be affected by the scissor. This
 * function therefore ensure that an empty clip stack is flushed
 * before performing the blit which means the scissor is effectively
 * ignored.
 *
 * The function also doesn't support specifying the buffers to copy
 * and instead only the color buffer is copied. When copying the depth
 * or stencil buffers the extension on GLES2.0 only supports copying
 * the full buffer which would be awkward to document with this
 * API. If we wanted to support that feature it may be better to have
 * a separate function to copy the entire buffer for a given mask.
 *
 * The @c error argument is optional, it can be NULL. If it is not NULL
 * and this function returns FALSE, an error object with a code from
 * COGL_SYSTEM_ERROR will be created.
 */
COGL_EXPORT gboolean
cogl_framebuffer_blit (CoglFramebuffer *framebuffer,
                       CoglFramebuffer *dst,
                       int src_x,
                       int src_y,
                       int dst_x,
                       int dst_y,
                       int width,
                       int height,
                       GError **error);

/**
 * cogl_framebuffer_flush:
 * @framebuffer: A #CoglFramebuffer pointer
 *
 * Flushes @framebuffer to ensure the current batch of commands is
 * submitted to the GPU.
 *
 * Unlike cogl_framebuffer_finish(), this does not block the CPU.
 */
COGL_EXPORT void
cogl_framebuffer_flush (CoglFramebuffer *framebuffer);

/**
 * cogl_framebuffer_create_timestamp_query: (skip)
 *
 * Creates a query for the GPU timestamp that will complete upon completion of
 * all previously submitted GL commands related to this framebuffer. E.g. when
 * the rendering is finished on this framebuffer.
 *
 * This function should only be called if the COGL_FEATURE_ID_TIMESTAMP_QUERY
 * feature is advertised.
 */
COGL_EXPORT CoglTimestampQuery *
cogl_framebuffer_create_timestamp_query (CoglFramebuffer *framebuffer);


/**
 * cogl_framebuffer_get_internal_format: (skip)
 *
 * Returns the pixel format used internally by the framebuffer.
 */
COGL_EXPORT CoglPixelFormat
cogl_framebuffer_get_internal_format (CoglFramebuffer *framebuffer);

G_END_DECLS
