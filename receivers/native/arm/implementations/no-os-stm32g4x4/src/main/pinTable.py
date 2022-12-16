import xml.etree.ElementTree as ElementTree
import sys
import re


for i in range(1, len(sys.argv)):
   functions = set()
   tree = ElementTree.parse(sys.argv[i])
   root = tree.getroot()
   for signal in root.iter("{http://mcd.rou.st.com/modules.php?name=mcu}PinSignal"):
#      for signal in pin.findall("{http://mcd.rou.st.com/modules.php?name=mcu}PinSignal"):
      functions.add(signal.get("Name"));
#   print(functions)
   functionPins = {}
   functionPinAF = {}
   for func in functions:
      functionPins[func] = []
      functionPinAF[func] = {}
      
   for pin in root.iter("{http://mcd.rou.st.com/modules.php?name=mcu}GPIO_Pin"):
      pinName = pin.get("Name")
      if pinName.find('-')>=0:
         pinName = pinName.rsplit('-')[0]
      if (pinName.find('[')<0):
         for signal in pin.findall("{http://mcd.rou.st.com/modules.php?name=mcu}PinSignal"):
            functionPins[signal.get("Name")].append(pinName)
            af = signal.find("{http://mcd.rou.st.com/modules.php?name=mcu}SpecificParameter").find("{http://mcd.rou.st.com/modules.php?name=mcu}PossibleValue").text
            string = af.rsplit("AF")[1].split("_")[0]
            functionPinAF[signal.get("Name")][pinName] = string
   outputName = sys.argv[i].replace(".xml", "")+".h"
   with open(outputName, "w") as out:
      name = re.split('\\.| |\\\\|/|-', sys.argv[i])
      name = name[len(name)-2]
      out.write("#ifndef "+name+"_ALTERNATE_FUNCTION\n")
      out.write("#define "+name+"_ALTERNATE_FUNCTION\n\n")
      for func in sorted(functions):
         for pin in functionPins[func]:
            out.write("#define "+func+"_"+pin+"_ "+pin+",GPIO_AF"+str(functionPinAF[func][pin])+"\n");
         for pin in functionPins[func]:
            out.write("#define _"+func+"_"+pin+"_ 1\n");
         out.write('\n')
      out.write("#endif /* "+name+"_ALTERNATE_FUNCTION */")
