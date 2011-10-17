
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil testsymlinks testdirperms testSetPerm testdates dateToEpoch testSplitString testtotalspace testRandomString testlock splitstring stringtovector

testdirperms_SOURCES = testdirperms.cpp
testdirperms_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testRandomString_SOURCES = testRandomString.cpp
testRandomString_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testsymlinks_SOURCES = testsymlinks.cpp
testsymlinks_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

splitstring_SOURCES = splitstring.cpp stringtosplit.h
splitstring_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

stringtovector_SOURCES = stringtovector.cpp stringtosplit.h
stringtovector_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testdates_SOURCES = testdates.cpp
testdates_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

dateToEpoch_SOURCES = dateToEpoch.cpp
dateToEpoch_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testDoIconv_SOURCES = testDoIconv.cpp
testDoIconv_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testXmlCondition_SOURCES = testXmlCondition.cpp
testXmlCondition_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testSetPerm_SOURCES = testSetPerm.cpp
testSetPerm_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testSplitString_SOURCES = testSplitString.cpp
testSplitString_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testtotalspace_SOURCES = testtotalspace.cpp
testtotalspace_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testlock_SOURCES = testlock.cpp
testlock_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

INCLUDES = -I./futil $(GLIBMM_CFLAGS)

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

