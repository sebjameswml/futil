
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil testsymlinks testdirperms testSetPerm testdates dateToEpoch testSplitString testtotalspace testRandomString testlock splitstring stringtovector

testdirperms_SOURCES = testdirperms.cpp
testdirperms_LDADD = futil/libfutil.la -luuid -lmagic

testRandomString_SOURCES = testRandomString.cpp
testRandomString_LDADD = futil/libfutil.la -luuid -lmagic

testsymlinks_SOURCES = testsymlinks.cpp
testsymlinks_LDADD = futil/libfutil.la -luuid -lmagic

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid -lmagic

splitstring_SOURCES = splitstring.cpp stringtosplit.h
splitstring_LDADD = futil/libfutil.la -luuid -lmagic

stringtovector_SOURCES = stringtovector.cpp stringtosplit.h
stringtovector_LDADD = futil/libfutil.la -luuid -lmagic

testdates_SOURCES = testdates.cpp
testdates_LDADD = futil/libfutil.la -luuid -lmagic

dateToEpoch_SOURCES = dateToEpoch.cpp
dateToEpoch_LDADD = futil/libfutil.la -luuid -lmagic

testDoIconv_SOURCES = testDoIconv.cpp
testDoIconv_LDADD = futil/libfutil.la -luuid -lmagic

testXmlCondition_SOURCES = testXmlCondition.cpp
testXmlCondition_LDADD = futil/libfutil.la -luuid -lmagic

testSetPerm_SOURCES = testSetPerm.cpp
testSetPerm_LDADD = futil/libfutil.la -luuid -lmagic

testSplitString_SOURCES = testSplitString.cpp
testSplitString_LDADD = futil/libfutil.la -luuid -lmagic

testtotalspace_SOURCES = testtotalspace.cpp
testtotalspace_LDADD = futil/libfutil.la -luuid -lmagic

testlock_SOURCES = testlock.cpp
testlock_LDADD = futil/libfutil.la -luuid -lmagic

INCLUDES = -I./futil

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

