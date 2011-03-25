#
# MeeGo Content Aggregation - Email Plugin
# Copyright © 2010, Intel Corporation.
#

VERSION = 0.0.5
TARGET = emailplugin
TEMPLATE = lib

CONFIG += plugin link_pkgconfig

PKGCONFIG += meegocontent meegoemail qmfclient

# use pkg-config paths for include in both g++ and moc
INCLUDEPATH += $$system(pkg-config --cflags meegocontent \
    | tr \' \' \'\\n\' | grep ^-I | cut -d 'I' -f 2-)

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    emailfeedmodel.cpp \
    emailplugin.cpp \
    emailservicemodel.cpp

HEADERS += \
    emailfeedmodel.h \
    emailplugin.h \
    emailservicemodel.h

target.path = $$[QT_INSTALL_PLUGINS]/meegocontent
INSTALLS += target

PROJECT_NAME = meego-content-plugin-email

dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION} &&
dist.commands += git clone . $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}/.git &&
dist.commands += rm -f $${PROJECT_NAME}-$${VERSION}/.gitignore &&
dist.commands += tar jcpvf $${PROJECT_NAME}-$${VERSION}.tar.bz2 $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION} &&
dist.commands += echo; echo Created $${PROJECT_NAME}-$${VERSION}.tar.bz2
QMAKE_EXTRA_TARGETS += dist
