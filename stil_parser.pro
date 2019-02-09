TEMPLATE = app
CONFIG += console c++11 c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lboost_system
SOURCES += \
        main.cpp \
    skipper.cpp \
    stil.cpp

HEADERS += \
    ast.hpp \
    ast_adapted.hpp \
    error_handler.hpp \
    config.hpp \
    skipper.hpp \
    common.hpp \
    skipper_def.hpp \
    stil.hpp \
    stil_def.hpp

DISTFILES += \
    test.stil
