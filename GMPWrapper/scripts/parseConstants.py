import re

def parseConstants(constants):
  again = True
  while again:
    again = False
    for key,value in constants.items():
      if (value):
        constants[key] = value % constants
        again = (again) or (constants[key] != value)

if __name__ == "__main__":
  parseConstants()