#!/usr/bin/python
'''
/***************************************************************************//**
 * @file imageConsol.py
 * @brief 
 * @version 0.0.1
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/
'''

import os, sys
from PIL import Image, ImageTk
import PIL.Image
import tkinter as tk
from tkinter import *
# import tkMessageBox
from tkinter import filedialog as filedialog 
# import numpy as np
import glob
import serial
import serial.tools.list_ports
import time
import threading



def main():
    print ('/******************************************************************************/')
    print ('/*******************imageConsol python script started**************************/')
    print ('/******************************************************************************/\n\n')
    App()
    print ('\n\n/**************************************Exit************************************/')



class Application(tk.Frame):              
    def __init__(self, master=None):
	
        #Variable
        self.filenameAndPath = ''
        self.photoimg_ref = []
        self.actualRotate = 0
        self.smallImage = None
        self.originaPictureFrame = None
        self.generatedPictureFrame = None
        self.uartPortList = []
        self.DTM1bytes = []
        self.DTM2bytes = []
        self.serial = None
        self.uartThread = None
        self.killThread = False
  		
        tk.Frame.__init__(self, master)   
        self.portSelect = IntVar()
        
        self.grid()                       
        self.createWidgetFileMenuButton()
        self.createWidgetSettingsMenuButton()
        self.createWidgetProcessMenuButton()
        self.originaPictureFrame = self.createWidgetOriginalPicture()
        self.generatedPictureFrame = self.createWidgetGeneratedPicture()
        #self.createBitStreamFromImage()
        #self.start_thread()
        
    def thread_function(self):
        while(1):
            x = self.serial.readline()
            # print x
            
            if (self.killThread == True):
                break
    
    def start_thread(self):
        self.uartThread = threading.Thread(target=self.thread_function)
        self.uartThread.start()
        
    def kill_thread(self):
        self.uartThread.kill()
    
    def serial_ports(self):

        
        ports = serial.tools.list_ports.comports()
        result = []

        for port, desc, hwid in sorted(ports):
            print("{}: {} ".format(port, desc))
            print (ports)
            result.append(port)
        
        return ports    
    
  
    def createBitStreamFromImage(self, image):
    # 255+255+255 -> 765
    # If all is 0 -> 0+0+0 -> balck
    # if all color 255 -> 255+255+255 -> white
    # if on midle -> reduce
    # so add the numbers, then
    # 0 < num < 255 -> black
    # 255 < num < 510 -> red
    # 510 < num < 765 -> white
    #      
        self.outbistreamDTM1File = open("tmp/bitstream_DTM1.txt", "w")
        self.outbistreamDTM2File = open("tmp/bitstream_DTM2.txt", "w")
        
        self.outbytestreamDTM1File = open("tmp/bytestream_DTM1.txt", "w")
        self.outbytestreamDTM2File = open("tmp/bytestream_DTM2.txt", "w")
        
        self.DTM1bitArray = []
        self.DTM2bitArray = []
    
        #self.img = PIL.Image.open('tmp/generated.jpg', "r")
        self.img = image
        self.img.save('tmp/result.png')
        self.width, self.height = self.img.size
        
        print ('self.width ' + str(self.width))
        print ('self.height ' + str(self.height))
        
        # start with the first line
        for x in range(self.width-1, 0-1, -1):
            
                
            # start with the most column
            for y in range(0, self.height , 1):
                
                # get the color (rgb)
                self.current_color = self.img.getpixel( (x,y) )
                
                # add colour elements
                self.sumofColors = self.current_color[0] + self.current_color[1] + self.current_color[2]
                
                # Color mapping:
                # ==============
                #          DTM1 | DTM2
                #  Red   |   1  |  0
                #  Black |   0  |  1 
                #  White |   1  |  1
                #
                # make decision whether it is white/black/red
                if (self.sumofColors >= 0 and self.sumofColors < 255):
                    # color is black
                    self.outbistreamDTM1File.write("0")
                    self.outbistreamDTM2File.write("1")
                    
                    self.DTM1bitArray.append(0)
                    self.DTM2bitArray.append(1)
                    
                elif (self.sumofColors >= 255 and self.sumofColors < 510):
                    # color is red
                    self.outbistreamDTM1File.write("1")
                    self.outbistreamDTM2File.write("0")
                    
                    self.DTM1bitArray.append(1)
                    self.DTM2bitArray.append(0)
                    
                    
                elif (self.sumofColors >= 510 and self.sumofColors <= 765):
                    # color is white 
                    self.outbistreamDTM1File.write("1")
                    self.outbistreamDTM2File.write("1")
                    
                    self.DTM1bitArray.append(1)
                    self.DTM2bitArray.append(1)
                    
                else: 
                
                    # do nothing
                    # maybe error
                    pass
        
        #close files
        self.outbistreamDTM1File.close()
        self.outbistreamDTM2File.close()
        
        # debug message
        print ('Size of bitarray: ' + str(len(self.DTM1bitArray)))
        
        # convert bit arrays to bytes - DTM1
        self.tmp = ''
        self.cnt = 0
        
        for x in range(0, len(self.DTM1bitArray) ):
            self.tmp = self.tmp + str(self.DTM1bitArray[x])
            self.cnt = self.cnt + 1
            
            if (self.cnt == 8):
                self.cnt = 0;
                #print self.tmp
                #print hex(int(self.tmp,2))
                #print "\n"
                # need comma or not among the numbers
                if (x == 7):
                    # do not take comma to after the last byte
                    self.outbytestreamDTM1File.write(hex(int(self.tmp,2)) )
                else:
                    self.outbytestreamDTM1File.write(' ,' + hex(int(self.tmp,2)) )
                
                ## byte array for version 0
                # self.DTM1bytes.append(chr(int(self.tmp,2)))
                self.tmpByte = format((int(self.tmp,2)),'x')
                # print self.tmpByte
                self.DTM1bytes.append(self.tmpByte)
                self.tmp = ''
        self.outbytestreamDTM1File.close()
        
                # debug message
        print ('Size of bytearray: ' + str(len(self.DTM1bytes)))
        
        # convert bit arrays to bytes - DTM2
        
        self.tmp = ''
        self.cnt = 0
        
        for x in range(0, len(self.DTM2bitArray) ):
            self.tmp = self.tmp + str(self.DTM2bitArray[x])
            self.cnt = self.cnt + 1
            
            if (self.cnt == 8):
                self.cnt = 0;
                #print self.tmp
                #print hex(int(self.tmp,2))
                #print "\n"
                if (x == 7):
                    self.outbytestreamDTM2File.write(hex(int(self.tmp,2)) )
                else:
                    self.outbytestreamDTM2File.write(' ,' + hex(int(self.tmp,2)) )
                ## byte array for version 0
                # self.DTM2bytes.append(chr(int(self.tmp,2)))
                self.DTM2bytes.append(format((int(self.tmp,2)),'x'))
                self.tmp = ''
        self.outbytestreamDTM2File.close()
        
	
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	# Widgets                                                                                         #
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''	
	
    def createWidgetOriginalPicture(self):
        self.originaPictureFrame = tk.Frame(self, width=640, height=480, background='white')
        self.originaPictureFrame.grid(row=1, column = 0, columnspan=7, rowspan=5,sticky=W , padx=5, pady=5)
        return self.originaPictureFrame

    def createWidgetGeneratedPicture(self):
        self.generatedPictureFrame = tk.Frame(self, width=297, height=129, background='white')
        self.generatedPictureFrame.grid(row=2, column = 8, columnspan=3, rowspan=2,sticky=W , padx=5, pady=5)
        return self.generatedPictureFrame

    def createWidgetQuitButton(self):
        self.quitButton = tk.Button(self, text='Quit', command=self.quit)            
        self.quitButton.grid() 
		
    def createWidgetFileMenuButton(self):
        self.filemenuButton = tk.Menubutton(self , text="File", relief=RAISED) 
        #self.quitButton = tk.Button(self, text='Quit', command=self.quit)            
        self.filemenuButton.grid(row=0, column = 0, sticky=W+E+N+S)  
        self.filemenuButton.menu =  Menu ( self.filemenuButton, tearoff = 0 )
        self.filemenuButton["menu"] =  self.filemenuButton.menu

        self.filemenuButton.menu.add_command ( label="Open File", command = self.openFile )
        self.filemenuButton.menu.add_command ( label="Quit", command = self.closeApp )
		
    def createWidgetProcessMenuButton(self):
        self.processmenuButton = tk.Menubutton(self , text="Process", relief=RAISED) 
        #self.quitButton = tk.Button(self, text='Quit', command=self.quit)            
        self.processmenuButton.grid(row=0, column = 1, sticky=W+E+N+S)  
        self.processmenuButton.menu =  Menu ( self.processmenuButton, tearoff = 0 )
        self.processmenuButton["menu"] =  self.processmenuButton.menu

        self.processmenuButton.menu.add_command ( label="Rotate Right", command= lambda: self.rotateRight(self.generatedPictureFrame) )
        self.processmenuButton.menu.add_command ( label="Rotate Left",  command= lambda: self.rotateLeft(self.generatedPictureFrame) )	
        self.processmenuButton.menu.add_command ( label="Send to device", command = self.sendToDevice )	
        self.processmenuButton.menu.add_command ( label="Clean display", command = self.cleanDisplay )	
        self.processmenuButton.menu.add_command ( label="Go to sleep", command = self.goToSleep )	
        
    def createWidgetSettingsMenuButton(self):
        self.settingsmenuButton = tk.Menubutton(self , text="Settings", relief=RAISED)            
        self.settingsmenuButton.grid(row=0, column = 2, sticky=W+E+N+S)  
        self.settingsmenuButton.menu =  Menu ( self.settingsmenuButton, tearoff = 0 )
        
        self.portMenuButton = Menu (self.settingsmenuButton.menu, tearoff = 0)
        self.settingsmenuButton.menu.add_cascade(label="Ports", menu=self.portMenuButton)
        
        self.settingsmenuButton["menu"] =  self.settingsmenuButton.menu

                       
        ports = serial.tools.list_ports.comports()
        
        i = 0
        for port, desc, hwid in sorted(ports):
            portName = "{}: {} ".format(port, desc)
            self.uartPortList.append("{}".format(port))
            print(portName)
            self.portMenuButton.add_radiobutton ( label=portName, var = self.portSelect, value = i )
            i=i+1
            print (self.portSelect.get())

        #set callback for radioButtons 
        self.portSelect.trace("w",self.changePortsCallback)  

        
        
        
        #self.settingsmenuButton.pack()

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	# File menu commands                                                                             #
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''	
    def openFile(self):
        print ('File open ...')
        self.filenameAndPath = filedialog.askopenfilename(initialdir = "/", title = "Select a File", filetypes = (("jpeg files","*.jpg*"),("all files","*.*"))) 
        if self.filenameAndPath == '': 
            print ('No selected file')
        else:
            print ('Address:' + self.filenameAndPath)	
            # clear previous image references
            self.resetPictureBuffer()
            if self.photoimg_ref:
                self.photoimg_ref[:] = []
            self.addPictureToOriginalFrame(self.originaPictureFrame)
            self.addPictureToGeneratedFrame(self.generatedPictureFrame, self.actualRotate)			


	
    def closeApp(self):
        print ('Quit ...')
        print ('Address:' + self.filenameAndPath)
        sys.exit(0)
		
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	# Process menu commands                                                                          #
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''	
    def rotateLeft(self, frame):
        # remove the EINK picture related image reference
        #print self.photoimg_ref
        # clear DTM lists
        self.resetPictureBuffer()
        self.actualRotate = self.actualRotate - 90
        self.addPictureToGeneratedFrame(self.generatedPictureFrame, self.actualRotate)
			
    def rotateRight(self, frame):
        # clear DTM lists
        self.resetPictureBuffer()
        self.actualRotate = self.actualRotate + 90
        self.addPictureToGeneratedFrame(self.generatedPictureFrame, self.actualRotate)

    def cleanDisplay(self):  
        self.serial = serial.Serial (self.uartPortList[ self.portSelect.get() ] , baudrate = 115200, rtscts = True )
        print ("\n cleanDisplay() called")
        self.serial.write(("eink_clean\x0D").encode() )
        self.serial.close() 
        
    def goToSleep(self): 
        self.serial = serial.Serial (self.uartPortList[ self.portSelect.get() ] , baudrate = 115200, rtscts = True )
        print ("\n goToSleep() called")
        self.serial.write(("gotosleep\x0D").encode() )
        self.serial.close() 
   
    def sendToDevice(self):  
    
        # send DTM1 content
        self.serial = serial.Serial (self.uartPortList[ self.portSelect.get() ] , baudrate = 115200, rtscts = True )

        print ("DTM1 Len: " +  str(len(self.DTM1bytes)))
                
        self.actualPacket = "{"
        self.offset = 0
        self.bytecnt = 0
                
        for i in range(0, len(self.DTM1bytes) ):  
                        
            if ((self.bytecnt == 31) or (i == len(self.DTM1bytes)-1)):
                self.actualPacket = self.actualPacket + " " + str(self.DTM1bytes[i]).zfill(2) + "}"
                self.bytecnt = 0
                self.serial.write(("dtmx_update_buffer " + str(self.offset) + " 1 " + self.actualPacket + "\x0D").encode() )
                print            ("dtmx_update_buffer " + str(self.offset) + " 1 " + self.actualPacket + "\n" )
                self.offset = self.offset + 1
                self.actualPacket = "{"

            else:     
                self.actualPacket = self.actualPacket + " " + str(self.DTM1bytes[i]).zfill(2)
                self.bytecnt = self.bytecnt + 1
        
            # print self.DTM1bytes[i]
            
        # send DTM2 content
        print ("DTM2 Len: " +  str(len(self.DTM2bytes)))
              
        self.actualPacket = "{"
        self.offset = 0
        self.bytecnt = 0
        
        for i in range(0, len(self.DTM2bytes) ):  
                        
            if ((self.bytecnt == 31) or (i == len(self.DTM1bytes)-1)):
                self.actualPacket = self.actualPacket + " " + str(self.DTM2bytes[i]).zfill(2) + "}"
                self.bytecnt = 0
                self.serial.write(("dtmx_update_buffer " + str(self.offset) + " 2 " + self.actualPacket + "\x0D").encode() )
                print            ("dtmx_update_buffer " + str(self.offset) + " 2 " + self.actualPacket + "\n" )
                self.offset = self.offset + 1
                self.actualPacket = "{"

            else:     
                self.actualPacket = self.actualPacket + " " + str(self.DTM2bytes[i]).zfill(2)
                self.bytecnt = self.bytecnt + 1
        


        self.serial.write(("eink_update\x0D").encode() )  

        self.serial.close() 

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	# Settings menu commands                                                                          #
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    def changePortsCallback(self, *args):
        print ("Port selected: " + self.uartPortList[ self.portSelect.get() ] )


            
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    # Picture processing related functions                                                              #
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''	
    def resetPictureBuffer(self):
        self.DTM1bytes *= 0
        self.DTM2bytes *= 0        
    
    def addPictureToOriginalFrame(self, frame): 
        # remove previous contents from Frame
        for widget in frame.winfo_children():
            widget.destroy()		
            pass
	
        self.img = PIL.Image.open(self.filenameAndPath)
				
        self.size = 640,480
		# 1. This part for proper image sizeing on GUI's canvas
		# The logic is not used - thumbnail(self.size, PIL.Image.ANTIALIAS) does it

		# 1.1. get size
        self.width, self.height = self.img.size

        # 1.2.if size bigger than 640*480 then resize required
        if self.width > 640 or self.height > 480:

     		# claculate ratio - height/width
            self.ratio = self.height/self.width

            # ratio is same as canvas, so does not matter if the indenatation basing on height or width
            if self.ratio == 0.75:
                #calculate the difference between the original picture width and canvas width
                #self.diff = self.width / 640
                self.img.thumbnail(self.size, PIL.Image.ANTIALIAS)
				

			# self ratio is smaller than 0.75, so indentation according the height
            elif self.ratio > 0.75:
                self.img.thumbnail(self.size, PIL.Image.ANTIALIAS)	

			# self ratio is higher than 0.75, so indentation according the width
            elif self.ratio < 0.75:
                self.img.thumbnail(self.size, PIL.Image.ANTIALIAS)					
		
        self.photoimg = ImageTk.PhotoImage(self.img)
		
        #reference for images
        self.photoimg_ref.append(self.photoimg)
		
        frame.pack_propagate(0)		
        self.label = tk.Label(frame, image=self.photoimg)
        self.label.pack()
		
    def addPictureToGeneratedFrame(self, frameb, rotate_i): 
        # remove previous contents from Frame
        for widget in frameb.winfo_children():
            widget.destroy()
            pass			
	
        self.img = PIL.Image.open(self.filenameAndPath)
		
		# rotate the picture
        self.img = self.img.rotate(rotate_i, expand=True)
        
        # get size picture size in pixel
        self.width, self.height = self.img.size
        print ("Picture size before the modification: w:"+str(self.width)+" pixel; h: "+str(self.height) + "pixel" ) 

        
        # if size bigger than 297*129 then resize required
        if (self.width > self.height):
            print (' - the width higher then height')
            self.picture_ratio = float(self.width) / float(self.height)
            if (1):	
                print ('not rotated')            
                if self.width >= 296 or self.height > 128:
                    
                    # claculate ratio - height/width
                    self.ratio = float(self.height)/float(self.width)
                    print ('picture ratio: '+str(self.ratio))

                    # ratio is same as canvas, so does not matter if the indenatation basing on height or width
                    # display ratio is 129 / 297 = 0.43
                    if self.ratio == 0.43:
                        print ('ratio is 0.43 - no calculation needed')
                        #calculate the difference between the original picture width and canvas width

                        self.diff = float(self.width / 296)
                        self.img = self.img.resize((self.width / self.diff, self.height / self.diff), PIL.Image.ANTIALIAS)
                    

                    # self ratio is smaller than 0.75, so indentation according the height
                    elif self.ratio < 0.43:
                        print ('ratio is smaller then 0.43')				
                        self.diff = float(self.width) / float(296)
                        self.newwidth = self.width / self.diff
                        self.newhight = self.height / self.diff
                        print ("calculated divider: "+str(self.diff)+"; calculated height: "+str(self.newhight)+"; calculated width: "+str(self.newwidth))						
                        self.img = self.img.resize((int(self.newwidth),int(self.newhight)), PIL.Image.ANTIALIAS)
						

                    # self ratio is higher than 0.75, so indentation according the width
                    elif self.ratio > 0.43:
                        print ('ratio is greater then 0.43'	)			

                        self.diff = float(self.height) / float(128)
                        self.newwidth = (self.width / self.diff) 
                        self.newhight = (self.height / self.diff) 
                        print ("calculated divider: "+str(self.diff)+"; calculated height: "+str(self.newhight)+"; calculated width: "+str(self.newwidth))
                        self.img = self.img.resize((int(self.newwidth),int(self.newhight)), PIL.Image.ANTIALIAS)	


     
        else:
            if (1):
                print ('self.width < self.height')            
                if self.width >= 128 or self.height > 296:
                    
                    # claculate ratio - height/width
                    self.ratio = float(self.width)/float(self.height)
                    print ('slef ratio')
                    print (self.ratio)

                    # ratio is same as canvas, so does not matter if the indenatation basing on height or width
                    # display ratio is 129 / 297 = 0.43
                    if self.ratio == 0.43:
                        print ('ratio is 0.43 - no calculation needed')
                        #calculate the difference between the original picture width and canvas width

                        print ('ratio 0.75')
                        self.diff = float(self.height / 296)
                        self.img = self.img.resize((self.width / self.diff, self.height / self.diff), PIL.Image.ANTIALIAS)
                    

                    # self ratio is smaller than 0.75, so indentation according the height
                    elif self.ratio < 0.43:
                        print ('ratio is smaller then 0.43' )                   
                        self.diff = float(self.height) / float(296)
                        self.newwidth = self.width / self.diff
                        self.newhight = self.height / self.diff
                        print ("calculated divider: "+str(self.diff)+"; calculated height: "+str(self.newhight)+"; calculated width: "+str(self.newwidth))						
                        self.img = self.img.resize((int(self.newwidth),int(self.newhight)), PIL.Image.ANTIALIAS)
						

                    # self ratio is higher than 0.75, so indentation according the width
                    elif self.ratio > 0.43:
                        print ('ratio is greater then 0.43')
                        self.diff = float(self.height) / float(128)
                        self.newwidth = self.width / self.diff
                        self.newhight = self.height / self.diff
                        print ("calculated divider: "+str(self.diff)+"; calculated height: "+str(self.newhight)+"; calculated width: "+str(self.newwidth))
                        self.img = self.img.resize((int(self.newwidth),int(self.newhight)), PIL.Image.ANTIALIAS)	

        '''		
        # resize with PIL method
        if self.width > self.height:
            
            self.img = self.img.resize((296,128))
        ''' 
		# get size - again, for checking
        self.width, self.height = self.img.size
        print ("Picture size after the modification: w:"+str(self.width)+" pixel; h: "+str(self.height) + "pixel \n\n" )

        # save the image to temp file
        self.img.save('tmp/generated.jpg')
		
        self.photoimg = ImageTk.PhotoImage(self.img)

		# if there is reference from small picture, then remove it
        if len(self.photoimg_ref) > 1:
            self.photoimg_ref.pop()
		
        # reference for images
        self.photoimg_ref.append(self.photoimg)
		
        frameb.pack_propagate(0)		
        self.label = tk.Label(frameb, image=self.photoimg)
        self.label.pack()
        
        # generate the output streams
        self.createBitStreamFromImage(self.img)
		
		
	
def App():
    app = Application()                       
    app.master.title('Silabs - EINK Image Processing tool')    
    app.mainloop()   

if __name__ == "__main__":
    main()
	
	
