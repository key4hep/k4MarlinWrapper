import re

def parseConstants(CONSTANTS):
  CONSTANTS_B = dict()
  for key,value in CONSTANTS.items():
    if value:
      captured_patterns = re.findall('\$\{\w*\}', value)
      if len(captured_patterns) > 0:
        for pattern in captured_patterns:
          CONSTANTS_B[key] = CONSTANTS[key].replace(pattern, CONSTANTS[pattern[2:-1]])
      else:
        CONSTANTS_B[key] = value

  return CONSTANTS_B

if __name__ == "__main__":
  parseConstants()