import re

def parseConstants(constants):
  """Keeps looping over constants dictionary,
  replacing constants until no more are found
  """
  again = True
  while again:
    again = False
    for key, value in constants.items():
      if isinstance(value, list):
        for idx, val in enumerate(value):
          value[idx] = val % constants
          again = again or (value[idx] != val)
        constants[key] = " ".join(value)
      elif value:
        if (value):
          constants[key] = value % constants
          again = again or (constants[key] != value)
