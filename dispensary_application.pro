#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T19:37:26
#
#-------------------------------------------------

QT       += core gui network xml sql

QMAKE_CXXFLAGS += -std=c++11
# QMAKE_LFLAGS +=
INCLUDEPATH += ../beaglebone/hw_drivers/bbb_motor_control
INCLUDEPATH += /afs/aaker.org/home/kdaaker/src/smc_6_3_0/lib/C++

fsm_target_cpp.target = formulary_fsm_sm.cpp
fsm_target_cpp.depends = $$PWD/formulary_fsm.sm
fsm_target_cpp.commands = smc -g -d $$OUT_PWD $$fsm_target_cpp.depends

fsm_target_h.target = formulary_fsm_sm.h
fsm_target_h.depends = $$PWD/formulary_fsm.sm
fsm_target_h.commands = smc -d $$OUT_PWD $$fsm_target_h.depends

QMAKE_EXTRA_TARGETS += fsm_target_cpp fsm_target_h

QMAKE_CLEAN += $$fsm_target_h.target $$fsm_target_cpp.target

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dispensary_application
TEMPLATE = app

#LIBS += -lQxtZeroconf -lQxtCore -lQxtNetwork
LIBS += -lavahi-common -lavahi-client

SOURCES += main.cpp\
    meds_dispensary.cpp \
    $$fsm_target_cpp.target \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_pwm.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_gpio.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_adc.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_util.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/motor.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_keypad.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_sensor.cpp \
    dispensary_sslsock.cpp \
    qt-watch.cpp \
    zconfservice.cpp \
    zconfserviceclient.cpp

HEADERS  += meds_dispensary.h \
    $$fsm_target_h.target \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_adc.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_gpio.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_pwm.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_util.h \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_sensor.h \
    ../beaglebone/hw_drivers/bbb_motor_control/i2c-dev.lm-sensors.h \
    ../beaglebone/hw_drivers/bbb_motor_control/ssd1306.h \
    ../beaglebone/hw_drivers/bbb_motor_control/motor.h \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_keypad.h \
    dispensary_sslsock.h \
    qt-watch.h \
    zconfservice.h \
    zconfserviceclient.h

FORMS    += meds_dispensary.ui

target.path += /home/kdaaker/test_code
INSTALLS += target

OTHER_FILES += \
    formulary_fsm.sm
