SUBDIRS = examples formula transducer engine modelchecker frontend structure

bin_PROGRAMS = rmc

rmc_SOURCES = 
rmc_LDADD = frontend/libfrontend.la librmc.la 

librmc_la_SOURCES = 
librmc_la_LIBADD = formula/libformula.la transducer/libtransducer.la engine/libengine.la modelchecker/libmodelchecker.la structure/libstructure.la @GAUTOMATA_LIBS@
noinst_LTLIBRARIES = librmc.la

if HAS_DOXYGEN

doxygen::
	doxygen Doxyfile

docdir = $(prefix)/share/doc/@PACKAGE@-@VERSION@

install-data-local: doxygen
	$(mkinstalldirs) $(DESTDIR)$(docdir)/html
	$(INSTALL_DATA) $(srcdir)/docs/html/* $(DESTDIR)$(docdir)/html
endif

