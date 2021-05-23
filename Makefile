# Development makefile, just some helpful aliases
.PHONY: memcheck
memcheck: 
	$(shell valgrind --leak-check=full \
					 --show-leak-kinds=all \
					 --track-origins=yes \
					 --verbose \
  				 	 build/dotEngine)

.PHONY: clean
clean:
	@rm -rf build/*

.PHONY: ctags
ctags:
	$(shell ctags --recurse=yes --exclude=.git --exclude=BUILD --exclude=.svn --exclude=vendor/* --exclude=node_modules/* --exclude=db/* --exclude=log/* .)
