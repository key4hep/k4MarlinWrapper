import re

def parseConstants(constants):
  again = True
  while again:
    again = False
    for key,value in constants.items():
      if type(value) is list:
        for idx, val in enumerate(value):
          value[idx] = val % constants
          again = (again) or (value[idx] != val)
        formatted_list = ""
        for val in value:
          formatted_list += '<{}>, '.format(val)
        constants[key] = formatted_list
      else:
        if (value):
          constants[key] = value % constants
          again = (again) or (constants[key] != value)

if __name__ == "__main__":
  parseConstants()