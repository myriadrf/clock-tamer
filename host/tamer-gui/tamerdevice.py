#!/usr/bin/python
# -*- coding: utf-8 -*-

# td.py

import sys
import string
import re
import array
import termios
import time
import serial
from ctypes import cdll, c_int, POINTER

defUsbDevice = "/dev/ttyACM0"

class UsbDevice(object):
    def __init__(self, devname=defUsbDevice):
        self.devname = devname
        self.dev = serial.Serial(self.devname, 115200, timeout=1)
        self.retries=0
        self.max_retries=1

        # turn off echo
        old = termios.tcgetattr(self.dev.fileno())
        old[3] = old[3] & ~termios.ECHO
        termios.tcsetattr(self.dev.fileno(), termios.TCSADRAIN, old)

    def close(self):
        self.dev.close()

    def enterGpsMode(self):
        ''' Leaves command mode and enables GPS data passing from the GPS module to a host'''
        string = "%%%"
        self.dev.write(string + "\r\n")
        self.dev.readline()
        self.dev.flushInput()
        print "CMD='%s' NORET" % (string, )
        return 0

    def leaveGpsMode(self):
        ''' Disables GPS data passing from the GPS module to a host and returns to command mode. '''
        string = "%"
        self.dev.write(string + "\r\n")
        self.dev.readline()
        self.dev.flushInput()
        print "CMD='%s' NORET" % (string, )
        return 0

    def sendGPS(self, head, cmd):
        maxcnt = 100
        string = head + cmd
        res = ""
        print "GPS SEND: '%s'" % string

        self.dev.flushInput()
        self.dev.write(string + "\r\n")

        for j in xrange(maxcnt):
          res = self.dev.readline()
          if len(res) > 0:
            res = re.sub("^\s+", "", res)
            res = re.sub("\s+$", "", res)
            print "RAW GPS REPLY: '%s'" % res

            pos = res.find(head)
            if pos != -1:
              res = res[pos:].split("*")[0]
              print "GPS REPLY: '%s'" % res
              return res
          else:
            print "ZERO REPLY"
            return None

        print "sendGPS: FAILED: '%s'" % res

    def sendCmd(self, cmd, trg=None, det=None, val=None, shouldBeAnswer=True):
        string = str(cmd)
        if (trg != None):
            string = string + "," + str(trg)
        if (det != None):
            string = string + "," + str(det)
        if (val != None):
            string = string + "," + str(val)

        self.dev.write(string + "\r\n")
        res = self.dev.readline()
        res = re.sub("^\s+", "", res)
        res = re.sub("\s+$", "", res)
        if (not shouldBeAnswer) and len(res) == 0:
            print "CMD='%s' NORET" % (string)
            return 0

        print "CMD='%s' RET='%s'" % (string, res)
        if res == 'SYNTAX ERROR':
          #sometimes you get a 'SYNTAX ERROR' while the command will succeed if you try again
          if self.retries <= self.max_retries:
            self.retries=self.retries+1
            time.sleep(0.1)
            return self.sendCmd(cmd, trg, det, val, shouldBeAnswer)
          else:
            print "IO ERROR: max_retries reached for command ", cmd
        else:
          self.retries=0

        return res

    def getValueFromResult(self, returnStr):
        vals = str(returnStr).split(",");
        if len(vals) < 4:
            return None
        return int(vals[3])

    def getStringFromResult(self, returnStr):
        vals = str(returnStr).split(",");
        if len(vals) < 4:
            return None
        return str(vals[3])


class TamerDevice(object):
    def __init__(self, basedevice=UsbDevice()):
        self.dev = basedevice
        self.dev.leaveGpsMode()

    # for fulshing buffer in buggy firmware
    def flush(self):
        return self.dev.sendCmd("\r\n\r\n", None, None, None, False)

    def checkGps(self):
        self.enterGpsMode()
        res = self.dev.sendGPS("$PMTK", "000*32")
        self.leaveGpsMode()
        return res == "$PMTK001,0,3"

    def getGpsVer(self):
        self.enterGpsMode()
        res = self.dev.sendGPS("$PMTK", "604*30")
        self.leaveGpsMode()
        return res

    def getGpsFw(self):
        self.enterGpsMode()
        res = self.dev.sendGPS("$PMTK", "605*31")
        self.leaveGpsMode()
        return res



    def reset(self):
        return self.dev.sendCmd("RST")

    def loadEeprom(self):
        return self.dev.sendCmd("LDE")

    def storeEeprom(self):
        return self.dev.sendCmd("STE")

    def getHwi(self):
        return self.dev.sendCmd("HWI")
#        return self.getStringFromResult(res)

    def getVer(self):
        return self.dev.sendCmd("VER")
#        return self.getStringFromResult(res)

    def setOsc(self, freq=20000000):
        return self.dev.sendCmd("SET", "", "OSC", freq)

    def setOut(self, freq=52000000):
        return self.dev.sendCmd("SET", "", "OUT", freq)

    def setAut(self, mode=1):
        return self.dev.sendCmd("SET", "", "AUT", mode)

    def setOutputsMask(self, mask=255):
        return self.dev.sendCmd("SET", "LMK", "PRT", mask)

    def setIntOscState(self, enb=1):
        return self.dev.sendCmd("SET", "IOS", "ENB", enb)

    def setGpsAut(self, mode = 1):
        return self.dev.sendCmd("SET", "GPS", "AUT", mode)


    def getOsc(self):
        res = self.dev.sendCmd("INF", "", "OSC")
        return self.dev.getValueFromResult(res)

    def getOut(self):
        res = self.dev.sendCmd("INF", "", "OUT")
        return self.dev.getValueFromResult(res)

    def getAut(self):
        res = self.dev.sendCmd("INF", "", "AUT")
        return self.dev.getValueFromResult(res)

    def getOutputsMask(self):
        res = self.dev.sendCmd("INF", "LMK", "PRT")
        return self.dev.getValueFromResult(res)

    def getIntOscState(self):
        res = self.dev.sendCmd("INF", "IOS", "ENB")
        if "ERROR" in res:
            return None
        return self.dev.getValueFromResult(res)

    def getGps(self, reg="R00"):
        res = self.dev.sendCmd("INF", "GPS", reg)
        return self.dev.getValueFromResult(res)

    def getGpsAut(self):
        return self.getGps("AUT")

    def enterGpsMode(self):
        return self.dev.enterGpsMode()

    def leaveGpsMode(self):
        return self.dev.leaveGpsMode()
