/*
 * Cogl
 *
 * A Low Level GPU Graphics and Utilities API
 *
 * Copyright (C) 2008,2009,2010,2011 Intel Corporation.
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

#include "config.h"

#include "cogl/cogl-context-private.h"
#include "cogl/cogl-debug.h"
#include "cogl/cogl-pipeline-private.h"
#include "cogl/cogl-pipeline-layer-private.h"

#include <glib.h>

typedef struct
{
  int parent_id;
  int *node_id_ptr;
  GString *graph;
  int indent;
} PrintDebugState;

static void
dump_layer_cb (CoglPipelineLayer *layer,
               gpointer           user_data)
{
  PrintDebugState *state = user_data;
  int layer_id = *state->node_id_ptr;
  PrintDebugState state_out;
  GString *changes_label;
  gboolean changes = FALSE;

  if (state->parent_id >= 0)
    g_string_append_printf (state->graph, "%*slayer%p -> layer%p;\n",
                            state->indent, "",
                            layer->parent,
                            layer);

  g_string_append_printf (state->graph,
                          "%*slayer%p [label=\"layer=0x%p\\n"
                          "ref count=%d\" "
                          "color=\"blue\"];\n",
                          state->indent, "",
                          layer,
                          layer,
                          G_OBJECT (layer)->ref_count);

  changes_label = g_string_new ("");
  g_string_append_printf (changes_label,
                          "%*slayer%p -> layer_state%d [weight=100];\n"
                          "%*slayer_state%d [shape=box label=\"",
                          state->indent, "",
                          layer,
                          layer_id,
                          state->indent, "",
                          layer_id);

  if (layer->differences & COGL_PIPELINE_LAYER_STATE_UNIT)
    {
      changes = TRUE;
      g_string_append_printf (changes_label,
                              "\\lunit=%u\\n",
                              layer->unit_index);
    }

  if (layer->differences & COGL_PIPELINE_LAYER_STATE_TEXTURE_DATA)
    {
      changes = TRUE;
      g_string_append_printf (changes_label,
                              "\\ltexture=%p\\n",
                              layer->texture);
    }

  if (changes)
    {
      g_string_append_printf (changes_label, "\"];\n");
      g_string_append (state->graph, changes_label->str);
    }

  g_string_free (changes_label, TRUE);

  state_out.parent_id = layer_id;

  state_out.node_id_ptr = state->node_id_ptr;
  (*state_out.node_id_ptr)++;

  state_out.graph = state->graph;
  state_out.indent = state->indent + 2;

  for (CoglPipelineLayer *child = layer->first_child;
       child != NULL;
       child = child->next_sibling)
    {
      dump_layer_cb (child, &state_out);
    }
}

static gboolean
dump_layer_ref_cb (CoglPipelineLayer *layer, void *data)
{
  PrintDebugState *state = data;
  int pipeline_id = *state->node_id_ptr;

  g_string_append_printf (state->graph,
                          "%*spipeline_state%d -> layer%p;\n",
                          state->indent, "",
                          pipeline_id,
                          layer);

  return TRUE;
}

static void
dump_pipeline_cb (CoglPipeline *pipeline,
                  gpointer      user_data)
{
  PrintDebugState *state = user_data;
  int pipeline_id = *state->node_id_ptr;
  PrintDebugState state_out;
  GString *changes_label;
  gboolean changes = FALSE;
  gboolean layers = FALSE;

  if (state->parent_id >= 0)
    g_string_append_printf (state->graph, "%*spipeline%d -> pipeline%d;\n",
                            state->indent, "",
                            state->parent_id,
                            pipeline_id);

  g_string_append_printf (state->graph,
                          "%*spipeline%d [label=\"pipeline=0x%p\\n"
                          "ref count=%d\\n"
                          "breadcrumb=\\\"%s\\\"\" color=\"red\"];\n",
                          state->indent, "",
                          pipeline_id,
                          pipeline,
                          G_OBJECT (pipeline)->ref_count,
#ifdef COGL_ENABLE_DEBUG
                          pipeline->has_static_breadcrumb ?
                          pipeline->static_breadcrumb : "NULL"
#else
                          "NULL"
#endif
                          );

  changes_label = g_string_new ("");
  g_string_append_printf (changes_label,
                          "%*spipeline%d -> pipeline_state%d [weight=100];\n"
                          "%*spipeline_state%d [shape=box label=\"",
                          state->indent, "",
                          pipeline_id,
                          pipeline_id,
                          state->indent, "",
                          pipeline_id);


  if (pipeline->differences & COGL_PIPELINE_STATE_COLOR)
    {
      g_autofree char *color = NULL;

      changes = TRUE;
      color = cogl_color_to_string (&pipeline->color);
      g_string_append_printf (changes_label,
                              "\\lcolor=%s\\n", color);
    }

  if (pipeline->differences & COGL_PIPELINE_STATE_BLEND)
    {
      changes = TRUE;
      g_string_append_printf (changes_label,
                              "\\lblend\\n");
    }

  if (pipeline->differences & COGL_PIPELINE_STATE_LAYERS)
    {
      changes = TRUE;
      layers = TRUE;
      g_string_append_printf (changes_label, "\\ln_layers=%d\\n",
                              pipeline->n_layers);
    }

  if (changes)
    {
      g_string_append_printf (changes_label, "\"];\n");
      g_string_append (state->graph, changes_label->str);
    }

  g_string_free (changes_label, TRUE);

  if (layers)
    {
      g_list_foreach (pipeline->layer_differences,
                      (GFunc)dump_layer_ref_cb,
                      state);
    }

  state_out.parent_id = pipeline_id;

  state_out.node_id_ptr = state->node_id_ptr;
  (*state_out.node_id_ptr)++;

  state_out.graph = state->graph;
  state_out.indent = state->indent + 2;

  for (CoglPipeline *child = pipeline->first_child;
       child != NULL;
       child = child->next_sibling)
    {
      dump_pipeline_cb (child, &state_out);
    }
}

/* This function is just here to be called from GDB so we don't really
   want to put a declaration in a header and we just add it here to
   avoid a warning */
void
_cogl_debug_dump_pipelines_dot_file (const char  *filename,
                                     CoglContext *ctx);

void
_cogl_debug_dump_pipelines_dot_file (const char  *filename,
                                     CoglContext *ctx)
{
  GString *graph;
  PrintDebugState layer_state;
  PrintDebugState pipeline_state;
  int layer_id = 0;
  int pipeline_id = 0;

  if (!ctx->default_pipeline)
    return;

  graph = g_string_new ("");
  g_string_append_printf (graph, "digraph {\n");

  layer_state.graph = graph;
  layer_state.parent_id = -1;
  layer_state.node_id_ptr = &layer_id;
  layer_state.indent = 0;
  dump_layer_cb (ctx->default_layer_0, &layer_state);

  pipeline_state.graph = graph;
  pipeline_state.parent_id = -1;
  pipeline_state.node_id_ptr = &pipeline_id;
  pipeline_state.indent = 0;
  dump_pipeline_cb (ctx->default_pipeline, &pipeline_state);

  g_string_append_printf (graph, "}\n");

  if (filename)
    g_file_set_contents (filename, graph->str, -1, NULL);
  else
    g_print ("%s", graph->str);

  g_string_free (graph, TRUE);
}
