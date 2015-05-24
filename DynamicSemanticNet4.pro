#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T14:20:34
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = DynamicSemanticNet
TEMPLATE = app

CONFIG += c++11

QMAKE_MAC_SDK = macosx10.9
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+

SOURCES += main.cpp\
        widget.cpp \
    fileloader.cpp \
    kifloader.cpp \
    dynamicsemanticnet.cpp \
    PropNet/proposition.cpp \
    PropNet/propositionand.cpp \
    PropNet/propositionor.cpp \
    PropNet/propositionnot.cpp \
    PropNet/propositionconstant.cpp \
    PropNet/propositiondatabase.cpp \
    Logic/logic_relation.cpp \
    Logic/logic_rule.cpp \
    Logic/logic.cpp \
    Logic/logic_term.cpp \
    Unification/unification_equation.cpp \
    Unification/unification_problem.cpp \
    parser.cpp \
    StateMachine/statemachine.cpp \
    StateMachine/proverstatemachine.cpp \
    StateMachine/machinestate.cpp \
    StateMachine/role.cpp \
    StateMachine/move.cpp \
    knowledgebase.cpp

HEADERS  += widget.h \
    fileloader.h \
    kifloader.h \
    dynamicsemanticnet.h \
    tree.hh \
    Trilogic.h \
    PropNet/proposition.h \
    PropNet/propositionand.h \
    PropNet/propositionor.h \
    PropNet/propositionnot.h \
    PropNet/propositionconstant.h \
    PropNet/propositiondatabase.h \
    Logic/logic_relation.h \
    Logic/logic_rule.h \
    Logic/logic.h \
    Logic/logic_term.h \
    Unification/unification_equation.h \
    Unification/unification_problem.h \
    parser.h \
    StateMachine/statemachine.h \
    StateMachine/proverstatemachine.h \
    StateMachine/machinestate.h \
    StateMachine/role.h \
    StateMachine/move.h \
    knowledgebase.h

FORMS    += widget.ui
