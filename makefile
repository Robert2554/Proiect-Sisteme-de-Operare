build1:
	gcc -Wall -o city_manager src/main.c src/city_manager.c src/filter_functions.c

build2:
	gcc -Wall -o monitor_reports src/monitor_reports.c