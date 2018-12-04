@echo off
rem export.bat
rem This batch program exports all of the models, scenes, and animations

rem Temporarily changes the current directory to that of the batch script
cd %~dp0

set DIST="../dist"

rem Export the Magpie mesh data
blender --background --python export-meshes.py -- magpie_idle.blend:1 %DIST%/magpie/magpie_idle.pnc
blender --background --python export-meshes.py -- magpie_walk.blend:1 %DIST%/magpie/magpie_walk.pnc
blender --background --python export-meshes.py -- magpie_steal.blend:1 %DIST%/magpie/magpie_steal.pnc
blender --background --python export-meshes.py -- magpie_break.blend:1 %DIST%/magpie/magpie_break.pnc
blender --background --python export-meshes.py -- magpie_pick.blend:1 %DIST%/magpie/magpie_pick.pnc
blender --background --python export-meshes.py -- magpieDisguise_idle.blend:1 %DIST%/magpie/magpieDisguise_idle.pnc
blender --background --python export-meshes.py -- magpieDisguise_walk.blend:1 %DIST%/magpie/magpieDisguise_walk.pnc

rem Export the Guard Dog mesh data
blender --background --python export-meshes.py -- guardDog_alert.blend:1 %DIST%/guardDog/guardDog_alert.pnc
blender --background --python export-meshes.py -- guardDog_chase.blend:1 %DIST%/guardDog/guardDog_chase.pnc
blender --background --python export-meshes.py -- guardDog_patrol.blend:1 %DIST%/guardDog/guardDog_patrol.pnc
blender --background --python export-meshes.py -- guardDog_confused.blend:1 %DIST%/guardDog/guardDog_confused.pnc
blender --background --python export-meshes.py -- guardDog_cautious.blend:1 %DIST%/guardDog/guardDog_cautious.pnc
blender --background --python export-meshes.py -- guardDog_idle.blend:1 %DIST%/guardDog/guardDog_idle.pnc
blender --background --python export-meshes.py -- guardDog_eat.blend:1 %DIST%/guardDog/guardDog_eat.pnc

rem Export bounding box meshes for debuging
blender --background --python export-meshes.py -- bounding_box.blend:1 %DIST%/levels/bounding_box.pnc

rem Export the Donut
blender --background --python export-meshes.py -- donut.blend:1 %DIST%/levels/donut.pnc

rem Export the level scenery mesh data
blender --background --python export-meshes.py -- building-tiles.blend:1 %DIST%/levels/building_tiles.pnc

rem Export door model animation and mesh data
blender --background --python export-meshes.py -- door_openAnim.blend:1 %DIST%/levels/door.pnc
blender --background --python export-scene.py -- door_openAnim.blend:1 %DIST%/levels/door.model
blender --background --python export-transform-animation-hierarchy.py -- door_openAnim.blend door_purple_open_GRP 0 15 %DIST%/levels/door.tanim

rem Export green door model animation and mesh data
blender --background --python export-meshes.py -- doorGreen_openAnim.blend:1 %DIST%/levels/door_green.pnc
blender --background --python export-scene.py -- doorGreen_openAnim.blend:1 %DIST%/levels/door_green.model
blender --background --python export-transform-animation-hierarchy.py -- doorGreen_openAnim.blend door_keycardGreen_open_GRP 0 15 %DIST%/levels/door_green.tanim

rem Export pink door model animation and mesh data
blender --background --python export-meshes.py -- doorPink_openAnim.blend:1 %DIST%/levels/door_pink.pnc
blender --background --python export-scene.py -- doorPink_openAnim.blend:1 %DIST%/levels/door_pink.model
blender --background --python export-transform-animation-hierarchy.py -- doorPink_openAnim.blend door_keycardPink_open_GRP 0 15 %DIST%/levels/door_pink.tanim

rem Export FrontDoorModels
blender --background --python export-scene.py -- building-tiles.blend:1 %DIST%/levels/building_tiles.model

