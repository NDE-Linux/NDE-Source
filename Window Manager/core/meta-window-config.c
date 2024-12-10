/*
 * Copyright (C) 2024 Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "config.h"

#include "core/meta-window-config-private.h"

/**
 * MetaWindowConfig:
 *
 * An object representing the configuration of a top-level window
 *
 */
struct _MetaWindowConfig
{
  GObject parent;

  /* Whether this is an initial window configuration, cannot be changed by the callee */
  gboolean is_initial;

  /* The window geometry */
  MtkRectangle rect;

  gboolean is_fullscreen;
};

G_DEFINE_FINAL_TYPE (MetaWindowConfig, meta_window_config, G_TYPE_OBJECT)

enum
{
  PROP_0,

  PROP_RECT,
  PROP_IS_FULLSCREEN,

  PROP_LAST,
};

static GParamSpec *obj_props[PROP_LAST];

static void
meta_window_config_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  MetaWindowConfig *window_config = META_WINDOW_CONFIG (object);

  switch (prop_id)
    {
    case PROP_RECT:
      g_value_set_boxed (value, &window_config->rect);
      break;
    case PROP_IS_FULLSCREEN:
      g_value_set_boolean (value, window_config->is_fullscreen);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
meta_window_config_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  MetaWindowConfig *window_config = META_WINDOW_CONFIG (object);
  MtkRectangle *rect;

  switch (prop_id)
    {
    case PROP_RECT:
      rect = g_value_get_boxed (value);
      window_config->rect = *rect;
      break;
    case PROP_IS_FULLSCREEN:
      window_config->is_fullscreen = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
meta_window_config_class_init (MetaWindowConfigClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = meta_window_config_get_property;
  object_class->set_property = meta_window_config_set_property;

  obj_props[PROP_RECT] =
    g_param_spec_boxed ("rect", NULL, NULL,
                        MTK_TYPE_RECTANGLE,
                        G_PARAM_READWRITE |
                        G_PARAM_STATIC_STRINGS);

  obj_props[PROP_IS_FULLSCREEN] =
    g_param_spec_boolean ("is-fullscreen", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, PROP_LAST, obj_props);
}

static void
meta_window_config_init (MetaWindowConfig *window_config)
{
  window_config->rect = MTK_RECTANGLE_INIT (0, 0, 0, 0);
}

gboolean
meta_window_config_get_is_initial (MetaWindowConfig *window_config)
{
  return window_config->is_initial;
}

void
meta_window_config_set_rect (MetaWindowConfig *window_config,
                             MtkRectangle      rect)
{
  window_config->rect = rect;
}

MtkRectangle
meta_window_config_get_rect (MetaWindowConfig *window_config)
{
  return window_config->rect;
}

void
meta_window_config_set_is_fullscreen (MetaWindowConfig *window_config,
                                      gboolean          is_fullscreen)
{
  window_config->is_fullscreen = is_fullscreen;
}

void
meta_window_config_get_position (MetaWindowConfig *window_config,
                                 int              *x,
                                 int              *y)
{
  if (x)
    *x = window_config->rect.x;
  if (y)
    *y = window_config->rect.y;
}

void
meta_window_config_set_position (MetaWindowConfig *window_config,
                                 int               x,
                                 int               y)
{
  window_config->rect.x = x;
  window_config->rect.y = y;
}

void
meta_window_config_get_size (MetaWindowConfig *window_config,
                             int              *width,
                             int              *height)
{
  if (width)
    *width = window_config->rect.width;
  if (height)
    *height = window_config->rect.height;
}

void
meta_window_config_set_size (MetaWindowConfig *window_config,
                             int               width,
                             int               height)
{
  window_config->rect.width = width;
  window_config->rect.height = height;
}

gboolean
meta_window_config_get_is_fullscreen (MetaWindowConfig *window_config)
{
  return window_config->is_fullscreen;
}

MetaWindowConfig *
meta_window_config_new (void)
{
  return g_object_new (META_TYPE_WINDOW_CONFIG,
                       NULL);
}

MetaWindowConfig *
meta_window_config_initial_new (void)
{
  MetaWindowConfig *window_config;

  window_config = meta_window_config_new ();
  window_config->is_initial = TRUE;

  return window_config;
}
