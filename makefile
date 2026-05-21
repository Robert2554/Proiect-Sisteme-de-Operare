build1:
	gcc -Wall -o city_manager src/main.c src/city_manager.c src/filter_functions.c

build2:
	gcc -Wall -o monitor_reports src/monitor_reports.c

build3:
	gcc -Wall -o scorer src/scorer.c

build4:
	gcc -Wall -o city_hub src/city_hub.c