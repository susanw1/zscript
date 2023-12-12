import xml.etree.ElementTree as ElementTree
import sys
import csv
import re
import datetime

peripheralInheritenceInject = {} # peripherals which share a numbering system
peripheralInheritenceIncrement = {} # peripherals which don't

maxOfPeripheral = {}
peripheralPinTypes = {}

#find peripherals which can be treated as smaller peripherals - e.g. USART/UART
with open(sys.argv[1], newline='') as csvfile:
   reader = csv.reader(csvfile, delimiter='\t')
   for row in reader:
      if(len(row)>1):
         rest = []
         for i in range(2, len(row)):
            rest.append(row[i])
         if(row[1] == "inj"):
            peripheralInheritenceInject[row[0]] = rest
         else:
            peripheralInheritenceIncrement[row[0]] = rest
         

funcNameRegex = '^([^_-]*[^\d_-][^\d]*[^\d_-]*)\d*[_-].*|([^_-]*[^\d_-])'
funcType = '^[^_-]*[^\d_-][^\d]*[^\d_-]*\d*[_-](.*)'

for i in range(2, len(sys.argv)):
   functions = set()
   tree = ElementTree.parse(sys.argv[i])
   root = tree.getroot()
   for signal in root.iter("{http://mcd.rou.st.com/modules.php?name=mcu}PinSignal"):
      functions.add(signal.get("Name"));

   functionChildren = {}
   
   # find all the types of pin from a given function type, to prepare pin checkers
   for func in functions:
      strFuncName = re.search(funcNameRegex, func).expand('\\1\\2')
      if not strFuncName in peripheralPinTypes:
          peripheralPinTypes[strFuncName] = set()
      funcTypeM = re.search(funcType, func)
      if(funcTypeM):
         peripheralPinTypes[strFuncName].add(funcTypeM.expand('\\1'))
       
   # add peripherals which do share numbering systems
   extraFuncs = set()
   for func in sorted(functions):
      functionChildren[func] = set()
      strFuncName = re.search(funcNameRegex, func).expand('\\1\\2')
      if(strFuncName in peripheralInheritenceInject):
         for alt in peripheralInheritenceInject[strFuncName]:
            newFunc = alt+re.search('\d.*', func).group(0)
            extraFuncs.add(newFunc)
            functionChildren[func].add(newFunc)
   functions |= extraFuncs
   
   for func in extraFuncs:
      if(not func in functionChildren):
         functionChildren[func] = set()
         

         
         
   highestByType = {} # the highest label of each peripheral type 
   for func in functions:
      plainName = re.search(funcNameRegex, func).expand('\\1\\2')
      numberM = re.search('\d+_', func) # if the peripheral doesn't have a number, we don't care
      if(numberM):
         number = int(numberM.group(0).replace("_", ""))
         if((not plainName in highestByType) or number > highestByType[plainName]):
            highestByType[plainName] = number
   
   # Then add peripherals which don't share numbering
   functionInjectNumberMap = {} # preserves the numbering for given original peripherals
   extraFuncs = set()
   for func in sorted(functions):
      strFuncName = re.search(funcNameRegex, func).expand('\\1\\2')
      numberM = re.search('\d+', func)
      if(numberM and (strFuncName in peripheralInheritenceIncrement)):
         origNumber = numberM.group(0)
         for alt in peripheralInheritenceIncrement[strFuncName]:
            if (not alt in functionInjectNumberMap):
               functionInjectNumberMap[alt] = {}
            if (not origNumber in functionInjectNumberMap[alt]):
               functionInjectNumberMap[alt][origNumber] = highestByType[alt] + 1
               highestByType[alt] += 1
               
            number = functionInjectNumberMap[alt][origNumber]
            newFunc = alt + str(number) + re.search('[^\d]*$', func).group(0)
            extraFuncs.add(newFunc)
            functionChildren[func].add(newFunc)

   
   functions |= extraFuncs
   
   # find all the maximum count of every given function
   for func in functions:
      strFuncName = re.search(funcNameRegex, func).expand('\\1\\2')
      if strFuncName in highestByType:
         if((not strFuncName in maxOfPeripheral) or highestByType[strFuncName]>maxOfPeripheral[strFuncName]):
             maxOfPeripheral[strFuncName] = highestByType[strFuncName]
      else:
         if((not strFuncName in maxOfPeripheral) or 1>maxOfPeripheral[strFuncName]):
             maxOfPeripheral[strFuncName] = 1
      
      
   functionPins = {}
   functionPinAF = {}
   
   for func in functions:
      functionPins[func] = []
      functionPinAF[func] = {}
      
   # find what pins can do what functions
   for pin in root.iter("{http://mcd.rou.st.com/modules.php?name=mcu}GPIO_Pin"):
      pinName = pin.get("Name")
      if pinName.find('-')>=0:
         pinName = pinName.rsplit('-')[0]
      if (pinName.find('[')<0):
         for signal in pin.findall("{http://mcd.rou.st.com/modules.php?name=mcu}PinSignal"):
            funcName = signal.get("Name")
            functionPins[funcName].append(pinName)
            af = signal.find("{http://mcd.rou.st.com/modules.php?name=mcu}SpecificParameter").find("{http://mcd.rou.st.com/modules.php?name=mcu}PossibleValue").text
            afNum = af.rsplit("AF")[1].split("_")[0]
            functionPinAF[funcName][pinName] = afNum
            for alternate in functionChildren[funcName]:
               functionPins[alternate].append(pinName)
               functionPinAF[alternate][pinName] = afNum

   outputName = re.search('STM32[^_]*', sys.argv[i])
   outputName = outputName.group(0)+".h"
   with open(outputName, "w") as out:
      out.write("/*\n * This file is automatically generated by afTable.py from the xml descriptions provided as part of the STM32Cube IDE\n")
      out.write(" * Timestamp: "+datetime.datetime.utcnow().isoformat()+"\n")
      out.write(" */\n\n")
      
      out.write("#ifndef LOWLEVEL_INCLUDE_DEVICE_ALTERNATE_FUNCTION\n")
      out.write("#define LOWLEVEL_INCLUDE_DEVICE_ALTERNATE_FUNCTION\n\n")
      for func in sorted(functions):
         usableFuncName = func.replace("-", "_")
         for pin in functionPins[func]:
            out.write("#define "+usableFuncName+"_"+pin+"_ "+pin+",GPIO_AF"+str(functionPinAF[func][pin])+"\n");
         for pin in functionPins[func]:
            out.write("#define _"+usableFuncName+"_"+pin+"_ 1\n");
         out.write('\n')
      out.write("#endif /* LOWLEVEL_INCLUDE_DEVICE_ALTERNATE_FUNCTION */")
      