rem Export the Magpie model as a scene to preserve the transform hierarchy of character models
blender --background --python export-scene.py -- magpie_idle.blend:1 %DIST%/magpie/magpie_idle.model
blender --background --python export-scene.py -- magpie_walk.blend:1 %DIST%/magpie/magpie_walk.model
blender --background --python export-scene.py -- magpie_steal.blend:1 %DIST%/magpie/magpie_steal.model
blender --background --python export-scene.py -- magpie_pick.blend:1 %DIST%/magpie/magpie_pick.model
blender --background --python export-scene.py -- magpie_break.blend:1 %DIST%/magpie/magpie_break.model
blender --background --python export-scene.py -- magpieDisguise_idle.blend:1 %DIST%/magpie/magpieDisguise_idle.model
blender --background --python export-scene.py -- magpieDisguise_walk.blend:1 %DIST%/magpie/magpieDisguise_walk.model

rem Export the Guard Dog model as a scene to preserve the transform hierarchy of character models
blender --background --python export-scene.py -- guardDog_alert.blend:1 %DIST%/guardDog/guardDog_alert.model
blender --background --python export-scene.py -- guardDog_chase.blend:1 %DIST%/guardDog/guardDog_chase.model
blender --background --python export-scene.py -- guardDog_patrol.blend:1 %DIST%/guardDog/guardDog_patrol.model
blender --background --python export-scene.py -- guardDog_cautious.blend:1 %DIST%/guardDog/guardDog_cautious.model
blender --background --python export-scene.py -- guardDog_confused.blend:1 %DIST%/guardDog/guardDog_confused.model
blender --background --python export-scene.py -- guardDog_idle.blend:1 %DIST%/guardDog/guardDog_idle.model
blender --background --python export-scene.py -- guardDog_eat.blend:1 %DIST%/guardDog/guardDog_eat.model

rem Export the Magpie animations
blender --background --python export-transform-animation-hierarchy.py -- magpie_walk.blend magpieWalk_GRP 0 20 %DIST%/magpie/magpie_walk.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_idle.blend magpieIdle_GRP 0 60 %DIST%/magpie/magpie_idle.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_steal.blend magpieSteal_GRP 0 30 %DIST%/magpie/magpie_steal.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_pick.blend magpiePick_GRP 0 15 %DIST%/magpie/magpie_pick.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpie_break.blend magpieBreak_GRP 0 15 %DIST%/magpie/magpie_break.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpieDisguise_idle.blend magpieDisguiseIdle_GRP 0 1 %DIST%/magpie/magpieDisguise_idle.tanim
blender --background --python export-transform-animation-hierarchy.py -- magpieDisguise_walk.blend magpieDisguiseWalk_GRP 0 40 %DIST%/magpie/magpieDisguise_walk.tanim


rem Export the Guard Dog animations
blender --background --python export-transform-animation-hierarchy.py -- guardDog_alert.blend guardDogAlert_GRP 0 15 %DIST%/guardDog/guardDog_alert.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_patrol.blend guardDogPatrol_GRP 0 40 %DIST%/guardDog/guardDog_patrol.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_chase.blend guardDogChase_GRP 0 20 %DIST%/guardDog/guardDog_chase.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_cautious.blend guardDogCautious_GRP 0 40 %DIST%/guardDog/guardDog_cautious.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_confused.blend guardDogConfused_GRP 0 30 %DIST%/guardDog/guardDog_confused.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_idle.blend guardDogIdle_GRP 0 60 %DIST%/guardDog/guardDog_idle.tanim
blender --background --python export-transform-animation-hierarchy.py -- guardDog_eat.blend guardDogEat_GRP 0 40 %DIST%/guardDog/guardDog_eat.tanim

rem Export scene for camera position
blender --background --python export-scene.py -- camera_transform.blend:1 %DIST%/levels/camera_transform.scene

rem Set the path back to where we were
cd %CD%