#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T19:37:26
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += ../beaglebone/hw_drivers/bbb_motor_control

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dispensary_application
TEMPLATE = app


SOURCES += main.cpp\
        meds_dispensary.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_pwm.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_gpio.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_adc.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_util.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/motor.cpp \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_sensor.cpp

HEADERS  += meds_dispensary.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_adc.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_gpio.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_pwm.h \
    ../beaglebone/hw_drivers/bbb_motor_control/adafruit_bbio_util.h \
    ../beaglebone/hw_drivers/bbb_motor_control/gpio_sensor.h \
    ../beaglebone/hw_drivers/bbb_motor_control/i2c-dev.lm-sensors.h \
    ../beaglebone/hw_drivers/bbb_motor_control/ssd1306.h \
    ../beaglebone/hw_drivers/bbb_motor_control/motor.h

FORMS    += meds_dispensary.ui

target.path += /home/kdaaker/test_code
INSTALLS += target
