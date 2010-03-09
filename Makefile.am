
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil testsymlinks testdirperms testSetPerm

testdirperms_SOURCES = testdirperms.cpp
testdirperms_LDADD = futil/libfutil.la -luuid

testsymlinks_SOURCES = testsymlinks.cpp
testsymlinks_LDADD = futil/libfutil.la -luuid

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid

testDoIconv_SOURCES = testDoIconv.cpp
testDoIconv_LDADD = futil/libfutil.la -luuid

testXmlCondition_SOURCES = testXmlCondition.cpp
testXmlCondition_LDADD = futil/libfutil.la -luuid

testSetPerm_SOURCES = testSetPerm.cpp
testSetPerm_LDADD = futil/libfutil.la -luuid

INCLUDES = -I./futil

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

