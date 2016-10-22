AUTOMAKE_OPTIONS = 1.4

YFLAGS = -d
LFLAGS = -olex.yy.c

libformula_la_SOURCES = parser.yy formula.cc prettyprint.cc negationpush.cc typecheck.cc type.cc formulabuild.cc formulatransform.cc formulasubstitute.cc lexer.ll formula-parser.cc
noinst_HEADERS = \
formulabuild.h \
formula.h \
formulamod.h \
formula-parser.h \
formulasubstitute.h \
formulatransform.h \
negationpush.h \
parser-types.h \
prettyprint.h \
ref.h \
typecheck.h \
type.h

libformula_la_LIBADD = @LEXLIB@

noinst_LTLIBRARIES = libformula.la