for func in sorted(peripheralPinTypes):
   with open("peripheralPinCheck_"+func.lower()+".h", "w") as out:
      out.write("/*\n * This file is automatically generated by afTable.py from the xml descriptions provided as part of the STM32Cube IDE\n")
      out.write(" * Timestamp: "+datetime.datetime.utcnow().isoformat()+"\n")
      out.write(" */\n\n")
      
      for type in sorted(peripheralPinTypes[func]):
         out.write("#define FIND_VALUE_"+func+"_"+type+"_inner(INDEX,PIN) INDEX##_"+func+"_##PIN\n")
         out.write("#define FIND_VALUE_"+func+"_"+type+"(INDEX,PIN) FIND_VALUE_"+func+"_"+type+"_inner(INDEX,PIN)\n\n")
      for i in range(maxOfPeripheral[func]):
         periphName = func+str(i)
         out.write("#ifdef USE_"+periphName+"\n")
         for type in sorted(peripheralPinTypes[func]): 
            out.write("#if !FIND_VALUE_"+func+"_"+type+"(_"+periphName+", "+periphName+"_"+type+")\n");
            out.write("#error Invalid pin assignment for "+periphName+" "+type+" pin: "+periphName+"_"+type+"\n");
            out.write("#endif\n");
         out.write("#endif\n\n");
         

with open("peripheralPinCheck.h", "w") as out:
   out.write("/*\n * This file is automatically generated by afTable.py from the xml descriptions provided as part of the STM32Cube IDE\n")
   out.write(" * Timestamp: "+datetime.datetime.utcnow().isoformat()+"\n")
   out.write(" */\n\n")
   for func in sorted(peripheralPinTypes):
      out.write("#include \"PeripheralChecks/peripheralPinCheck_"+func.lower()+".h\"\n");










