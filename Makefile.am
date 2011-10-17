
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil testsymlinks testdirperms testSetPerm testdates dateToEpoch testSplitString testtotalspace testRandomString testlock splitstring stringtovector

testdirperms_SOURCES = testdirperms.cpp
testdirperms_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testRandomString_SOURCES = testRandomString.cpp
testRandomString_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testsymlinks_SOURCES = testsymlinks.cpp
testsymlinks_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

splitstring_SOURCES = splitstring.cpp stringtosplit.h
splitstring_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

stringtovector_SOURCES = stringtovector.cpp stringtosplit.h
stringtovector_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testdates_SOURCES = testdates.cpp
testdates_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

dateToEpoch_SOURCES = dateToEpoch.cpp
dateToEpoch_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testDoIconv_SOURCES = testDoIconv.cpp
testDoIconv_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testXmlCondition_SOURCES = testXmlCondition.cpp
testXmlCondition_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testSetPerm_SOURCES = testSetPerm.cpp
testSetPerm_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testSplitString_SOURCES = testSplitString.cpp
testSplitString_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testtotalspace_SOURCES = testtotalspace.cpp
testtotalspace_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

testlock_SOURCES = testlock.cpp
testlock_LDADD = futil/libfutil.la -luuid -lmagic -lglib-2.0 -lglibmm-2.4

INCLUDES = -I./futil $(GLIBMM_CFLAGS)

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

