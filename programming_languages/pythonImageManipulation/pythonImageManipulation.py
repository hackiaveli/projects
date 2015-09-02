
# Name: Andrej Ivanovic 
#
# UID: 604016344
#
# People I interacted with:
#
# Resources I used:
# https://docs.python.org/2/tutorial/inputoutput.html
#

#command to test first part unparsePPM(parsePPM('example.ppm'),"file.ppm")
#unparsePPM(mirrorImage(parsePPM('florence.ppm')),"goodO.ppm")

import math
import re
import struct 


# PROBLEM 1

# parse the file named fname into a dictionary of the form 
# {'width': int, 'height' : int, 'max' : int, 'pixels' : (int * int * int) list}

def makeTups(bytes):
    listPix=[]
    for a, b, c in zip(*[iter(bytes)]*3):
        listPix.append((a,b,c))
    return listPix

def revTups(bytes):
    listPix=[]
    for a, b, c in zip(*[iter(bytes)]*3):
        listPix.append((c,b,a))
    return listPix


def parsePPM(fname):
    m_file=[]
    openF = open(fname,'rb')
    theData=""
    for i,line in enumerate(openF):
        if i==3:
            theData+=line
        if i>3:
            theData+=line
        else:
            m_file.append(line.strip())
    allBytes =struct.unpack('%dB' %len(theData), theData)
    one=""
    two=""
    three=""
    listPix=[]
    listPix=makeTups(allBytes)

    listHW = m_file[1].split(" ")

    dictRet = {}
    dictRet['width']=int(listHW[0])
    dictRet['height']=int(listHW[1])
    dictRet['max']=int(m_file[2])
    dictRet['pixels']=listPix
    
    return dictRet


# a test to make sure you have the right format for your dictionaries
def testParsePPM():
    return parsePPM("example.ppm") =={'width': 2, 'max': 255, 'pixels': [(10, 23, 52), (82, 3, 215), (30, 181, 101), (33, 45, 205), (40, 68, 92), (111, 76, 1)], 'height': 3}


# write the given ppm dictionary as a PPM image file named fname
# the function should not return anything

def parsePixels(pix):
   match = re.findall(r'([0-9]+,?)',str(pix)) #find all the digits that are followed by a ,
   lis=[]
   
   [lis.append((match[i].split(','))[0]) for i in range(0, len(match))]#now kill the comma and take the digit
  
   finalList=[]
   [finalList.append(int(p)) for p in lis]
   return finalList

def unparsePPM(ppm, fname):
 
   wid=ppm['width']
   hite=ppm['height']
   maxi=ppm['max']
   pix=ppm['pixels']
   p6="P6"
   newL='\n'
   widHit=str(wid)+" "+str(hite) 

   finalList =[]
   finalList = parsePixels(pix)
   bytes = [struct.pack("B",p) for p in finalList] #go through all the ints and pack them

   f=open(str(fname),"wb") #write binary to make sure we write correctly the bytes

   f.write(p6)
   f.write(newL)
   f.write(widHit)
   f.write(newL)
   f.write(str(maxi))   
   f.write(newL)
   f.writelines(bytes)
   f.close()

# PROBLEM 2
def negate(ppm):
   wid=ppm['width']
   hite=ppm['height']
   maxi=ppm['max']
   pix=ppm['pixels']

   finalList=[]
   finalList= parsePixels(pix)

   newList=[]

   [newList.append(int(maxi-p)) for p in finalList]

   dictRet = {}
   dictRet['width']=wid
   dictRet['height']=hite
   dictRet['max']=maxi
   dictRet['pixels']=makeTups(newList)
   return dictRet

# PROBLEM 3
def mirrorImage(ppm):
   wid=ppm['width']
   hite=ppm['height']
   maxi=ppm['max']
   pix=ppm['pixels']

   # finalList=[]
   # finalList= parsePixels(pix)

   listOfTupLists=[]
   tempList=[]
   temp2List=[]
   # for p in finalList:
      # newList.append(int(maxi-p))
   i=1

   for tup in pix:
        # print tup
        # print tup
        if(i==wid):
            i=1
            tempList.extend(tup)
            temp2List=tempList[::-1]
            #temp2List = map(lambda x: x[::-1], tempList)
            listOfTupLists.extend(revTups(temp2List))
            tempList=[]    
        else:
            i+=1
            tempList.extend(tup)

   dictRet = {}
   dictRet['width']=wid
   dictRet['height']=hite
   dictRet['max']=maxi
   dictRet['pixels']=listOfTupLists

   return dictRet

