
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid

testDoIconv_SOURCES = testDoIconv.cpp
testDoIconv_LDADD = futil/libfutil.la -luuid

testXmlCondition_SOURCES = testXmlCondition.cpp
testXmlCondition_LDADD = futil/libfutil.la -luuid

INCLUDES = -I./futil

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

