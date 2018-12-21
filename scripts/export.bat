@echo off
rem export.bat
rem This batch program exports all of the map images in this directory

rem Temporarily changes the current directory to that of the batch script
cd %~dp0

set DIST="../dist/levels/floorplans"

python export_level_bitmap.py demo-map-simple.png %DIST%/demo-map-simple.lvl --validate
python export_level_bitmap.py sample_map.png %DIST%/sample-map.lvl --validate
python export_level_bitmap.py demo-map.png %DIST%/demo-map.lvl --validate
python export_level_bitmap.py final-map-composed.png %DIST%/final-map.lvl --validate

rem Set the path back to where we were
cd %CD%