# PROBLEM 4

# produce a greyscale version of the given ppm dictionary.
# the resulting dictionary should have the same format, 
# except it will only have a single value for each pixel, 
# rather than an RGB triple.
def greyscale(ppm):
   wid=ppm['width']
   hite=ppm['height']
   maxi=ppm['max']
   pix=ppm['pixels']
 
   newList=[int(round(.299 *tup[0]+.587 *tup[1]+.114 *tup[2])) for tup in pix]
   
   dictRet = {}
   dictRet['width']=wid
   dictRet['height']=hite
   dictRet['max']=maxi
   dictRet['pixels']= newList
   return dictRet

# take a dictionary produced by the greyscale function and write it as a PGM image file named fname
# the function should not return anything
def unparsePGM(pgm, fname):

   wid=pgm['width']
   hite=pgm['height']
   maxi=pgm['max']
   pix=pgm['pixels']
   p6="P5"
   newL='\n'
   widHit=str(wid)+" "+str(hite) 

   finalList =[]
   finalList = parsePixels(pix)
   bytes = [struct.pack("B",p) for p in finalList] #go through all the ints and pack them

   f=open(str(fname),"wb") #write binary to make sure we write correctly the bytes

   f.write(p6)
   f.write(newL)
   f.write(widHit)
   f.write(newL)
   f.write(str(maxi))   
   f.write(newL)
   f.writelines(bytes)
   f.close()


# PROBLEM 5

# gaussian blur code adapted from:
# http://stackoverflow.com/questions/8204645/implementing-gaussian-blur-how-to-calculate-convolution-matrix-kernel
def gaussian(x, mu, sigma):
  return math.exp( -(((x-mu)/(sigma))**2)/2.0 )

def gaussianFilter(radius, sigma):
    # compute the actual kernel elements
    hkernel = [gaussian(x, radius, sigma) for x in range(2*radius+1)]
    vkernel = [x for x in hkernel]
    kernel2d = [[xh*xv for xh in hkernel] for xv in vkernel]

    # normalize the kernel elements
    kernelsum = sum([sum(row) for row in kernel2d])
    kernel2d = [[x/kernelsum for x in row] for row in kernel2d]
    return kernel2d

# blur a given ppm dictionary, returning a new dictionary  
# the blurring uses a gaussian filter produced by the above function


def newVal (bigMatrix,jP,iP,radius,sigma):
  gM=gaussianFilter(radius,sigma)

  allR=0
  allG=0
  allB=0
  finalR=0
  finalG=0
  finalB=0
  for j in range(2*radius+1):
     for i in range(2*radius+1):
            allR+=round(gM[j][i]*(bigMatrix[jP-radius+j][iP-radius+i])[0])
            allG+=round(gM[j][i]*(bigMatrix[jP-radius+j][iP-radius+i])[1])
            allB+=round(gM[j][i]*(bigMatrix[jP-radius+j][iP-radius+i])[2])
  if(finalR>255):
    finalR=255
  if(finalG>255):
    finalG=255
  if(finalB>255):
    finalB=255
  finalR=int(round(allR))
  finalG=int(round(allG))
  finalB=int(round(allB))

  # print (finalR,finalG,finalB)
  return(finalR,finalG,finalB)

def gaussianBlur(ppm, radius, sigma):
    # obtain the filter
   wid=ppm['width']
   hite=ppm['height']
   maxi=ppm['max']
   pix=ppm['pixels']

   lss=[]
   [lss.append(pix[i:wid+i]) for i in range(0,hite*wid,wid)]

   bigMatrix=lss
   for i in range(len(bigMatrix)):
      for j in range(radius):
        bigMatrix[i].insert(0,bigMatrix[i][0])
        bigMatrix[i].append(bigMatrix[i][len(bigMatrix[i])-1])
   begin = bigMatrix[0]
   finisher = bigMatrix[len(bigMatrix)-1]
    
   for i in range(radius): 
        bigMatrix.insert(0,begin)
        bigMatrix.append(finisher)

   blurMatrix=[]
   for j in range(hite):
         for i in range(wid):
             myTup=newVal(bigMatrix,j,i,radius,sigma)
             blurMatrix.extend(myTup)

   blurMatrix=makeTups(blurMatrix)

   dictRet = {}
   dictRet['width']=wid
   dictRet['height']=hite
   dictRet['max']=maxi
   dictRet['pixels']=blurMatrix

   return dictRet
