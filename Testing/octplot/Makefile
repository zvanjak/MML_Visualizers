SUBMAKEDIRS = src

.PHONY: all install check icheck clean distclean dist $(SUBMAKEDIRS)

all: $(SUBMAKEDIRS)
	@echo "Build complete."

install: $(SUBMAKEDIRS)
	@echo " "
	@echo "Installation complete."
	@echo " "

clean: $(SUBMAKEDIRS)

distclean: 
	$(MAKE) clean

$(SUBMAKEDIRS):
	cd $@ && $(MAKE) $(MAKECMDGOALS)
