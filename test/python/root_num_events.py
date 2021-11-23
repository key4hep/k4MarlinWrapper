#########################################################
# Print the number of events of a ROOT file that has an "events" branch,
# as in edm4hep files
#########################################################

import ROOT
import sys

f = ROOT.TFile.Open(sys.argv[1], "READ")
tree = f.Get("events")
print(tree.GetEntries())
