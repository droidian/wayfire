plugins = [
  'move', 'resize', 'command', 'autostart', 'vswipe', 'wrot', 'expo',
  'switcher', 'fast-switcher', 'oswitch', 'place', 'invert',
  'fisheye', 'zoom', 'alpha', 'idle', 'extra-gestures', 'preserve-output',
  'wsets', 'xkb-bindings', 'autorotate-iio'
]

all_include_dirs = [wayfire_api_inc, wayfire_conf_inc, plugins_common_inc, vswitch_inc, wobbly_inc, grid_inc]
all_deps = [wlroots, pixman, wfconfig, wftouch, cairo, pango, pangocairo, json, giomm]

foreach plugin : plugins
  shared_module(plugin, plugin + '.cpp',
      include_directories: all_include_dirs,
      dependencies: all_deps,
      install: true,
      install_dir: conf_data.get('PLUGIN_PATH'))
endforeach
