@echo off
rem export.bat
rem This batch program exports all of the models, scenes, and animations

rem Temporarily changes the current directory to that of the batch script
cd %~dp0

set DIST="../dist"

rem Export the Magpie mesh data
blender --background --python export-meshes.py -- magpie_idle.blend:1 %DIST%/magpie_idle.pnc
blender --background --python export-meshes.py -- magpie_walk.blend:1 %DIST%/magpie_walk.pnc
blender --background --python export-meshes.py -- magpie_steal.blend:1 %DIST%/magpie_steal.pnc

rem Export the guard dog data
blender --background --python export-meshes.py -- guard_dog.blend:1 %DIST%/guard_dog.pnc
blender --background --python export-meshes.py -- guardDog_chase.blend:1 %DIST%/guardDog_chase.pnc
blender --background --python export-meshes.py -- guardDog_patrol.blend:1 %DIST%/guardDog_patrol.pnc

rem Export the Magpie model as a scene to preserve the transform hierarchy of character models
blender --background --python export-scene.py -- magpie_idle.blend:1 %DIST%/magpie_idle.scene
blender --background --python export-scene.py -- magpie_walk.blend:1 %DIST%/magpie_walk.scene
blender --background --python export-scene.py -- magpie_steal.blend:1 %DIST%/magpie_steal.scene

blender --background --python export-scene.py -- guard_dog.blend:1 %DIST%/guard_dog.scene
blender --background --python export-scene.py -- guardDog_chase.blend:1 %DIST%/guardDog_chase.scene
blender --background --python export-scene.py -- guardDog_patrol.blend:1 %DIST%/guardDog_patrol.scene

rem Export the Magpie animations
blender --background --python export-transform-animation-hierarchy.py -- magpie_walk.blend magpieWalk_GRP 0 20 %DIST%/player_walk.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_idle.blend magpieIdle_GRP 0 60 %DIST%/player_idle.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_steal.blend magpieSteal_GRP 0 30 %DIST%/player_steal.tanim

blender --background --python export-transform-animation-hierarchy.py -- guardDog_patrol.blend guardDogPatrol_GRP 0 60 %DIST%/guardDog_patrol.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_chase.blend guardDogChase_GRP 0 60 %DIST%/guardDog_chase.tanim

rem Set the path back to where we were
cd %CD%