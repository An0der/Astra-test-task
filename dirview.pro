QT += widgets
requires(qtConfig(treeview))

SOURCES       = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/dirview
INSTALLS += target

# my includes
INCLUDEPATH += "./include"
INCLUDEPATH += "./src"
# add include dir to qtcreator
HEADERS += include/myheader.h
# added src's
SOURCES += src/my.cpp
