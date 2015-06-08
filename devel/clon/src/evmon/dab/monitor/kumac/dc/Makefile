all: mon_map_print.kumac

mon_map_print.kumac: mon_map.kumac
	sed -e "s/min ... 1\;//g" -e s/colz/box/g \
		-e "s/e 2 3/e 1 3/g" \
		-e "s/read Region.//g" \
		mon_map.kumac > mon_map_print.kumac

clean:
	rm -f *_print.kumac
