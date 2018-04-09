#! /usr/bin/python
#-*-coding: utf-8 -*-

import ptLeverThread
import PTSimpleGPIO
import array

class PTLeverThread (object):
    """
    PTLeverThread controls a lever used for the AutoHeadFix program
    """
    def __init__ (self, posBufSizeP, circBuffSizeP, isReversedP, goalCuerPinP, cuerFreqP):
        self.posBuffer = array.array('B', [0]*posBufSizeP)
        self.leverThread = ptLeverThread.new (self.posBuffer, circBuffSizeP, isReversedP, goalCuerPinP, cuerFreqP)
        self.posBufSize = posBufSizeP
        self.circBuffSize = circBuffSizeP
        self.goalCuerPin= goalCuerPinP
        self.cuerFreq = cuerFreqP

    def setConstForce (self,newForce):
        ptLeverThread.setConstForce(self.leverThread, newForce)

    
    def getConstForce (self):
        return ptLeverThread.getConstForce(self.leverThread)


    def applyForce (self, theForce):
        ptLeverThread.applyForce(self.leverThread, theForce)

    def applyConstForce(self):
        ptLeverThread.applyForce (-1)

    def zeroLever (self, zeroMode, isLocking):
        return ptLeverThread.zeroLever(self.leverThread, zeroMode, isLocking)

    def setPerturbForce (self, perturbForce):
        ptLeverThread.setPerturbForce(self.leverThread, perturbForce)

    def setPerturbStartPos (self, startPos):
        ptLeverThread.setPerturbStartPos(self.leverThread, startPos)

    def startTrial (self):
        ptLeverThread.startTrial(self.leverThread)

    def checkTrial (self):
        resultTuple = ptLeverThread.checkTrial(self.leverThread)
        self.trialComplete = resultTuple [0]
        self.trialPos = resultTuple [1]
        self.inGoal = resultTuple [2]
        return resultTuple
    
    def turnOnGoalCue (self):
        ptLeverThread.doGoalCue(self.leverThread, 1)
    
    def turnOffGoalCue (self):
        ptLeverThread.doGoalCue(self.leverThread, 0)

    
    def setHoldParams(self, goalBottom, goalTop, nHoldTicks):
        ptLeverThread.setHoldParams (self.leverThread, goalBottom, goalTop, nHoldTicks)

    def getLeverPos (self):
        return ptLeverThread.getLeverPos(self.leverThread)

    def abortUnCuedTrial(self):
        ptLeverThread.abortUncuedTrial(self.leverThread)
    