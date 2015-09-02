#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T14:20:34
#
#-------------------------------------------------

QT       += core gui widgets testlib network

TARGET = DynamicSemanticNet
TEMPLATE = app

CONFIG += c++11

QMAKE_MAC_SDK = macosx10.9
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+

SOURCES += main.cpp\
    fileloader.cpp \
    kifloader.cpp \
    dynamicsemanticnet.cpp \
    PropNet/proposition.cpp \
    PropNet/propositionconstant.cpp \
    PropNet/propositiondatabase.cpp \
    Logic/logic_relation.cpp \
    Logic/logic_rule.cpp \
    Logic/logic.cpp \
    Logic/logic_term.cpp \
    parser.cpp \
    StateMachine/statemachine.cpp \
    StateMachine/proverstatemachine.cpp \
    StateMachine/machinestate.cpp \
    StateMachine/role.cpp \
    StateMachine/move.cpp \
    Unification/unification_term.cpp \
    Unification/unification_relation.cpp \
    UnitTest/logic_test.cpp \
    UnitTest/unification_test.cpp \
    UnitTest/knowledgebase_test.cpp \
    Prover/gdlprover.cpp \
    UnitTest/gdlprover_test.cpp \
    flags.cpp \
    UnitTest/proverstatemachine_test.cpp \
    Player/firstplayer.cpp \
    Player/player.cpp \
    UnitTest/player_test.cpp \
    Player/montecarlogamer.cpp \
    Prover/propnetprover.cpp \
    Prover/knowledgebase.cpp \
    UnitTest/propnet_test.cpp \
    UnitTest/unittest.cpp \
    PropNet/component.cpp \
    PropNet/componentor.cpp \
    PropNet/componentand.cpp \
    PropNet/componentnot.cpp \
    StateMachine/propnetstatemachine.cpp \
    UnitTest/propnetstatemachine_test.cpp \
    PropNet/propositionoptimized.cpp \
    Server/server.cpp \
    Server/servernetwork.cpp \
    mainwidget.cpp \
    serverwidget.cpp \
    playerwidget.cpp \
    Server/serverii.cpp \
    kifwidget.cpp \
    Player/playernetwork.cpp


HEADERS  += \
    fileloader.h \
    kifloader.h \
    dynamicsemanticnet.h \
    tree.hh \
    Trilogic.h \
    PropNet/proposition.h \
    PropNet/propositionconstant.h \
    PropNet/propositiondatabase.h \
    Logic/logic_relation.h \
    Logic/logic_rule.h \
    Logic/logic.h \
    Logic/logic_term.h \
    parser.h \
    StateMachine/statemachine.h \
    StateMachine/proverstatemachine.h \
    StateMachine/machinestate.h \
    StateMachine/role.h \
    StateMachine/move.h \
    Unification/unification_term.h \
    Unification/unification_relation.h \
    UnitTest/logic_test.h \
    UnitTest/unification_test.h \
    UnitTest/knowledgebase_test.h \
    Prover/gdlprover.h \
    flags.h \
    UnitTest/gdlprover_test.h \
    UnitTest/proverstatemachine_test.h \
    Player/firstplayer.h \
    Player/player.h \
    UnitTest/player_test.h \
    Player/montecarlogamer.h \
    Prover/propnetprover.h \
    Prover/knowledgebase.h \
    UnitTest/propnet_test.h \
    UnitTest/unittest.h \
    PropNet/component.h \
    PropNet/componentor.h \
    PropNet/componentand.h \
    PropNet/componentnot.h \
    StateMachine/propnetstatemachine.h \
    UnitTest/propnetstatemachine_test.h \
    PropNet/propositionoptimized.h \
    Server/server.h \
    Server/servernetwork.h \
    mainwidget.h \
    serverwidget.h \
    playerwidget.h \
    Server/serverii.h \
    kifwidget.h \
    Player/playernetwork.h

FORMS    += widget.ui
