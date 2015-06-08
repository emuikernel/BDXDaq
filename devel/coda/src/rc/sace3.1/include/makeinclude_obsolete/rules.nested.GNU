#----------------------------------------------------------------------------
#	%W%	%G%
#
#	Nested directory targets makefile definitions
#----------------------------------------------------------------------------

$(TARGETS_NESTED):
	@for dir in $(DIRS) ;\
	do \
		(cd $$dir ; \
		 echo "" ; \
		 echo " ========> Making $(@:.nested=) $(MAKEFLAGS): `pwd`"; \
		 $(MAKE) $(@:.nested=) ; \
		 echo "<========     End $(@:.nested=): `pwd`") \
	done
