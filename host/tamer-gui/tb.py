#!/usr/bin/python
# -*- coding: utf-8 -*-

# tb.py

import sys
import time
import re
#from PyQt4 import QtGui, QtCore, QtXml

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtXml import *
from Numeric import *



from tamer_basic_ui import *
from tamerdevice import *

tamer10_lmk1000 = ("LVDS", None, "LVDS",   None,        "LVPECL/SYNC", "LVPECL/LVCMOS", "LVCMOS", "LVPECL")


tamer11_lmk1010 = ("LVDS", "CMOS", "LVDS", None,        "LVDS",   "LVDS",   "CMOS", "LVDS")

tamer12_lmk1000 = ("LVDS", "CMOS", None,   "LVPECL",    "LVPECL", "LVPECL", None,   "LVPECL")
tamer12_lmk1010 = ("LVDS", "CMOS", None,   "LVDS",      "LVDS",   "LVDS",   "CMOS", "LVDS")

tamer_unknown  = ("unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown")

class MainWindow(QtGui.QWidget):
    def __init__(self, device=None):
        QtGui.QWidget.__init__(self)
        self.obj = Ui_Basic()
        self.obj.setupUi(self)
        self.dev = device

	self.outCb = (self.obj.cbOut0, self.obj.cbOut1, self.obj.cbOut2, self.obj.cbOut3, 
	              self.obj.cbOut4, self.obj.cbOut5, self.obj.cbOut6, self.obj.cbOut7)

	self.timer = QTimer(self)
	self.connect(self.timer,        QtCore.SIGNAL('timeout()'), self.onTimer)

	self.ParseHWI()
	self.ReadAll()
	
	self.connect(self.obj.btStore,  QtCore.SIGNAL('clicked()'), self.onStoreEeprom)
	self.connect(self.obj.btLoad,   QtCore.SIGNAL('clicked()'), self.onLoadEeprom)
	self.connect(self.obj.btSet,    QtCore.SIGNAL('clicked()'), self.onSet)
	self.connect(self.obj.btGet,    QtCore.SIGNAL('clicked()'), self.onGet)


    def onTimer(self):
	self.onGet()

    def ParseHWI(self):
	data = self.dev.getHwi()
	self.obj.leHWI.setText(data)
	
	if "GPS" in str(data):
	    self.gps = 1
	else:
	    self.gps = 0
	
	
	dataVer = self.dev.getVer()
	self.obj.leVER.setText(dataVer)
	
	vals = str(dataVer).split(" ")
	self.ver = vals[1].split("=")[1]
	print "Tamer version: %s" % self.ver
	
	vals = str(data).split(" ")
	self.lmx = int(vals[0].split("=")[1])
	self.lmk = int(vals[1].split("=")[1])
	
	if self.lmk == 1010 and self.ver == "1.2":
	    self.outputsConfig = tamer12_lmk1010
	elif self.lmk == 1000 and self.ver == "1.2":
	    self.outputsConfig = tamer12_lmk1010
	elif self.lmk == 1010 and self.ver == "1.1pre":
	    self.outputsConfig = tamer11_lmk1010
	elif self.ver == "1.0":
	    self.outputsConfig = tamer10_lmk1000
	else:
	    self.outputsConfig = tamer_unknown

	for i,v in enumerate(self.outputsConfig):
	    if v == None:
		self.outCb[i].setVisible(False)
	    else:
		self.outCb[i].setText("%d: %s" % (i, v))
	
	if self.gps == 0:
	    self.obj.gGps.setVisible(False)

    def ReadOutputs(self):
	data = self.dev.getOutputsMask()
	for i in range(0,8):
	    state = (2**i) & data
	    self.outCb[i].setChecked(state)

    def GetOututsMask(self):
	data = 0
	for i in range(0,8):
	    data = data + self.outCb[i].isChecked() * (2**i)
	return data

    def ReadFosc(self):
	data = self.dev.getOsc()
	self.obj.leFosc.setText(str(data))
	
    def ReadFout(self):
	data = self.dev.getOut()
	self.obj.leFout.setText(str(data))
	
    def GetFosc(self):
	data = self.obj.leFosc.text()
	data = re.sub("[^\d]", "", str(data))
	return data

    def GetFout(self):
	data = self.obj.leFout.text()
	data = re.sub("[^\d]", "", str(data))
	return data

    def ReadAll(self):
	data = self.dev.flush() ### Using this to reset buffer in buggy firmware
	
	self.onGet()

    def onStoreEeprom(self):
	self.dev.storeEeprom()

    def onLoadEeprom(self):
	self.dev.loadEeprom()
	self.onGet()


    def onGet(self):
	data = self.dev.getAut()
	self.obj.cbAutoStart.setChecked(data)
	
	data = self.dev.getIntOscState()
	if data == None:
	    self.obj.cbOscDisable.setEnabled(False)
	    self.oscState = False
	else:
	    self.obj.cbOscDisable.setChecked(not data)
	    self.oscState = True
	
	self.ReadFosc()
	self.ReadFout()
	self.ReadOutputs()
	if self.gps:
	    self.ReadGps()
	    
    def ReadGps(self):
	data = self.dev.getGpsAut()
	self.obj.cbGPSSync.setChecked(data)
	
	if data and not self.timer.isActive():
	    self.timer.start(3000)
	elif not data and self.timer.isActive():
	    self.timer.stop();
	
	data = self.dev.getGps("R01")
	
	try:
	    di = data - self.prev1pps
	    if di > 0:
		self.obj.lbGpsInfo.setText("In sync")
		self.obj.lbGpsInfo.setStyleSheet("QLabel { background-color: green; }");
	    else:
		self.obj.lbGpsInfo.setText("Out of sync!")
		self.obj.lbGpsInfo.setStyleSheet("QLabel { background-color: red; }");
	except:
	    pass
	
	self.prev1pps = data
	self.obj.le1PPS.setText(str(data))
	
	data = float(self.dev.getGps("R03")) / 32
	self.obj.leCalcFout.setText(str(data))

    def onSet(self):
	data = self.GetOututsMask()
	self.dev.setOutputsMask(data)
	#print data
	data = self.GetFosc()
	res1 = self.dev.setOsc(data)
	#print data
	data = self.GetFout()
	res2 = self.dev.setOut(data)
	
	if "Bad" in res1 or "Bad" in res2:
	    QMessageBox.critical(self, "Can't set frequency", "OSC: %s\r\nOUT: %s" %(res1, res2))
	    return
	#print data
	
	if self.oscState:
	    data = int(not self.obj.cbOscDisable.isChecked())
	    self.dev.setIntOscState(data)
	    #print data
	
	data = int(self.obj.cbAutoStart.isChecked())
	self.dev.setAut(data)
	#print data
	
	if self.gps:
	    data = self.obj.cbGPSSync.isChecked()
	    self.dev.setGpsAut(int(data))


if __name__ == '__main__':
      noCloseMainForm = False
      app = QtGui.QApplication(sys.argv)
      dev = TamerDevice()
      qb = MainWindow(dev)
#      if qb.error == 1:
#          sys.exit(-1)

      qb.show()
#      qb.setGeometry(10, 20, 1440, 900)
#      qb.setFocus()
      sys.exit(app.exec_())


