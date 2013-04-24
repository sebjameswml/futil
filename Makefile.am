
AUTOMAKE_OPTIONS = check-news dist-bzip2

SUBDIRS = futil doc

bin_PROGRAMS = testDoIconv testXmlCondition testfutil testsymlinks testdirperms testSetPerm testdates dateToEpoch testSplitString testSplitString2 testtotalspace testRandomString testlock splitstring stringtovector emptydirs logmsg

check_PROGRAMS = testXmlEscape testStringToWords testFilesDiffer testRC4 testValidMac testWrapLine

TESTS = testXmlEscape testStringToWords testFilesDiffer testRC4 testValidMac testWrapLine

logmsg_SOURCES = logmsg.cpp
# no LDADD

testdirperms_SOURCES = testdirperms.cpp
testdirperms_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testRandomString_SOURCES = testRandomString.cpp
testRandomString_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testsymlinks_SOURCES = testsymlinks.cpp
testsymlinks_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testfutil_SOURCES = testfutil.cpp
testfutil_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

emptydirs_SOURCES = emptydirs.cpp
emptydirs_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

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

testSplitString2_SOURCES = testSplitString2.cpp
testSplitString2_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testtotalspace_SOURCES = testtotalspace.cpp
testtotalspace_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testlock_SOURCES = testlock.cpp
testlock_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testXmlEscape_SOURCES = testXmlEscape.cpp
testXmlEscape_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testStringToWords_SOURCES = testStringToWords.cpp
testStringToWords_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testFilesDiffer_SOURCES = testFilesDiffer.cpp
testFilesDiffer_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testRC4_SOURCES = testRC4.cpp
testRC4_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testValidMac_SOURCES = testValidMac.cpp
testValidMac_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

testWrapLine_SOURCES = testWrapLine.cpp
testWrapLine_LDADD = futil/libfutil.la -luuid -lmagic $(GLIBMM_LIBS)

INCLUDES = -I./futil $(GLIBMM_CFLAGS)

CLEANFILES = *~

MAINTAINERCLEANFILES = aclocal.m4 futil-*.tar.gz futil-*.tar.bz2 futil-*.diff

