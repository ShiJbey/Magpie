
DIST="../dist/levels/floorplans"

demo_map: demo_map.png
	python export_level_bitmap.py demo-map.png $(DIST)/demo-map.lvl --validate

final_map: final-map-composed.png
	python export_level_bitmap.py final-map-composed.png $(DIST)/final-map.lvl --validate

all: demo_map final_map

clean:
	rm $(DIST)/final-map.lvl $(DIST)/demo_map.lvl