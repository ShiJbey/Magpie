@echo off
rem export.bat
rem This batch program exports all of the models, scenes, and animations

rem Temporarily changes the current directory to that of the batch script
cd %~dp0

set DIST="../dist"

rem Export the Magpie mesh data
blender --background --python export-meshes.py -- magpie.blend:1 %DIST%/magpie.pnc

rem Export the Magpie model as a scene to preserve the transform hierarchy
blender --background --python export-scene.py -- magpie.blend:1 %DIST%/magpie.scene

rem Export the Magpie animations
blender --background --python export-transform-animation-hierarchy.py -- magpie.blend magpieWalk_GRP 0 60 %DIST%/player_walk.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie.blend magpieIdle_GRP 0 60 %DIST%/player_idle.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie.blend magpieSteal_GRP 0 60 %DIST%/player_steal.tanim

rem Set the path back to where we were
cd %CD%