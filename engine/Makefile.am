AUTOMAKE_OPTIONS = 1.4

SUBDIRS = subset eqcol fixpoint tc linear

libengine_la_SOURCES = regular-composition.cc acceleration-strategy.cc
libengine_la_LIBADD = subset/libsubset.la eqcol/libeqcol.la fixpoint/libfixpoint.la tc/libtc.la linear/liblinear.la
noinst_LTLIBRARIES = libengine.la

noinst_HEADERS = engine.h regular-composition.h acceleration-strategy.h



