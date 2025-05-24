all_include_dirs = [wayfire_api_inc, wayfire_conf_inc, plugins_common_inc, include_directories('.')]
all_deps = [wlroots, wfconfig, wftouch]

shared_module('mobile-tweaks', ['mobile-tweaks-plugin.cpp', 'dpms-ctrl.cpp'],
        include_directories: all_include_dirs,
        dependencies: all_deps,
        install: true,
        install_dir: conf_data.get('PLUGIN_PATH'))